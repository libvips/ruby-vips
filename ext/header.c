#include "ruby_vips.h"
#include "image.h"

static VALUE
gvalue_to_rb(GValue *val) {
	GType type = G_VALUE_TYPE(val);
	char *buf;
	size_t len;

    switch(type) {
		case G_TYPE_INT:    return INT2NUM(g_value_get_int(val));
		case G_TYPE_DOUBLE: return DBL2NUM(g_value_get_double(val));
		case G_TYPE_STRING: return rb_tainted_str_new2(g_value_get_string(val));
		default:
			if( type == IM_TYPE_BLOB ) {
				buf = im_blob_get(val, &len);
				return rb_tainted_str_new(buf, len);
			}
			else if( type == IM_TYPE_REF_STRING )
				return rb_tainted_str_new2(im_ref_string_get(val));
	}

	return Qnil;
}

static void*
header_iterate(IMAGE *im, const char *field, GValue *val, void *a)
{
	VALUE header = (VALUE)a;
    VALUE header_key, header_val;

	header_val = gvalue_to_rb(val);
	header_key = rb_tainted_str_new2(field);
    rb_hash_aset(header, header_key, header_val);

    return NULL;
}

static void*
exif_iterate(IMAGE *im, const char *field, GValue *val, void *a)
{
	VALUE header = (VALUE)a;
    VALUE header_key, header_val;

	/* We identify exif header fields that are strings by their GType.
     */
    if( G_VALUE_TYPE(val) == IM_TYPE_REF_STRING ) {
		header_val = gvalue_to_rb(val);
		header_key = rb_tainted_str_new2(field);
		rb_hash_aset(header, header_key, header_val);
	}

    return NULL;
}

static VALUE
img_header_to_hash(VALUE obj)
{
	GetImg(obj, data, im);
    VALUE header = rb_hash_new();

    im_header_map(im, (im_header_map_fn) header_iterate, (void *)header);

    return header;
}

static VALUE
img_exif_to_hash(VALUE obj)
{
	GetImg(obj, data, im);
    VALUE exif = rb_hash_new();

    im_header_map(im, (im_header_map_fn) exif_iterate, (void *)exif);

    return exif;
}

static VALUE
img_exif_p(VALUE obj)
{
	GetImg(obj, data, im);

	if (im_header_get_typeof(im, IM_META_EXIF_NAME))
		return Qtrue;

    return Qfalse;
}

static VALUE
img_icc_p(VALUE obj)
{
	GetImg(obj, data, im);

	if (im_header_get_typeof(im, IM_META_ICC_NAME))
		return Qtrue;

    return Qfalse;
}

static VALUE
img_exif(VALUE obj)
{
	GetImg(obj, data, im);
	void *buf;
	size_t len;

	if (im_meta_get_blob(im, IM_META_EXIF_NAME, &buf, &len))
		return Qnil;

	return rb_tainted_str_new((char *)buf, len);
}

static VALUE
img_icc(VALUE obj)
{
	GetImg(obj, data, im);
	void *buf;
	size_t len;

	if( im_meta_get_blob(im, IM_META_ICC_NAME, &buf, &len))
		return Qnil;

	return rb_tainted_str_new((char *)buf, len);
}

#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 22

static VALUE
img_remove_icc(VALUE obj)
{
	GetImg(obj, data, im);

	if (im_meta_get_typeof(im, IM_META_ICC_NAME) &&
		!im_meta_remove(im, IM_META_ICC_NAME))
		vips_lib_error();

	return obj;
}

static VALUE
img_remove_exif(VALUE obj)
{
	GetImg(obj, data, im);

	if (im_meta_get_typeof(im, IM_META_EXIF_NAME) &&
		!im_meta_remove(im, IM_META_EXIF_NAME) )
		vips_lib_error();

	return obj;
}

#endif

void
init_header(void)
{
    rb_define_method(cVIPSImage, "header_to_hash", img_header_to_hash, 0);
    rb_define_method(cVIPSImage, "exif_to_hash", img_exif_to_hash, 0);
	rb_define_method(cVIPSImage, "exif?", img_exif_p, 0);
	rb_define_method(cVIPSImage, "icc?", img_icc_p, 0);
    rb_define_method(cVIPSImage, "exif", img_exif, 0);
    rb_define_method(cVIPSImage, "icc", img_icc, 0);

	#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 22
	rb_define_method(cVIPSImage, "remove_exif", img_remove_exif, 0);
	rb_define_method(cVIPSImage, "remove_icc", img_remove_icc, 0);
	#endif
}

