#include "ruby_vips.h"
#include "image.h"

ID id_match_left, id_match_right, id_match_both;

static VALUE
img_lrmerge(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, dx, dy, mwidth_v;
    int mwidth = -1;

    rb_scan_args(argc, argv, "31", &obj2, &dx, &dy, &mwidth_v);
    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_lrmerge(im, im2, im_new, NUM2INT(dx), NUM2INT(dy), mwidth))
        vips_lib_error();

    return new;
}

static VALUE
img_tbmerge(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, dx, dy, mwidth_v;
    int mwidth = -1;

    rb_scan_args(argc, argv, "31", &obj2, &dx, &dy, &mwidth_v);
    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

    GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_tbmerge(im, im2, im_new, NUM2INT(dx), NUM2INT(dy), mwidth))
        vips_lib_error();

    return new;
}

static VALUE
img_lrmerge1(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, mwidth_v;
    int mwidth = -1;

    rb_scan_args(argc, argv, "91", &obj2, &xr1, &yr1, &xs1, &ys1, &xr2, &yr2,
        &xs2, &ys2, &mwidth_v);
    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_lrmerge1(im, im2, im_new, NUM2INT(xr1), NUM2INT(yr1), NUM2INT(xs1),
		NUM2INT(ys1), NUM2INT(xr2), NUM2INT(yr2), NUM2INT(xs2),	NUM2INT(ys2),
		mwidth))
        vips_lib_error();

    return new;
}

static VALUE
img_tbmerge1(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, mwidth_v;
    int mwidth = -1;

    rb_scan_args(argc, argv, "91", &obj2, &xr1, &yr1, &xs1, &ys1, &xr2, &yr2,
        &xs2, &ys2, &mwidth_v);
    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_tbmerge1(im, im2, im_new, NUM2INT(xr1), NUM2INT(yr1), NUM2INT(xs1),
		NUM2INT(ys1), NUM2INT(xr2), NUM2INT(yr2), NUM2INT(xs2), NUM2INT(ys2),
		mwidth))
        vips_lib_error();

    return new;
}

static VALUE
img_lrmosaic(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, bandno, xref, yref, xsec, ysec, halfcorrelation_v, halfarea_v,
        balancetype_v, mwidth_v;
    ID balancetype_id;
    int mwidth = -1, halfcorrelation = 5, halfarea = 14, balancetype = 0;

    rb_scan_args(argc, argv, "64", &obj2, &bandno, &xref, &yref, &xsec, &ysec,
        &halfcorrelation_v, &halfarea_v, &balancetype_v, &mwidth_v);

    if (!NIL_P(halfcorrelation_v))
        halfcorrelation = NUM2INT(halfcorrelation_v);

    if (!NIL_P(halfarea_v))
        halfarea = NUM2INT(halfarea_v);

    if (!NIL_P(balancetype_v)) {
        balancetype_id = SYM2ID(balancetype_v);

        if (balancetype_id == id_match_left) balancetype = 1;
        else if (balancetype_id == id_match_right) balancetype = 2;
        else if (balancetype_id == id_match_both) balancetype = 3;
        else
            rb_raise(rb_eArgError, "Balance type must be nil, :match_left, :match_right, or :match_both");
    }

    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

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
img_tbmosaic(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, bandno, xref, yref, xsec, ysec, halfcorrelation_v, halfarea_v,
        balancetype_v, mwidth_v;
    ID balancetype_id;
    int mwidth = -1, halfcorrelation = 5, halfarea = 14, balancetype = 0;

    rb_scan_args(argc, argv, "64", &obj2, &bandno, &xref, &yref, &xsec, &ysec,
        &halfcorrelation_v, &halfarea_v, &balancetype_v, &mwidth_v);

    if (!NIL_P(halfcorrelation_v))
        halfcorrelation = NUM2INT(halfcorrelation_v);

    if (!NIL_P(halfarea_v))
        halfarea = NUM2INT(halfarea_v);

    if (!NIL_P(balancetype_v)) {
        balancetype_id = SYM2ID(balancetype_v);

        if (balancetype_id == id_match_left) balancetype = 1;
        else if (balancetype_id == id_match_right) balancetype = 2;
        else if (balancetype_id == id_match_both) balancetype = 3;
        else
            rb_raise(rb_eArgError, "Balance type must be nil, :match_left, :match_right, or :match_both");
    }

    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

    GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_tbmosaic(im, im2, im_new, NUM2INT(bandno), NUM2INT(xref),
        NUM2INT(yref), NUM2INT(xsec), NUM2INT(ysec), halfcorrelation, halfarea,
        balancetype, mwidth))
        vips_lib_error();

    return new;
}

static VALUE
img_lrmosaic1(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, bandno, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2,
        halfcorrelation_v, halfarea_v, balancetype_v, mwidth_v;
    ID balancetype_id;
    int mwidth = -1, halfcorrelation = 5, halfarea = 14, balancetype = 0;

    rb_scan_args(argc, argv, "95", &obj2, &bandno, &xr1, &yr1, &xs1, &ys1, &xr2,
        &yr2, &xs2, &ys2, &halfcorrelation_v, &halfarea_v, &balancetype_v,
        &mwidth_v);
    
    if (argc < 10)
        rb_raise(rb_eArgError, "Need at least 10 arguments.");

    if (!NIL_P(halfcorrelation_v))
        halfcorrelation = NUM2INT(halfcorrelation_v);

    if (!NIL_P(halfarea_v))
        halfarea = NUM2INT(halfarea_v);

    if (!NIL_P(balancetype_v)) {
        balancetype_id = SYM2ID(balancetype_v);

        if (balancetype_id == id_match_left) balancetype = 1;
        else if (balancetype_id == id_match_right) balancetype = 2;
        else if (balancetype_id == id_match_both) balancetype = 3;
        else
            rb_raise(rb_eArgError, "Balance type must be nil, :match_left, :match_right, or :match_both");
    }

    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_lrmosaic1(im, im2, im_new, NUM2INT(bandno), NUM2INT(xr1),
		NUM2INT(yr1), NUM2INT(xs1), NUM2INT(ys1), NUM2INT(xr2), NUM2INT(yr2),
		NUM2INT(xs2), NUM2INT(ys2), halfcorrelation, halfarea, balancetype,
        mwidth))
        vips_lib_error();

    return new;
}

static VALUE
img_tbmosaic1(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, bandno, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2,
        halfcorrelation_v, halfarea_v, balancetype_v, mwidth_v;
    ID balancetype_id;
    int mwidth = -1, halfcorrelation = 5, halfarea = 14, balancetype = 0;

    rb_scan_args(argc, argv, "95", &obj2, &bandno, &xr1, &yr1, &xs1, &ys1, &xr2,
        &yr2, &xs2, &ys2, &halfcorrelation_v, &halfarea_v, &balancetype_v,
        &mwidth_v);

    if (argc < 10)
        rb_raise(rb_eArgError, "Need at least 10 arguments.");

    if (!NIL_P(halfcorrelation_v))
        halfcorrelation = NUM2INT(halfcorrelation_v);

    if (!NIL_P(halfarea_v))
        halfarea = NUM2INT(halfarea_v);

    if (!NIL_P(balancetype_v)) {
        balancetype_id = SYM2ID(balancetype_v);

        if (balancetype_id == id_match_left) balancetype = 1;
        else if (balancetype_id == id_match_right) balancetype = 2;
        else if (balancetype_id == id_match_both) balancetype = 3;
        else
            rb_raise(rb_eArgError, "Balance type must be nil, :match_left, :match_right, or :match_both");
    }

    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_tbmosaic1(im, im2, im_new, NUM2INT(bandno), NUM2INT(xr1),
		NUM2INT(yr1), NUM2INT(xs1), NUM2INT(ys1), NUM2INT(xr2), NUM2INT(yr2),
		NUM2INT(xs2), NUM2INT(ys2), halfcorrelation, halfarea, balancetype,
        mwidth))
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
    rb_define_method(cVIPSImage, "lrmerge", img_lrmerge, -1);
    rb_define_method(cVIPSImage, "tbmerge", img_tbmerge, -1);
    rb_define_method(cVIPSImage, "lrmerge1", img_lrmerge1, -1);
    rb_define_method(cVIPSImage, "tbmerge1", img_tbmerge1, -1);
    rb_define_method(cVIPSImage, "lrmosaic", img_lrmosaic, -1);
    rb_define_method(cVIPSImage, "tbmosaic", img_tbmosaic, -1);
    rb_define_method(cVIPSImage, "lrmosaic1", img_lrmosaic1, -1);
    rb_define_method(cVIPSImage, "tbmosaic1", img_tbmosaic1, -1);
    rb_define_method(cVIPSImage, "global_balance", img_global_balance, 1);
    rb_define_method(cVIPSImage, "global_balancef", img_global_balancef, 1);
    rb_define_method(cVIPSImage, "correl", img_correl, 7);
    rb_define_method(cVIPSImage, "remosaic", img_remosaic, 2);
	rb_define_method(cVIPSImage, "align_bands", img_align_bands, 0);
	rb_define_method(cVIPSImage, "maxpos_subpel", img_maxpos_subpel, 0);

    id_match_left = rb_intern("match_left");
    id_match_right = rb_intern("match_right");
    id_match_both = rb_intern("match_both");
}
