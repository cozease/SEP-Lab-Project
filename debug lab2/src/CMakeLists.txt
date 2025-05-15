
add_executable(run "main.cpp" "core.cpp")
target_link_libraries(run PRIVATE llama_tokenizer CLI11::CLI11 ggml fmt)
target_include_directories(run PUBLIC .)
