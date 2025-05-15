#pragma once

#include "fmt/format.h"
#include "ggml.h"
#include "llama-vocab.h"
#include "tools.hpp"
#include <cassert>
#include <cstdint>
#include <string>
#include <vector>
namespace sep {

struct Config {
	uint32_t dim			= 0;
	uint32_t hidden_dim		= 0;
	uint32_t n_layers		= 0;
	uint32_t n_heads		= 0;
	uint32_t n_kv_heads		= 0;
	uint32_t vocab_size		= 0;
	uint32_t seq_len		= 0;
	uint32_t rope_dim_count = 0;

	Config(gguf_context *ctx) {
		dim			   = gguf_get_val_u32(ctx, gguf_find_key(ctx, "llama.embedding_length"));
		hidden_dim	   = gguf_get_val_u32(ctx, gguf_find_key(ctx, "llama.feed_forward_length"));
		n_heads		   = gguf_get_val_u32(ctx, gguf_find_key(ctx, "llama.attention.head_count"));
		n_kv_heads	   = gguf_get_val_u32(ctx, gguf_find_key(ctx, "llama.attention.head_count_kv"));
		n_layers	   = gguf_get_val_u32(ctx, gguf_find_key(ctx, "llama.block_count"));
		seq_len		   = gguf_get_val_u32(ctx, gguf_find_key(ctx, "llama.context_length"));
		vocab_size	   = gguf_get_val_u32(ctx, gguf_find_key(ctx, "llama.vocab_size"));
		rope_dim_count = gguf_get_val_u32(ctx, gguf_find_key(ctx, "llama.rope.dimension_count"));
	}
	~Config() = default;
};
// Note: key / value cache is a cache buffer, and we need save result computed before in attention
struct RunState {
	float *x;	   // activation at current time stamp (dim,)
	float *xb;	   // same, but inside a residual branch (dim,)
	float *xb2;	   // an additional buffer just for convenience (dim,)
	float *hb;	   // buffer for hidden dimension in the ffn (hidden_dim,)
	float *hb2;	   // buffer for hidden dimension in the ffn (hidden_dim,)
	float *q;	   // query (dim,)
	float *k;	   // key (dim,)
	float *v;	   // value (dim,)
	float *att;	   // buffer for scores/attention values (n_heads, seq_len)
	float *logits; // output logits
	// kv cache
	float *key_cache;	// (layer, seq_len, dim)
	float *value_cache; // (layer, seq_len, dim)

	Config *config;

	RunState(Config *config);
	RunState(const RunState &other) : RunState(other.config) {
		memcpy(x, other.x, sizeof(float) * config->dim);
		memcpy(xb, other.xb, sizeof(float) * config->dim);
		memcpy(xb2, other.xb2, sizeof(float) * config->dim);
		memcpy(hb, other.hb, sizeof(float) * config->hidden_dim);
		memcpy(hb2, other.hb2, sizeof(float) * config->hidden_dim);
		memcpy(q, other.q, sizeof(float) * config->dim);
		memcpy(att, other.att, sizeof(float) * config->n_heads * config->seq_len);

		memcpy(logits, other.logits, sizeof(float) * config->vocab_size);
		auto kv_dim				= (config->dim * config->n_kv_heads) / config->n_heads;
		uint64_t key_cache_size = sizeof(float) * config->n_layers * config->seq_len * kv_dim;
		memcpy(key_cache, other.key_cache, key_cache_size);
		memcpy(value_cache, other.value_cache, key_cache_size);
	};
	~RunState();
};

struct LayerWeight {
	float *attn_norm; // "blk.$.attn_norm.weight"
	float *ffn_norm;  // "blk.$.ffn_norm.weight"

	float *attn_q;		// "blk.$.attn_q.weight"
	float *attn_k;		// "blk.$.attn_k.weight"
	float *attn_v;		// "blk.$.attn_v.weight"
	float *attn_output; // "blk.$.attn_output.weight"

	float *ffn_gate; // "blk.$.ffn_gate.weight"
	float *ffn_up;	 // "blk.$.ffn_up.weight"
	float *ffn_down; // "blk.$.ffn_down.weight"

	LayerWeight(ggml_context *ctx, uint32_t layer)
		: attn_norm(get_tensor(ctx, layer, "attn_norm.weight")),
		  ffn_norm(get_tensor(ctx, layer, "ffn_norm.weight")),
		  attn_q(get_tensor(ctx, layer, "attn_q.weight")),
		  attn_k(get_tensor(ctx, layer, "attn_k.weight")),
		  attn_v(get_tensor(ctx, layer, "attn_v.weight")),
		  attn_output(get_tensor(ctx, layer, "attn_output.weight")),
		  ffn_gate(get_tensor(ctx, layer, "ffn_gate.weight")),
		  ffn_up(get_tensor(ctx, layer, "ffn_up.weight")),
		  ffn_down(get_tensor(ctx, layer, "ffn_down.weight")) {}
};

struct Weight {
	// token embedding table
	float *token_embedding_table;
	float *output_weight;
	float *rms_final_weight;

	std::vector<LayerWeight> lw;

	Weight(ggml_context *ctx, uint32_t n_layers)
		: token_embedding_table((float *)ggml_get_tensor(ctx, "token_embd.weight")->data),
		  output_weight((float *)ggml_get_tensor(ctx, "output.weight")->data),
		  rms_final_weight((float *)ggml_get_tensor(ctx, "output_norm.weight")->data) {
		for (uint32_t i = 0; i < n_layers; i++) {
			lw.emplace_back(ctx, i);
		}
	}

	~Weight() = default;
};

struct Sampler {
	int vocab_size;

	Sampler(int vocab_size) : vocab_size(vocab_size) {}
	~Sampler() {}

	int sample(float *logits) {
		int next;
		next = sample_argmax(logits, vocab_size);
		return next;
	}
};

struct Tokenizer {

	using Token = llama_vocab::id;

	struct llama_vocab vocab;

	Tokenizer(std::string vocab_path) {
		struct ggml_context *ctx	   = nullptr;
		struct gguf_init_params params = {
			.no_alloc = true,
			.ctx	  = &ctx,
		};

		struct gguf_context *meta = gguf_init_from_file(vocab_path.c_str(), params);
		assert(meta);

		llm_load_vocab(vocab, meta);

		gguf_free(meta);
	}

	size_t n_vocabs() const { return vocab.n_vocab; }
	Token bos_token() const { return vocab.special_bos_id; }
	std::vector<Token> tokenize(const std::string &text, bool add_special) const {
		return llama_tokenize_internal(vocab, text, add_special, true);
	}
	std::string to_string(Token token) const { return llama_token_to_piece(vocab, token); }
};

struct Transformer {

	std::string filename;
	Config *config;
	Weight *weight;
	RunState *state;

	Transformer(std::string filename);
	~Transformer();

	void multihead_attention(uint32_t pos, uint64_t loff, Config &p, RunState &s);
	void attention(int pos, int L);
	void ffn(int L);
	float *forward(int token, int pos);

	void generate(Tokenizer *tk, Sampler *sampler, std::string prompt, int steps);

	ggml_context *ggml_ctx_;
	gguf_context *gguf_ctx_;
};

static void rope(int pos, Config *p, RunState *s) {

	auto dim	   = p->dim;
	auto head_size = dim / p->n_heads;
	auto kv_dim	   = (p->dim * p->n_kv_heads) / p->n_heads;

	for (int i = 0; i < dim; i += 2) {
		int head_dim = i % head_size;
		float freq	 = 1.0f / powf(10000.0f, head_dim / (float)head_size);
		float val	 = pos * freq;
		float fcr	 = cosf(val);
		float fci	 = sinf(val);
		int rotn	 = i < kv_dim ? 2 : 1;
		for (int v = 0; v < rotn; v++) {
			float *vec = v == 0 ? s->q : s->k;
			float v0   = vec[i];
			float v1   = vec[i + 1];
			vec[i]	   = v0 * fcr - v1 * fci;
			vec[i + 1] = v0 * fci + v1 * fcr;
		}
	}
}

} // namespace sep