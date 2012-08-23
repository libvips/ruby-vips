#ifndef RUBY_VIPS_H
#define RUBY_VIPS_H

#include <ruby.h>
#include <vips/vips.h>

extern VALUE mVIPS, eVIPSError;

void vips_lib_error();

/* If we're not using GNU C, elide __attribute__ */
#ifndef __GNUC__
#  define  __attribute__(x)  /*NOTHING*/
#endif

/* Backports from ruby 1.9.2 for ruby 1.8.7
 */

#ifndef DBL2NUM
#define DBL2NUM(dbl) rb_float_new(dbl)
#endif

#ifndef RARRAY_LENINT
#define RARRAY_LENINT(ary) RARRAY_LEN(ary)
#endif

#ifndef rb_str_new_cstr
#define rb_str_new_cstr(str) rb_str_new(str, (long)strlen(str))
#endif

/* vips-7.26.3 and earlier 7.26 (used on Ubuntu 12.04) has a broken compat
 * macro for im_openout(). Make sure we have the correct one defined.
 */
#ifdef im_openout
#undef im_openout
#define im_openout( F ) vips_image_new_mode( F, "w" )
#endif /*im_openout*/

#include "header.h"
#include "image_arithmetic.h"
#include "image_boolean.h"
#include "image_colour.h"
#include "image_conversion.h"
#include "image_convolution.h"
#include "image_freq_filt.h"
#include "image.h"
#include "image_histograms_lut.h"
#include "image_morphology.h"
#include "image_mosaicing.h"
#include "image_relational.h"
#include "image_resample.h"
#include "interpolator.h"
#include "mask.h"
#include "reader.h"
#include "writer.h"

#endif
