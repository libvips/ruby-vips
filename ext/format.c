#include "ruby_vips.h"
#include "image.h"
#include "format.h"

VALUE cVIPSFormat;
VALUE hVIPSFormats;
static VALUE fmt_nickname(VALUE);

static void*
fmt_register(VipsFormatClass *fmt)
{
    VALUE obj;
    obj = Data_Wrap_Struct(cVIPSFormat, 0, 0, fmt);
    rb_hash_aset(hVIPSFormats, rb_str_intern(fmt_nickname(obj)), obj);
    return NULL;
}

static void
fmt_register_builtin(void)
{
    vips_class_map_concrete_all(
        g_type_from_name( "VipsFormat" ),
        (void *) fmt_register,
        (void *) NULL
    );
}

/* Data accessors. */

static VALUE
fmt_priority(VALUE obj)
{
    VipsFormatClass *fmt;
    Data_Get_Struct(obj, VipsFormatClass, fmt);
    return INT2FIX(fmt->priority);
}

static VALUE
fmt_suffs(VALUE obj)
{
    VipsFormatClass *fmt;
  	const char **p;
    VALUE suffixes = rb_ary_new();

    Data_Get_Struct(obj, VipsFormatClass, fmt);

	  for( p = fmt->suffs; *p; p++ )
		    rb_ary_push(suffixes, rb_tainted_str_new2(*p));

    return suffixes;
}

static VALUE
fmt_nickname(VALUE obj)
{
    VipsObjectClass *v_obj;
    Data_Get_Struct(obj, VipsObjectClass, v_obj);
    return rb_str_new2(v_obj->nickname);
}

static VALUE
fmt_description(VALUE obj)
{
    VipsObjectClass *v_obj;
    Data_Get_Struct(obj, VipsObjectClass, v_obj);
    return rb_str_new2(v_obj->description);
}

/* Calls to VipsFormat function pointers. */

static VALUE
vips_flags_to_rb(VipsFormatFlags flags)
{
    VALUE r_flags = rb_ary_new();
    if(flags & VIPS_FORMAT_PARTIAL)
        rb_ary_push(r_flags, ID2SYM(rb_intern("PARTIAL")));

    return r_flags;
}

static VALUE
fmt_flags(VALUE obj, VALUE path)
{
    VipsFormatFlags flags;
    VipsFormatClass *fmt;

    Data_Get_Struct(obj, VipsFormatClass, fmt);

    if( !(flags = vips_format_get_flags(fmt, StringValuePtr(path))) )
        return rb_ary_new();

    return vips_flags_to_rb(flags);
}

static VALUE
fmt_header(VALUE obj, VALUE path)
{
    VipsFormatClass *fmt;
    VipsImage *vips_image;
    vipsImg *im;
    VALUE new = img_init_partial();

    Data_Get_Struct(obj, VipsFormatClass, fmt);
    Data_Get_Struct(new, vipsImg, im);

	  if( fmt->header(StringValuePtr(path), im->in) ) {
		    im_close(im->in);
		    vips_lib_error();
	  }

    return new;
}

/* Other methods from libvips format.h */

static VALUE
fmt_s_for_file(VALUE klass, VALUE path)
{
    VALUE obj;
    VipsFormatClass *fmt;

    if( !(fmt = vips_format_for_file( StringValuePtr(path) )) )
        vips_lib_error();
 
    obj = Data_Wrap_Struct(cVIPSFormat, 0, 0, fmt);
 
    return obj;
}

static VALUE
fmt_s_for_name(VALUE klass, VALUE file)
{
    VALUE obj;
    VipsFormatClass *fmt;

    if( !(fmt = vips_format_for_name( StringValuePtr(file) )) )
        vips_lib_error();
 
    obj = Data_Wrap_Struct(cVIPSFormat, 0, 0, fmt);
 
    return obj;
}

/* Image functions from libvips format.h */

static VALUE
img_s_read(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (vips_format_read(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_write(VALUE obj, VALUE path)
{
	GetImg(obj, data, im);

    if (vips_format_write(im, StringValuePtr(path)))
        vips_lib_error();

    return obj;
}

static VALUE
img_s_from_jpeg(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_jpeg2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_to_jpeg(VALUE obj, VALUE path)
{
	GetImg(obj, data, im);

    if (im_vips2jpeg(im, StringValuePtr(path)))
        vips_lib_error();

    return obj;
}

static VALUE
img_to_bufjpeg(VALUE obj, VALUE qfac)
{
    VipsImage *im_out;
    char *buf = NULL;
    int length;
    VALUE str;
	GetImg(obj, data, im);

    if (!(im_out = im_open("img_to_bufjpeg", "p")))
        vips_lib_error();

    if (im_vips2bufjpeg(im, im_out, NUM2INT(qfac), &buf, &length)) {
		im_free(im_out);
        vips_lib_error();
	}

    str = rb_tainted_str_new(buf, length);
    im_close(im_out);

    return str;
}

static VALUE
img_s_from_tiff(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_tiff2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_to_tiff(VALUE obj, VALUE path)
{
	GetImg(obj, data, im);

    if (im_vips2jpeg(im, StringValuePtr(path)))
        vips_lib_error();

    return obj;
}

static VALUE
img_s_from_magick(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_magick2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_s_from_exr(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_exr2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_s_from_ppm(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_ppm2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_to_ppm(VALUE obj, VALUE path)
{
	GetImg(obj, data, im);

    if (im_vips2ppm(im, StringValuePtr(path)))
        vips_lib_error();

    return obj;
}

static VALUE
img_s_from_analyze(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_analyze2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_s_from_csv(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_csv2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_to_csv(VALUE obj, VALUE path)
{
	GetImg(obj, data, im);

    if (im_vips2csv(im, StringValuePtr(path)))
        vips_lib_error();

    return obj;
}

static VALUE
img_s_from_png(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_png2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

static VALUE
img_to_png(VALUE obj, VALUE path)
{
	GetImg(obj, data, im);

    if (im_vips2png(im, StringValuePtr(path)))
        vips_lib_error();

    return obj;
}

#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 23

static VALUE
img_to_bufpng(VALUE obj, VALUE compression, VALUE interlace)
{
    char *buf = NULL;
    size_t length;
    VALUE str;
	GetImg(obj, data, im);

    if (im_vips2bufpng(im, NULL, NUM2INT(compression), NUM2INT(interlace),
		&buf, &length))
        vips_lib_error();

    str = rb_tainted_str_new(buf, length);
	im_free(buf);

    return str;
}

#endif

static VALUE
img_s_from_raw(VALUE klass, VALUE path, VALUE width, VALUE height, VALUE bpp,
    VALUE offset)
{
	OutPartial(new, data, im);

    if (im_raw2vips(StringValuePtr(path), im, NUM2INT(width), NUM2INT(height),
		NUM2INT(bpp), NUM2INT(offset)))
        vips_lib_error();

    return new;
}

/* VIPS 7.20.4 requires a file handle, and this is being deprecated
static VALUE
img_to_raw(VALUE obj, VALUE path)
{
    vipsImg *img;

    Data_Get_Struct(obj, vipsImg, img);

    if( im_vips2raw(img->in, StringValuePtr(path)) )
        vips_lib_error();

    return obj;
}

static VALUE
img_s_from_mat(VALUE klass, VALUE path)
{
	OutPartial(new, data, im);

    if (im_mat2vips(StringValuePtr(path), im))
        vips_lib_error();

    return new;
}

/* libvips 7.20.04 doesn't support reading or writing rad images
static VALUE
img_s_from_rad(VALUE klass, VALUE path)
{
    vipsImg *img;
    VALUE new = img_init_partial();

    Data_Get_Struct(new, vipsImg, img);

    if( im_rad2vips(StringValuePtr(path), img->in) )
        vips_lib_error();

    return new;
}

static VALUE
img_to_rad(VALUE obj, VALUE path)
{
    vipsImg *img;

    Data_Get_Struct(obj, vipsImg, img);

    if( im_vips2rad(img->in, StringValuePtr(path)) )
        vips_lib_error();

    return obj;
}
*/

void
init_format(void)
{
    /* VIPS::Format class */
    cVIPSFormat = rb_define_class_under(mVIPS, "Format", rb_cObject);
    rb_define_singleton_method(cVIPSFormat, "for_file", fmt_s_for_file, 1);
    rb_define_singleton_method(cVIPSFormat, "for_name", fmt_s_for_name, 1);
    rb_define_method(cVIPSFormat, "priority", fmt_priority, 0);
    rb_define_method(cVIPSFormat, "suffs", fmt_suffs, 0);
    rb_define_method(cVIPSFormat, "nickname", fmt_nickname, 0);
    rb_define_method(cVIPSFormat, "description", fmt_description, 0);
    rb_define_method(cVIPSFormat, "flags", fmt_flags, 1);
    rb_define_method(cVIPSFormat, "header", fmt_header, 1);

    /* VIPS::Format::FORMATS hash */
    hVIPSFormats = rb_hash_new();
    rb_define_const(cVIPSFormat, "FORMATS", hVIPSFormats);
    fmt_register_builtin();

    /* VIPS::Image methods */
    rb_define_singleton_method(cVIPSImage, "read", img_s_read, 1);
    rb_define_singleton_method(cVIPSImage, "from_jpeg", img_s_from_jpeg, 1);
    rb_define_singleton_method(cVIPSImage, "from_tiff", img_s_from_tiff, 1);
    rb_define_singleton_method(cVIPSImage, "from_magick", img_s_from_magick, 1);
    rb_define_singleton_method(cVIPSImage, "from_exr", img_s_from_exr, 1);
    rb_define_singleton_method(cVIPSImage, "from_ppm", img_s_from_ppm, 1);
    rb_define_singleton_method(cVIPSImage, "from_analyze", img_s_from_analyze, 1);
    rb_define_singleton_method(cVIPSImage, "from_csv", img_s_from_csv, 1);
    rb_define_singleton_method(cVIPSImage, "from_png", img_s_from_png, 1);
    rb_define_singleton_method(cVIPSImage, "from_raw", img_s_from_raw, 5);
    rb_define_method(cVIPSImage, "write", img_write, 1);
    rb_define_method(cVIPSImage, "to_jpeg", img_to_jpeg, 1);
    rb_define_method(cVIPSImage, "to_bufjpeg", img_to_bufjpeg, 1);
    rb_define_method(cVIPSImage, "to_tiff", img_to_tiff, 1);
    rb_define_method(cVIPSImage, "to_ppm", img_to_ppm, 1);
    rb_define_method(cVIPSImage, "to_csv", img_to_csv, 1);
    rb_define_method(cVIPSImage, "to_png", img_to_png, 1);

	#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 23
    rb_define_method(cVIPSImage, "to_bufpng", img_to_bufpng, 2);
	#endif
}

