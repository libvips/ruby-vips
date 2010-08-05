#include "ruby_vips.h"
#include "image.h"
#include "mask.h"

static VALUE
img_conv(VALUE obj, VALUE m)
{
    vipsMask *msk;
	GetImg(obj, data, im);
	OutImg2(obj, m, new, data_new, im_new);

    Data_Get_Struct(m, vipsMask, msk);

    if (msk->imask) {
        if (im_conv(im, im_new, msk->imask))
            vips_lib_error();
    } else if (im_conv_f(im, im_new, msk->dmask))
        vips_lib_error();

    return new;
}   

static VALUE
img_convsep(VALUE obj, VALUE mask)
{
    vipsMask *msk;
	GetImg(obj, data, im);
	OutImg2(obj, mask, new, data_new, im_new);

    Data_Get_Struct(mask, vipsMask, msk);

    if(msk->imask) {
        if (im_convsep(im, im_new, msk->imask))
            vips_lib_error();
    } else if (im_convsep_f(im, im_new, msk->dmask))
        vips_lib_error();

    return new;
}

static VALUE
img_compass(VALUE obj, VALUE mask)
{
    vipsMask *msk;
	GetImg(obj, data, im);
	OutImg2(obj, mask, new, data_new, im_new);

    Data_Get_Struct(mask, vipsMask, msk);

    if (im_compass(im, im_new, msk->imask))
        vips_lib_error();

    return new;
}

static VALUE
img_gradient(VALUE obj, VALUE mask)
{
    vipsMask *msk;
	GetImg(obj, data, im);
	OutImg2(obj, mask, new, data_new, im_new);

    Data_Get_Struct(mask, vipsMask, msk);

    if (im_gradient(im, im_new, msk->imask) )
        vips_lib_error();

    return new;
}

static VALUE
img_lindetect(VALUE obj, VALUE mask)
{
    vipsMask *msk;
	GetImg(obj, data, im);
	OutImg2(obj, mask, new, data_new, im_new);

    Data_Get_Struct(mask, vipsMask, msk);

    if (im_lindetect(im, im_new, msk->imask))
        vips_lib_error();

    return new;
}

static VALUE
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

static VALUE
img_grad_x(VALUE obj)
{
	RUBY_VIPS_UNARY(im_grad_x);
}

static VALUE
img_grad_y(VALUE obj)
{
	RUBY_VIPS_UNARY(im_grad_y);
}

static VALUE
img_fastcor(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_fastcor);
}

static VALUE
img_spcor(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_spcor);
}

static VALUE
img_gradcor(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_gradcor);
}

static VALUE
img_contrast_surface(VALUE obj, VALUE half_win_size, VALUE spacing)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_contrast_surface(im, im_new, NUM2INT(half_win_size),
		NUM2INT(spacing)))
        vips_lib_error();

    return new;
}

static VALUE
img_addgnoise(VALUE obj, VALUE sigma)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_addgnoise(im, im_new, NUM2INT(sigma)))
        vips_lib_error();

    return new;
}

void
init_convolution()
{
    rb_define_method(cVIPSImage, "conv", img_conv, 1);
    rb_define_method(cVIPSImage, "convsep", img_convsep, 1);
    rb_define_method(cVIPSImage, "compass", img_compass, 1);
    rb_define_method(cVIPSImage, "gradient", img_gradient, 1);
    rb_define_method(cVIPSImage, "lindetect", img_lindetect, 1);
    rb_define_method(cVIPSImage, "sharpen", img_sharpen, 6);
    rb_define_method(cVIPSImage, "grad_x", img_grad_x, 0);
    rb_define_method(cVIPSImage, "grad_y", img_grad_y, 0);
    rb_define_method(cVIPSImage, "fastcor", img_fastcor, 1);
    rb_define_method(cVIPSImage, "spcor", img_spcor, 1);
    rb_define_method(cVIPSImage, "gradcor", img_gradcor, 1);
    rb_define_method(cVIPSImage, "contrast_surface", img_contrast_surface, 2);
    rb_define_method(cVIPSImage, "addgnoise", img_addgnoise, 1);
}

