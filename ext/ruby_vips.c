#include "ruby_vips.h"

VALUE mVIPS;
VALUE eVIPSError;

void
vips_lib_error()
{
    VALUE vips_error = rb_str_new2(im_error_buffer());
    im_error_clear();
    rb_raise(eVIPSError, "VIPS error: %s", RSTRING_PTR(vips_error));
}

static VALUE
vips_s_version_string()
{
    return rb_str_new2(im_version_string());
}

static VALUE
vips_s_version_array()
{
    VALUE arr = rb_ary_new2(3);
    int i;

    for(i = 0; i < 3; i++)
        rb_ary_push(arr, INT2FIX(im_version(i)));

    return arr;
}

void
Init_vips_ext()
{
	VALUE argv = rb_const_get(rb_mKernel, rb_intern("ARGV"));
	if(NIL_P(argv) || RARRAY_LEN(argv) == 0)
		im_init_world("");
	else
		im_init_world(RSTRING_PTR(RARRAY_PTR(argv)[0]));

    mVIPS = rb_define_module("VIPS");
    eVIPSError = rb_define_class_under(mVIPS, "Error", rb_eStandardError);

    rb_define_const(mVIPS, "LIB_VERSION", vips_s_version_string());
    rb_define_const(mVIPS, "LIB_VERSION_ARRAY", vips_s_version_array());
    init_header();
    init_image();
    init_mask();
    init_interpolator();

    init_arithmetic();
    init_resample();
    init_convolution();
    init_conversion();
    init_boolean();
    init_colour();
    init_freq_filt();
	init_histograms_lut();
    init_morphology();
    init_mosaicing();
	init_relational();
    init_writer();
    init_reader();
}

