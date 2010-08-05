#include "ruby_vips.h"
#include "image.h"
#include "mask.h"

static ID id_perceptual, id_relative_colorimetric, id_saturation,
	id_absolute_colorimetric;

static VALUE
img_lab_to_lch(VALUE obj)
{
	RUBY_VIPS_UNARY(im_Lab2LCh);
}

static VALUE
img_lch_to_lab(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LCh2Lab);
}

static VALUE
img_labq_to_xyz(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LabQ2XYZ);
}

static VALUE
img_rad_to_float(VALUE obj)
{
	RUBY_VIPS_UNARY(im_rad2float);
}

static VALUE
img_float_to_rad(VALUE obj)
{
	RUBY_VIPS_UNARY(im_float2rad);
}

static VALUE
img_lch_to_ucs(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LCh2UCS);
}

static VALUE
img_lab_to_labq(VALUE obj)
{
	RUBY_VIPS_UNARY(im_Lab2LabQ);
}

static VALUE
img_lab_to_labs(VALUE obj)
{
	RUBY_VIPS_UNARY(im_Lab2LabS);
}

static VALUE
img_lab_to_xyz(VALUE obj)
{
	RUBY_VIPS_UNARY(im_Lab2XYZ);
}

static VALUE
img_lab_to_xyz_temp(VALUE obj, VALUE x0, VALUE y0, VALUE z0)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_Lab2XYZ_temp(im, im_new, NUM2DBL(x0), NUM2DBL(y0), NUM2DBL(z0)))
        vips_lib_error();

    return new;
}

static VALUE
img_lab_to_ucs(VALUE obj)
{
	RUBY_VIPS_UNARY(im_Lab2UCS);
}

static VALUE
img_labq_to_lab(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LabQ2Lab);
}

static VALUE
img_labq_to_labs(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LabQ2LabS);
}

static VALUE
img_labs_to_labq(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LabS2LabQ);
}

static VALUE
img_labs_to_lab(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LabS2Lab);
}

static VALUE
img_ucs_to_xyz(VALUE obj)
{
	RUBY_VIPS_UNARY(im_UCS2XYZ);
}

static VALUE
img_ucs_to_lch(VALUE obj)
{
	RUBY_VIPS_UNARY(im_UCS2LCh);
}

static VALUE
img_ucs_to_lab(VALUE obj)
{
	RUBY_VIPS_UNARY(im_UCS2Lab);
}

static VALUE
img_xyz_to_lab(VALUE obj)
{
	RUBY_VIPS_UNARY(im_XYZ2Lab);
}

static VALUE
img_xyz_to_lab_temp(VALUE obj, VALUE x0, VALUE y0, VALUE z0)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_XYZ2Lab_temp(im, im_new, NUM2DBL(x0), NUM2DBL(y0), NUM2DBL(z0)))
        vips_lib_error();

    return new;
}

static VALUE
img_xyz_to_ucs(VALUE obj)
{
	RUBY_VIPS_UNARY(im_XYZ2UCS);
}

static VALUE
img_srgb_to_xyz(VALUE obj)
{
	RUBY_VIPS_UNARY(im_sRGB2XYZ);
}

static VALUE
img_xyz_to_srgb(VALUE obj)
{
	RUBY_VIPS_UNARY(im_XYZ2sRGB);
}

static VALUE
img_yxy_to_xyz(VALUE obj)
{
	RUBY_VIPS_UNARY(im_Yxy2XYZ);
}

static VALUE
img_xyz_to_yxy(VALUE obj)
{
	RUBY_VIPS_UNARY(im_XYZ2Yxy);
}

static VALUE
img_decmc_from_lab(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_dECMC_fromLab);
}

static VALUE
img_de00_from_lab(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_dE00_fromLab);
}

static VALUE
img_de_from_xyz(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_dE_fromXYZ);
}

static VALUE
img_de_from_lab(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_dE_fromLab);
}

static VALUE
img_lab_morph(VALUE obj, VALUE mask, VALUE l_offset, VALUE l_scale,
	VALUE a_scale, VALUE b_scale)
{
    vipsMask *msk;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    Data_Get_Struct(mask, vipsMask, msk);

	if( im_lab_morph(im, im_new, msk->dmask, NUM2DBL(l_offset),
		NUM2DBL(l_scale), NUM2DBL(a_scale), NUM2DBL(b_scale)) )
		vips_lib_error();

    return new;
}   

VipsIntent
img_id_to_intent(ID rb)
{
    if (rb == id_perceptual)                 return IM_INTENT_PERCEPTUAL;
    else if (rb == id_relative_colorimetric) return IM_INTENT_RELATIVE_COLORIMETRIC;
    else if (rb == id_saturation)            return IM_INTENT_SATURATION;
    else if (rb == id_absolute_colorimetric) return IM_INTENT_ABSOLUTE_COLORIMETRIC;

    return (VipsIntent)NULL;
}

static VALUE
img_icc_transform(VALUE obj, VALUE input_profile_filename,
	VALUE output_profile_filename, VALUE intent)
{
	ID id_intent = SYM2ID(intent);
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_icc_transform(im, im_new, StringValuePtr(input_profile_filename),
		StringValuePtr(output_profile_filename), img_id_to_intent(id_intent)))
        vips_lib_error();

    return new;
}

static VALUE
img_icc_import(VALUE obj, VALUE input_profile_filename, VALUE intent)
{
	ID id_intent = SYM2ID(intent);
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_icc_import(im, im_new, StringValuePtr(input_profile_filename),
		img_id_to_intent(id_intent)))
        vips_lib_error();

    return new;
}

static VALUE
img_icc_import_embedded(VALUE obj, VALUE intent)
{
	ID id_intent = SYM2ID(intent);
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_icc_import_embedded(im, im_new, img_id_to_intent(id_intent)))
        vips_lib_error();

    return new;
}

static VALUE
img_icc_export_depth(VALUE obj, VALUE depth, VALUE output_profile_filename,
	VALUE intent)
{
	ID id_intent = SYM2ID(intent);
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_icc_export_depth(im, im_new, NUM2INT(depth),
		StringValuePtr(output_profile_filename), img_id_to_intent(id_intent)))
        vips_lib_error();

    return new;
}

static VALUE
img_icc_ac2rc(VALUE obj, VALUE depth, VALUE profile_filename)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_icc_ac2rc(im, im_new, StringValuePtr(profile_filename)))
        vips_lib_error();

    return new;
}

void
init_colour(void)
{
    rb_define_method(cVIPSImage, "lab_to_lch", img_lab_to_lch, 0);
    rb_define_method(cVIPSImage, "lch_to_lab", img_lch_to_lab, 0);
    rb_define_method(cVIPSImage, "labq_to_xyz", img_labq_to_xyz, 0);
    rb_define_method(cVIPSImage, "rad_to_float", img_rad_to_float, 0);
    rb_define_method(cVIPSImage, "float_to_rad", img_float_to_rad, 0);
    rb_define_method(cVIPSImage, "lch_to_ucs", img_lch_to_ucs, 0);
    rb_define_method(cVIPSImage, "lab_to_labq", img_lab_to_labq, 0);
    rb_define_method(cVIPSImage, "lab_to_labs", img_lab_to_labs, 0);
    rb_define_method(cVIPSImage, "lab_to_xyz", img_lab_to_xyz, 0);
    rb_define_method(cVIPSImage, "lab_to_xyz_temp", img_lab_to_xyz_temp, 3);
    rb_define_method(cVIPSImage, "lab_to_ucs", img_lab_to_ucs, 0);
    rb_define_method(cVIPSImage, "labq_to_lab", img_labq_to_lab, 0);
    rb_define_method(cVIPSImage, "labq_to_labs", img_labq_to_labs, 0);
    rb_define_method(cVIPSImage, "labs_to_labq", img_labs_to_labq, 0);
    rb_define_method(cVIPSImage, "labs_to_lab", img_labs_to_lab, 0);
    rb_define_method(cVIPSImage, "ucs_to_xyz", img_ucs_to_xyz, 0);
    rb_define_method(cVIPSImage, "ucs_to_lch", img_ucs_to_lch, 0);
    rb_define_method(cVIPSImage, "ucs_to_lab", img_ucs_to_lab, 0);
    rb_define_method(cVIPSImage, "xyz_to_lab", img_xyz_to_lab, 0);
    rb_define_method(cVIPSImage, "xyz_to_lab_temp", img_xyz_to_lab_temp, 3);
    rb_define_method(cVIPSImage, "xyz_to_ucs", img_xyz_to_ucs, 0);
    rb_define_method(cVIPSImage, "srgb_to_xyz", img_srgb_to_xyz, 0);
    rb_define_method(cVIPSImage, "xyz_to_srgb", img_xyz_to_srgb, 0);
    rb_define_method(cVIPSImage, "yxy_to_xyz", img_yxy_to_xyz, 0);
    rb_define_method(cVIPSImage, "xyz_to_yxy", img_xyz_to_yxy, 0);

    rb_define_method(cVIPSImage, "decmc_from_lab", img_decmc_from_lab, 1);
    rb_define_method(cVIPSImage, "de00_from_lab", img_de00_from_lab, 1);
    rb_define_method(cVIPSImage, "de_from_xyz", img_de_from_xyz, 1);
    rb_define_method(cVIPSImage, "de_from_lab", img_de_from_lab, 1);
	rb_define_method(cVIPSImage, "im_lab_morph", img_lab_morph, 5);

	rb_define_method(cVIPSImage, "icc_transform", img_icc_transform, 3);
	rb_define_method(cVIPSImage, "icc_import", img_icc_import, 2);
	rb_define_method(cVIPSImage, "icc_import_embedded", img_icc_import_embedded, 1);
	rb_define_method(cVIPSImage, "icc_export_depth", img_icc_export_depth, 3);
	rb_define_method(cVIPSImage, "icc_ac2rc", img_icc_ac2rc, 1);

	id_perceptual = rb_intern("PERCEPTUAL");
	id_relative_colorimetric = rb_intern("COLORIMETRIC");
	id_saturation = rb_intern("SATURATION");
	id_absolute_colorimetric = rb_intern("ABSOLUTE_COLORIMETRIC");
}

