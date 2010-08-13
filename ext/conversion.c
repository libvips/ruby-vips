#include "ruby_vips.h"
#include "image.h"
#include "mask.h"
#include "header.h"

static VALUE
img_to_mask(VALUE obj)
{
    vipsMask *msk;
    VALUE mask = mask_alloc(cVIPSMask);
	GetImg(obj, data, im);

    Data_Get_Struct(mask, vipsMask, msk);

    if (!(msk->dmask = im_vips2mask(im, "img_vips2mask")))
        vips_lib_error();

    return mask;
}

static VALUE
mask_to_image(VALUE obj)
{
    vipsImg *out;
    vipsMask *msk;
    VALUE img = img_alloc(cVIPSImage);

    Data_Get_Struct(obj, vipsMask, msk);
    Data_Get_Struct(img, vipsImg, out);

    if( im_mask2vips(msk->dmask, out->in) )
        vips_lib_error();

    return img;
}

/* TODO - Find out if there is a better way to do this. A long-running
 * server may expect to be able to call Image#dup indefinately, the current
 * approach will eat memory over time by continually adding to the ancestor
 * chain and preventing GC.
 */
static VALUE
img_dup(VALUE obj)
{
	RUBY_VIPS_UNARY(im_copy);
}

static VALUE
img_copy_swap(VALUE obj)
{
	RUBY_VIPS_UNARY(im_copy_swap);
}

/* TODO: Conditionally compile in this method - it is in the trunk of libvips,
 * but not in Ubuntu's 7.20.4. This method may not even be needed if ruby can
 * figure out native byte ordering.

static VALUE
img_copy_native(VALUE obj, VALUE input_msb_first)
{
    vipsImg *in, *out;
    int msb_first;

    VALUE new = img_spawn(obj);

    Data_Get_Struct(obj, vipsImg, in);
    Data_Get_Struct(new, vipsImg, out);

    msb_first = input_msb_first == Qtrue ? TRUE : FALSE;
    if( im_copy_native(in->in, out->in, msb_first) )
        vips_lib_error();

    return new;
}
 */

static VALUE
img_copy_file(VALUE obj)
{
    vipsImg *in, *out;

    VALUE new = img_spawn(obj);

    Data_Get_Struct(obj, vipsImg, in);
    Data_Get_Struct(new, vipsImg, out);

    if( im_copy_file(in->in, out->in) )
        vips_lib_error();

    return new;
}

static VALUE
img_clip2fmt(VALUE obj, VALUE fmt)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_clip2fmt(im, im_new, header_id_to_band_fmt(SYM2ID(fmt))))
        vips_lib_error();

    return new;
}

static VALUE
img_scale(VALUE obj)
{
	RUBY_VIPS_UNARY(im_scale);
}

static VALUE
img_msb(int argc, VALUE *argv, VALUE obj)
{
	VALUE v_num;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	rb_scan_args(argc, argv, "01", &v_num);

	if (NIL_P(v_num)) {
		if (im_msb(im, im_new))
			vips_lib_error();
	} else if (im_msb_band(im, im_new, NUM2INT(v_num)))
		vips_lib_error();
	
	return new;
}

static VALUE
img_c2amph(VALUE obj)
{
	RUBY_VIPS_UNARY(im_c2amph);
}

static VALUE
img_c2rect(VALUE obj)
{
	RUBY_VIPS_UNARY(im_c2rect);
}

static VALUE
img_ri2c(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_ri2c);
}

static VALUE
img_c2imag(VALUE obj)
{
	RUBY_VIPS_UNARY(im_c2imag);
}

static VALUE
img_c2real(VALUE obj)
{
	RUBY_VIPS_UNARY(im_c2real);
}

static VALUE
img_scaleps(VALUE obj)
{
	RUBY_VIPS_UNARY(im_scaleps);
}

static VALUE
img_falsecolour(VALUE obj)
{
	RUBY_VIPS_UNARY(im_falsecolour);
}

static VALUE
img_s_gaussnoise(VALUE obj, VALUE x, VALUE y, VALUE mean, VALUE sigma)
{
	OutPartial(new, data, im_new);

    if (im_gaussnoise(im_new, NUM2INT(x), NUM2INT(y), NUM2DBL(mean),
        NUM2DBL(sigma)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_black(VALUE obj, VALUE width, VALUE height, VALUE bands)
{
	OutPartial(new, data, im_new);

    if (im_black(im_new, NUM2INT(width), NUM2INT(height), NUM2DBL(bands)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_text(VALUE obj, VALUE text, VALUE font, VALUE width,
    VALUE alignment, VALUE dpi)
{
	OutPartial(new, data, im_new);

    if (im_text(im_new, StringValuePtr(text), StringValuePtr(font),
		NUM2INT(width), NUM2INT(alignment), NUM2INT(dpi)))
        vips_lib_error();

    return new;
}

static VALUE
img_extract_band(int argc, VALUE *argv, VALUE obj)
{
	VALUE v_start_band, v_num_bands;
	int num_bands;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	rb_scan_args(argc, argv, "11", &v_start_band, &v_num_bands);
	num_bands = NIL_P(v_num_bands) ? 1 : NUM2INT(v_num_bands);

	if (im_extract_bands(im, im_new, NUM2INT(v_start_band), num_bands))
        vips_lib_error();

    return new;
}

static VALUE
img_extract_area(int argc, VALUE *argv, VALUE obj)
{
	VALUE v_left, v_top, v_width, v_height, v_start_band, v_num_bands;
	int start_band, num_bands;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	rb_scan_args(argc, argv, "42", &v_left, &v_top, &v_width, &v_height,
		&v_start_band, &v_num_bands);

	/* Start band defaults to the first band */
	start_band = NIL_P(v_start_band) ? 0 : NUM2INT(v_start_band);
	
	/* Number of bands defaults to all bands or the selected band.
	 */
	if (NIL_P(v_num_bands))
		num_bands = NIL_P(v_start_band) ? im->Bands : 1;
	else
		num_bands = NUM2INT(v_num_bands);
	
    if (im_extract_areabands(im, im_new, NUM2INT(v_left), NUM2INT(v_top),
        NUM2INT(v_width), NUM2INT(v_height), start_band, num_bands))
        vips_lib_error();

    return new;  
}

static VALUE
img_embed(VALUE obj, VALUE type, VALUE x, VALUE y, VALUE width, VALUE height)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if( im_embed(im, im_new, NUM2INT(type), NUM2INT(x), NUM2INT(y),
		NUM2INT(width), NUM2INT(height)) )
        vips_lib_error();

    return new;  
}

static VALUE
img_bandjoin(int argc, VALUE *argv, VALUE obj)
{
	vipsImg *im_t;
    IMAGE **ins;
    int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	if(argc == 0)
		rb_raise(rb_eArgError, "Need at least one argument");

    ins = IM_ARRAY(im_new, argc + 1, IMAGE*);
	ins[0] = im;

    for (i = 0; i < argc; i++) {
		img_add_dep(data_new, argv[i]);
        Data_Get_Struct(argv[i], vipsImg, im_t);
        ins[i + 1] = im_t->in;
    }

    if (im_gbandjoin(ins, im_new, argc + 1))
        vips_lib_error();

    return new;
}

/* TODO: Evaluate combining with the below method & optional flag */
static VALUE
img_insert_noexpand(VALUE obj, VALUE obj2, VALUE x, VALUE y)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_insert_noexpand(im, im2, im_new, NUM2INT(x), NUM2INT(y)))
        vips_lib_error();

    return new;
}

static VALUE
img_insert_one(VALUE obj, VALUE obj2, VALUE x, VALUE y)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_insert(im, im2, im_new, NUM2INT(x), NUM2INT(y)))
        vips_lib_error();

    return new;
}

static VALUE
img_insertset(int argc, VALUE *argv, VALUE obj, VALUE obj2)
{
	int i, *x, *y;
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    x = IM_ARRAY(im_new, argc - 1, int);
    y = IM_ARRAY(im_new, argc - 1, int);

    for(i = 1; i < argc; i++) {
        x[i] = NUM2INT(RARRAY_PTR(argv[i])[0]);
        y[i] = NUM2INT(RARRAY_PTR(argv[i])[1]);
    }

    if( im_insertset(im, im2, im_new, argc - 1, x, y) )
        vips_lib_error();

    return new;
}

static VALUE
img_insert(int argc, VALUE *argv, VALUE obj)
{
	VALUE obj2, x_or_ary, y_or_arys;

	rb_scan_args(argc, argv, "2*", &obj2, &x_or_ary, &y_or_arys);

	if (TYPE(x_or_ary) == T_ARRAY) {
		if (!NIL_P(y_or_arys))
			rb_ary_unshift(y_or_arys, x_or_ary);

		return img_insertset(RARRAY_LEN(y_or_arys), RARRAY_PTR(y_or_arys), obj,
			obj2);
	}

	if (RARRAY_LEN(y_or_arys) != 1)
		rb_raise(rb_eArgError,
			"Need two coordinates or an array of coordinates");

	return img_insert_one(obj, obj2, x_or_ary, RARRAY_PTR(y_or_arys)[0]);
}

static VALUE
img_lrjoin(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_lrjoin);
}

static VALUE
img_tbjoin(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_tbjoin);
}

static VALUE
img_replicate(VALUE obj, VALUE across, VALUE down)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_replicate(im, im_new, NUM2INT(across), NUM2INT(down)))
        vips_lib_error();

    return new;  
}

static VALUE
img_grid(VALUE obj, VALUE tile_height, VALUE across, VALUE down)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_grid(im, im_new, NUM2INT(tile_height), NUM2INT(across),
		NUM2INT(down)))
        vips_lib_error();

    return new;  
}

static VALUE
img_wrap(VALUE obj, VALUE x, VALUE y)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_wrap(im, im_new, NUM2INT(x), NUM2INT(y)))
        vips_lib_error();

    return new;  
}

static VALUE
img_fliphor(VALUE obj)
{
	RUBY_VIPS_UNARY(im_fliphor);
}

static VALUE
img_flipver(VALUE obj)
{
	RUBY_VIPS_UNARY(im_flipver);
}

/* TODO: Consider making a single method: img_rot(deg) */
static VALUE
img_rot90(VALUE obj)
{
	RUBY_VIPS_UNARY(im_rot90);
}

static VALUE
img_rot180(VALUE obj)
{
	RUBY_VIPS_UNARY(im_rot180);
}

static VALUE
img_rot270(VALUE obj)
{
	RUBY_VIPS_UNARY(im_rot270);
}

static VALUE
img_subsample(int argc, VALUE *argv, VALUE obj)
{
    VALUE x, y;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	rb_scan_args(argc, argv, "11", &x, &y);
    if (NIL_P(y))
        y = x;
    
    if (im_subsample(im, im_new, NUM2INT(x), NUM2INT(y)))
        vips_lib_error();

    return new;  
}

static VALUE
img_zoom(int argc, VALUE *argv, VALUE obj)
{
    VALUE x, y;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	rb_scan_args(argc, argv, "11", &x, &y);
    if (NIL_P(y))
        y = x;

    if (im_zoom(im, im_new, NUM2INT(x), NUM2INT(y)))
        vips_lib_error();

    return new;
}

void
init_conversion()
{
    rb_define_singleton_method(cVIPSImage, "gaussnoise", img_s_gaussnoise, 4);
    rb_define_singleton_method(cVIPSImage, "black", img_s_black, 3);
    rb_define_singleton_method(cVIPSImage, "text", img_s_text, 5);

    rb_define_method(cVIPSImage, "to_mask", img_to_mask, 0);
/*
 *  rb_define_method(cVIPSMask, "to_image", mask_to_image, 0);
 *  rb_define_method(cVIPSImage, "copy_file", img_copy_file, 0);
 */
    rb_define_method(cVIPSImage, "dup", img_dup, 0);
    rb_define_method(cVIPSImage, "clip2fmt", img_clip2fmt, 1);
    rb_define_method(cVIPSImage, "scale", img_scale, 0);
    rb_define_method(cVIPSImage, "msb", img_msb, -1);
    rb_define_method(cVIPSImage, "copy_swap", img_copy_swap, 0);
/*
 *    rb_define_method(cVIPSImage, "copy_native", img_copy_native, 1);
 */
    rb_define_method(cVIPSImage, "c2amph", img_c2amph, 0);
    rb_define_method(cVIPSImage, "c2rect", img_c2rect, 0);
    rb_define_method(cVIPSImage, "ri2c", img_ri2c, 1);
    rb_define_method(cVIPSImage, "c2imag", img_c2imag, 0);
    rb_define_method(cVIPSImage, "c2real", img_c2real, 0);
    rb_define_method(cVIPSImage, "scaleps", img_scaleps, 0);
    rb_define_method(cVIPSImage, "falsecolour", img_falsecolour, 0);
    rb_define_method(cVIPSImage, "extract_band", img_extract_band, -1);
    rb_define_method(cVIPSImage, "extract_area", img_extract_area, -1);
    rb_define_method(cVIPSImage, "embed", img_embed, 5);
    rb_define_method(cVIPSImage, "bandjoin", img_bandjoin, -1);
    rb_define_method(cVIPSImage, "insert", img_insert, -1);
    rb_define_method(cVIPSImage, "insert_noexpand", img_insert_noexpand, 3);
    rb_define_method(cVIPSImage, "lrjoin", img_lrjoin, 1);
    rb_define_method(cVIPSImage, "tbjoin", img_tbjoin, 1);
    rb_define_method(cVIPSImage, "replicate", img_replicate, 2);
    rb_define_method(cVIPSImage, "grid", img_grid, 3);
    rb_define_method(cVIPSImage, "wrap", img_wrap, 2);
    rb_define_method(cVIPSImage, "fliphor", img_fliphor, 0);
    rb_define_method(cVIPSImage, "flipver", img_flipver, 0);
    rb_define_method(cVIPSImage, "rot90", img_rot90, 0);
    rb_define_method(cVIPSImage, "rot180", img_rot180, 0);
    rb_define_method(cVIPSImage, "rot270", img_rot270, 0);
    rb_define_method(cVIPSImage, "subsample", img_subsample, -1);
    rb_define_method(cVIPSImage, "zoom", img_zoom, -1);
}

