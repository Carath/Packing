/*
 * Wrapper for a generic 64-bit random number generator.
 *
 * Ideally, rng64.h should only be included in source files, for it contains functions bodies
 * to be inlined. Those functions are thread-safe, as long as each thread uses its own rng:
 *
 * - Initializing the internal state of the given rng. The 'stream' parameter
 *   is optional since several rng do not support this, and can be left to 0:
 *
 *     void rng64_init(void *generic_rng64, uint64_t seed, uint64_t stream);
 *
 * - Generating the next unsigned 64-bit integer, from the given rng:
 *
 *     uint64_t rng64_nextInt(void *generic_rng64);
 *
 * - Generating a float uniformly between 0. and 1., from the given rng:
 *
 *     float rng64_nextFloat(void *generic_rng64);
 *
 * A stream, when available, is useful for producing distincts sequences of random number,
 * from the same rng and the same seed. Finally, RNG64_MAX is the maximum value that the
 * defined 64-bit rng is able to output.
 *
 * A rng64 instance is to be declared (anywhere) in order to be used, and passed by
 * address in the previous functions. A basic example is shown below, where the rng
 * is initialized with the current system time (assuming <time.h> has been included),
 * and the stream ID is 0. This will print a random integer between 0 and 9 included
 * (with a negligeable bias due to 10 no dividing RNG64_MAX):
 *
 * rng64 rng;
 * rng64_init(&rng, time(NULL), 0);
 * printf("%d\n", rng64_nextInt(&rng) % 10);
 *
 * Be wary that the resolution of time() is only the second, thus it may not be suited to provide
 * parallel processes with unique seeds. Use either a finer time resolution for this, or better
 * an index unique for each processes, or even the address of a resource unique to each of them.
*/
/* --------------------------------------------------------------------------- */
/*
 * What follows is the Lehmer 64-bit RNG, wrapped in a generic form for ease of use/change.
 * Functions are also inlined for speed. Note that this implementation does not support streams:
 */

#ifndef RNG64_H
#define RNG64_H

#if __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <limits.h>

typedef __uint128_t rng64;

#define RNG64_MAX ULONG_MAX

static inline uint64_t rng64_nextInt(void *generic_rng64)
{
	rng64 *rng = (rng64*) generic_rng64;
	*rng *= 0xda942042e4dd58b5;
	return (*rng) >> 64;
}

static inline uint32_t rng64_nextInt32(void *generic_rng64)
{
	return rng64_nextInt(generic_rng64) >> 32;
}

__attribute__((unused)) static void rng64_init(void *generic_rng64, uint64_t seed, uint64_t stream)
{
	rng64 *rng = (rng64*) generic_rng64;
	*rng = 1u;
	rng64_nextInt(rng);
	*rng = (*rng + seed) | 1u; /* must be odd */
	rng64_nextInt(rng);
}

static inline double rng64_nextDouble(void *generic_rng64)
{
	return rng64_nextInt(generic_rng64) / (double) RNG64_MAX;
}

/* Faster (but less precise) than rng64_nextDouble(): */
static inline float rng64_nextFloat(void *generic_rng64)
{
	return rng64_nextInt32(generic_rng64) / (float) (RNG64_MAX >> 32);
}

#if __cplusplus
}
#endif

#endif
