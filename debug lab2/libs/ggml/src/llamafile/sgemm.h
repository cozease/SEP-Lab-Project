#pragma once
#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

bool llamafile_sgemm(int64_t, int64_t, int64_t, const void *, int64_t,
					 const void *, int64_t, void *, int64_t, int, int, int, int,
					 int);

#ifdef __cplusplus
}
#endif
