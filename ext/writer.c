#include "ruby_vips.h"

/* :nodoc: */

static VALUE
writer_initialize(int argc, VALUE *argv, VALUE obj)
{
    VALUE image, opts;
	rb_scan_args(argc, argv, "11", &image, &opts);
    GetImg(image, data, im);
    GetImg(obj, data_new, im_new);

    img_add_dep(data_new, image);
    if (im_copy(im, im_new))
        vips_lib_error();

    return obj;
}

/*
 *  call-seq:
 *     writer.image -> image
 * 
 *  Returns the image associated with *self*.
 */

static VALUE
writer_image(VALUE obj)
{
    GetImg(obj, data, im);

    if(data->deps)
        return data->deps[0];

    return Qnil;
}

static VALUE
writer_meta_set(VALUE obj, const char* name, VALUE str)
{
    GetImg(obj, data, im);

	size_t len = RSTRING_LEN(str);
    void *buf = malloc(len);
    memcpy(buf, RSTRING_PTR(str), len);

    if (im_meta_set_blob(im, name, (im_callback_fn)xfree, buf, len)) {
        xfree(buf);
        vips_lib_error();
    }

    return str;
}

static VALUE
writer_meta_remove(VALUE obj, const char* name)
{
    GetImg(obj, data, im);
#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 22
	if (im_meta_remove(im, name))
		return Qfalse;
#else
    rb_raise(eVIPSError, "This method is not implemented in your version of VIPS");
#endif
    return Qtrue;
}

/*
 *  call-seq:
 *     writer.exif = exif_data
 *
 *  Sets the exif header of the writer to <i>exif_data</i>. This will be
 *  written only if the file format supports embedded exif data.
 *
 */

static VALUE
writer_exif_set(VALUE obj, VALUE str)
{
    return writer_meta_set(obj, IM_META_EXIF_NAME, str);
}

/*
 *  call-seq:
 *     writer.remove_exif -> true or false
 *
 *  Removes exif data associated with *self*.
 */

static VALUE
writer_remove_exif(VALUE obj)
{
    return writer_meta_remove(obj, IM_META_EXIF_NAME);
}

/*
 *  call-seq:
 *     writer.icc = icc_data
 *
 *  Sets the icc header of the writer to <i>icc_data</i>. This will be written
 *  only if the file format supports embedded icc data.
 *
 */

static VALUE
writer_icc_set(VALUE obj, VALUE str)
{
    return writer_meta_set(obj, IM_META_ICC_NAME, str);
}

/*
 *  call-seq:
 *     writer.remove_icc -> true or false
 *
 *  Removes icc data associated with *self*.
 */

static VALUE
writer_remove_icc(VALUE obj)
{
    return writer_meta_remove(obj, IM_META_ICC_NAME);
}

/* :nodoc: */

static VALUE
writer_write_internal(VALUE obj, VALUE path)
{
    VipsImage *out;
    GetImg(obj, data, im);

    if (!(out = im_open(StringValuePtr(path), "w")) || im_copy(im, out))
        vips_lib_error();

    im_close(out);

    return obj;
}

/* :nodoc: */

static VALUE
jpeg_buf_internal(VALUE obj, VALUE quality)
{
    char *buf = NULL;
    int length;

    GetImg(obj, data, im);

    if (im_vips2bufjpeg(im, NULL, NUM2INT(quality), &buf, &length)) 
        vips_lib_error();

    return rb_tainted_str_new(buf, length);
}

/* :nodoc: */

static VALUE
jpeg_write_internal(VALUE obj, VALUE path)
{
    GetImg(obj, data, im);

    if (im_vips2jpeg(im, RSTRING_PTR(path)))
        vips_lib_error();

    return obj;
}

/* :nodoc: */

static VALUE
tiff_write_internal(VALUE obj, VALUE path)
{
    GetImg(obj, data, im);

    if (im_vips2tiff(im, RSTRING_PTR(path)))
        vips_lib_error();

    return obj;
}

/* :nodoc: */

static VALUE
ppm_write_internal(VALUE obj, VALUE path)
{
    GetImg(obj, data, im);

    if (im_vips2ppm(im, RSTRING_PTR(path)))
        vips_lib_error();

    return obj;
}

/* :nodoc: */

static VALUE
png_buf_internal(VALUE obj, VALUE compression, VALUE interlace)
{
#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 23
    char *buf;
    size_t length;
    GetImg(obj, data, im);

    if (im_vips2bufpng(im, NULL, NUM2INT(compression), NUM2INT(interlace),
        &buf, &length)) 
        vips_lib_error();

    return rb_tainted_str_new(buf, length);
#else
    rb_raise(eVIPSError, "This method is not implemented in your version of VIPS");
#endif
}

/* :nodoc: */

static VALUE
png_write_internal(VALUE obj, VALUE path)
{
    GetImg(obj, data, im);

    if (im_vips2png(im, RSTRING_PTR(path)))
        vips_lib_error();

    return obj;
}

/* :nodoc: */

static VALUE
csv_write_internal(VALUE obj, VALUE path)
{
    GetImg(obj, data, im);

    if (im_vips2csv(im, RSTRING_PTR(path)))
        vips_lib_error();

    return obj;
}

/* :nodoc: */

static VALUE
vips_write_internal(VALUE obj, VALUE path)
{
    VipsImage *im_new;
    GetImg(obj, data, im);

    if (!(im_new = (VipsImage *)im_openout(RSTRING_PTR(path))))
        vips_lib_error();

    if (im_copy(im, im_new))
        vips_lib_error();

    im_close(im_new);

    return obj;
}

/*
 *  Base class for image format readers.
 */

void
init_Writer(void)
{
    VALUE writer = rb_define_class_under(mVIPS, "Writer", rb_cObject);

    rb_include_module(writer, mVIPSHeader);
    rb_define_alloc_func(writer, img_init_partial_anyclass);
    rb_define_method(writer, "initialize", writer_initialize, -1);
    rb_define_method(writer, "image", writer_image, 0);
    rb_define_method(writer, "exif=", writer_exif_set, 1);
    rb_define_method(writer, "remove_exif", writer_remove_exif, 0);
    rb_define_method(writer, "icc=", writer_icc_set, 1);
    rb_define_method(writer, "remove_icc", writer_remove_icc, 0);
    rb_define_private_method(writer, "write_internal", writer_write_internal, 1);

    /*
     * Write JPEG images.
     */

    VALUE jpeg_writer = rb_define_class_under(mVIPS, "JPEGWriter", writer);
    rb_define_private_method(jpeg_writer, "buf_internal", jpeg_buf_internal, 1);
    rb_define_private_method(jpeg_writer, "write_internal", jpeg_write_internal, 1);

    /*
     * Write TIFF images.
     */

    VALUE tiff_writer = rb_define_class_under(mVIPS, "TIFFWriter", writer);
    rb_define_private_method(tiff_writer, "write_internal", tiff_write_internal, 1);

    /*
     * Write PPM images.
     */

    VALUE ppm_writer = rb_define_class_under(mVIPS, "PPMWriter", writer);
    rb_define_private_method(ppm_writer, "write_internal", ppm_write_internal, 1);

    /*
     * Write PNG images.
     */

    VALUE png_writer = rb_define_class_under(mVIPS, "PNGWriter", writer);
    rb_define_private_method(png_writer, "write_internal", png_write_internal, 1);
    rb_define_private_method(png_writer, "buf_internal", png_buf_internal, 2);

    /*
     * Write CSV images.
     */

    VALUE csv_writer = rb_define_class_under(mVIPS, "CSVWriter", writer);
    rb_define_private_method(csv_writer, "write_internal", csv_write_internal, 1);

    /*
     * Write native VIPS images.
     */

    VALUE vips_writer = rb_define_class_under(mVIPS, "VIPSWriter", writer);
    rb_define_private_method(vips_writer, "write_internal", vips_write_internal, 1);

#if 0
    VALUE mVIPS = rb_define_module("VIPS");
#endif
}
