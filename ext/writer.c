#include "ruby_vips.h"
#include "image.h"
#include "header.h"

static VALUE
writer_initialize(VALUE obj, VALUE image)
{
    GetImg(image, data, im);
    GetImg(obj, data_new, im_new);

    img_add_dep(data_new, image);
    if (im_copy(im, im_new))
        vips_lib_error();

    return obj;
}

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
#endif
    return Qtrue;
}

static VALUE
writer_exif_set(VALUE obj, VALUE str)
{
    return writer_meta_set(obj, IM_META_EXIF_NAME, str);
}

static VALUE
writer_remove_exif(VALUE obj)
{
    return writer_meta_remove(obj, IM_META_EXIF_NAME);
}

static VALUE
writer_icc_set(VALUE obj, VALUE str)
{
    return writer_meta_set(obj, IM_META_ICC_NAME, str);
}

static VALUE
writer_remove_icc(VALUE obj)
{
    return writer_meta_remove(obj, IM_META_ICC_NAME);
}

static VALUE
jpeg_buf_internal(VALUE obj, VALUE quality)
{
    VipsImage *im_out;
    char *buf = NULL;
    int length;
    GetImg(obj, data, im);

    if (!(im_out = im_open("writer_jpeg_buf", "p")))
        vips_lib_error();

    if (im_vips2bufjpeg(im, im_out, NUM2INT(quality), &buf, &length)) {
		im_close(im_out);
        vips_lib_error();
	}

    im_close(im_out);

    return rb_tainted_str_new(buf, length);
}

static VALUE
jpeg_write_internal(VALUE obj, VALUE path)
{
    GetImg(obj, data, im);

    if (im_vips2jpeg(im, RSTRING_PTR(path)))
        vips_lib_error();

    return obj;
}

static VALUE
tiff_write_internal(VALUE obj, VALUE path)
{
    GetImg(obj, data, im);

    if (im_vips2tiff(im, RSTRING_PTR(path)))
        vips_lib_error();

    return obj;
}

static VALUE
ppm_write_internal(VALUE obj, VALUE path)
{
    GetImg(obj, data, im);

    if (im_vips2ppm(im, RSTRING_PTR(path)))
        vips_lib_error();

    return obj;
}

#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 23

static VALUE
png_buf_internal(VALUE obj, VALUE compression, VALUE interlace)
{
    VipsImage *im_out;
    char *buf;
    int length;
    GetImg(obj, data, im);

    if (!(im_out = im_open("writer_png_buf", "p")))
        vips_lib_error();

    if (im_vips2bufpng(im, im_out, NUM2INT(compression), NUM2INT(interlace),
        &buf, &length)) {
		im_close(im_out);
        vips_lib_error();
	}

    im_close(im_out);

    return rb_tainted_str_new(buf, length);
}

#endif

static VALUE
png_write_internal(VALUE obj, VALUE path)
{
    GetImg(obj, data, im);

    if (im_vips2png(im, RSTRING_PTR(path)))
        vips_lib_error();

    return obj;
}

static VALUE
csv_write_internal(VALUE obj, VALUE path)
{
    GetImg(obj, data, im);

    if (im_vips2csv(im, RSTRING_PTR(path)))
        vips_lib_error();

    return obj;
}

static VALUE
vips_write_internal(VALUE obj, VALUE path)
{
    GetImg(obj, data, im);
    OutPartial(new, data_new, im_new);

    if (!(im_new = (VipsImage *)im_openout(RSTRING_PTR(path))))
        vips_lib_error();

    if (im_copy(im, im_new))
        vips_lib_error();

    return obj;
}

void
init_writer()
{
    VALUE writer = rb_define_class_under(mVIPS, "Writer", rb_cObject);
    rb_include_module(writer, mVIPSHeader);
    rb_define_alloc_func(writer, img_init_partial_anyclass);
    rb_define_method(writer, "initialize", writer_initialize, 1);
    rb_define_method(writer, "image", writer_image, 0);
    rb_define_method(writer, "exif=", writer_exif_set, 1);
    rb_define_method(writer, "remove_exif", writer_remove_exif, 0);
    rb_define_method(writer, "icc=", writer_icc_set, 1);
    rb_define_method(writer, "remove_icc", writer_remove_icc, 0);

    VALUE jpeg_writer = rb_define_class_under(mVIPS, "JPEGWriter", writer);
    rb_define_private_method(jpeg_writer, "buf_internal", jpeg_buf_internal, 1);
    rb_define_private_method(jpeg_writer, "write_internal", jpeg_write_internal, 1);

    VALUE tiff_writer = rb_define_class_under(mVIPS, "TIFFWriter", writer);
    rb_define_private_method(tiff_writer, "write_internal", tiff_write_internal, 1);

    VALUE ppm_writer = rb_define_class_under(mVIPS, "PPMWriter", writer);
    rb_define_private_method(ppm_writer, "write_internal", ppm_write_internal, 1);

    VALUE png_writer = rb_define_class_under(mVIPS, "PNGWriter", writer);
    rb_define_private_method(png_writer, "write_internal", png_write_internal, 1);
#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 23
    rb_define_private_method(png_writer, "buf_internal", png_buf_internal, 2);
#endif

    VALUE csv_writer = rb_define_class_under(mVIPS, "CSVWriter", writer);
    rb_define_private_method(csv_writer, "write_internal", csv_write_internal, 1);

    VALUE vips_writer = rb_define_class_under(mVIPS, "VIPSWriter", writer);
    rb_define_private_method(vips_writer, "write_internal", vips_write_internal, 1);
}
