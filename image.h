#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <x86intrin.h>

// This will actually contain 4 packed half-precision floats converted with F16C
// We don't have a proper type for this, though, so uint64_t will have to do
typedef uint64_t rgba16_pixel;

typedef struct _rgba32_pixel {
	float r;
	float g;
	float b;
	float a;
} rgba32_pixel;

static inline rgba32_pixel load_pixel(const rgba16_pixel *ptr) {
	rgba32_pixel pixel;
	__m128i packed = _mm_load_sd((const void*)ptr);
	__m128 unpacked = _mm_cvtph_ps(packed);
	_mm_store_si128((void*)&pixel, unpacked);
	return pixel;
}

static inline void store_pixel(rgba16_pixel *ptr, rgba32_pixel pixel) {
	__m128 unpacked = _mm_load_si128((const void*)&pixel);
	__m128i packed = _mm_cvtps_ph(unpacked, 0);
	_mm_store_sd((void*)ptr, packed);
}

typedef struct _image {
	uint16_t width;
	uint16_t height;
	rgba16_pixel pixels[];
} image;

#endif
