#include "ruby_vips.h"
#include "image.h"

static VALUE
img_and_img(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_andimage);
}

static VALUE
img_and_const(VALUE obj, VALUE cnst)
{
	double c;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = NUM2DBL(cnst);
	if (im_andimage_vec(im, im_new, 1, &c))
		vips_lib_error();
	
	return new;
}

static VALUE
img_and_ary(int argc, VALUE *argv, VALUE obj)
{
	double *c;
	int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = IM_ARRAY(im_new, argc, double);

	for (i = 0; i < argc; i++)
		c[i] = NUM2DBL(argv[i]);

	if (im_andimage_vec(im, im_new, argc, c))
		vips_lib_error();
	
	return new;
}

static VALUE
img_and(VALUE obj, VALUE arg)
{
	switch( TYPE(arg) ) {
	case T_ARRAY:
		return img_and_ary(RARRAY_LEN(arg), RARRAY_PTR(arg), obj);
	case T_FLOAT:
	case T_FIXNUM:
		return img_and_const(obj, arg);
	default:
	    if( CLASS_OF(arg) == cVIPSImage )
			return img_and_img(obj, arg);
		else
			rb_raise(rb_eArgError, "Need an image, an array, or a number");
	}
}

static VALUE
img_or_img(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_orimage);
}

static VALUE
img_or_const(VALUE obj, VALUE cnst)
{
	double c;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = NUM2DBL(cnst);
	if (im_orimage_vec(im, im_new, 1, &c))
		vips_lib_error();
	
	return new;
}

static VALUE
img_or_ary(int argc, VALUE *argv, VALUE obj)
{
	double *c;
	int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = IM_ARRAY(im_new, argc, double);

	for (i = 0; i < argc; i++)
		c[i] = NUM2DBL(argv[i]);

	if (im_orimage_vec(im, im_new, argc, c))
		vips_lib_error();
	
	return new;
}

static VALUE
img_or(VALUE obj, VALUE arg)
{
	switch( TYPE(arg) ) {
	case T_ARRAY:
		return img_or_ary(RARRAY_LEN(arg), RARRAY_PTR(arg), obj);
	case T_FLOAT:
	case T_FIXNUM:
		return img_or_const(obj, arg);
	default:
	    if( CLASS_OF(arg) == cVIPSImage )
			return img_or_img(obj, arg);
		else
			rb_raise(rb_eArgError, "Need an image, an array, or a number");
	}
}

static VALUE
img_xor_img(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_eorimage);
}

static VALUE
img_xor_const(VALUE obj, VALUE cnst)
{
	double c;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = NUM2DBL(cnst);
	if (im_eorimage_vec(im, im_new, 1, &c))
		vips_lib_error();
	
	return new;
}

static VALUE
img_xor_ary(int argc, VALUE *argv, VALUE obj)
{
	double *c;
	int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = IM_ARRAY(im_new, argc, double);

	for (i = 0; i < argc; i++)
		c[i] = NUM2DBL(argv[i]);

	if (im_eorimage_vec(im, im_new, argc, c))
		vips_lib_error();
	
	return new;
}

static VALUE
img_xor(VALUE obj, VALUE arg)
{
	switch( TYPE(arg) ) {
	case T_ARRAY:
		return img_xor_ary(RARRAY_LEN(arg), RARRAY_PTR(arg), obj);
	case T_FLOAT:
	case T_FIXNUM:
		return img_xor_const(obj, arg);
	default:
	    if( CLASS_OF(arg) == cVIPSImage )
			return img_xor_img(obj, arg);
		else
			rb_raise(rb_eArgError, "Need an image, an array, or a number");
	}
}

static VALUE
img_shiftleft(int argc, VALUE *argv, VALUE obj)
{
    double *c;
    int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    c = IM_ARRAY(im, argc, double);

    for(i = 0; i < argc; i++)
        c[i] = NUM2DBL(argv[i]);

    if( im_shiftleft_vec(im, im_new, argc, c) )
        vips_lib_error();

    return new;
}

static VALUE
img_shiftright(int argc, VALUE *argv, VALUE obj)
{
    double *c;
    int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    c = IM_ARRAY(im, argc, double);

    for(i = 0; i < argc; i++)
        c[i] = NUM2DBL(argv[i]);

    if( im_shiftright_vec(im, im_new, argc, c) )
        vips_lib_error();

    return new;
}

void
init_boolean(void)
{
    rb_define_method(cVIPSImage, "&", img_and, 1);
    rb_define_method(cVIPSImage, "|", img_or, 1);
    rb_define_method(cVIPSImage, "^", img_xor, 1);
    rb_define_method(cVIPSImage, "<<", img_shiftleft, -1);
    rb_define_method(cVIPSImage, ">>", img_shiftright, -1);
}

