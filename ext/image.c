#include "ruby_vips.h"
#include "header.h"

#include "image.h"
#include "image_arithmetic.h"
#include "image_boolean.h"
#include "image_colour.h"
#include "image_conversion.h"
#include "image_convolution.h"
#include "image_freq_filt.h"
#include "image_histograms_lut.h"
#include "image_morphology.h"
#include "image_mosaicing.h"
#include "image_relational.h"
#include "image_resample.h"

VALUE cVIPSImage;

static ID id_b_w, id_histogram, id_fourier, id_xyz, id_lab, id_cmyk, id_labq,
    id_rgb, id_ucs, id_lch, id_labs, id_srgb, id_yxy, id_rgb16, id_grey16;

static ID id_none, id_rad;

static void
img_free(vipsImg *im)
{
    if(im->in)
        im_close(im->in);

	if(im->deps)
		xfree(im->deps);

    xfree(im);
}

static void
img_mark(vipsImg *im)
{
	int i;

	for (i = 0; i < im->deps_len; i++)
		rb_gc_mark(im->deps[i]);
}

VALUE
img_alloc(VALUE klass)
{
    vipsImg *im;
    VALUE new = Data_Make_Struct(klass, vipsImg, img_mark, img_free, im);
    im->in = NULL;
    im->deps = NULL;
	im->deps_len = 0;

    return new;
}

void
img_add_dep(vipsImg *im, VALUE dep)
{
	int i;
	
	for (i = 0; i < im->deps_len; i++)
		if (im->deps[i] == dep)
			return;

	im->deps_len++;
	im->deps = realloc(im->deps, im->deps_len * sizeof(VALUE*));
	im->deps[im->deps_len - 1] = dep;
}

VALUE
img_init(VALUE klass, VipsImage *im)
{
    vipsImg *data;
    VALUE new;

    new = img_alloc(klass);
    Data_Get_Struct(new, vipsImg, data);
    data->in = im;

    return new;
}

VALUE
img_init_partial_anyclass(VALUE klass)
{
    VipsImage *im;

    if (!(im = im_open("img_init_partial", "p")))
        vips_lib_error();

    return img_init(klass, im);
}

VALUE
img_init_partial()
{
    return img_init_partial_anyclass(cVIPSImage);
}

VALUE
img_spawn(VALUE parent)
{
    vipsImg *im;
    VALUE new = img_init_partial();
    Data_Get_Struct(new, vipsImg, im);
    img_add_dep(im, parent);

    return new;
}

VALUE
img_spawn2(VALUE parent1, VALUE parent2)
{
	VALUE new = img_spawn(parent1);
    vipsImg *im;
    Data_Get_Struct(new, vipsImg, im);

    img_add_dep(im, parent2);
    return new;
}

VALUE
img_spawn3(VALUE parent1, VALUE parent2, VALUE parent3)
{
	VALUE new = img_spawn2(parent1, parent2);
    vipsImg *im;
    Data_Get_Struct(new, vipsImg, im);

    img_add_dep(im, parent3);
    return new;
}

ID
img_vtype_to_id(VipsType vtype)
{
    switch(vtype) {
        case IM_TYPE_B_W:       return id_b_w;
        case IM_TYPE_HISTOGRAM: return id_histogram;
        case IM_TYPE_FOURIER:   return id_fourier;
        case IM_TYPE_XYZ:       return id_xyz;
        case IM_TYPE_LAB:       return id_lab;
        case IM_TYPE_CMYK:      return id_cmyk;
        case IM_TYPE_LABQ:      return id_labq;
        case IM_TYPE_RGB:       return id_rgb;
        case IM_TYPE_UCS:       return id_ucs;
        case IM_TYPE_LCH:       return id_lch;
        case IM_TYPE_LABS:      return id_labs;
        case IM_TYPE_sRGB:      return id_srgb;
        case IM_TYPE_YXY:       return id_yxy;
        case IM_TYPE_RGB16:     return id_rgb16;
        case IM_TYPE_GREY16:    return id_grey16;
    }
    return id_b_w;
}

ID
img_coding_to_id(VipsCoding coding)
{
    switch(coding) {
        case IM_CODING_NONE: return id_none;
        case IM_CODING_LABQ: return id_labq;
        case IM_CODING_RAD:  return id_rad;
    }
    return id_none;
}

/*
 *  call-seq:
 *     im.vtype -> image_type_symbol
 *
 *  Returns the type of image that Vips has assigned to *self*. This can by any
 *  of:
 *
 *  :B_W, :HISTOGRAM, :FOURIER, :XYZ, :LAB, :CMYK, :LABQ, :RGB, :UCS, :LCH,
 *  :LABS, :sRGB, :YXY, :RGB16, :GREY16
 */

static VALUE
img_vtype(VALUE obj)
{
	GetImg(obj, data, im);

    if (im)
    	return ID2SYM(img_vtype_to_id(im->Type));

    return Qnil;
}

/*
 *  call-seq:
 *     im.coding -> coding_symbol
 *
 *  Returns the data coding for this image. Coding can be one of:
 *  :NONE, :LABQ, :RAD.
 */

static VALUE
img_coding(VALUE obj)
{
	GetImg(obj, data, im);

    if (im)
        return ID2SYM(img_coding_to_id(im->Coding));

    return Qnil;
}

/*
 *  call-seq:
 *     im.kill -> number
 *
 *  Returns whether this image has been flagged to stop processing.
 */

static VALUE
img_kill(VALUE obj)
{
	GetImg(obj, data, im);
    return INT2FIX(im->kill);
}

/*
 *  call-seq:
 *     im.filename -> string
 *
 *  Returns the path from which *self* was read.
 */

static VALUE
img_filename(VALUE obj)
{
	GetImg(obj, data, im);

    if (im)
        return rb_tainted_str_new2(im->filename);

    return Qnil;
}


#define GETPIX( TYPE, CONVERSION ) { \
	TYPE *p = (TYPE *) IM_IMAGE_ADDR(im, x, y); \
\
	if (sz == 1) \
		return CONVERSION(*p); \
\
	ary = rb_ary_new2(sz); \
	for (i = 0; i < sz; i++) \
		rb_ary_push(ary, CONVERSION(p[i])); \
\
	return ary; \
}

#define CGETPIX( TYPE, CONVERSION ) { \
	TYPE *p = (TYPE *) IM_IMAGE_ADDR(im, x, y); \
\
	ary = rb_ary_new2(sz); \
	for (i = 0; i < sz; i++) { \
		rb_ary_push(ary, rb_ary_new3(2, CONVERSION(p[0]), CONVERSION(p[1]))); \
		p += 2; \
	} \
\
	return ary; \
}

static VALUE
img_pixel_to_rb(VipsImage *im, int x, int y)
{
	const int sz = im->Bands;
	int i;
	VALUE ary;

	switch( im->BandFmt ) {
	case IM_BANDFMT_UCHAR:     GETPIX( unsigned char, UINT2NUM ); break;
	case IM_BANDFMT_CHAR:      GETPIX( signed char, INT2NUM ); break;
	case IM_BANDFMT_USHORT:    GETPIX( unsigned short, UINT2NUM ); break;
	case IM_BANDFMT_SHORT:     GETPIX( signed short, INT2NUM ); break;
	case IM_BANDFMT_UINT:      GETPIX( unsigned int, UINT2NUM ); break;
	case IM_BANDFMT_INT:       GETPIX( signed int, INT2FIX ); break;
	case IM_BANDFMT_FLOAT:     GETPIX( float, rb_float_new ); break;

	case IM_BANDFMT_DOUBLE:    GETPIX( double, rb_float_new ); break;
	case IM_BANDFMT_COMPLEX:   CGETPIX( float, rb_float_new ); break; 
	case IM_BANDFMT_DPCOMPLEX: CGETPIX( double, rb_float_new ); break; 
	}
}

/*
 *  call-seq:
 *     im[x, y] -> number, ...
 *
 *  Returns the band values at the given <i>x</i>, <i>y</i> location of the
 *  image. The upper left corner of the image is at 0, 0, and the lower right
 *  corner is at im.x_size - 1, im.y_size - 1.
 *
 *  If it is a one-band image, then this method returns the value without an
 *  array.
 */

static VALUE
img_aref(VALUE obj, VALUE v_x, VALUE v_y)
{
	int x = NUM2INT(v_x);
	int y = NUM2INT(v_y);
	GetImg(obj, data, im);

	if (im_incheck(im) || im_check_uncoded("img_aref", im))
		vips_lib_error();

	if (x >= im->Xsize || x < 0 || y >= im->Ysize || y < 0)
		rb_raise(rb_eIndexError, "Index out of bounds");

	return img_pixel_to_rb(im, x, y);
}

/*
 *  call-seq:
 *    im.each_pixel{|value, x, y|}
 *
 *  Yields every pixel in the image, with coordinates.
 */

static VALUE
img_each_pixel(VALUE obj)
{
	int x, y;
	VALUE pixel;
	GetImg(obj, data, im);

	if (im_incheck(im) || im_check_uncoded("img_each_pixel", im))
		return( Qnil );

    for(y = 0; y < im->Ysize; y++) {
		for(x = 0; x < im->Xsize; x++) {
			pixel = img_pixel_to_rb(im, x, y);
			rb_yield(rb_ary_new3(3, pixel, INT2FIX(x), INT2FIX(y)));
		}
	}

	return obj;
}

/*
 *  call-seq:
 *     im.data -> data_string
 *
 *  Returns the uncompressed data of the image.
 */

static VALUE
img_data(VALUE obj)
{
	GetImg(obj, data, im);

	if (im_incheck(im) || im_check_uncoded("img_aref", im))
		return( Qnil );

	return rb_tainted_str_new(im->data, IM_IMAGE_SIZEOF_LINE(im) * im->Ysize);
}

/*
 *  Image objects reference in-memory images. All operations that manipulate and
 *  return an image will simply add the operation to the pipeline and return an
 *  image that references the image after the operation. The actual operation is
 *  not performed until the image is written to a file or to a string.
 */
void
init_Image(void)
{
    cVIPSImage = rb_define_class_under(mVIPS, "Image", rb_cObject);
    rb_define_alloc_func(cVIPSImage, img_alloc);
    rb_include_module(cVIPSImage, mVIPSHeader);

    rb_define_singleton_method(cVIPSImage, "linreg", img_s_linreg, -1); // in image_arithmetic.c
    rb_define_singleton_method(cVIPSImage, "gaussnoise", img_s_gaussnoise, 4); // in image_conversion.c
    rb_define_singleton_method(cVIPSImage, "black", img_s_black, 3); // in image_conversion.c
    rb_define_singleton_method(cVIPSImage, "text", img_s_text, 5); // in image_conversion.c
    rb_define_singleton_method(cVIPSImage, "fmask_ideal_highpass", img_s_fmask_ideal_highpass, 3); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_ideal_lowpass", img_s_fmask_ideal_lowpass, 3); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_butterworth_highpass", img_s_fmask_butterworth_highpass, 5); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_butterworth_lowpass", img_s_fmask_butterworth_lowpass, 5); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_gauss_highpass", img_s_fmask_gauss_highpass, 4); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_gauss_lowpass", img_s_fmask_gauss_lowpass, 4); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_ideal_ringpass", img_s_fmask_ideal_ringpass, 4); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_ideal_ringreject", img_s_fmask_ideal_ringreject, 4); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_butterworth_ringpass", img_s_fmask_butterworth_ringpass, 6); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_butterworth_ringreject", img_s_fmask_butterworth_ringreject, 6); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_gauss_ringpass", img_s_fmask_gauss_ringpass, 5); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_gauss_ringreject", img_s_fmask_gauss_ringreject, 5); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_ideal_bandpass", img_s_fmask_ideal_bandpass, 5); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_ideal_bandreject", img_s_fmask_ideal_bandreject, 5); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_butterworth_bandpass", img_s_fmask_butterworth_bandpass, 7); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_butterworth_bandreject", img_s_fmask_butterworth_bandreject, 7); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_gauss_bandpass", img_s_fmask_gauss_bandpass, 6); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_gauss_bandreject", img_s_fmask_gauss_bandreject, 6); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fmask_fractal_flt", img_s_fmask_fractal_flt, 3); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "fractsurf", img_s_fractsurf, 2); // in image_freq_filt.c
    rb_define_singleton_method(cVIPSImage, "identity", img_s_identity, 1); // in image_histograms_lut.c
    rb_define_singleton_method(cVIPSImage, "identity_ushort", img_s_identity_ushort, 2); // in image_histograms_lut.c
	rb_define_singleton_method(cVIPSImage, "invertlut", img_s_invertlut, 2); // in image_histograms_lut.c
	rb_define_singleton_method(cVIPSImage, "buildlut", img_s_buildlut, 1); // in image_histograms_lut.c
    rb_define_singleton_method(cVIPSImage, "tone_build_range", img_s_tone_build_range, 10); // in image_histograms_lut.c
    rb_define_singleton_method(cVIPSImage, "tone_build", img_s_tone_build, 8); // in image_histograms_lut.c

    rb_define_method(cVIPSImage, "[]", img_aref, 2);
    rb_define_method(cVIPSImage, "each_pixel", img_each_pixel, 0);
    rb_define_method(cVIPSImage, "data", img_data, 0);
    rb_define_method(cVIPSImage, "vtype", img_vtype, 0);
    rb_define_method(cVIPSImage, "coding", img_coding, 0);
    rb_define_method(cVIPSImage, "filename", img_filename, 0);
    rb_define_method(cVIPSImage, "kill", img_kill, 0);
	rb_define_method(cVIPSImage, "measure_area", img_measure_area, 7); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "stats", img_stats, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "max", img_max, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "min", img_min, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "avg", img_avg, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "deviate", img_deviate, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "maxpos", img_maxpos, -1); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "minpos", img_minpos, -1); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "maxpos_avg", img_maxpos_avg, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "bandmean", img_bandmean, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "add", img_add, 1); // in image_arithmetic.c
	rb_define_alias(cVIPSImage, "+", "add");
	rb_define_method(cVIPSImage, "subtract", img_subtract, 1); // in image_arithmetic.c
	rb_define_alias(cVIPSImage, "-", "subtract");
	rb_define_method(cVIPSImage, "invert", img_invert, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "lin", img_lin, 2); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "multiply", img_multiply, 1); // in image_arithmetic.c
	rb_define_alias(cVIPSImage, "*", "multiply");
	rb_define_method(cVIPSImage, "divide", img_divide, 1); // in image_arithmetic.c
	rb_define_alias(cVIPSImage, "/", "divide");
	rb_define_method(cVIPSImage, "remainder", img_remainder, -1); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "%", img_remainder_binop, 1);
	rb_define_method(cVIPSImage, "recomb", img_recomb, 1); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "sign", img_sign, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "abs", img_abs, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "floor", img_floor, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "rint", img_rint, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "ceil", img_ceil, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "point", img_point, 4); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "pow", img_pow, -1); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "**", img_pow_binop, 1); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "expn", img_expn, -1); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "log", img_log, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "log10", img_log10, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "sin", img_sin, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "cos", img_cos, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "tan", img_tan, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "asin", img_asin, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "acos", img_acos, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "atan", img_atan, 0); // in image_arithmetic.c
	rb_define_method(cVIPSImage, "cross_phase", img_cross_phase, 1); // in image_arithmetic.c
    rb_define_method(cVIPSImage, "&", img_and, 1); // in image_boolean.c
    rb_define_method(cVIPSImage, "|", img_or, 1); // in image_boolean.c
    rb_define_method(cVIPSImage, "^", img_xor, 1); // in image_boolean.c
    rb_define_method(cVIPSImage, "<<", img_shiftleft, 1); // in image_boolean.c
    rb_define_method(cVIPSImage, ">>", img_shiftright, 1); // in image_boolean.c
    rb_define_method(cVIPSImage, "lab_to_lch", img_lab_to_lch, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "lch_to_lab", img_lch_to_lab, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "labq_to_xyz", img_labq_to_xyz, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "rad_to_float", img_rad_to_float, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "float_to_rad", img_float_to_rad, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "lch_to_ucs", img_lch_to_ucs, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "lab_to_labq", img_lab_to_labq, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "lab_to_labs", img_lab_to_labs, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "lab_to_xyz", img_lab_to_xyz, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "lab_to_xyz_temp", img_lab_to_xyz_temp, 3); // in image_colour.c
    rb_define_method(cVIPSImage, "lab_to_ucs", img_lab_to_ucs, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "labq_to_lab", img_labq_to_lab, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "labq_to_labs", img_labq_to_labs, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "labs_to_labq", img_labs_to_labq, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "labs_to_lab", img_labs_to_lab, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "ucs_to_xyz", img_ucs_to_xyz, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "ucs_to_lch", img_ucs_to_lch, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "ucs_to_lab", img_ucs_to_lab, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "xyz_to_lab", img_xyz_to_lab, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "xyz_to_lab_temp", img_xyz_to_lab_temp, 3); // in image_colour.c
    rb_define_method(cVIPSImage, "xyz_to_ucs", img_xyz_to_ucs, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "srgb_to_xyz", img_srgb_to_xyz, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "xyz_to_srgb", img_xyz_to_srgb, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "yxy_to_xyz", img_yxy_to_xyz, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "xyz_to_yxy", img_xyz_to_yxy, 0); // in image_colour.c
    rb_define_method(cVIPSImage, "decmc_from_lab", img_decmc_from_lab, 1); // in image_colour.c
    rb_define_method(cVIPSImage, "de00_from_lab", img_de00_from_lab, 1); // in image_colour.c
    rb_define_method(cVIPSImage, "de_from_xyz", img_de_from_xyz, 1); // in image_colour.c
    rb_define_method(cVIPSImage, "de_from_lab", img_de_from_lab, 1); // in image_colour.c
	rb_define_method(cVIPSImage, "im_lab_morph", img_lab_morph, 5); // in image_colour.c
	rb_define_method(cVIPSImage, "icc_transform", img_icc_transform, 3); // in image_colour.c
	rb_define_method(cVIPSImage, "icc_import", img_icc_import, 2); // in image_colour.c
	rb_define_method(cVIPSImage, "icc_import_embedded", img_icc_import_embedded, 1); // in image_colour.c
	rb_define_method(cVIPSImage, "icc_export_depth", img_icc_export_depth, 3); // in image_colour.c
	rb_define_method(cVIPSImage, "icc_ac2rc", img_icc_ac2rc, 1); // in image_colour.c
    rb_define_method(cVIPSImage, "to_mask", img_to_mask, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "copy_file", img_copy_file, 0);  // in image_conversion.c
    rb_define_method(cVIPSImage, "dup", img_dup, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "clip2fmt", img_clip2fmt, 1); // in image_conversion.c
    rb_define_method(cVIPSImage, "scale", img_scale, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "msb", img_msb, -1); // in image_conversion.c
    rb_define_method(cVIPSImage, "copy_swap", img_copy_swap, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "copy_native", img_copy_native, 1); // in image_conversion.c
    rb_define_method(cVIPSImage, "c2amph", img_c2amph, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "c2rect", img_c2rect, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "ri2c", img_ri2c, 1); // in image_conversion.c
    rb_define_method(cVIPSImage, "c2imag", img_c2imag, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "c2real", img_c2real, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "scaleps", img_scaleps, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "falsecolour", img_falsecolour, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "extract_band", img_extract_band, -1); // in image_conversion.c
    rb_define_method(cVIPSImage, "extract_area", img_extract_area, -1); // in image_conversion.c
    rb_define_method(cVIPSImage, "embed", img_embed, 5); // in image_conversion.c
    rb_define_method(cVIPSImage, "bandjoin", img_bandjoin, -1); // in image_conversion.c
    rb_define_method(cVIPSImage, "insert", img_insert, -1); // in image_conversion.c
    rb_define_method(cVIPSImage, "insert_noexpand", img_insert_noexpand, 3); // in image_conversion.c
    rb_define_method(cVIPSImage, "lrjoin", img_lrjoin, 1); // in image_conversion.c
    rb_define_method(cVIPSImage, "tbjoin", img_tbjoin, 1); // in image_conversion.c
    rb_define_method(cVIPSImage, "replicate", img_replicate, 2); // in image_conversion.c
    rb_define_method(cVIPSImage, "grid", img_grid, 3); // in image_conversion.c
    rb_define_method(cVIPSImage, "wrap", img_wrap, 2); // in image_conversion.c
    rb_define_method(cVIPSImage, "fliphor", img_fliphor, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "flipver", img_flipver, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "rot90", img_rot90, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "rot180", img_rot180, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "rot270", img_rot270, 0); // in image_conversion.c
    rb_define_method(cVIPSImage, "subsample", img_subsample, -1); // in image_conversion.c
    rb_define_method(cVIPSImage, "zoom", img_zoom, -1); // in image_conversion.c
    rb_define_method(cVIPSImage, "conv", img_conv, 1); // in image_convolution.c
    rb_define_method(cVIPSImage, "convsep", img_convsep, 1); // in image_convolution.c
    rb_define_method(cVIPSImage, "compass", img_compass, 1); // in image_convolution.c
    rb_define_method(cVIPSImage, "gradient", img_gradient, 1); // in image_convolution.c
    rb_define_method(cVIPSImage, "lindetect", img_lindetect, 1); // in image_convolution.c
    rb_define_method(cVIPSImage, "sharpen", img_sharpen, 6); // in image_convolution.c
    rb_define_method(cVIPSImage, "grad_x", img_grad_x, 0); // in image_convolution.c
    rb_define_method(cVIPSImage, "grad_y", img_grad_y, 0); // in image_convolution.c
    rb_define_method(cVIPSImage, "fastcor", img_fastcor, 1); // in image_convolution.c
    rb_define_method(cVIPSImage, "spcor", img_spcor, 1); // in image_convolution.c
    rb_define_method(cVIPSImage, "gradcor", img_gradcor, 1); // in image_convolution.c
    rb_define_method(cVIPSImage, "contrast_surface", img_contrast_surface, 2); // in image_convolution.c
    rb_define_method(cVIPSImage, "addgnoise", img_addgnoise, 1); // in image_convolution.c
    rb_define_method(cVIPSImage, "fwfft", img_fwfft, 0); // in image_freq_filt.c
    rb_define_method(cVIPSImage, "invfft", img_invfft, 0); // in image_freq_filt.c
	rb_define_method(cVIPSImage, "rotquad", img_rotquad, 0); // in image_freq_filt.c
    rb_define_method(cVIPSImage, "invfftr", img_invfftr, 0); // in image_freq_filt.c
    rb_define_method(cVIPSImage, "freqflt", img_freqflt, 1); // in image_freq_filt.c
    rb_define_method(cVIPSImage, "disp_ps", img_disp_ps, 0); // in image_freq_filt.c
    rb_define_method(cVIPSImage, "phasecor_fft", img_phasecor_fft, 1); // in image_freq_filt.c
    rb_define_method(cVIPSImage, "histgr", img_histgr, -1); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "histnd", img_histnd, 1); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "hist_indexed", img_hist_indexed, 1); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "project", img_project, 0); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "histnorm", img_histnorm, 0); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "histcum", img_histcum, 0); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "histeq", img_histeq, 0); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "histspec", img_histspec, 1); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "monotonic?", img_monotonic_p, 0); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "hist", img_hist, -1); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "hsp", img_hsp, 1); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "gammacorrect", img_gammacorrect, 1); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "mpercent_hist", img_mpercent_hist, 1); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "mpercent", img_mpercent, 1); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "heq", img_heq, -1); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "lhisteq", img_lhisteq, 2); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "stdif", img_stdif, 6); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "tone_analyze", img_tone_analyze, 6); // in image_histograms_lut.c
	rb_define_method(cVIPSImage, "maplut", img_maplut, 1); // in image_histograms_lut.c
	rb_define_method(cVIPSImage, "histplot", img_histplot, 0); // in image_histograms_lut.c
    rb_define_method(cVIPSImage, "dilate", img_dilate, 1); // in image_morphology.c
    rb_define_method(cVIPSImage, "erode", img_erode, 1); // in image_morphology.c
    rb_define_method(cVIPSImage, "rank", img_rank, 3); // in image_morphology.c
	rb_define_method(cVIPSImage, "rank_image", img_rank_image, -1); // in image_morphology.c
	rb_define_method(cVIPSImage, "maxvalue", img_maxvalue, -1); // in image_morphology.c
	rb_define_method(cVIPSImage, "cntlines_h", img_cntlines_h, 0); // in image_morphology.c
	rb_define_method(cVIPSImage, "cntlines_v", img_cntlines_v, 0); // in image_morphology.c
	rb_define_method(cVIPSImage, "zerox_pos", img_zerox_pos, 0); // in image_morphology.c
	rb_define_method(cVIPSImage, "zerox_neg", img_zerox_neg, 0); // in image_morphology.c
	rb_define_method(cVIPSImage, "profile_h", img_profile_h, 0); // in image_morphology.c
	rb_define_method(cVIPSImage, "profile_v", img_profile_v, 0); // in image_morphology.c
	rb_define_method(cVIPSImage, "label_regions", img_label_regions, 0); // in image_morphology.c
    rb_define_method(cVIPSImage, "lrmerge", img_lrmerge, -1); // in image_mosaicing.c
    rb_define_method(cVIPSImage, "tbmerge", img_tbmerge, -1); // in image_mosaicing.c
    rb_define_method(cVIPSImage, "lrmerge1", img_lrmerge1, -1); // in image_mosaicing.c
    rb_define_method(cVIPSImage, "tbmerge1", img_tbmerge1, -1); // in image_mosaicing.c
    rb_define_method(cVIPSImage, "lrmosaic", img_lrmosaic, -1); // in image_mosaicing.c
    rb_define_method(cVIPSImage, "tbmosaic", img_tbmosaic, -1); // in image_mosaicing.c
    rb_define_method(cVIPSImage, "lrmosaic1", img_lrmosaic1, -1); // in image_mosaicing.c
    rb_define_method(cVIPSImage, "tbmosaic1", img_tbmosaic1, -1); // in image_mosaicing.c
    rb_define_method(cVIPSImage, "global_balance", img_global_balance, 1); // in image_mosaicing.c
    rb_define_method(cVIPSImage, "global_balancef", img_global_balancef, 1); // in image_mosaicing.c
    rb_define_method(cVIPSImage, "correl", img_correl, 7); // in image_mosaicing.c
	rb_define_method(cVIPSImage, "align_bands", img_align_bands, 0); // in image_mosaicing.c
	rb_define_method(cVIPSImage, "maxpos_subpel", img_maxpos_subpel, 0); // in image_mosaicing.c
    rb_define_method(cVIPSImage, "equal", img_equal, -1); // in image_relational.c
    rb_define_method(cVIPSImage, "notequal", img_notequal, -1); // in image_relational.c
    rb_define_method(cVIPSImage, "less", img_less, -1); // in image_relational.c
    rb_define_method(cVIPSImage, "lesseq", img_lesseq, -1); // in image_relational.c
    rb_define_method(cVIPSImage, "more", img_more, -1); // in image_relational.c
    rb_define_method(cVIPSImage, "moreeq", img_moreeq, -1); // in image_relational.c
    rb_define_method(cVIPSImage, "ifthenelse", img_ifthenelse, 2); // in image_relational.c
    rb_define_method(cVIPSImage, "blend", img_blend, 2); // in image_relational.c
    rb_define_method(cVIPSImage, "affinei", img_affinei, -1); // in image_resample.c
    rb_define_method(cVIPSImage, "affinei_resize", img_affinei_resize, -1); // in image_resample.c
    rb_define_method(cVIPSImage, "stretch3", img_stretch3, -1); // in image_resample.c
    rb_define_method(cVIPSImage, "shrink", img_shrink, -1); // in image_resample.c
    rb_define_method(cVIPSImage, "rightshift_size", img_rightshift_size, 3); // in image_resample.c
    rb_define_method(cVIPSImage, "match_linear", img_match_linear, 9); // in image_resample.c
    rb_define_method(cVIPSImage, "match_linear_search", img_match_linear_search, 11); // in image_resample.c

	id_b_w = rb_intern("B_W");
	id_histogram = rb_intern("HISTOGRAM");
	id_fourier = rb_intern("FOURIER");
	id_xyz = rb_intern("XYZ");
	id_lab = rb_intern("LAB");
	id_cmyk = rb_intern("CMYK");
	id_labq = rb_intern("LABQ");
	id_rgb = rb_intern("RGB");
	id_ucs = rb_intern("UCS");
	id_lch = rb_intern("LCH");
	id_labs = rb_intern("LABS");
	id_srgb = rb_intern("sRGB");
	id_yxy = rb_intern("YXY");
	id_rgb16 = rb_intern("RGB16");
	id_grey16 = rb_intern("GREY16");

	id_none = rb_intern("NONE");
	id_rad = rb_intern("RAD");

    init_Image_colour();
    init_Image_conversion();
    init_Image_mosaicing();

#if 0
    mVIPS = rb_define_module("VIPS");
    mVIPSHeader = rb_define_module_under(mVIPS, "Header");
#endif
}

