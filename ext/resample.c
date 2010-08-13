#include "ruby_vips.h"
#include "image.h"
#include "interpolator.h"
#include "header.h"

static VALUE
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

static VALUE
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

static VALUE
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

static VALUE
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

static VALUE
img_rightshift_size(VALUE obj, VALUE xshift, VALUE yshift, VALUE fmt)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if(im_rightshift_size(im, im_new, NUM2DBL(xshift), NUM2DBL(yshift),
		header_id_to_band_fmt(SYM2ID(fmt))))
        vips_lib_error();

    return new;
}

static VALUE
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

static VALUE
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

void
init_resample()
{
    rb_define_method(cVIPSImage, "affinei", img_affinei, -1);
    rb_define_method(cVIPSImage, "affinei_resize", img_affinei_resize, -1);
    rb_define_method(cVIPSImage, "stretch3", img_stretch3, -1);
    rb_define_method(cVIPSImage, "shrink", img_shrink, -1);
    rb_define_method(cVIPSImage, "rightshift_size", img_rightshift_size, 3);
    rb_define_method(cVIPSImage, "match_linear", img_match_linear, 9);
    rb_define_method(cVIPSImage, "match_linear_search", img_match_linear_search, 11);
}

