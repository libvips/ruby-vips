#include "ruby_vips.h"
#include "image.h"
#include "mask.h"
#include "image_morphology.h"

/*
 *  call-seq:
 *     im.dilate(mask) -> image
 *
 *  Dilates *self*, according to <i>mask</i>. The output image is the same size
 *  as the input. Sets pixels in the output if *any* part of the mask matches.
 *
 *  *self* must be a one channel binary image ie. with pixels that are either 0
 *  (black) or 255 (white). This method assume that *self* contains white
 *  objects against a black background.
 *
 *  <i>mask</i> can be a two-dimensional array or a Mask object. All mask values
 *  must be integers or this method will raise an exception.
 *
 *  Mask coefficients can be either 0 (for object) or 255 (for background) or
 *  128 (for do not care). The mask should have odd length sides and the origin
 *  of the mask is at location (mask_columns/2, mask_rows/2) integer division.
 *
 *  Based on the book "Fundamentals of Digital Image Processing" by A.  Jain,
 *  pp 384-388, Prentice-Hall, 1989.
 */

VALUE
img_dilate(VALUE obj, VALUE mask)
{
    INTMASK *imask;

	GetImg(obj, data, im);
	OutImg2(obj, mask, new, data_new, im_new);

    mask_arg2mask(mask, &imask, NULL);

    if (im_dilate(im, im_new, imask))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.erode(mask) -> image
 *
 *  Erodes *self*, according to <i>mask</i>. The output image is the same size
 *  as the input. Sets pixels in the output if *all* part of the mask matches.
 *
 *  *self* must be a one channel binary image ie. with pixels that are either 0
 *  (black) or 255 (white). This method assume that *self* contains white
 *  objects against a black background.
 *
 *  <i>mask</i> can be a two-dimensional array or a Mask object. All mask values
 *  must be integers or this method will raise an exception.
 *
 *  Mask coefficients can be either 0 (for object) or 255 (for background) or
 *  128 (for do not care). The mask should have odd length sides and the origin
 *  of the mask is at location (mask_columns/2, mask_rows/2) integer division.
 *
 *  Based on the book "Fundamentals of Digital Image Processing" by A.  Jain,
 *  pp 384-388, Prentice-Hall, 1989.
 */

VALUE
img_erode(VALUE obj, VALUE mask)
{
    INTMASK *imask;

	GetImg(obj, data, im);
	OutImg2(obj, mask, new, data_new, im_new);

    mask_arg2mask(mask, &imask, NULL);

    if (im_erode(im, im_new, imask))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.rank(xsize, ysize, n) -> image
 *
 *  Does rank filtering on an image. A window of size <i>xsize</i> by
 *  <i>ysize</i> is passed over the image. At each position, the pixels inside
 *  the window are sorted into ascending order and the pixel at the <i>n</i>th
 *  position is output. <i>n</i> numbers from 0.
 *
 *  It works for any non-complex image type, with any number of bands. The input
 *  is expanded by copying edge pixels before performing the operation so that
 *  the output image has the same size as *self*. Edge pixels in the output
 *  image are therefore only approximate.
 */

VALUE
img_rank(VALUE obj, VALUE xsize, VALUE ysize, VALUE order)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_rank(im, im_new, NUM2INT(xsize), NUM2INT(ysize), NUM2INT(order)))
        vips_lib_error();

    return new;
}

VALUE
img_rank_image_internal(int argc, VALUE *argv, VALUE obj, int index)
{
	vipsImg *im_t;
    IMAGE **ins;
    int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    ins = IM_ARRAY(im_new, argc + 1, IMAGE*);
	ins[0] = im;

    for (i = 0; i < argc; i++) {
		img_add_dep(data_new, argv[i]);
        Data_Get_Struct(argv[i], vipsImg, im_t);
        ins[i + 1] = im_t->in;
    }

    if (im_rank_image(ins, im_new, argc + 1, index))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.rank_image(index, other_image, ...)
 *
 *  Sorts the input images pixel-wise, then outputs an image in which each pixel
 *  is selected from the sorted list by <i>index</i> parameter. For example, if
 *  <i>index</i> is zero, then each output pixel will be the minimum of all the
 *  corresponding input pixels.
 *
 *  It works for any uncoded, non-complex image type. All input images must
 *  match in size, format, and number of bands.
 */

VALUE
img_rank_image(int argc, VALUE *argv, VALUE obj)
{
	VALUE index, *images;
	if (argc < 2)
		rb_raise(rb_eArgError, "Need an index and at least one image");

	index = argv[0];
	images = RARRAY_PTR(rb_ary_new4(argc - 1, argv + 1));

	return img_rank_image_internal(argc - 1, images, obj, NUM2INT(index));
}

/*
 *  call-seq:
 *     im.maxvalue(other_image, ...) -> image
 *
 *  Sorts the input images pixel-wise, then outputs an image in which each pixel
 *  is the maximum from the input pixels.
 *
 *  It works for any uncoded, non-complex image type. All input images must
 *  match in size, format, and number of bands.
 */

VALUE
img_maxvalue(int argc, VALUE *argv, VALUE obj)
{
	return img_rank_image_internal(argc, argv, obj, argc - 1);
}

static VALUE
img_cntlines(VALUE obj, int flag) {
    double nolines;
	GetImg(obj, data, im);
    
    if (im_cntlines(im, &nolines, flag))
        vips_lib_error();

    return DBL2NUM(nolines);
}

/*
 *  call-seq:
 *     im.cntlines_h -> number
 *
 *  Calculates the number of transitions between black and white for the
 *  horizontal direction of an image. black is < 128, and white is >= 128.
 *
 *  Returns the mean of the result. Input should be binary one channel.
 */

VALUE
img_cntlines_h(VALUE obj) {
	return img_cntlines(obj, 0);
}

/*
 *  call-seq:
 *     im.cntlines_v -> number
 *
 *  Calculates the number of transitions between black and white for the
 *  vertical direction of an image. black is < 128, and white is >= 128.
 *
 *  Returns the mean of the result. Input should be binary one channel.
 */

VALUE
img_cntlines_v(VALUE obj) {
	return img_cntlines(obj, 1);
}

static VALUE
img_zerox(VALUE obj, int flag)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_zerox(im, im_new, flag))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.zerox_pos -> image
 *
 * Detects the +ve edges of zero crossings of *self*. Works on integer images.
 * The output image is byte with zero crossing set to 255 and all other values
 * set to zero.
 */

VALUE
img_zerox_pos(VALUE obj)
{
    return img_zerox(obj, 1);
}

/*
 *  call-seq:
 *     im.zerox_neg -> image
 *
 * Detects the -ve edges of zero crossings of *self*. Works on integer images.
 * The output image is byte with zero crossing set to 255 and all other values
 * set to zero.
 */

VALUE
img_zerox_neg(VALUE obj)
{
    return img_zerox(obj, -1);
}

static VALUE
img_profile(VALUE obj, int dir)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_profile(im, im_new, dir))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.profile_h -> image
 *
 *  For each horizontal line, find the position of the first non-zero pixel from
 *  the left. Output is USHORT with width = 1 and height = input height.
 */

VALUE
img_profile_h(VALUE obj)
{
	return img_profile(obj, 1);
}

/*
 *  call-seq:
 *     im.profile_v -> image
 *
 *  For each vertical line, find the position of the first non-zero pixel from
 *  the top. Output is USHORT with width = input width and height = 1.
 */

VALUE
img_profile_v(VALUE obj)
{
	return img_profile(obj, 0);
}

/*
 *  call-seq:
 *     im.label_regions -> image, segments
 *
 *  *self* is repeatedly scanned and regions of 4-connected pixels with the same
 *  pixel value found. Every time a region is discovered, those pixels are
 *  marked in the output image with a unique serial number. Once all pixels have
 *  been labelled, the operation returns, returning an an image and
 *  <i>segments</i>, the number of discrete regions which were detected.
 *
 *  The output image is always a 1-band image with band format :UINT, and of the
 *  same dimensions as *self*.
 *
 *  This operation is useful for, for example, blob counting. You can use the
 *  morphological operators to detect and isolate a series of objects, then use
 *  this method to number them all.
 *
 *  Use Image#histindexed to (for example) find blob coordinates.
 */

VALUE
img_label_regions(VALUE obj)
{
	int segments;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_label_regions(im, im_new, &segments))
        vips_lib_error();

    return rb_ary_new3(2, new, segments);
}
