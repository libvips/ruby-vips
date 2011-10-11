#include "ruby_vips.h"
#include "image.h"
#include "header.h"

ID id_iv__vips_fmt;

VipsFormatClass*
reader_get_fmt_class(VALUE klass)
{
    VipsFormatClass *fmt_class;
    VALUE nickname_v;
    char *nickname;

    nickname_v = rb_ivar_get(klass, id_iv__vips_fmt);
    if (NIL_P(nickname_v))
        return NULL;

    nickname = StringValuePtr(nickname_v);

	if (!(fmt_class = (VipsFormatClass*)vips_class_find("VipsFormat", nickname)))
		return NULL;

    return fmt_class;
}

/* :nodoc: */

static VALUE
reader_read_header(VALUE obj, VALUE path)
{
    VipsImage *im_new;
    VipsFormatClass *fmt_class = reader_get_fmt_class(CLASS_OF(obj));
    GetImg(obj, data, im);

	if (!fmt_class)
		return Qfalse;

    if (!(im_new = (VipsImage*)im_open("reader_read_header", "p")))
        vips_lib_error();

    if (fmt_class->header(StringValuePtr(path), im_new)) {
        im_close(im_new);
        vips_lib_error();
    }

    data->in = im_new;

    return Qtrue;
}

/* :nodoc: */

static VALUE
reader_s_recognized_p(VALUE klass, VALUE path)
{
    VipsFormatClass *fmt_class = reader_get_fmt_class(klass);

    if (fmt_class && fmt_class->is_a && fmt_class->is_a(StringValuePtr(path)))
        return Qtrue;

    return Qfalse;
}

/* :nodoc: */

static VALUE
reader_read_internal(VALUE obj, VALUE path)
{
    VipsImage *im;

    if (!(im = im_open(StringValuePtr(path), "r")))
        vips_lib_error();

    return img_init(cVIPSImage, im);
}

/* :nodoc: */

static VALUE
jpeg_read_internal(VALUE obj, VALUE path)
{
    OutPartial(new, data, im);

    if (im_jpeg2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

/* :nodoc: */

static VALUE
tiff_read_internal(VALUE obj, VALUE path)
{
    OutPartial(new, data, im);

    if (im_tiff2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

/* :nodoc: */

static VALUE
magick_read_internal(VALUE obj, VALUE path)
{
	OutPartial(new, data, im);

    if (im_magick2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

/* :nodoc: */

static VALUE
exr_read_internal(VALUE obj, VALUE path)
{
	OutPartial(new, data, im);

    if (im_exr2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

/* :nodoc: */

static VALUE
ppm_read_internal(VALUE obj, VALUE path)
{
	OutPartial(new, data, im);

    if (im_ppm2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

/* :nodoc: */

static VALUE
analyze_read_internal(VALUE obj, VALUE path)
{
	OutPartial(new, data, im);

    if (im_analyze2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

/* :nodoc: */

static VALUE
csv_read_internal(VALUE obj, VALUE path)
{
	OutPartial(new, data, im);

    if (im_csv2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

/* :nodoc: */

static VALUE
png_read_internal(VALUE obj, VALUE path)
{
	OutPartial(new, data, im);

    if (im_png2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

/* :nodoc: */

static VALUE
raw_read_internal(VALUE obj, VALUE path, VALUE width, VALUE height, VALUE bpp,
    VALUE offset)
{
	OutPartial(new, data, im);

    if (im_raw2vips(StringValuePtr(path), im, NUM2INT(width), NUM2INT(height),
		NUM2INT(bpp), NUM2INT(offset)))
        vips_lib_error();

    return new;
}

/* :nodoc: */

static VALUE
vips_read_internal(VALUE obj, VALUE path)
{
    VipsImage *im;

    if (!(im = (VipsImage *)im_open(StringValuePtr(path),"r")))
        vips_lib_error();

    return img_init(cVIPSImage, im);
}

static void
reader_fmt_set(VALUE klass, const char* val)
{
    rb_ivar_set(klass, id_iv__vips_fmt, rb_str_new_cstr(val));
}

/*
 *  Base class for image format readers.
 */

void
init_Reader()
{
    id_iv__vips_fmt = rb_intern("@_vips_fmt");

    VALUE reader = rb_define_class_under(mVIPS, "Reader", rb_cObject);
    rb_include_module(reader, mVIPSHeader);
    rb_define_alloc_func(reader, img_alloc);

    rb_define_singleton_method(reader, "recognized?", reader_s_recognized_p, 1);
    rb_define_private_method(reader, "read_header", reader_read_header, 1);
    rb_define_private_method(reader, "read_internal", reader_read_internal, 1);

    /*
     * Read JPEG images.
     */

    VALUE jpeg_reader = rb_define_class_under(mVIPS, "JPEGReader", reader);
    rb_define_private_method(jpeg_reader, "read_internal", jpeg_read_internal, 1);
    reader_fmt_set(jpeg_reader, "jpeg");

    /*
     * Read TIFF images.
     */

    VALUE tiff_reader = rb_define_class_under(mVIPS, "TIFFReader", reader);
    rb_define_private_method(tiff_reader, "read_internal", tiff_read_internal, 1);
    reader_fmt_set(tiff_reader, "tiff");

    /*
     * Read PPM images.
     */

    VALUE ppm_reader = rb_define_class_under(mVIPS, "PPMReader", reader);
    rb_define_private_method(ppm_reader, "read_internal", ppm_read_internal, 1);
    reader_fmt_set(ppm_reader, "ppm");

    /*
     * Read PNG images.
     */

    VALUE png_reader = rb_define_class_under(mVIPS, "PNGReader", reader);
    rb_define_private_method(png_reader, "read_internal", png_read_internal, 1);
    reader_fmt_set(png_reader, "png");

    /*
     * Read CSV images.
     */

    VALUE csv_reader = rb_define_class_under(mVIPS, "CSVReader", reader);
    rb_define_private_method(csv_reader, "read_internal", csv_read_internal, 1);
    reader_fmt_set(csv_reader, "csv");

    /*
     * Read EXR images.
     */

    VALUE exr_reader = rb_define_class_under(mVIPS, "EXRReader", reader);
    rb_define_private_method(exr_reader, "read_internal", exr_read_internal, 1);
    reader_fmt_set(exr_reader, "exr");

    /*
     * Read native VIPS images.
     */

    VALUE vips_reader = rb_define_class_under(mVIPS, "VIPSReader", reader);
    rb_define_private_method(vips_reader, "read_internal", vips_read_internal, 1);
    reader_fmt_set(vips_reader, "vips");

    /*
     * Read images using a magick library.
     */

    VALUE magick_reader = rb_define_class_under(mVIPS, "MagickReader", reader);
    rb_define_private_method(magick_reader, "read_internal", magick_read_internal, 1);
    reader_fmt_set(magick_reader, "magick");

    /*
     * Read Analyze images.
     */

    VALUE analyze_reader = rb_define_class_under(mVIPS, "AnalyzeReader", reader);
    rb_define_private_method(analyze_reader, "read_internal", analyze_read_internal, 1);
    reader_fmt_set(analyze_reader, "analyze");

    /*
     * Read RAW images.
     */

    VALUE raw_reader = rb_define_class_under(mVIPS, "RAWReader", reader);
    rb_define_private_method(raw_reader, "read_internal", raw_read_internal, 5);
    reader_fmt_set(raw_reader, "raw");

#if 0
    VALUE mVIPS = rb_define_module("VIPS");
#endif
}
