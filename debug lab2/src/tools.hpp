#pragma once

#include "fmt/format.h"
#include "ggml.h"
#include <ctime>
#include <string>
namespace sep {

static float *get_tensor(ggml_context *ctx, uint32_t layer, const char *name) {
	std::string tensor_name = fmt::format("blk.{}.{}", layer, name);
	ggml_tensor *t			= ggml_get_tensor(ctx, tensor_name.c_str());
	if (t == nullptr) {
		throw std::runtime_error(fmt::format("Failed to get tensor: {}", tensor_name));
	}
	return (float *)t->data;
}

static int sample_argmax(float *probabilities, int n) {
	// return the index that has the highest probability
	int max_i	= 0;
	float max_p = probabilities[0];
	for (int i = 1; i < n; i++) {
		if (probabilities[i] > max_p) {
			max_i = i;
			max_p = probabilities[i];
		}
	}
	return max_i;
}

static void rmsnorm(float *o, float *x, float *weight, int64_t size) {
	// calculate sum of squares
	float ss = 0.0f;
	for (int j = 0; j < size; j++) {
		ss += x[j] * x[j];
	}
	ss /= size;
	ss += 1e-5f;
	ss = 1.0f / sqrtf(ss);
	// normalize and scale
	for (int j = 0; j < size; j++) {
		o[j] = weight[j] * (ss * x[j]);
	}
}

static void matmul(float *xout, float *x, float *w, int n, int d) {
	// W (d,n) @ x (n,) -> xout (d,)
	// by far the most amount of time is spent inside this little function
	int i;
#pragma omp parallel for private(i)
	for (i = 0; i < d; i++) {
		float val = 0.0f;
		for (int j = 0; j < n; j++) {
			val += w[i * n + j] * x[j];
		}
		xout[i] = val;
	}
}

static void softmax(float *x, int64_t size) {
	// find max value (for numerical stability)
	float max_val = x[0];
	for (int i = 1; i < size; i++) {
		if (x[i] > max_val) {
			max_val = x[i];
		}
	}
	// exp and sum
	float sum = 0.0f;
	for (int i = 0; i < size; i++) {
		x[i] = expf(x[i] - max_val);
		sum += x[i];
	}
	// normalize
	for (int i = 0; i < size; i++) {
		x[i] /= sum;
	}
}

} // namespace sep
