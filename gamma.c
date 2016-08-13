#include "gamma.h"

#include <math.h>

#include "image.h"

#define SRGB_ALPHA 0.055f

// Convert from sRGB gamma-encoded RGB values to linear RGB
// Leaves alpha untouched
// Algorithm from: https://en.wikipedia.org/wiki/SRGB#The_reverse_transformation

static inline float gamma_to_linear_subpixel(float value) {
	return (value <= 0.04045f) ? (value/12.92f) : powf((value + SRGB_ALPHA)/(1+SRGB_ALPHA), 2.4f);
}

void gamma_to_linear(image *img) {
	for (size_t i = 0; i < img->width * img->height; i++) {
		rgba32_pixel pixel = load_pixel(&img->pixels[i]);
		pixel.r = gamma_to_linear_subpixel(pixel.r);
		pixel.g = gamma_to_linear_subpixel(pixel.g);
		pixel.b = gamma_to_linear_subpixel(pixel.b);
		store_pixel(&img->pixels[i], pixel);
	}
}

static inline float linear_to_gamma_subpixel(float value) {
	return (value <= 0.0031308f) ? (12.92f * value) : ((1+SRGB_ALPHA) * powf(value, 1/2.4f) - SRGB_ALPHA);
}

void linear_to_gamma(image *img) {
	for (size_t i = 0; i < img->width * img->height; i++) {
		rgba32_pixel pixel = load_pixel(&img->pixels[i]);
		pixel.r = linear_to_gamma_subpixel(pixel.r);
		pixel.g = linear_to_gamma_subpixel(pixel.g);
		pixel.b = linear_to_gamma_subpixel(pixel.b);
		store_pixel(&img->pixels[i], pixel);
	}
}
