#include "ruby_vips.h"
#include "image.h"

static VALUE
img_equal_const(int argc, VALUE *argv, VALUE obj)
{
    double *c;
    int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = IM_ARRAY(im_new, argc, double);

	for(i = 0; i < argc; i++)
		c[i] = NUM2DBL(argv[i]);

	if (im_equal_vec(im, im_new, argc, c))
		vips_lib_error();

	return new;
}

static VALUE
img_equal_img(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_equal);
}

static VALUE
img_equal(int argc, VALUE *argv, VALUE obj)
{
	if (argc < 1)
		rb_raise(rb_eArgError, "Need at least one argument.");

	if (TYPE(argv[0]) == T_FLOAT || TYPE(argv[0]) == T_FIXNUM)
		return img_equal_const(argc, argv, obj);
	else if (argc != 1)
		rb_raise(rb_eArgError, "Need constants or one image.");

	return img_equal_img(obj, argv[0]);
}

static VALUE
img_notequal_const(int argc, VALUE *argv, VALUE obj)
{
    double *c;
    int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = IM_ARRAY(im_new, argc, double);

	for(i = 0; i < argc; i++)
		c[i] = NUM2DBL(argv[i]);

	if (im_notequal_vec(im, im_new, argc, c))
		vips_lib_error();

	return new;
}

static VALUE
img_notequal_img(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_notequal);
}

static VALUE
img_notequal(int argc, VALUE *argv, VALUE obj)
{
	if (argc < 1)
		rb_raise(rb_eArgError, "Need at least one argument.");

	if (TYPE(argv[0]) == T_FLOAT || TYPE(argv[0]) == T_FIXNUM)
		return img_notequal_const(argc, argv, obj);
	else if (argc != 1)
		rb_raise(rb_eArgError, "Need constants or one image.");

	return img_notequal_img(obj, argv[0]);
}

static VALUE
img_less_const(int argc, VALUE *argv, VALUE obj)
{
    double *c;
    int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = IM_ARRAY(im_new, argc, double);

	for(i = 0; i < argc; i++)
		c[i] = NUM2DBL(argv[i]);

	if (im_less_vec(im, im_new, argc, c))
		vips_lib_error();

	return new;
}

static VALUE
img_less_img(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_less);
}

static VALUE
img_less(int argc, VALUE *argv, VALUE obj)
{
	if (argc < 1)
		rb_raise(rb_eArgError, "Need at least one argument.");

	if (TYPE(argv[0]) == T_FLOAT || TYPE(argv[0]) == T_FIXNUM)
		return img_less_const(argc, argv, obj);
	else if (argc != 1)
		rb_raise(rb_eArgError, "Need constants or one image.");

	return img_less_img(obj, argv[0]);
}

static VALUE
img_lesseq_const(int argc, VALUE *argv, VALUE obj)
{
    double *c;
    int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = IM_ARRAY(im_new, argc, double);

	for(i = 0; i < argc; i++)
		c[i] = NUM2DBL(argv[i]);

	if (im_lesseq_vec(im, im_new, argc, c))
		vips_lib_error();

	return new;
}

static VALUE
img_lesseq_img(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_lesseq);
}

static VALUE
img_lesseq(int argc, VALUE *argv, VALUE obj)
{
	if (argc < 1)
		rb_raise(rb_eArgError, "Need at least one argument.");

	if (TYPE(argv[0]) == T_FLOAT || TYPE(argv[0]) == T_FIXNUM)
		return img_lesseq_const(argc, argv, obj);
	else if (argc != 1)
		rb_raise(rb_eArgError, "Need constants or one image.");

	return img_lesseq_img(obj, argv[0]);
}

static VALUE
img_more_const(int argc, VALUE *argv, VALUE obj)
{
    double *c;
    int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = IM_ARRAY(im_new, argc, double);

	for(i = 0; i < argc; i++)
		c[i] = NUM2DBL(argv[i]);

	if (im_more_vec(im, im_new, argc, c))
		vips_lib_error();

	return new;
}

static VALUE
img_more_img(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_more);
}

static VALUE
img_more(int argc, VALUE *argv, VALUE obj)
{
	if (argc < 1)
		rb_raise(rb_eArgError, "Need at least one argument.");

	if (TYPE(argv[0]) == T_FLOAT || TYPE(argv[0]) == T_FIXNUM)
		return img_more_const(argc, argv, obj);
	else if (argc != 1)
		rb_raise(rb_eArgError, "Need constants or one image.");

	return img_more_img(obj, argv[0]);
}

static VALUE
img_moreeq_const(int argc, VALUE *argv, VALUE obj)
{
    double *c;
    int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = IM_ARRAY(im_new, argc, double);

	for(i = 0; i < argc; i++)
		c[i] = NUM2DBL(argv[i]);

	if (im_moreeq_vec(im, im_new, argc, c))
		vips_lib_error();

	return new;
}

static VALUE
img_moreeq_img(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_moreeq);
}

static VALUE
img_moreeq(int argc, VALUE *argv, VALUE obj)
{
	if (argc < 1)
		rb_raise(rb_eArgError, "Need at least one argument.");

	if (TYPE(argv[0]) == T_FLOAT || TYPE(argv[0]) == T_FIXNUM)
		return img_moreeq_const(argc, argv, obj);
	else if (argc != 1)
		rb_raise(rb_eArgError, "Need constants or one image.");

	return img_moreeq_img(obj, argv[0]);
}

static VALUE
img_ifthenelse(VALUE obj, VALUE obj2, VALUE obj3)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	GetImg(obj3, data3, im3);
	OutImg3(obj, obj2, obj3, new, data_new, im_new);

    if (im_ifthenelse(im, im2, im3, im_new))
        vips_lib_error();

    return new;
}

static VALUE
img_blend(VALUE obj, VALUE obj2, VALUE obj3)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	GetImg(obj3, data3, im3);
	OutImg3(obj, obj2, obj3, new, data_new, im_new);

    if (im_blend(im, im2, im3, im_new))
        vips_lib_error();

    return new;
}

void
init_relational(void)
{
    rb_define_method(cVIPSImage, "equal", img_equal, -1);
    rb_define_method(cVIPSImage, "notequal", img_notequal, -1);
    rb_define_method(cVIPSImage, "less", img_less, -1);
    rb_define_method(cVIPSImage, "lesseq", img_lesseq, -1);
    rb_define_method(cVIPSImage, "more", img_more, -1);
    rb_define_method(cVIPSImage, "moreeq", img_moreeq, -1);
    rb_define_method(cVIPSImage, "ifthenelse", img_ifthenelse, 2);
    rb_define_method(cVIPSImage, "blend", img_blend, 2);
}

