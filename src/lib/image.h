#ifndef IMAGE_H
#define IMAGEH

#include <stdbool.h>
#include <stddef.h>

/*
 * Get the height of an image, in pixels.
 */
size_t get_image_height(void);

/*
 * Get the width of an image, in pixels.
 */
size_t get_image_width(void);

/*
/*
 * Get the total number of bytes needed to store an image, in pixels.
 * This is equal to height * (width * 4), as each image has width * 4
 * bits in a row.
 */
size_t get_image_size(void);

/* 
 * Fill in the image for the selected staff member image.
 * First initial params:
 * j: julie
 * p: pat
 * s: sean
 * e: peter
 * a: anna
 * l: liana
 * If char specified is not one of these options, defaults to julie.
 * `buf` is an array of bytes of length get_image_size().
 */
void get_image_to_draw(unsigned int buf[], size_t buflen, char first_initial);

#endif // IMAGE_H
