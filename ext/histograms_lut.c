#include "ruby_vips.h"
#include "image.h"
#include "mask.h"

static VALUE
img_histgr(int argc, VALUE *argv, VALUE obj)
{
	VALUE v_bandno;
	int bandno;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	rb_scan_args(argc, argv, "01", &v_bandno);
	bandno = NIL_P(v_bandno) ? -1 : NUM2INT(v_bandno);

    if (im_histgr(im, im_new, bandno))
        vips_lib_error();

    return new;  
}

static VALUE
img_histnd(VALUE obj, VALUE bins)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_histnD(im, im_new, NUM2INT(bins)))
        vips_lib_error();

    return new;  
}

static VALUE
img_hist_indexed(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_hist_indexed);
}

static VALUE
img_s_identity(VALUE obj, VALUE bands)
{
	OutPartial(new, data, im);

    if (im_identity(im, NUM2INT(bands)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_identity_ushort(VALUE obj, VALUE bands, VALUE sz)
{
	OutPartial(new, data, im);

    if (im_identity_ushort(im, NUM2INT(bands), NUM2INT(sz)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_invertlut(VALUE obj, VALUE input, VALUE lut_size)
{
    DOUBLEMASK *dmask;
	OutPartial(new, data, im);

    mask_arg2mask(input, NULL, &dmask);

    if (im_invertlut(dmask, im, NUM2INT(lut_size)))
        vips_lib_error();

    return new;
}


static VALUE
img_s_buildlut(VALUE obj, VALUE input)
{
    DOUBLEMASK *dmask;
	OutPartial(new, data, im);

    mask_arg2mask(input, NULL, &dmask);

    if (im_buildlut(dmask, im))
        vips_lib_error();

    return new;
}

static VALUE
img_project(VALUE obj)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);
	OutImg(obj, new2, data_new2, im_new2);

    if (im_project(im, im_new, im_new2))
        vips_lib_error();

    return rb_ary_new3(2, new, new2);
}

static VALUE
img_histnorm(VALUE obj)
{
	RUBY_VIPS_UNARY(im_histnorm);
}

static VALUE
img_histcum(VALUE obj)
{
	RUBY_VIPS_UNARY(im_histcum);
}

static VALUE
img_histeq(VALUE obj)
{
	RUBY_VIPS_UNARY(im_histeq);
}

static VALUE
img_histspec(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_histspec);
}

static VALUE
img_maplut(VALUE obj, VALUE obj2)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

	if (im_maplut(im, im_new, im2))
		vips_lib_error();

	return new;
}

static VALUE
img_histplot(VALUE obj)
{
	RUBY_VIPS_UNARY(im_histplot);
}

static VALUE
img_monotonic_p(VALUE obj)
{
    int ret;
	GetImg(obj, data, im);

	if( im_ismonotonic(im, &ret) )
	    vips_lib_error();

	return( ret == 0 ? Qtrue : Qfalse );
}

static VALUE
img_hist(int argc, VALUE *argv, VALUE obj)
{
	VALUE v_bandno;
	int bandno;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	rb_scan_args(argc, argv, "01", &v_bandno);
	bandno = NIL_P(v_bandno) ? -1 : NUM2INT(v_bandno);

    if (im_hist(im, im_new, bandno))
        vips_lib_error();

    return new;  
}

static VALUE
img_hsp(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_hsp);
}

static VALUE
img_gammacorrect(VALUE obj, VALUE exponent)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_gammacorrect(im, im_new, NUM2DBL(exponent)))
        vips_lib_error();

    return new;
}

#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 22

static VALUE
img_mpercent_hist(VALUE obj, VALUE percent)
{
    int ret;
	GetImg(obj, data, im);
    
    if (im_mpercent_hist(im, NUM2DBL(percent), &ret))
        vips_lib_error();

    return INT2NUM(ret);
}

#endif

static VALUE
img_mpercent(VALUE obj, VALUE percent)
{
    int ret;
	GetImg(obj, data, im);
    
    if (im_mpercent(im, NUM2DBL(percent), &ret))
        vips_lib_error();

    return INT2NUM(ret);
}

static VALUE
img_heq(int argc, VALUE *argv, VALUE obj)
{
	VALUE v_bandno;
	int bandno;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	rb_scan_args(argc, argv, "01", &v_bandno);
	bandno = NIL_P(v_bandno) ? -1 : NUM2INT(v_bandno);

    if (im_heq(im, im_new, bandno))
        vips_lib_error();

    return new;
}

static VALUE
img_lhisteq(VALUE obj, VALUE xwin, VALUE ywin)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_lhisteq(im, im_new, NUM2INT(xwin), NUM2INT(ywin)))
        vips_lib_error();

    return new;
}

static VALUE
img_stdif(VALUE obj,
	VALUE a, VALUE m0, VALUE b, VALUE s0, VALUE xwin, VALUE ywin)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_stdif(im, im_new,
		NUM2DBL(a), NUM2DBL(m0), NUM2DBL(b), NUM2DBL(s0),
		NUM2INT(xwin), NUM2INT(ywin)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_tone_build_range(VALUE obj,
	VALUE in_max, VALUE out_max,
	VALUE lb, VALUE lw, VALUE ps, VALUE pm, VALUE ph, VALUE s, VALUE m, VALUE h)
{
	OutPartial(new, data, im);

    if (im_tone_build_range(im,
		NUM2DBL(in_max), NUM2DBL(out_max),
		NUM2DBL(lb), NUM2DBL(lw), NUM2DBL(ps), NUM2DBL(pm), NUM2DBL(ph),
		NUM2DBL(s), NUM2DBL(m), NUM2DBL(h)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_tone_build(VALUE obj,
	VALUE lb, VALUE lw, VALUE ps, VALUE pm, VALUE ph, VALUE s, VALUE m, VALUE h)
{
	OutPartial(new, data, im);

    if (im_tone_build(im,
		NUM2DBL(lb), NUM2DBL(lw), NUM2DBL(ps), NUM2DBL(pm), NUM2DBL(ph),
		NUM2DBL(s), NUM2DBL(m), NUM2DBL(h)))
        vips_lib_error();

    return new;
}

#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 23
static VALUE
img_tone_analyze(VALUE obj,
	VALUE ps, VALUE pm, VALUE ph, VALUE s, VALUE m, VALUE h)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_tone_analyze(im, im_new,
		NUM2DBL(ps), NUM2DBL(pm), NUM2DBL(ph),
		NUM2DBL(s), NUM2DBL(m), NUM2DBL(h)) )
        vips_lib_error();

    return new;
}
#endif

void
init_histograms_lut(void)
{
    rb_define_method(cVIPSImage, "histgr", img_histgr, -1);
    rb_define_method(cVIPSImage, "histnd", img_histnd, 1);
    rb_define_method(cVIPSImage, "hist_indexed", img_hist_indexed, 1);
    rb_define_singleton_method(cVIPSImage, "identity", img_s_identity, 1);
    rb_define_singleton_method(cVIPSImage, "identity_ushort", img_s_identity_ushort, 2);
	rb_define_singleton_method(cVIPSImage, "invertlut", img_s_invertlut, 2);
	rb_define_singleton_method(cVIPSImage, "buildlut", img_s_buildlut, 1);
    rb_define_method(cVIPSImage, "project", img_project, 0);
    rb_define_method(cVIPSImage, "histnorm", img_histnorm, 0);
    rb_define_method(cVIPSImage, "histcum", img_histcum, 0);
    rb_define_method(cVIPSImage, "histeq", img_histeq, 0);
    rb_define_method(cVIPSImage, "histspec", img_histspec, 1);
    rb_define_method(cVIPSImage, "monotonic?", img_monotonic_p, 0);
    rb_define_method(cVIPSImage, "hist", img_hist, -1);
    rb_define_method(cVIPSImage, "hsp", img_hsp, 1);
    rb_define_method(cVIPSImage, "gammacorrect", img_gammacorrect, 1);

	#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 22
    rb_define_method(cVIPSImage, "mpercent_hist", img_mpercent_hist, 1);
	#endif

    rb_define_method(cVIPSImage, "mpercent", img_mpercent, 1);
    rb_define_method(cVIPSImage, "heq", img_heq, -1);
    rb_define_method(cVIPSImage, "lhisteq", img_lhisteq, 2);
    rb_define_method(cVIPSImage, "stdif", img_stdif, 6);
    rb_define_singleton_method(cVIPSImage, "tone_build_range", img_s_tone_build_range, 10);
    rb_define_singleton_method(cVIPSImage, "tone_build", img_s_tone_build, 8);
	#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 23
    rb_define_method(cVIPSImage, "tone_analyze", img_tone_analyze, 6);
	#endif
	rb_define_method(cVIPSImage, "maplut", img_maplut, 1);
	rb_define_method(cVIPSImage, "histplot", img_histplot, 0);
}

