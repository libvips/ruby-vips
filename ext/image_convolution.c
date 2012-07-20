#include "ruby_vips.h"

/*
 *  call-seq:
 *     im.conv(mask) -> image
 *
 *  Convolve *self* with <i>mask</i>. The output image always has the same band format
 *  as *self*. Non-complex images only.
 *
 *  Each output pixel is calculated as sigma[i]{pixel[i] * <i>mask</i>[i]} /
 *  scale + offset, where scale and offset are part of <i>mask</i>. For integer
 *  *self*, the division by scale includes round-to-nearest.
 *
 *  <i>mask</i> can be an array in which case scale defaults to 1 and offset
 *  defaults to zero. <i>mask</i> can also be a Mask object.
 */

VALUE
img_conv(VALUE obj, VALUE m)
{
    DOUBLEMASK *dmask;
    INTMASK *imask;

    GetImg(obj, data, im);
	OutImg2(obj, m, new, data_new, im_new);

    mask_arg2mask(m, &imask, &dmask);

    if (imask) {
        if (im_conv(im, im_new, imask))
            vips_lib_error();
    } else if (im_conv_f(im, im_new, dmask))
        vips_lib_error();

    return new;
}   

/*
 *  call-seq:
 *     im.convsep(mask) -> image
 *
 *  Perform a separable convolution of *self* with <i>mask</i> using integer
 *  arithmetic.
 *
 *  <i>mask</i> must be 1xn or nx1 elements.
 *
 *  The output image always has the same band format as *self*. Non-complex
 *  images only.
 *
 *  The image is convolved twice: once with <i>mask</i> and then again with
 *  <i>mask</i> rotated by 90 degrees. This is much faster for certain types of
 *  mask (gaussian blur, for example) than doing a full 2D convolution.
 *
 *  Each output pixel is calculated as sigma[i]{pixel[i] * <i>mask</i>[i]} /
 *  scale + offset, where scale and offset are part of <i>mask</i>. For integer
 *  *self*, the division by scale includes round-to-nearest.
 *
 *  <i>mask</i> can be an array in which case scale defaults to 1 and offset
 *  defaults to zero. <i>mask</i> can also be a Mask object.
 */

VALUE
img_convsep(VALUE obj, VALUE mask)
{
    DOUBLEMASK *dmask;
    INTMASK *imask;

	GetImg(obj, data, im);
	OutImg2(obj, mask, new, data_new, im_new);

    mask_arg2mask(mask, &imask, &dmask);

    if(imask) {
        if (im_convsep(im, im_new, imask))
            vips_lib_error();
    } else if (im_convsep_f(im, im_new, dmask))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.compass(mask) -> image
 *
 *  *self* is convolved 8 times with <i>mask</i>, each time <i>mask</i> is
 *  rotated by 45 degrees. Each output pixel is the largest absolute value of
 *  the 8 convolutions.
 *
 *  <i>mask</i> can be an array or a Mask object.
 */

VALUE
img_compass(VALUE obj, VALUE mask)
{
    INTMASK *imask;
	GetImg(obj, data, im);
	OutImg2(obj, mask, new, data_new, im_new);

    mask_arg2mask(mask, &imask, NULL);

    if (im_compass(im, im_new, imask))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.gradient(mask) -> image
 *
 *  *self* is convolved with <i>mask</i> and with <i>mask</i> after a 90 degree
 *  rotation. The result is the sum of the absolute value of the two
 *  convolutions.
 *
 *  <i>mask</i> can be an array or a Mask object.
 */

VALUE
img_gradient(VALUE obj, VALUE mask)
{
    INTMASK *imask;

    GetImg(obj, data, im);
	OutImg2(obj, mask, new, data_new, im_new);

    mask_arg2mask(mask, &imask, NULL);

    if (im_gradient(im, im_new, imask) )
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.lindetect(mask) -> image
 *
 *  *self* is convolved four times with @mask, each time @mask is rotated by 45
 *  degrees. Each output pixel is the largest absolute value of the four
 *  convolutions.
 *
 *  <i>mask</i> can be an array or a Mask object.
 */

VALUE
img_lindetect(VALUE obj, VALUE mask)
{
    INTMASK *imask;

    GetImg(obj, data, im);
	OutImg2(obj, mask, new, data_new, im_new);

    mask_arg2mask(mask, &imask, NULL);

    if (im_lindetect(im, im_new, imask))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.sharpen(mask_size, x1, y2, y3, m1, m2) -> image
 *
 *  Selectively sharpen the L channel of a LAB image. Works for :LABQ coding and
 *  LABS images.
 *
 *  The operation performs a gaussian blur of size <i>mask_size</i> and
 *  subtract from *self* to generate a high-frequency signal. This signal is
 *  passed through a lookup table formed from the five parameters and added back
 *  to *self*.
 *
 *
 *  For printing, we recommend the following settings:
 *
 *    mask_size == 7
 *    x1 == 1.5
 *    y2 == 20         (don't brighten by more than 20 L*)
 *    y3 == 50         (can darken by up to 50 L*)
 *
 *    m1 == 1          (some sharpening in flat areas)
 *    m2 == 2          (more sharpening in jaggy areas)
 *
 *  If you want more or less sharpening, we suggest you just change the
 *  <i>m1</i> and <i>m2</i> parameters.
 *
 *  The <i>mask_size</i> parameter changes the width of the fringe and can be
 *  adjusted according to the output printing resolution. As an approximate
 *  guideline, use 3 for 4 pixels/mm (CRT display resolution), 5 for 8
 *  pixels/mm, 7 for 12 pixels/mm and 9 for 16 pixels/mm (300 dpi == 12
 *  pixels/mm). These figures refer to the image raster, not the half-tone
 *  resolution.
 */

VALUE
img_sharpen(VALUE obj, VALUE mask_size, VALUE x1, VALUE y2, VALUE y3, VALUE m1,
    VALUE m2)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_sharpen(im, im_new, NUM2INT(mask_size), NUM2DBL(x1), NUM2DBL(y2),
		NUM2DBL(y3), NUM2DBL(m1), NUM2DBL(m2)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.grad_x -> image
 *
 *  Find horizontal differences between adjacent pixels.
 *
 *  Generates an image where the value of each pixel is the difference between
 *  it and the pixel to its right. The output has the same height as the input
 *  and one pixel less width. One-band integer formats only. The result is
 *  always band format :INT.
 *
 *  This operation is much faster than (though equivalent to) Image#conv with
 *  the mask [[-1, 1]].
 */

VALUE
img_grad_x(VALUE obj)
{
	RUBY_VIPS_UNARY(im_grad_x);
}

/*
 *  call-seq:
 *     im.grad_y -> image
 *
 *  Find vertical differences between adjacent pixels.
 *
 *  Generates an image where the value of each pixel is the difference between
 *  it and the pixel below it. The output has the same width as the input
 *  and one pixel less height. One-band integer formats only. The result is
 *  always band format :INT.
 *
 *  This operation is much faster than (though equivalent to) Image#conv with
 *  the mask [[-1], [1]].
 */

VALUE
img_grad_y(VALUE obj)
{
	RUBY_VIPS_UNARY(im_grad_y);
}

/*
 *  call-seq:
 *     im.fastcor(other_image) -> image
 *
 *  Calculate a fast correlation surface.
 *
 *  <i>other_image</i> is placed at every position in *self* and the sum of
 *  squares of differences calculated. One-band, 8-bit unsigned images only. The
 *  output image is always band format :UINT. <i>other_image</i> must be smaller
 *  than or equal to *self*. The output image is the same size as the input.
 */

VALUE
img_fastcor(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_fastcor);
}

/*
 *  call-seq:
 *     im.spcor(other_image) -> image
 *
 *  Calculate a correlation surface.
 *
 *  <i>other_image</i> is placed at every position in *self* and the correlation
 *  coefficient calculated. One-band, 8 or 16-bit images only. *self* and
 *  <i>other_image</i> must have the same band format.. The output image is
 *  always band format :FLOAT. <i>other_image</i> must be smaller than or equal
 *  to *self*. The output image is the same size as *self*.
 *
 *  The correlation coefficient is calculated as:
 *
 *            sumij (ref(i,j)-mean(ref))(inkl(i,j)-mean(inkl))
 *   c(k,l) = ------------------------------------------------
 *            sqrt(sumij (ref(i,j)-mean(ref))^2) *
 *                        sqrt(sumij (inkl(i,j)-mean(inkl))^2)
 *
 *  where inkl is the area of *self* centred at position (k,l).
 *
 *  from Niblack "An Introduction to Digital Image Processing", Prentice/Hall,
 *  pp 138.
 */

VALUE
img_spcor(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_spcor);
}

/*
 *  call-seq:
 *     im.gradcor(other_image) -> image
 *
 *  Calculate a correlation surface.
 *
 *  <i>other_image</i> is placed at every position in *self* and a correlation
 *  coefficient calculated. One-band, integer images only. *self* and
 *  <i>other_image</i> must have the same band format. The output image is
 *  always band format :FLOAT. <i>other_image</i> must be smaller than *self*.
 *  The output image is the same size as the input.
 *
 *  The method takes the gradient images of the two images then takes the
 *  dot-product correlation of the two vector images. The vector expression of
 *  this method is my (tcv) own creation. It is equivalent to the complex-number
 *  method of:
 *
 *  ARGYRIOU, V. et al. 2003. Estimation of sub-pixel motion using gradient
 *  cross correlation. Electronics Letters, 39 (13).
 */

VALUE
img_gradcor(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_gradcor);
}

/*
 *  call-seq:
 *    im.contrast_surface(half_win_size, spacing) -> image
 *
 *  Generate an image where the value of each pixel represents the contrast
 *  within a window of half_win_size from the corresponsing point in the input
 *  image. Sub-sample by a factor of spacing.
 */

VALUE
img_contrast_surface(VALUE obj, VALUE half_win_size, VALUE spacing)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_contrast_surface(im, im_new, NUM2INT(half_win_size),
		NUM2INT(spacing)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *    im.addgnoise(sigma) -> image
 *
 *  Add gaussian noise with mean 0 and variance sigma to *self*. The noise is
 *  generated by averaging 12 random numbers, see page 78, PIETGEN, 1989.
 */

VALUE
img_addgnoise(VALUE obj, VALUE sigma)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_addgnoise(im, im_new, NUM2INT(sigma)))
        vips_lib_error();

    return new;
}
