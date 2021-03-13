#ifndef IMAGE_H
#define IMAGEH

#include <stdbool.h>
#include <stddef.h>

size_t get_image_height(void);

/*
 * `font_get_glpyh_width`
 *
 * Get the width in pixels of each glyph in this font.
 *
 * @return    the width in pixels
 */
size_t get_image_width(void);

/*
 * `font_get_glpyh_size`
 *
 * Get the total number of bytes needed to store the glyph image
 * for one character. This is equal to the product of
 * (glyph height * glyph width).
 *
 * @return    the size in bytes of a glyph image
 */
size_t get_image_size(void);

/*
 * `font_get_glpyh`
 *
 * Fill in the glyph image for character `ch` into `buf`.
 * `buf` is an array of bytes of length font_get_glyph_size().
 * Each 'on' pixel has value 0xff, 'off' pixel has value 0x0.
 *
 * @param ch    the requested character
 * @param buf   the buffer in which to place the glyph image
 * @param buflen the length of `buf`.
 *              `buflen` should be equal to value returned by font_get_glyph_size()
 *
 * @return      returns true when successfully filled `buf`, false otherwise.
 *              Failure is when `ch` is not available in this font or `buflen`
 *              does not equal the value returned by font_get_glyph_size()
 */
void get_image_to_draw(unsigned int buf[], size_t buflen, char first_initial);

#endif // IMAGE_H
