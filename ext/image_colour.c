#include "ruby_vips.h"

static ID id_perceptual, id_relative_colorimetric, id_saturation,
	id_absolute_colorimetric;

/*
 *  call-seq:
 *     im.lab_to_lch -> image
 *
 *  Turn Lab to LCh.
 */

VALUE
img_lab_to_lch(VALUE obj)
{
	RUBY_VIPS_UNARY(im_Lab2LCh);
}

/*
 *  call-seq:
 *     im.lch_to_lab -> image
 *
 *  Turn LCh to Lab.
 */

VALUE
img_lch_to_lab(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LCh2Lab);
}

/*
 *  call-seq:
 *     im.labq_to_xyz -> image
 *
 *  Turn LabQ to XYZ.
 */

VALUE
img_labq_to_xyz(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LabQ2XYZ);
}

/*
 *  call-seq:
 *     im.rad_to_float -> image
 *
 *  Unpack a RAD image to a three-band float image.
 */

VALUE
img_rad_to_float(VALUE obj)
{
	RUBY_VIPS_UNARY(im_rad2float);
}

/*
 *  call-seq:
 *     im.float_to_rad -> image
 *
 *  Convert a three-band float image to Radiance 32-bit packed format.
 */

VALUE
img_float_to_rad(VALUE obj)
{
	RUBY_VIPS_UNARY(im_float2rad);
}

/*
 *  call-seq:
 *     im.lch_to_ucs -> image
 *
 *  Turn LCh to UCS.
 */

VALUE
img_lch_to_ucs(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LCh2UCS);
}

/*
 *  call-seq:
 *     im.lab_to_labq -> image
 *
 *  Turn Lab to LabQ.
 */

VALUE
img_lab_to_labq(VALUE obj)
{
	RUBY_VIPS_UNARY(im_Lab2LabQ);
}

/*
 *  call-seq:
 *     im.lab_to_labs -> image
 *
 *  Turn Lab to LabS.
 */

VALUE
img_lab_to_labs(VALUE obj)
{
	RUBY_VIPS_UNARY(im_Lab2LabS);
}

/*
 *  call-seq:
 *     im.lab_to_xyz -> image
 *
 *  Turn Lab to XYZ.
 */

VALUE
img_lab_to_xyz(VALUE obj)
{
	RUBY_VIPS_UNARY(im_Lab2XYZ);
}

/*
 *  call-seq:
 *     im.lab_to_xyz_temp(x0, y0, z0) -> image
 *
 *  Turn Lab to XYZ. <i>x0</i>, <i>y0</i>, <i>z0</i> give the Lab colour
 *  temperature.
 */

VALUE
img_lab_to_xyz_temp(VALUE obj, VALUE x0, VALUE y0, VALUE z0)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_Lab2XYZ_temp(im, im_new, NUM2DBL(x0), NUM2DBL(y0), NUM2DBL(z0)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.lab_to_ucs -> image
 *
 *  Turn Lab to UCS.
 */

VALUE
img_lab_to_ucs(VALUE obj)
{
	RUBY_VIPS_UNARY(im_Lab2UCS);
}

/*
 *  call-seq:
 *     im.labq_to_lab -> image
 *
 *  Turn LabQ to Lab.
 */

VALUE
img_labq_to_lab(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LabQ2Lab);
}

/*
 *  call-seq:
 *     im.labq_to_labs -> image
 *
 *  Turn LabQ to LabS.
 */

VALUE
img_labq_to_labs(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LabQ2LabS);
}

/*
 *  call-seq:
 *     im.labs_to_labq -> image
 *
 *  Turn LabS to LabQ.
 */

VALUE
img_labs_to_labq(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LabS2LabQ);
}

/*
 *  call-seq:
 *     im.labs_to_lab -> image
 *
 *  Turn LabS to Lab.
 */

VALUE
img_labs_to_lab(VALUE obj)
{
	RUBY_VIPS_UNARY(im_LabS2Lab);
}

/*
 *  call-seq:
 *     im.ucs_to_xyz -> image
 *
 *  Turn UCS to XYZ.
 */

VALUE
img_ucs_to_xyz(VALUE obj)
{
	RUBY_VIPS_UNARY(im_UCS2XYZ);
}

/*
 *  call-seq:
 *     im.ucs_to_lch -> image
 *
 *  Turn UCS to LCh.
 */

VALUE
img_ucs_to_lch(VALUE obj)
{
	RUBY_VIPS_UNARY(im_UCS2LCh);
}

/*
 *  call-seq:
 *     im.ucs_to_lab -> image
 *
 *  Turn UCS to Lab.
 */

VALUE
img_ucs_to_lab(VALUE obj)
{
	RUBY_VIPS_UNARY(im_UCS2Lab);
}

/*
 *  call-seq:
 *     im.xyz_to_lab -> image
 *
 *  Turn XYZ to Lab.
 */

VALUE
img_xyz_to_lab(VALUE obj)
{
	RUBY_VIPS_UNARY(im_XYZ2Lab);
}

/*
 *  call-seq:
 *     im.xyz_to_lab_temp(x0, y0, z0) -> image
 *
 *  Turn XYZ to LAB. <i>x0</i>, <i>y0</i>, <i>z0</i> give the Lab colour
 *  temperature.
 */

VALUE
img_xyz_to_lab_temp(VALUE obj, VALUE x0, VALUE y0, VALUE z0)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_XYZ2Lab_temp(im, im_new, NUM2DBL(x0), NUM2DBL(y0), NUM2DBL(z0)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.xyz_to_ucs -> image
 *
 *  Turn XYZ to UCS.
 */

VALUE
img_xyz_to_ucs(VALUE obj)
{
	RUBY_VIPS_UNARY(im_XYZ2UCS);
}

/*
 *  call-seq:
 *     im.srgb_to_xyz -> image
 *
 *  Turn sRGB to XYZ.
 */

VALUE
img_srgb_to_xyz(VALUE obj)
{
	RUBY_VIPS_UNARY(im_sRGB2XYZ);
}

/*
 *  call-seq:
 *     im.xyz_to_srgb -> image
 *
 *  Turn XYZ to sRGB.
 */

VALUE
img_xyz_to_srgb(VALUE obj)
{
	RUBY_VIPS_UNARY(im_XYZ2sRGB);
}

/*
 *  call-seq:
 *     im.yxy_to_xyz -> image
 *
 *  Turn Yxy to XYZ.
 */

VALUE
img_yxy_to_xyz(VALUE obj)
{
	RUBY_VIPS_UNARY(im_Yxy2XYZ);
}

/*
 *  call-seq:
 *     im.xyz_to_yxy -> image
 *
 *  Turn XYZ to Yxy.
 */

VALUE
img_xyz_to_yxy(VALUE obj)
{
	RUBY_VIPS_UNARY(im_XYZ2Yxy);
}

/*
 *  call-seq:
 *     im.decmc_from_lab(other_image) -> image
 *
 *  Calculate dE CMC from two Lab images.
 */

VALUE
img_decmc_from_lab(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_dECMC_fromLab);
}

/*
 *  call-seq:
 *     im.de00_from_lab(other_image) -> image
 *
 *  Calculate CIE dE00 from two Lab images.
 */

VALUE
img_de00_from_lab(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_dE00_fromLab);
}

/*
 *  call-seq:
 *     im.de_from_xyz(other_image) -> image
 *
 *  Calculate CIELAB dE 1976 from a pair of XYZ images.
 */

VALUE
img_de_from_xyz(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_dE_fromXYZ);
}

/*
 *  call-seq:
 *     im.de_from_lab(other_image) -> image
 *
 *  Calculate CIE dE 1976 from two Lab images.
 */

VALUE
img_de_from_lab(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_dE_fromLab);
}

/*
 *  call-seq:
 *    im.lab_morph(mask, l_offset, l_scale, a_scale, b_scale) -> image
 *
 *  Morph an image in CIELAB colour space. Useful for certain types of gamut
 *  mapping, or correction of greyscales on some printers.
 *
 *  We perform three adjustments.
 *
 *  * cast
 *
 *    Pass in <i>mask</i> containing CIELAB readings for a neutral greyscale.
 *    For example:
 * 
 *      mask = [
 *        [14.23, 4.8 , -3.95],
 *        [18.74, 2.76, -2.62],
 *        [23.46, 1.4 , -1.95],
 *        [27.46, 1.76, -2.01]
 *      ]
 *
 *    Interpolation from this makes cast corrector. The top and tail are
 *    interpolated towards [0, 0, 0] and [100, 0, 0], intermediate values are
 *    interpolated along straight lines fitted between the specified points.
 *    Rows may be in any order (ie. they need not be sorted on L*).
 *
 *    Each pixel is displaced in a/b by the amount specified for that L in the
 *    table.
 *
 *  * L*
 *
 *    Pass in <i>l_scale</i> and <i>l_offset</i> for L. L' = (L + offset) *
 *    scale.
 *
 *  * saturation
 *
 *    scale a and b by these amounts, eg. 1.5 increases saturation.
 *
 *  Find the top two by generating and printing a greyscale. Find the bottom
 *  by printing a Macbeth and looking at a/b spread
 */

VALUE
img_lab_morph(VALUE obj, VALUE mask, VALUE l_offset, VALUE l_scale,
	VALUE a_scale, VALUE b_scale)
{
    DOUBLEMASK *dmask;

	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    mask_arg2mask(mask, NULL, &dmask);

	if( im_lab_morph(im, im_new, dmask, NUM2DBL(l_offset),
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

/*
 *  call-seq:
 *     im.icc_transform(input_filename, output_filename, intent) -> image
 *
 *  Transform an image with the ICC library. The input image is moved to
 *  profile-connection space with the input profile and then to the output
 *  space with the output profile.
 *
 *  Use Image#icc_import and Image#icc_export_depth to do either the first or
 *  second half of this operation in isolation.
 */

VALUE
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

/*
 *  call-seq:
 *     im.icc_import(input_filename, intent) -> image
 *
 *  Import an image with the ICC library. The input image in device space
 *  is moved to D65 LAB with the input profile.
 */

VALUE
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

/*
 *  call-seq:
 *     im.icc_import_embedded(intent) -> image
 *
 *  Import an image with the ICC library. The input image in device space
 *  is moved to D65 LAB with the input profile from the input image header
 *  attached.
 */

VALUE
img_icc_import_embedded(VALUE obj, VALUE intent)
{
	ID id_intent = SYM2ID(intent);
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_icc_import_embedded(im, im_new, img_id_to_intent(id_intent)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.icc_export_depth(depth, output_filename, intent) -> image
 *
 *  Export an image with the ICC library. The input image in
 *  D65 LAB is transformed to device space using the supplied profile.
 *  <i>depth</i> can be 8 or 16, for 8 or 16-bit image export.
 */

VALUE
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

/*
 *  call-seq:
 *     im.icc_ac2rc(depth, profile_filename) -> image
 *
 *  Transform an image from absolute to relative colorimetry using the
 *  MediaWhitePoint stored in the ICC profile.
 */

VALUE
img_icc_ac2rc(VALUE obj, VALUE depth, VALUE profile_filename)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_icc_ac2rc(im, im_new, StringValuePtr(profile_filename)))
        vips_lib_error();

    return new;
}

void
init_Image_colour()
{
    id_perceptual = rb_intern("PERCEPTUAL");
    id_relative_colorimetric = rb_intern("COLORIMETRIC");
    id_saturation = rb_intern("SATURATION");
    id_absolute_colorimetric = rb_intern("ABSOLUTE_COLORIMETRIC");
}
