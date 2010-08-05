#include "ruby_vips.h"
#include "image.h"

static VALUE
img_lrmerge(VALUE obj, VALUE obj2, VALUE dx, VALUE dy, VALUE mwidth)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_lrmerge(im, im2, im_new, NUM2INT(dx), NUM2INT(dy), NUM2INT(mwidth)))
        vips_lib_error();

    return new;
}

static VALUE
img_tbmerge(VALUE obj, VALUE obj2, VALUE dx, VALUE dy, VALUE mwidth)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_tbmerge(im, im2, im_new, NUM2INT(dx), NUM2INT(dy), NUM2INT(mwidth)))
        vips_lib_error();

    return new;
}

static VALUE
img_lrmerge1(VALUE obj, VALUE obj2, VALUE xr1, VALUE yr1, VALUE xs1, VALUE ys1,
	VALUE xr2, VALUE yr2, VALUE xs2, VALUE ys2, VALUE mwidth)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_lrmerge1(im, im2, im_new, NUM2INT(xr1), NUM2INT(yr1), NUM2INT(xs1),
		NUM2INT(ys1), NUM2INT(xr2), NUM2INT(yr2), NUM2INT(xs2),	NUM2INT(ys2),
		NUM2INT(mwidth)))
        vips_lib_error();

    return new;
}

static VALUE
img_tbmerge1(VALUE obj, VALUE obj2, VALUE xr1, VALUE yr1, VALUE xs1, VALUE ys1,
	VALUE xr2, VALUE yr2, VALUE xs2, VALUE ys2, VALUE mwidth)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_tbmerge1(im, im2, im_new, NUM2INT(xr1), NUM2INT(yr1), NUM2INT(xs1),
		NUM2INT(ys1), NUM2INT(xr2), NUM2INT(yr2), NUM2INT(xs2), NUM2INT(ys2),
		NUM2INT(mwidth)))
        vips_lib_error();

    return new;
}

static VALUE
img_lrmosaic(VALUE obj, VALUE obj2, VALUE bandno, VALUE xref, VALUE yref,
	VALUE xsec, VALUE ysec, VALUE halfcorrelation, VALUE halfarea,
	VALUE balancetype, VALUE mwidth)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_lrmosaic(im, im2, im_new, NUM2INT(bandno), NUM2INT(xref),
		NUM2INT(yref), NUM2INT(xsec), NUM2INT(ysec), NUM2INT(halfcorrelation),
		NUM2INT(halfarea), NUM2INT(balancetype), NUM2INT(mwidth)))
        vips_lib_error();

    return new;
}

static VALUE
img_tbmosaic(VALUE obj, VALUE obj2, VALUE bandno, VALUE xref, VALUE yref,
	VALUE xsec, VALUE ysec, VALUE halfcorrelation, VALUE halfarea,
	VALUE balancetype, VALUE mwidth)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_tbmosaic(im, im2, im_new, NUM2INT(bandno), NUM2INT(xref),
		NUM2INT(yref), NUM2INT(xsec), NUM2INT(ysec), NUM2INT(halfcorrelation),
		NUM2INT(halfarea), NUM2INT(balancetype), NUM2INT(mwidth)))
        vips_lib_error();

    return new;
}

static VALUE
img_lrmosaic1(VALUE obj, VALUE obj2, VALUE bandno, VALUE xr1, VALUE yr1,
	VALUE xs1, VALUE ys1, VALUE xr2, VALUE yr2, VALUE xs2, VALUE ys2,
	VALUE halfcorrelation, VALUE halfarea, VALUE balancetype, VALUE mwidth)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_lrmosaic1(im, im2, im_new, NUM2INT(bandno), NUM2INT(xr1),
		NUM2INT(yr1), NUM2INT(xs1), NUM2INT(ys1), NUM2INT(xr2), NUM2INT(yr2),
		NUM2INT(xs2), NUM2INT(ys2), NUM2INT(halfcorrelation), NUM2INT(halfarea),
		NUM2INT(balancetype), NUM2INT(mwidth)) )
        vips_lib_error();

    return new;
}

static VALUE
img_tbmosaic1(VALUE obj, VALUE obj2, VALUE bandno, VALUE xr1, VALUE yr1,
	VALUE xs1, VALUE ys1, VALUE xr2, VALUE yr2, VALUE xs2, VALUE ys2,
	VALUE halfcorrelation, VALUE halfarea, VALUE balancetype, VALUE mwidth)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_tbmosaic1(im, im2, im_new, NUM2INT(bandno), NUM2INT(xr1),
		NUM2INT(yr1), NUM2INT(xs1), NUM2INT(ys1), NUM2INT(xr2), NUM2INT(yr2),
		NUM2INT(xs2), NUM2INT(ys2), NUM2INT(halfcorrelation), NUM2INT(halfarea),
		NUM2INT(balancetype), NUM2INT(mwidth)) )
        vips_lib_error();

    return new;
}

static VALUE
img_global_balance(VALUE obj, VALUE gamma)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_global_balance(im, im_new, NUM2DBL(gamma)))
        vips_lib_error();

    return new;
}

static VALUE
img_global_balancef(VALUE obj, VALUE gamma)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_global_balancef(im, im_new, NUM2DBL(gamma)))
        vips_lib_error();

    return new;
}

static VALUE
img_correl(VALUE obj, VALUE obj2, VALUE xref, VALUE yref, VALUE xsec,
	VALUE ysec, VALUE hwindowsize, VALUE hsearchsize)
{
    int x, y;
	double correlation;
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);

    if (im_correl(im, im2, NUM2INT(xref), NUM2INT(yref), NUM2INT(xsec),
		NUM2INT(ysec), NUM2INT(hwindowsize), NUM2INT(hsearchsize), &correlation,
		&x, &y))
        vips_lib_error();

    return rb_ary_new3(3, DBL2NUM(correlation), INT2NUM(x), INT2NUM(y));
}

static VALUE
img_remosaic(VALUE obj, VALUE old_str, VALUE new_str)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	img_add_dep(data, old_str);
	img_add_dep(data, new_str);

    if (im_remosaic(im, im_new, StringValuePtr(old_str),
		StringValuePtr(new_str)))
        vips_lib_error();

    return new;
}

static VALUE
img_align_bands(VALUE obj)
{
	RUBY_VIPS_UNARY(im_align_bands);
}

static VALUE
img_maxpos_subpel(VALUE obj)
{
    double x, y;
	GetImg(obj, data, im);

    if (im_maxpos_subpel(im, &x, &y))
        vips_lib_error();

    return rb_ary_new3(2, DBL2NUM(x), INT2NUM(y));
}

void
init_mosaicing(void)
{
    rb_define_method(cVIPSImage, "lrmerge", img_lrmerge, 4);
    rb_define_method(cVIPSImage, "tbmerge", img_tbmerge, 4);
    rb_define_method(cVIPSImage, "lrmerge1", img_lrmerge1, 10);
    rb_define_method(cVIPSImage, "tbmerge1", img_tbmerge1, 10);
    rb_define_method(cVIPSImage, "lrmosaic", img_lrmosaic, 10);
    rb_define_method(cVIPSImage, "tbmosaic", img_tbmosaic, 10);
    rb_define_method(cVIPSImage, "lrmosaic1", img_lrmosaic1, 14);
    rb_define_method(cVIPSImage, "tbmosaic1", img_tbmosaic1, 14);
    rb_define_method(cVIPSImage, "global_balance", img_global_balance, 1);
    rb_define_method(cVIPSImage, "global_balancef", img_global_balancef, 1);
    rb_define_method(cVIPSImage, "correl", img_correl, 7);
    rb_define_method(cVIPSImage, "remosaic", img_remosaic, 2);
	rb_define_method(cVIPSImage, "align_bands", img_align_bands, 0);
	rb_define_method(cVIPSImage, "maxpos_subpel", img_maxpos_subpel, 0);
}

