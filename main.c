#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "netpbm.h"
#include "image.h"
#include "gamma.h"

static void desaturate(image *img, float saturation) {
	for (size_t i = 0; i < img->width * img->height; i++) {
		rgba32_pixel pixel = load_pixel(&img->pixels[i]);
		float averageValue = (pixel.r + pixel.g + pixel.b) / 3;
		pixel.r = averageValue + ((pixel.r - averageValue) * saturation);
		pixel.g = averageValue + ((pixel.g - averageValue) * saturation);
		pixel.b = averageValue + ((pixel.b - averageValue) * saturation);
		store_pixel(&img->pixels[i], pixel);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Wrong number of arguments\n");
		return EXIT_FAILURE;
	}

	char *inputFilename = argv[1];
	char *outputFilename = argv[2];

	FILE *readFp = fopen(inputFilename, "rb");
	if (readFp == NULL) {
		perror("Couldn't open file for reading");
	}

	image *img = ppm_read(readFp);
	fclose(readFp);
	if (img == NULL) {
		return EXIT_FAILURE;
	}

	fprintf(stderr, "Read success?\n");

	gamma_to_linear(img);

	desaturate(img, 0.5);

	linear_to_gamma(img);

	FILE *writeFp = fopen(outputFilename, "wb");
	if (writeFp == NULL) {
		perror("Couldn't open file for writing");
	}

	bool success = ppm_write(writeFp, img);
	fclose(writeFp);
	if (!success) {
		return EXIT_FAILURE;
	}

	fprintf(stderr, "Write success?\n");

	free(img);

	return EXIT_SUCCESS;
}
