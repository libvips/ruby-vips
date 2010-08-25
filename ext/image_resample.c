#include "ruby_vips.h"
#include "image.h"
#include "interpolator.h"
#include "header.h"
#include "image_resample.h"

/*
 *  call-seq:
 *     im.affinei(interpolator, a, b, c, d, dx, dy)                 -> image
 *     im.affinei(interpolator, a, b, c, d, dx, dy, ox, oy, ow, oh) -> image
 *
 *  Applies an affine transformation on *self*. *self* many have any number of
 *  bands, be any size, and have any non-complex type.
 *
 *  The transformation is described by <i>a</i>, <i>b</i>, <i>c</i>, <i>d</i>,
 *  <i>dx</i>, <i>dy</i>. The point (x,y) in
 *  the input is mapped onto point (X,Y) in the output by
 *
 *    X = a * x + b * y + dx
 *    Y = c * x + d * y + dy
 *
 *  In the first form, the entire image is transformed. In the second form, the
 *  area of the output image given by <i>ow</i>, <i>oh</i>, <i>ox</i>, <i>oy</i>
 *  is generated. (0,0) is the position of the transformed top-left-hand corner
 *  of the input image.
 *
 *  Points are generated using the supplied interpolator.
 */

VALUE
img_affinei(int argc, VALUE *argv, VALUE obj)
{
    VALUE interpolator, a, b, c, d, dx, dy, ox, oy, ow, oh;
    VipsInterpolate *itrp_vips;

    rb_scan_args(argc, argv, "74", &interpolator, &a, &b, &c, &d, &dx, &dy, &ox,
        &oy, &ow, &oh);

    itrp_vips = interp_lookup(interpolator);
    GetImg(obj, data, im);
    OutImg(obj, new, data_new, im_new);

    if (NIL_P(ox)) {
        if (im_affinei_all(im, im_new, itrp_vips,
            NUM2DBL(a), NUM2DBL(b), NUM2DBL(c), NUM2DBL(d),
            NUM2DBL(dx), NUM2DBL(dy)))
            vips_lib_error();
    } else {
        if (NIL_P(oh))
            rb_raise(rb_eArgError, "If you supply any output parameters, you have to supply them all");

        if (im_affinei(im, im_new, itrp_vips,
            NUM2DBL(a), NUM2DBL(b), NUM2DBL(c), NUM2DBL(d),
            NUM2DBL(dx), NUM2DBL(dy),
            NUM2INT(ox), NUM2INT(oy), NUM2INT(ow), NUM2INT(oh)))
        vips_lib_error();
    }

    return new;
}

/*
 *  call-seq:
 *     im.affinei_resize(interpolator, x [,y]) -> image
 *
 *  Applies an affine resize using the supplied interpolator. If <i>y</i> is
 *  ommitted then the aspect ratio of *self* will be maintained.
 */

VALUE
img_affinei_resize(int argc, VALUE *argv, VALUE obj)
{
    VALUE interpolator, x_scale, y_scale;
    VipsInterpolate *itrp_vips;

    rb_scan_args(argc, argv, "21", &interpolator, &x_scale, &y_scale);

    GetImg(obj, data, im);
    OutImg(obj, new, data_new, im_new);

    itrp_vips = interp_lookup(interpolator);

    if (NIL_P(y_scale))
        y_scale = x_scale;

    if (im_affinei_all(im, im_new, itrp_vips, NUM2DBL(x_scale), 0, 0,
        NUM2DBL(y_scale), 0, 0))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.stretch3(dx [,dy]) -> image
 *
 *  Stretches the input image by 3% horizontally, and displaces it by
 *  <i>dx</i>/<i>dy</i>. It uses bi-cubic interpolation, but runs quickly. It
 *  works only for unsigned short images.
 *
 *  This function is part of the MARC acquisition software, but is generally
 *  useful for squaring up the pixels in images from the Kontron ProgRes camera
 *  range.
 */

VALUE
img_stretch3(int argc, VALUE *argv, VALUE obj)
{
    VALUE dx, dy;

    rb_scan_args(argc, argv, "11", &dx, &dy);
    if (NIL_P(dy))
        dy = dx;

	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_stretch3(im, im_new, NUM2DBL(dx), NUM2DBL(dy)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.shrink(width_ratio [,height_ratio]) -> image
 *
 *  Shrink *self* by <i>width_ratio</i> along the horizontal and
 *  <i>height_ratio</i> along the vertical direction. If <i>height_ratio</i> is
 *  not given, it uses <i>width_ratio</i>. The function does not perform
 *  subpixel interpolation and therefore the resultant image can present
 *  aliasing especially for small x and y factors. Any size image, any
 *  non-complex type, any number of bands.
 */

VALUE
img_shrink(int argc, VALUE *argv, VALUE obj)
{
    VALUE width_ratio, height_ratio;

    rb_scan_args(argc, argv, "11", &width_ratio, &height_ratio);
    if (NIL_P(height_ratio))
        height_ratio = width_ratio;

    GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_shrink(im, im_new, NUM2DBL(width_ratio), NUM2DBL(height_ratio)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.rightshift_size(xshift, yshift, fmt) -> image
 *
 *  Decreases the size of an integer type image by a power-of-two factor, very
 *  quickly, by summing the values of adjacent pixels. The sum is shifted to
 *  produce output of the specified band format.
 *
 *  <i>xshift</i> and <i>yshift</i> are positive and give the base-two
 *  logarithms of the scale factors.
 *
 *  If the input image is a signed type, then the band format must be one of
 *  :CHAR, :SHORT or :INT.  If it is an unsigned type, then the band format must
 *  be one of :UCHAR, :USHORT or :UINT.
 */

VALUE
img_rightshift_size(VALUE obj, VALUE xshift, VALUE yshift, VALUE fmt)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if(im_rightshift_size(im, im_new, NUM2DBL(xshift), NUM2DBL(yshift),
		header_id_to_band_fmt(SYM2ID(fmt))))
        vips_lib_error();

    return new;
}


/*
 *  call-seq:
 *     im.match_linear(other_image, xr1, yr1, xs1, ys1, xr2, yr2, xs2,
 *       ys2) -> image
 *
 *  Works exactly as Image#match_linear_search, but does not attempt to
 *  correlate to correct your tie points. It can thus be used for any angle and
 *  any scale, but you must be far more careful in your selection.
 */

VALUE
img_match_linear(VALUE obj, VALUE obj2,
    VALUE xr1, VALUE yr1, VALUE xs1, VALUE ys1,
    VALUE xr2, VALUE yr2, VALUE xs2, VALUE ys2)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg(obj, new, data_new, im_new);

    if (im_match_linear(im, im2, im_new,
        NUM2INT(xr1), NUM2INT(yr1), NUM2INT(xs1), NUM2INT(ys1),
        NUM2INT(xr2), NUM2INT(yr2), NUM2INT(xs2), NUM2INT(ys2)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.match_linear_search(other_image, xr1, yr1, xs1, ys1, xr2, yr2, xs2,
 *       ys2, hwindowsize, hsearchsize) -> image
 *
 *  Attempts to transform sec to make it match ref. The transformation is
 *  linear, that is, it only involves scale, rotate and translate.
 *
 *  Requires a pair of tie points to fix the parameters of its transformation.
 *  You should pick points as far apart as possible to increase accuracy. It
 *  will search the area in the image around each tie point for a good fit, so
 *  your selection of points need not be exact. WARNING! This searching process
 *  will fail for rotations of more than about 10 degrees or for scales of more
 *  than about 10 percent.
 *
 *  The best you can hope for is < 1 pixel error, since the command does not
 *  attempt sub-pixel correlation.
 *
 *  <i>hwindowsize</i> and <i>hsearchsize</i> set the size of the area to be
 *  searched: we recommend values of 5 and 14.
 *
 *  The output image is positioned and clipped so that you can immediately
 *  subtract it from orig to obtain pixel difference images.
 */

VALUE
img_match_linear_search(VALUE obj, VALUE obj2,
    VALUE xr1, VALUE yr1, VALUE xs1, VALUE ys1,
    VALUE xr2, VALUE yr2, VALUE xs2, VALUE ys2,
    VALUE hwindowsize, VALUE hsearchsize)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg(obj, new, data_new, im_new);

    if (im_match_linear_search(im, im2, im_new,
        NUM2INT(xr1), NUM2INT(yr1), NUM2INT(xs1), NUM2INT(ys1),
        NUM2INT(xr2), NUM2INT(yr2), NUM2INT(xs2), NUM2INT(ys2),
        NUM2INT(hwindowsize), NUM2INT(hsearchsize)))
        vips_lib_error();

    return new;
}
