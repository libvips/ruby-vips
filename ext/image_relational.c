#include "ruby_vips.h"
#include "image.h"
#include "image_relational.h"

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

/*
 *  call-seq:
 *     im.equal(other_image)   -> image
 *     im.equal(constant, ...) -> image
 * 
 *  In the first form, this operation calculates *self* == <i>other_image</i>
 *  (image element equals image element) and writes the result to the output
 *  image.
 *
 *  In the second form, this operation calculates *self* == <i>constant</i>
 *  (image element equals constant array) and writes the result to the output
 *  image.
 */

VALUE
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

/*
 *  call-seq:
 *     im.notequal(other_image)   -> image
 *     im.notequal(constant, ...) -> image
 *
 *  In the first form, this operation calculates *self* != <i>other_image</i>
 *  (image element equals image element) and writes the result to the output
 *  image.
 *
 *  In the second form, this operation calculates *self* != <i>constant</i>
 *  (image element equals constant array) and writes the result to the output
 *  image.
 */

VALUE
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

/*
 *  call-seq:
 *     im.less(other_image)   -> image
 *     im.less(constant, ...) -> image
 *
 *  In the first form, this operation calculates *self* < <i>other_image</i>
 *  (image element equals image element) and writes the result to the output
 *  image.
 *
 *  In the second form, this operation calculates *self* < <i>constant</i>
 *  (image element equals constant array) and writes the result to the output
 *  image.
 */

VALUE
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

/*
 *  call-seq:
 *     im.lesseq(other_image)   -> image
 *     im.lesseq(constant, ...) -> image
 *
 *  In the first form, this operation calculates *self* <= <i>other_image</i>
 *  (image element equals image element) and writes the result to the output
 *  image.
 *
 *  In the second form, this operation calculates *self* <= <i>constant</i>
 *  (image element equals constant array) and writes the result to the output
 *  image.
 */

VALUE
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

/*
 *  call-seq:
 *     im.more(other_image)   -> image
 *     im.more(constant, ...) -> image
 *
 *  In the first form, this operation calculates *self* > <i>other_image</i>
 *  (image element equals image element) and writes the result to the output
 *  image.
 *
 *  In the second form, this operation calculates *self* > <i>constant</i>
 *  (image element equals constant array) and writes the result to the output
 *  image.
 */

VALUE
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

/*
 *  call-seq:
 *     im.moreeq(other_image)   -> image
 *     im.moreeq(constant, ...) -> image
 *
 *  In the first form, this operation calculates *self* >= <i>other_image</i>
 *  (image element equals image element) and writes the result to the output
 *  image.
 *
 *  In the second form, this operation calculates *self* >= <i>constant</i>
 *  (image element equals constant array) and writes the result to the output
 *  image.
 */

VALUE
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

/*
 *  call-seq:
 *     im.ifthenelse(a_image, b_image) -> image
 *
 *  This operation scans the condition image *self* and uses it to select pixels
 *  from either the then image <i>a_image</i> or the else image <i>b_image</i>.
 *  Non-zero means <i>a_image</i>, 0 means <i>b_image</i>.
 *
 *  Any image can have either 1 band or n bands, where n is the same for all
 *  the non-1-band images. Single band images are then effectively copied to
 *  make n-band images.
 *
 *  Images <i>a_image</i> and <i>b_image</i> are cast up to the smallest common
 *  format.
 *
 *  Images <i>a_image</i> and <i>b_image</i> must match exactly in size.
 */

VALUE
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

/*
 *  call-seq:
 *     im.blend(a_image, b_image) -> image
 *
 *  This operation scans the condition image *self* (which must be unsigned
 *  char) and uses it to blend pixels from either the then image <i>a_image</i>
 *  or the else image <i>b_image</i>. 255 means <i>a_image</i> only, 0 means
 *  <i>b_image</i> only, and intermediate values are a mixture.
 *
 *  Any image can have either 1 band or n bands, where n is the same for all
 *  the non-1-band images. Single band images are then effectively copied to
 *  make n-band images.
 *
 *  Images <i>a_image</i> and <i>b_image</i> are cast up to the smallest common
 *  format.
 *
 *  Images <i>a_image</i> and <i>b_image</i> must match exactly in size.
 */

VALUE
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
