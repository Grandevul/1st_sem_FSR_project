#include "lodepng.h"
#include "image.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int i, j;

//
unsigned char* load_png_file  (const char *filename, unsigned int *width, unsigned int *height)
{
	unsigned char *image = NULL;
	int error = lodepng_decode32_file(&image, width, height, filename);
	if (error) {
		printf("error %u: %s\n", error, lodepng_error_text(error));
		return NULL;
	}

	return (image);
}

void save_png_file(const char *filename, const unsigned char *image, unsigned int width, unsigned int height)
{
	int error = lodepng_encode32_file(filename, image, width, height);
	if (error) {
		printf("error %u: %s\n", error, lodepng_error_text(error));
		return;
	}
}

