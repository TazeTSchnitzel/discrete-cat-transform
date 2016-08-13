#include "netpbm.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "image.h"

static void skip_whitespace(FILE *fp) {
	while (true) {
		char c;
		size_t read = fread(&c, sizeof(char), 1, fp);
		if (read != 1) {
			break;
		}

		if (!isspace(c)) {
			if (c == '#') {
				while (true) {
					read = fread(&c, sizeof(char), 1, fp);
					if (read != 1) {
						break;
					}

					if (c == '\n') {
						break;
					}
				}
				continue;
			} else {
				fseek(fp, -1, SEEK_CUR);
				break;
			}
		}
	}
}

static bool fread_uint16t(FILE *fp, uint16_t *value) {
	return fscanf(fp, "%hu", value) == 1;
}

typedef uint8_t packed_pixel_rgb8[3];

image *ppm_read(FILE *fp) {
	char magicNumber[3] = {0};
	size_t read;

	errno = 0;
	read = fread(&magicNumber, sizeof(char), 2, fp);
	if (read < 2) {
		perror("Couldn't read magic number");
		return NULL;
	}
	if (strcmp("P6", magicNumber) != 0) {
		fprintf(stderr, "Unsupported format: %s\n", magicNumber);
		return NULL;
	}

	skip_whitespace(fp);

	uint16_t width;
	if (!fread_uint16t(fp, &width)) {
		perror("Couldn't read image dimensions");
		return NULL;
	}

	skip_whitespace(fp);

	uint16_t height;
	if (!fread_uint16t(fp, &height)) {
		perror("Couldn't read image dimensions");
		return NULL;
	}

	skip_whitespace(fp);

	uint16_t maxval;
	if (!fread_uint16t(fp, &maxval)) {
		perror("Couldn't read image maximum pixel value");
		return NULL;
	}

	if (maxval == 0) {
		fprintf(stderr, "Invalid image maximum pixel value, must exceed 0\n");
		return NULL;
	}
	if (maxval > 255) {
		fprintf(stderr, "Can't handle 16bpp at this time\n");
		return NULL;
	}

	char finalWhitespace;
	read = fread(&finalWhitespace, sizeof(char), 1, fp);
	if (read != 1) {
		perror("Couldn't read mandatory whitespace");
		return NULL;
	}
	if (!isspace(finalWhitespace)) {
		fprintf(stderr, "Unexpected non-whitespace character: %d\n", finalWhitespace);
		return NULL;
	}

	size_t pixelCount = width * height;
	
	packed_pixel_rgb8 *packedPixels = calloc(sizeof(packed_pixel_rgb8), pixelCount);
	if (!packedPixels) {
		fprintf(stderr, "Could not allocate RAM for %ux%u pixels\n", width, height);
		return NULL;
	}

	read = fread(packedPixels, sizeof(packed_pixel_rgb8), pixelCount, fp);
	if (read != pixelCount) {
		perror("Couldn't read pixel data");
		return NULL;
	}

	image *img = malloc(sizeof(image) + pixelCount * sizeof(rgba16_pixel));
	if (!img) {
		fprintf(stderr, "Could not allocate RAM for %ux%u pixels\n", width, height);
		free(packedPixels);
		return NULL;
	}

	img->width = width;
	img->height = height;

	for (size_t i = 0; i < pixelCount; i++) {
		rgba32_pixel pixel;
		pixel.r = (float)packedPixels[i][0] / 255;
		pixel.g = (float)packedPixels[i][1] / 255;
		pixel.b = (float)packedPixels[i][2] / 255;
		pixel.a = 1.0;
		store_pixel(&img->pixels[i], pixel);
	}

	free(packedPixels);

	return img;
}

bool ppm_write(FILE *fp, image *img) {
	if (fprintf(fp, "P6\n%u %u\n# Obligatory obnoxious advertising comment - drink Pepsi-Cola(R)!\n%u\n", img->width, img->height, 255) < 0) {
		perror("Couldn't write file header");
		return false;
	}

	size_t pixelCount = img->width * img->height;

	packed_pixel_rgb8 *packedPixels = calloc(sizeof(packed_pixel_rgb8), pixelCount);
	if (!packedPixels) {
		fprintf(stderr, "Could not allocate RAM for %ux%u pixels\n", img->width, img->height);
		return NULL;
	}

	for (size_t i = 0; i < pixelCount; i++) {
		rgba32_pixel pixel = load_pixel(&img->pixels[i]);
		packedPixels[i][0] = (uint8_t)(pixel.r * 255);
		packedPixels[i][1] = (uint8_t)(pixel.g * 255);
		packedPixels[i][2] = (uint8_t)(pixel.b * 255);
	}

	size_t written = fwrite(packedPixels, sizeof(packed_pixel_rgb8), pixelCount, fp);
	if (written != pixelCount) {
		perror("Couldn't write pixel data");
		free(packedPixels);
		return false;
	}

	free(packedPixels);
	
	return true;
}
