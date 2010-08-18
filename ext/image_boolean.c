#include "ruby_vips.h"
#include "image.h"

static VALUE
img_and_img(VALUE obj, VALUE obj2)
{
    RUBY_VIPS_BINARY(im_andimage);
}

/*
 *  call-seq:
 *     im & other_image -> image
 *     im & c           -> image
 *     im & [ c1, ... ] -> image
 *
 *  In the first form, this operation calculates the bitwise and between each
 *  pixel in this image and <i>other_image</i>. The images must be the same
 *  size. They may have any format.
 *
 *  If the number of bands differs, one of the images must have one band. In
 *  this case, an n-band image is formed from the one-band image by joining n
 *  copies of the one-band image together and then the two n-band images are
 *  operated upon.
 *
 *  In the second form, it calculates the bitwise and of image pixels with a
 *  constant. If the image has multiple bands, then the constant is applied to
 *  all bands.
 *
 *  In the third form, it calculates the bitwise and of image pixels with an
 *  array of constants. The number of constants must match the number of bands
 *  in the image.
 */

VALUE
img_and(VALUE obj, VALUE arg)
{
    double c_one;
    double *c = &c_one;
    int i, len = 1;
    VALUE *argv;

    GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (CLASS_OF(arg) == cVIPSImage)
        return img_and_img(obj, arg);
    
    if (TYPE(arg) == T_ARRAY) {
        len = RARRAY_LEN(arg);
        argv = RARRAY_PTR(arg);
        c = IM_ARRAY(im_new, len, double);
        for (i = 0; i < len; i++)
            c[i] = NUM2DBL(argv[i]);
    } else
        c_one = NUM2DBL(arg);

    if (im_andimage_vec(im, im_new, len, c))
        vips_lib_error();

    return new;
}

static VALUE
img_or_img(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_orimage);
}

/*
 *  call-seq:
 *     im | other_image -> image
 *     im | c           -> image
 *     im | [ c1, ... ] -> image
 *
 *  In the first form, this operation calculates the bitwise or between each
 *  pixel in this image and <i>other_image</i>. The images must be the same
 *  size. They may have any format.
 *
 *  If the number of bands differs, one of the images must have one band. In
 *  this case, an n-band image is formed from the one-band image by joining n
 *  copies of the one-band image together and then the two n-band images are
 *  operated upon.
 *
 *  In the second form, it calculates the bitwise or of image pixels with a
 *  constant. If the image has multiple bands, then the constant is applied to
 *  all bands.
 *
 *  In the third form, it calculates the bitwise or of image pixels with an
 *  array of constants. The number of constants must match the number of bands
 *  in the image.
 */

VALUE
img_or(VALUE obj, VALUE arg)
{
    double c_one;
    double *c = &c_one;
    int i, len = 1;
    VALUE *argv;

    GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (CLASS_OF(arg) == cVIPSImage)
        return img_or_img(obj, arg);

    if (TYPE(arg) == T_ARRAY) {
        len = RARRAY_LEN(arg);
        argv = RARRAY_PTR(arg);
        c = IM_ARRAY(im_new, len, double);
        for (i = 0; i < len; i++)
            c[i] = NUM2DBL(argv[i]);
    } else
        c_one = NUM2DBL(arg);

    if (im_orimage_vec(im, im_new, len, c))
        vips_lib_error();

    return new;
}

static VALUE
img_xor_img(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_eorimage);
}

/*
 *  call-seq:
 *     im ^ other_image -> image
 *     im ^ c           -> image
 *     im ^ [ c1, ... ] -> image
 *
 *  In the first form, this operation calculates the bitwise xor between each
 *  pixel in this image and <i>other_image</i>. The images must be the same
 *  size. They may have any format.
 *
 *  If the number of bands differs, one of the images must have one band. In
 *  this case, an n-band image is formed from the one-band image by joining n
 *  copies of the one-band image together and then the two n-band images are
 *  operated upon.
 *
 *  In the second form, it calculates the bitwise xor of image pixels with a
 *  constant. If the image has multiple bands, then the constant is applied to
 *  all bands.
 *
 *  In the third form, it calculates the bitwise xor of image pixels with an
 *  array of constants. The number of constants must match the number of bands
 *  in the image.
 */

VALUE
img_xor(VALUE obj, VALUE arg)
{
    double c_one;
    double *c = &c_one;
    int i, len = 1;
    VALUE *argv;

    GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (CLASS_OF(arg) == cVIPSImage)
        return img_xor_img(obj, arg);

    if (TYPE(arg) == T_ARRAY) {
        len = RARRAY_LEN(arg);
        argv = RARRAY_PTR(arg);
        c = IM_ARRAY(im_new, len, double);
        for (i = 0; i < len; i++)
            c[i] = NUM2DBL(argv[i]);
    } else
        c_one = NUM2DBL(arg);

    if (im_eorimage_vec(im, im_new, len, c))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im << c           -> image
 *     im << [ c1, ... ] -> image
 *
 *  Calculates the bitwise left-shift by <i>c</i> bits.
 *
 *  If only one constant is given, it calculates the left-shift for all bands
 *  in the image. If an array of constants is given, it applies each constant
 *  to an image band.
 */

VALUE
img_shiftleft(VALUE obj, VALUE arg)
{
    double c_one;
    double *c = &c_one;
    int i, len = 1;
    VALUE *argv;

    GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (TYPE(arg) == T_ARRAY) {
        len = RARRAY_LEN(arg);
        argv = RARRAY_PTR(arg);
        c = IM_ARRAY(im_new, len, double);
        for (i = 0; i < len; i++)
            c[i] = NUM2DBL(argv[i]);
    } else
        c_one = NUM2DBL(arg);

    if (im_shiftleft_vec(im, im_new, len, c))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im >> c           -> image
 *     im >> [ c1, ... ] -> image
 *
 *  Calculates the bitwise right-shift by <i>c</i> bits.
 *
 *  If only one constant is given, it calculates the right-shift for all bands
 *  in the image. If an array of constants is given, it applies each constant
 *  to an image band.
 */

VALUE
img_shiftright(VALUE obj, VALUE arg)
{
    double c_one;
    double *c = &c_one;
    int i, len = 1;
    VALUE *argv;

    GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (TYPE(arg) == T_ARRAY) {
        len = RARRAY_LEN(arg);
        argv = RARRAY_PTR(arg);
        c = IM_ARRAY(im_new, len, double);
        for (i = 0; i < len; i++)
            c[i] = NUM2DBL(argv[i]);
    } else
        c_one = NUM2DBL(arg);

    if (im_shiftright_vec(im, im_new, len, c))
        vips_lib_error();

    return new;
}
