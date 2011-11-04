#include "ruby_vips.h"
#include "image.h"
#include "header.h"

VALUE mVIPSHeader;

static ID id_notset, id_uchar, id_char, id_ushort, id_short, id_uint, id_int,
	id_float, id_complex, id_double, id_dbcomplex;

ID
header_band_fmt_to_id(VipsBandFmt band_fmt)
{
    switch(band_fmt) {
        case IM_BANDFMT_NOTSET:    return id_notset;
        case IM_BANDFMT_UCHAR:     return id_uchar;
        case IM_BANDFMT_CHAR:      return id_char;
        case IM_BANDFMT_USHORT:    return id_ushort;
        case IM_BANDFMT_SHORT:     return id_short;
        case IM_BANDFMT_UINT:      return id_uint;
        case IM_BANDFMT_INT:       return id_int;
        case IM_BANDFMT_FLOAT:     return id_float;
        case IM_BANDFMT_COMPLEX:   return id_complex; // two floats
        case IM_BANDFMT_DOUBLE:    return id_double;
        case IM_BANDFMT_DPCOMPLEX: return id_dbcomplex; // two doubles
    }
    return id_notset;
}

VipsBandFmt
header_id_to_band_fmt(ID rb)
{
    if(rb == id_notset)         return IM_BANDFMT_NOTSET;
    else if(rb == id_uchar)     return IM_BANDFMT_UCHAR;
    else if(rb == id_char)      return IM_BANDFMT_CHAR;
    else if(rb == id_ushort)    return IM_BANDFMT_USHORT;
    else if(rb == id_short)     return IM_BANDFMT_SHORT;
    else if(rb == id_uint)      return IM_BANDFMT_UINT;
    else if(rb == id_int)       return IM_BANDFMT_INT;
    else if(rb == id_float)     return IM_BANDFMT_FLOAT;
    else if(rb == id_complex)   return IM_BANDFMT_COMPLEX;
    else if(rb == id_double)    return IM_BANDFMT_DOUBLE;
    else if(rb == id_dbcomplex) return IM_BANDFMT_DPCOMPLEX;

    return (VipsBandFmt)NULL;
}

/*
 *  call-seq:
 *     im.x_size -> number
 *
 *  Get the width in pixels of the image.
 */

static VALUE
header_x_size(VALUE obj)
{
	GetImg(obj, data, im);

    if (im)
        return INT2FIX(im->Xsize);

    return Qnil;
}

/*
 *  call-seq:
 *    im.y_size -> number
 *
 *  Get the height in pixels of the image.
 */

static VALUE
header_y_size(VALUE obj)
{
	GetImg(obj, data, im);

    if (im)
        return INT2FIX(im->Ysize);

    return Qnil;
}

/*
 *  call-seq:
 *    im.bands -> number
 *
 *  Get the number of bands in the image.
 */

static VALUE
header_bands(VALUE obj)
{
	GetImg(obj, data, im);

    if (im)
        return INT2FIX(im->Bands);

    return Qnil;
}

/*
 *  call-seq:
 *    im.band_fmt -> band_format_sym
 *
 *  Get the band format of the image.
 */

static VALUE
header_band_fmt(VALUE obj)
{
	GetImg(obj, data, im);

    if (im)
        return ID2SYM(header_band_fmt_to_id(im->BandFmt));

    return Qnil;
}

/*
 *  call-seq:
 *    im.x_res -> number
 *
 *  Get the x-resolution of the image.
 */

static VALUE
header_x_res(VALUE obj)
{
	GetImg(obj, data, im);

    if (im)
        return rb_float_new(im->Xres);

    return Qnil;
}

/*
 *  call-seq:
 *    im.y_res -> number
 *
 *  Get the y-resolution of the image.
 */

static VALUE
header_y_res(VALUE obj)
{
	GetImg(obj, data, im);

    if (im)
        return rb_float_new(im->Yres);

    return Qnil;
}

/*
 *  call-seq:
 *    im.x_offset -> number
 *
 *  Get the x-offset of the image.
 */

static VALUE
header_x_offset(VALUE obj)
{
	GetImg(obj, data, im);

    if (im)
        return INT2FIX(im->Xoffset);

    return Qnil;
}

/*
 *  call-seq:
 *     im.y_offset -> number
 *
 *  Get the y-offset of the image.
 */

static VALUE
header_y_offset(VALUE obj)
{
	GetImg(obj, data, im);

    if (im)
        return INT2FIX(im->Yoffset);

    return Qnil;
}

/* VipsImage macros with useful information */

/*
 *  call-seq:
 *     im.sizeof_element -> number
 *
 *  Returns the size of a single image band item, in bytes.
 */

static VALUE
header_sizeof_element(VALUE obj)
{
    GetImg(obj, data, im);

    if (im)
        return INT2FIX(IM_IMAGE_SIZEOF_ELEMENT(im));

    return Qnil;
}

/*
 *  call-seq:
 *     im.sizeof_pel -> number
 *
 *  Returns the size of a pixel in the image, in bytes.
 */

static VALUE
header_sizeof_pel(VALUE obj)
{
    GetImg(obj, data, im);

    if (im)
        return INT2FIX(IM_IMAGE_SIZEOF_PEL(im));

    return Qnil;
}

/*
 *  call-seq:
 *    im.sizeof_line -> number
 *
 *  Returns the size of all pixels in the row of the image, uncompressed, in
 *  bytes.
 */

static VALUE
header_sizeof_line(VALUE obj)
{
    GetImg(obj, data, im);

    if (im)
        return INT2FIX(IM_IMAGE_SIZEOF_LINE(im));

    return Qnil;
}

/*
 *  call-seq:
 *     im.n_elements -> number
 *
 *  Returns the number of elements in an image row, i.e. bands * x_size.
 */

static VALUE
header_n_elements(VALUE obj)
{
    GetImg(obj, data, im);

    if (im)
        return INT2FIX(IM_IMAGE_N_ELEMENTS(im));

    return Qnil;
}

static VALUE
header_meta_get(VALUE obj, const char* name)
{
    GetImg(obj, data, im);

    void *buf;
    size_t len;

    if (im_meta_get_blob(im, name, &buf, &len))
        return Qnil;

    return rb_tainted_str_new((char *)buf, len);
}

static VALUE
header_meta_get_string(VALUE obj, const char* name)
{
    GetImg(obj, data, im);

    char *str;
    VALUE result;

    if (vips_image_get_as_string(im, name, &str))
        vips_lib_error();
    result = rb_tainted_str_new(str, strlen(str));
    g_free(str);

    return result;
}

static void
header_meta_set_string(VALUE obj, const char* name, const char* value)
{
    GetImg(obj, data, im);

    vips_image_set_string(im, name, value);
}

static VALUE
header_meta_p(VALUE obj, const char* name)
{
    GetImg(obj, data, im);

    if (im_header_get_typeof(im, name))
        return Qtrue;

    return Qfalse;
}

/*
 *  call-seq:
 *     im.exif -> string
 *
 *  Returns a binary string containing the raw exif header data.
 */

static VALUE
header_exif(VALUE obj)
{
    return header_meta_get(obj, IM_META_EXIF_NAME);
}

/*
 *  call-seq:
 *    im.exif? -> true or false
 *
 *  Indicates whether the image has an exif header attached to it.
 */

static VALUE
header_exif_p(VALUE obj)
{
    return header_meta_p(obj, IM_META_EXIF_NAME);
}

/*
 *  call-seq:
 *     im.icc -> string
 *
 *  Returns a binary string containing the raw icc header data.
 */

static VALUE
header_icc(VALUE obj)
{
    return header_meta_get(obj, IM_META_ICC_NAME);
}

/*
 *  call-seq:
 *     im.icc? -> true or false
 *
 *  Indicates whether the image has an icc header attached to it.
 */

static VALUE
header_icc_p(VALUE obj)
{
    return header_meta_p(obj, IM_META_ICC_NAME);
}

/*
 *  call-seq:
 *     im.get(name) -> string
 *
 *  Return metadata item 'name' as a string.
 */

static VALUE
header_get(VALUE obj, VALUE name)
{
    return header_meta_get_string(obj, StringValuePtr(name));
}

/*
 *  call-seq:
 *     im.set(name, value)
 *
 *  Set metadata item 'name' to value.
 */

static VALUE
header_set(VALUE obj, VALUE name, VALUE value)
{
    header_meta_set_string(obj, 
                StringValuePtr(name), StringValuePtr(value));

    return Qnil;
}

/*
 * The header module holds image header operations that are common to readers,
 * writers and image objects.
 */

void
init_Header()
{
    mVIPSHeader = rb_define_module_under(mVIPS, "Header");

    rb_define_method(mVIPSHeader, "x_size", header_x_size, 0);
    rb_define_method(mVIPSHeader, "y_size", header_y_size, 0);
    rb_define_method(mVIPSHeader, "bands", header_bands, 0);
    rb_define_method(mVIPSHeader, "band_fmt", header_band_fmt, 0);
    rb_define_method(mVIPSHeader, "x_res", header_x_res, 0);
    rb_define_method(mVIPSHeader, "y_res", header_y_res, 0);
    rb_define_method(mVIPSHeader, "x_offset", header_x_offset, 0);
    rb_define_method(mVIPSHeader, "y_offset", header_y_offset, 0);
    rb_define_method(mVIPSHeader, "sizeof_element", header_sizeof_element, 0);
    rb_define_method(mVIPSHeader, "sizeof_pel", header_sizeof_pel, 0);
    rb_define_method(mVIPSHeader, "sizeof_line", header_sizeof_line, 0);
    rb_define_method(mVIPSHeader, "n_elements", header_n_elements, 0);
    rb_define_method(mVIPSHeader, "exif", header_exif, 0);
    rb_define_method(mVIPSHeader, "exif?", header_exif_p, 0);
    rb_define_method(mVIPSHeader, "icc", header_icc, 0);
    rb_define_method(mVIPSHeader, "icc?", header_icc_p, 0);
    rb_define_method(mVIPSHeader, "get", header_get, 1);
    rb_define_method(mVIPSHeader, "set", header_set, 2);

	id_notset = rb_intern("NOTSET");
	id_uchar = rb_intern("UCHAR");
	id_char = rb_intern("CHAR");
	id_ushort = rb_intern("USHORT");
	id_short = rb_intern("SHORT");
	id_uint = rb_intern("UINT");
	id_int = rb_intern("INT");
	id_float = rb_intern("FLOAT");
	id_complex = rb_intern("COMPLEX");
	id_double = rb_intern("DOUBLE");
	id_dbcomplex = rb_intern("DBCOMPLEX");

#if 0
    VALUE mVIPS = rb_define_module("VIPS");
#endif
}
