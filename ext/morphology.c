#include "ruby_vips.h"
#include "image.h"
#include "mask.h"

static VALUE
img_dilate(VALUE obj, VALUE mask)
{
    INTMASK *imask;

	GetImg(obj, data, im);
	OutImg2(obj, mask, new, data_new, im_new);

    mask_arg2mask(mask, &imask, NULL);

    if (im_dilate(im, im_new, imask))
        vips_lib_error();

    return new;
}

static VALUE
img_erode(VALUE obj, VALUE mask)
{
    INTMASK *imask;

	GetImg(obj, data, im);
	OutImg2(obj, mask, new, data_new, im_new);

    mask_arg2mask(mask, &imask, NULL);

    if (im_erode(im, im_new, imask))
        vips_lib_error();

    return new;
}

static VALUE
img_rank(VALUE obj, VALUE xsize, VALUE ysize, VALUE order)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_rank(im, im_new, NUM2INT(xsize), NUM2INT(ysize), NUM2INT(order)))
        vips_lib_error();

    return new;
}

static VALUE
img_rank_image_internal(int argc, VALUE *argv, VALUE obj, int index)
{
	vipsImg *im_t;
    IMAGE **ins;
    int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    ins = IM_ARRAY(im_new, argc + 1, IMAGE*);
	ins[0] = im;

    for (i = 0; i < argc; i++) {
		img_add_dep(data_new, argv[i]);
        Data_Get_Struct(argv[i], vipsImg, im_t);
        ins[i + 1] = im_t->in;
    }

    if (im_rank_image(ins, im_new, argc + 1, index))
        vips_lib_error();

    return new;
}

static VALUE
img_rank_image(int argc, VALUE *argv, VALUE obj)
{
	VALUE index, *images;
	if (argc < 2)
		rb_raise(rb_eArgError, "Need an index and at least one image");

	index = argv[0];
	images = RARRAY_PTR(rb_ary_new4(argc - 1, argv + 1));

	return img_rank_image_internal(argc - 1, images, obj, NUM2INT(index));
}

static VALUE
img_maxvalue(int argc, VALUE *argv, VALUE obj)
{
	img_rank_image_internal(argc, argv, obj, argc - 1);
}

static VALUE
img_cntlines(VALUE obj, int flag) {
    double nolines;
	GetImg(obj, data, im);
    
    if (im_cntlines(im, &nolines, flag))
        vips_lib_error();

    return DBL2NUM(nolines);
}

static VALUE
img_cntlines_h(VALUE obj) {
	return img_cntlines(obj, 0);
}

static VALUE
img_cntlines_v(VALUE obj) {
	return img_cntlines(obj, 1);
}

static VALUE
img_zerox(VALUE obj, int flag)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_zerox(im, im_new, flag))
        vips_lib_error();

    return new;
}

static VALUE
img_zerox_pos(VALUE obj)
{
    return img_zerox(obj, 1);
}

static VALUE
img_zerox_neg(VALUE obj)
{
    return img_zerox(obj, -1);
}

static VALUE
img_profile(VALUE obj, int dir)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_profile(im, im_new, dir))
        vips_lib_error();

    return new;
}

static VALUE
img_profile_h(VALUE obj)
{
	return img_profile(obj, 1);
}

static VALUE
img_profile_v(VALUE obj)
{
	return img_profile(obj, 0);
}

static VALUE
img_label_regions(VALUE obj)
{
	int segments;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_label_regions(im, im_new, &segments))
        vips_lib_error();

    return rb_ary_new3(2, new, segments);
}

void
init_morphology(void)
{
    rb_define_method(cVIPSImage, "dilate", img_dilate, 1);
    rb_define_method(cVIPSImage, "erode", img_erode, 1);
    rb_define_method(cVIPSImage, "rank", img_rank, 3);
	rb_define_method(cVIPSImage, "rank_image", img_rank_image, -1);
	rb_define_method(cVIPSImage, "maxvalue", img_maxvalue, -1);
	rb_define_method(cVIPSImage, "cntlines_h", img_cntlines_h, 0);
	rb_define_method(cVIPSImage, "cntlines_v", img_cntlines_v, 0);
	rb_define_method(cVIPSImage, "zerox_pos", img_zerox_pos, 0);
	rb_define_method(cVIPSImage, "zerox_neg", img_zerox_neg, 0);
	rb_define_method(cVIPSImage, "profile_h", img_profile_h, 0);
	rb_define_method(cVIPSImage, "profile_v", img_profile_v, 0);
	rb_define_method(cVIPSImage, "label_regions", img_label_regions, 0);
}

