#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

unsigned char* load_png_file(const char *filename, unsigned int *width, unsigned int *height);

void save_png_file(const char *filename, const unsigned char *image, unsigned int width, unsigned int height);

struct pixel {
	char R;
	char G;
	char B;
	char alpha;
};

#endif // IMAGE_H_INCLUDED
