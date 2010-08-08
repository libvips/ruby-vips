#include "ruby_vips.h"
#include "image.h"

static VALUE
img_s_read(VALUE klass, VALUE path)
{
    VALUE new = img_alloc(cVIPSImage);
    vipsImg *data;

    Data_Get_Struct(new, vipsImg, data);

    if (!(data->in = (VipsImage*)im_open_vips(StringValuePtr(path))))
        vips_lib_error();

    return new;
}

static VALUE
img_s_read_jpeg(int argc, VALUE *argv, VALUE klass)
{
    VALUE path, shrink, ary;
    OutPartial(new, data, im);

	rb_scan_args(argc, argv, "11", &path, &shrink);

    if (!NIL_P(shrink)) {
        ary = rb_ary_new3(2, path, shrink);
        path = rb_ary_join(ary, rb_str_new_cstr(":"));
    }

    if (im_jpeg2vips(RSTRING_PTR(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_s_read_tiff(int argc, VALUE *argv, VALUE klass)
{
    VALUE path, page, ary;
	OutPartial(new, data, im);

	rb_scan_args(argc, argv, "11", &path, &page);

    if (!NIL_P(page)) {
        ary = rb_ary_new3(2, path, page);
        path = rb_ary_join(ary, rb_str_new_cstr(":"));
    }

    if (im_tiff2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_s_read_magick(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_magick2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_s_read_exr(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_exr2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_s_read_ppm(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_ppm2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_s_read_analyze(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_analyze2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_s_read_csv(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_csv2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_s_read_png(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_png2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_s_read_raw(VALUE klass, VALUE path, VALUE width, VALUE height, VALUE bpp,
    VALUE offset)
{
	OutPartial(new, data, im);

    if (im_raw2vips(StringValuePtr(path), im, NUM2INT(width), NUM2INT(height),
		NUM2INT(bpp), NUM2INT(offset)))
        vips_lib_error();

    return new;
}

void
init_format(void)
{
    rb_define_singleton_method(cVIPSImage, "read", img_s_read, 1);
    rb_define_singleton_method(cVIPSImage, "read_jpeg", img_s_read_jpeg, -1);
    rb_define_singleton_method(cVIPSImage, "read_tiff", img_s_read_tiff, -1);
    rb_define_singleton_method(cVIPSImage, "read_magick", img_s_read_magick, 1);
    rb_define_singleton_method(cVIPSImage, "read_exr", img_s_read_exr, 1);
    rb_define_singleton_method(cVIPSImage, "read_ppm", img_s_read_ppm, 1);
    rb_define_singleton_method(cVIPSImage, "read_analyze", img_s_read_analyze, 1);
    rb_define_singleton_method(cVIPSImage, "read_csv", img_s_read_csv, 1);
    rb_define_singleton_method(cVIPSImage, "read_png", img_s_read_png, 1);
    rb_define_singleton_method(cVIPSImage, "read_raw", img_s_read_raw, 5);
}
