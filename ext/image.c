#include "ruby_vips.h"
#include "header.h"
#include "image.h"

VALUE cVIPSImage;

//static ID idNULL;
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

static VALUE
img_initialize(VALUE obj, VALUE path)
{
	GetImg(obj, data, im);

    if (!(data->in = im_open(StringValuePtr(path), "r")))
        vips_lib_error();

    return obj;
}

/* C constant & enum conversion */

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

/* VipsImage struct data accessors.
 * TODO: Skipping VipsProgress
 */

static VALUE
img_vtype(VALUE obj)
{
	GetImg(obj, data, im);

    if (im)
    	return ID2SYM(img_vtype_to_id(im->Type));

    return Qnil;
}

static VALUE
img_coding(VALUE obj)
{
	GetImg(obj, data, im);

    if (im)
        return ID2SYM(img_coding_to_id(im->Coding));

    return Qnil;
}

static VALUE
img_kill(VALUE obj)
{
	GetImg(obj, data, im);
    return INT2FIX(im->kill);
}

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

static VALUE
img_data(VALUE obj)
{
	GetImg(obj, data, im);

	if (im_incheck(im) || im_check_uncoded("img_aref", im))
		return( Qnil );

	return rb_tainted_str_new(im->data, IM_IMAGE_SIZEOF_LINE(im) * im->Ysize);
}

void
init_image(void)
{
    cVIPSImage = rb_define_class_under(mVIPS, "Image", rb_cObject);
    rb_include_module(cVIPSImage, mVIPSHeader);
    rb_define_alloc_func(cVIPSImage, img_alloc);

    rb_define_method(cVIPSImage, "initialize", img_initialize, 1);
    rb_define_method(cVIPSImage, "[]", img_aref, 2);
    rb_define_method(cVIPSImage, "each_pixel", img_each_pixel, 0);
    rb_define_method(cVIPSImage, "data", img_data, 0);
    rb_define_method(cVIPSImage, "vtype", img_vtype, 0);
    rb_define_method(cVIPSImage, "coding", img_coding, 0);
    rb_define_method(cVIPSImage, "filename", img_filename, 0);
    rb_define_method(cVIPSImage, "kill", img_kill, 0);

//	idNULL = rb_intern("");
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
}

