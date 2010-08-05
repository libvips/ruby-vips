#include "ruby_vips.h"
#include "image.h"
#include "interpolator.h"

static VALUE
img_affinei(VALUE obj, VALUE interpolator, VALUE a, VALUE b, VALUE c, VALUE d,
	VALUE dx, VALUE dy, VALUE ox, VALUE oy, VALUE ow, VALUE oh)
{
    VALUE itrp_obj;
    VipsInterpolate *itrp_vips = interp_lookup(interpolator);
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_affinei(im, im_new, itrp_vips,
        NUM2DBL(a), NUM2DBL(b), NUM2DBL(c), NUM2DBL(d),
        NUM2DBL(dx), NUM2DBL(dy),
        NUM2INT(ox), NUM2INT(oy), NUM2INT(ow), NUM2INT(oh)) )
        vips_lib_error();

    return new;
}

static VALUE
img_affinei_all(VALUE obj, VALUE interpolator, VALUE a, VALUE b, VALUE c,
	VALUE d, VALUE dx, VALUE dy)
{
    VALUE itrp_obj;
    VipsInterpolate *itrp_vips = interp_lookup(interpolator);
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if( im_affinei_all(im, im_new, itrp_vips,
        NUM2DBL(a), NUM2DBL(b), NUM2DBL(c), NUM2DBL(d),
        NUM2DBL(dx), NUM2DBL(dy)) )
        vips_lib_error();

    return new;
}

static VALUE
img_stretch3(VALUE obj, VALUE dx, VALUE dy)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_stretch3(im, im_new, NUM2DBL(dx), NUM2DBL(dy)))
        vips_lib_error();

    return new;
}

static VALUE
img_shrink(VALUE obj, VALUE width_ratio, VALUE height_ratio)
{
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
		img_id_to_band_fmt(SYM2ID(fmt))))
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
    rb_define_method(cVIPSImage, "affinei", img_affinei, 11);
    rb_define_method(cVIPSImage, "affinei_all", img_affinei_all, 7);
    rb_define_method(cVIPSImage, "stretch3", img_stretch3, 2);
    rb_define_method(cVIPSImage, "shrink", img_shrink, 2);
    rb_define_method(cVIPSImage, "rightshift_size", img_rightshift_size, 3);
    rb_define_method(cVIPSImage, "match_linear", img_match_linear, 9);
    rb_define_method(cVIPSImage, "match_linear_search", img_match_linear_search, 11);
}

