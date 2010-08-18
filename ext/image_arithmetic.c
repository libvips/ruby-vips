#include "ruby_vips.h"
#include "image.h"
#include "mask.h"
#include "interpolator.h"

/*
 *  call-seq:
 *     im.measure_area(left, top, width, height, h, v, sel) -> array
 *
 *  Analyse a grid of colour patches, producing an array of averages.
 *  Pass a box defined by <i>left</i>, <i>top</i>, <i>width</i>, and
 *  <i>height</i>, the number of horizontal and vertical patches <i>h</i> and
 *  <i>v</i>, an array giving the numbers of the patches to measure <i>sel</i>
 *  (patches are numbered left-to-right, top-to-bottom, starting with 1). Return
 *  an array in which rows are patches and columns are bands. Only the central
 *  50% of each patch is averaged.
 */

VALUE
img_measure_area(VALUE obj, VALUE left, VALUE top, VALUE width, VALUE height,
	VALUE h, VALUE v, VALUE sel)
{
    DOUBLEMASK *ret;
    VALUE results;
    int *a, i, len = RARRAY_LENINT(sel);
	GetImg(obj, data, im);

    a = ALLOC_N(int, len);
    for (i = 0; i < len; i++)
        a[i] = NUM2INT(RARRAY_PTR(sel)[i]);

    ret = im_measure_area(im, NUM2INT(left), NUM2INT(top), NUM2INT(width),
        NUM2INT(height), NUM2INT(h), NUM2INT(v), a, len, "img_measure_area");

    xfree(a);

    return dmask2rb(ret);
}

/*
 *  call-seq:
 *     im.stats -> array
 *
 *  Find many image statistics in a single pass through the pixels. Returns an
 *  array of 6 columns by n+1 (where n is number of bands in the image) rows.
 *  Columns are statistics, and are, in order: minimum, maximum, sum, sum of
 *  squares, mean, standard deviation. Row 0 has statistics for all bands
 *  together, row 1 has stats for band 1, and so on.
 */

VALUE 
img_stats(VALUE obj)
{
    DOUBLEMASK *ret;
	GetImg(obj, data, im);

    if (!(ret = im_stats(im)))
        vips_lib_error();

    return dmask2rb(ret);
}

/*
 *  call-seq:
 *     im.max -> number
 *
 *  Finds the the maximum value of *self*. If input is complex, the max modulus
 *  is returned. Finds the maximum of all bands: if you want to find the maximum
 *  of each band separately, use Image#stats.
 */

VALUE
img_max(VALUE obj) {
    double out;
	GetImg(obj, data, im);

    if (im_max(im, &out))
        vips_lib_error();

    return DBL2NUM(out);
}

/*
 *  call-seq:
 *     im.min -> number
 *
 *  Finds the the minimum value of the image. If input is complex, the min
 *  modulus is returned. Finds the minimum of all bands: if you want to find the
 *  minimum of each band separately, use Image#stats.
 */

VALUE
img_min(VALUE obj) {
    double out;
	GetImg(obj, data, im);

    if (im_min(im, &out))
        vips_lib_error();

    return DBL2NUM(out);
}

/*
 *  call-seq:
 *     im.avg -> number
 *
 *  Finds the average of an image. Works on any non-complex image format.
 */

VALUE
img_avg(VALUE obj) {
    double out;
	GetImg(obj, data, im);
    
    if (im_avg(im, &out))
        vips_lib_error();

    return DBL2NUM(out);
}

/*
 *  call-seq:
 *     im.deviate -> number
 *
 *  This operation finds the standard deviation of all pixels in the image. It
 *  operates on all bands of the input image: use Image#stats if you need to
 *  calculate an average for each band.
 *
 *  Non-complex images only.
 */

VALUE
img_deviate(VALUE obj) {
    double out;
	GetImg(obj, data, im);
    
    if (im_deviate(im, &out))
        vips_lib_error();

    return DBL2NUM(out);
}

/*
 *  call-seq:
 *     im.maxpos_avg -> [ x, y, value ]
 *
 *  Function to find the maximum of an image. In the event of a draw, returns
 *  average of all drawing coords.
 */

VALUE
img_maxpos_avg(VALUE obj) {
    double x, y, out;
	GetImg(obj, data, im);

    if (im_maxpos_avg(im, &x, &y, &out))
        vips_lib_error();

    return rb_ary_new3(3, DBL2NUM(x), DBL2NUM(y), DBL2NUM(out));
}

static VALUE
img_maxpos_single(VALUE obj) {
	double out;
	int x, y;
	GetImg(obj, data, im);

	if (im_maxpos(im, &x, &y, &out))
	    vips_lib_error();

    return rb_ary_new3(3, INT2NUM(x), INT2NUM(y), DBL2NUM(out));
}

static VALUE
img_maxpos_n(VALUE obj, VALUE n) {
	double *out;
	int im_return, i, *x, *y, len;
	VALUE t, ary;
	GetImg(obj, data, im);

	len = NUM2INT(n);
	ary = rb_ary_new2(len);

	x = ALLOC_N(int, len);
	y = ALLOC_N(int, len);
	out = ALLOC_N(double, len);

	if (!(im_return = im_maxpos_vec(im, x, y, out, len))) {
	    for (i = 0; i < len; i++) {
	        t = rb_ary_new3(3, INT2NUM(x[i]), INT2NUM(y[i]), DBL2NUM(out[i]));
	        rb_ary_push(ary, t);
	    }
	}

	xfree(x);
	xfree(y);
	xfree(out);

	if(im_return)
	    vips_lib_error();

    return ary;
}

/*
 *  call-seq:
 *     im.maxpos         -> x, y, value
 *     im.maxpos(n, ...) -> [ x, y, value ], ...
 *
 *  Function to find the maximum of an image. Works for any image type. Returns
 *  an array with the x and y coordinates of the maximum value and the
 *  corresponding value. If <i>n</i> is given, returns the <i>n</i> largest
 *  values. For complex images, finds the pixel with the highest modulus.
 */

VALUE
img_maxpos(int argc, VALUE *argv, VALUE obj) {
	VALUE v_num;

	rb_scan_args(argc, argv, "01", &v_num);
	if (NIL_P(v_num))
		return img_maxpos_single(obj);
	else
		return img_maxpos_n(obj, v_num);
}

static VALUE
img_minpos_single(VALUE obj) {
	double out;
	int x, y;
	GetImg(obj, data, im);

	if (im_minpos(im, &x, &y, &out))
	    vips_lib_error();

    return rb_ary_new3(3, INT2NUM(x), INT2NUM(y), DBL2NUM(out));
}

static VALUE
img_minpos_n(VALUE obj, VALUE n) {
	double *out;
	int im_return, i, *x, *y, len;
	VALUE t, ary;
	GetImg(obj, data, im);

	len = NUM2INT(n);
	ary = rb_ary_new2(len);

	x = ALLOC_N(int, len);
	y = ALLOC_N(int, len);
	out = ALLOC_N(double, len);

	if (!(im_return = im_minpos_vec(im, x, y, out, len))) {
	    for (i = 0; i < len; i++) {
	        t = rb_ary_new3(3, INT2NUM(x[i]), INT2NUM(y[i]), DBL2NUM(out[i]));
	        rb_ary_push(ary, t);
	    }
	}

	xfree(x);
	xfree(y);
	xfree(out);

	if(im_return)
	    vips_lib_error();

    return ary;
}

/*
 *  call-seq:
 *     im.minpos    -> x, y, value
 *     im.minpos(n) -> [ x, y, value ], ...
 *
 *  Function to find the minimum of an image. Works for any image type. Returns
 *  an array with the x and y coordinates of the minimum value and the
 *  corresponding value. If <i>n</i> is given, returns the <i>n</i> lowest
 *  values. For complex images, finds the pixel with the lowest modulus.
 */

VALUE
img_minpos(int argc, VALUE *argv, VALUE obj) {
	VALUE v_num;

    rb_scan_args(argc, argv, "01", &v_num);
	if (NIL_P(v_num))
		return img_minpos_single(obj);
	else
		return img_minpos_n(obj, v_num);
}

/*
 *  call-seq:
 *     im.bandmean -> image
 *
 *  Creates a one-band image where each pixel is the average of the bands for
 *  that pixel in the input image. The output band format is the same as the
 *  input band format. Integer types use round-to-nearest averaging.
 *
 */

VALUE
img_bandmean(VALUE obj)
{
	RUBY_VIPS_UNARY(im_bandmean);
}

/*
 *  call-seq:
 *     im.add(other_image) -> image
 *
 *  This operation calculates im + <i>other_image</i> and writes the result to
 *  a new image. The images must be the same size. They may have any format.
 *
 *  If the number of bands differs, one of the images must have one band. In
 *  this case, an n-band image is formed from the one-band image by joining n
 *  copies of the one-band image together, and then the two n-band images are
 *  operated upon.
 *
 *  The two input images are cast up to the smallest common type.
 */

VALUE
img_add(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_add);
}

/*
 *  call-seq:
 *     im.subtract(other_image) -> image
 *
 *  This operation calculates im - <i>image</i> and writes the result to a new
 *  image. The images must be the same size. They may have any format.
 *
 *  If the number of bands differs, one of the images must have one band. In this
 *  case, an n-band image is formed from the one-band image by joining n copies
 *  of the one-band image together, and then the two n-band images are operated
 *  upon.
 *
 *  The two input images are cast up to the smallest common type.
 */

VALUE
img_subtract(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_subtract);
}

/*
 *  call-seq:
 *     im.invert -> image
 *
 *  This operation calculates (255 - im). The operation works on uchar images
 *  only. The input can have any no of channels.
 *
 *  See Image#exptra for a function which can process any input image type.
 */

VALUE
img_invert(VALUE obj)
{
	RUBY_VIPS_UNARY(im_invert);
}

static VALUE
img_lin_single(VALUE obj, VALUE a, VALUE b)
{
    double da, db;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	da = NUM2DBL(a);
	db = NUM2DBL(b);

	if (im_lintra_vec(1, &da, im, &db, im_new))
		vips_lib_error();

    return new;
}

static VALUE
img_lin_mult(VALUE obj, VALUE a_v, VALUE b_v)
{
    double *a, *b;
    int i, len = RARRAY_LEN(a_v);

    if (RARRAY_LEN(b_v) != len)
        rb_raise(rb_eArgError, "Both arrays must be of the same length");

	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	a = IM_ARRAY(im_new, len, double);
	b = IM_ARRAY(im_new, len, double);

	for (i = 0; i < len; i++) {
	    a[i] = NUM2DBL(RARRAY_PTR(a_v)[i]);
	    b[i] = NUM2DBL(RARRAY_PTR(b_v)[i]);
	}

	if (im_lintra_vec(len, a, im, b, im_new))
	    vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.lin(a, b)                       -> image
 *     im.lin([a1, a2, a3], [b1, b2, b3]) -> image
 *
 *  Pass an image through a linear transform - ie. im * <i>a</i> + <i>b</i>.
 *  Output is always float for integer input, double for double input, complex
 *  for complex input and double complex for double complex input.
 *
 *  If the arrays are passed in and they have the same number of elements as
 *  there are bands in the image, then one array element is used for each band.
 *  If the arrays have more than one element and the image only has a single
 *  band, the result is a many-band image where each band corresponds to one
 *  array element.
 */

VALUE
img_lin(VALUE obj, VALUE a, VALUE b)
{
    if (TYPE(a) == T_ARRAY)
        return img_lin_mult(obj, a, b);
    else
        return img_lin_single(obj, a, b);
}

/*
 *  call-seq:
 *     im.multiply(other_image) -> image
 *
 *  This operation calculates im * <i>other_image</i>. The images must be the
 *  same size. They may have any format.
 *
 *  If the number of bands differs, one of the images must have one band. In
 *  this case, an n-band image is formed from the one-band image by joining n
 *  copies of the one-band image together, and then the two n-band images are
 *  operated upon.
 *
 *  The two input images are cast up to the smallest common type.
 */

VALUE
img_multiply(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_multiply);
}

static VALUE
img_remainder_img(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_remainder);
}

static VALUE
img_remainder_single(VALUE obj, VALUE cnst)
{
	double c = NUM2DBL(cnst);
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	if (im_remainder_vec(im, im_new, 1, &c))
	    vips_lib_error();

	return new;
}

static VALUE
img_remainder_mult(int argc, VALUE *argv, VALUE obj)
{
	double *c;
	int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = IM_ARRAY(im_new, argc, double);

	for (i = 0; i < argc; i++)
	    c[i] = NUM2DBL(argv[i]);

	if (im_remainder_vec(im, im_new, argc, c))
	    vips_lib_error();

	return new;
}

/*
 *  call-seq:
 *     im.remainder(other_image) -> image
 *     im.remainder(c, ...)      -> image
 *
 *  The first form calculates im % <i>other_image</i> (remainder after
 *  division). The images must be the same size. They may have any non-complex
 *  format. For float formats, it calculates im - <i>other_image</i> * floor (im
 *  / <i>other_image</i>).
 *
 *  If the number of bands differs, one of the images must have one band. In
 *  this case, an n-band image is formed from the one-band image by joining n
 *  copies of the one-band image together, and then the two n-band images are
 *  operated upon.
 *
 *  The two input images are cast up to the smallest common type.
 * 
 *  The second form calculates im % <i>c</i> (remainder after division by
 *  constant). The image may have any non-complex format. For float formats,
 *  calculates im - <i>c</i> * floor (im / <i>c</i>).
 *
 *  If the number of image bands and constants differs, then the image must have
 *  one band or there must only one constant. Either the image is up-banded by
 *  joining n copies of the one-band image together, or the same constant is
 *  used for all image bands.
 */

VALUE
img_remainder(int argc, VALUE *argv, VALUE obj)
{
    if (argc == 1) {
		if (CLASS_OF(argv[0]) == cVIPSImage)
			return img_remainder_img(obj, argv[0]);
		else
			return img_remainder_single(obj, argv[0]);
	}

	return img_remainder_mult(argc, argv, obj);
}

/*
 *  call-seq:
 *     im.divide(other_image) -> image
 *
 *  This operation calculates im / <i>other_image</i>. The images must be the
 *  same size. They may have any format.
 *
 *  If the number of bands differs, one of the images must have one band. In
 *  this case, an n-band image is formed from the one-band image by joining n
 *  copies of the one-band image together, and then the two n-band images are
 *  operated upon.
 *
 *  The two input images are cast up to the smallest common type.
 */

VALUE
img_divide(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_divide);
}

/*
 *  call-seq:
 *     im.recomb(array) -> image
 *
 *  This operation recombines an image's bands. Each pixel in im is treated as
 *  an n-element vector, where n is the number of bands in im, and multipled by
 *  the n x m matrix <i>array</i> to produce the m-band output image.
 *
 *  The output image is always float, unless im is double, in which case it is
 *  double too. No complex images allowed.
 *
 *  It's useful for various sorts of colour space conversions.
 */

VALUE
img_recomb(VALUE obj, VALUE recomb)
{
    DOUBLEMASK *dmask;

    GetImg(obj, data, im);
    OutImg(obj, new, data_new, im_new);

    mask_arg2mask(recomb, NULL, &dmask);

    if (im_recomb(im, im_new, dmask))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.sign -> image
 *
 *  Finds the unit vector in the direction of the pixel value. For non-complex
 *  images, it returns a signed char image with values -1, 0, and 1 for
 *  negative, zero and positive pixels. For complex images, it returns a complex
 *  normalised to length 1.
 */

VALUE
img_sign(VALUE obj)
{
	RUBY_VIPS_UNARY(im_sign);
}

/*
 *  call-seq:
 *     im.abs -> image
 *
 *  This operation finds the absolute value of an image. It does a copy for
 *  unsigned integer types, negate for negative values in signed integer types,
 *  fabs(3) for float types, and calculate modulus for complex types.
 */

VALUE
img_abs(VALUE obj)
{
	RUBY_VIPS_UNARY(im_abs);
}

/*
 *  call-seq:
 *     im.floor -> image
 *
 *  For each pixel, find the largest integral value not less than. Copy for
 *  integer types. Output type == input type.
 */

VALUE
img_floor(VALUE obj)
{
	RUBY_VIPS_UNARY(im_floor);
}

/*
 *  call-seq:
 *     im.rint -> image
 *
 *  Finds the nearest integral value. Copy for integer types. Output type ==
 *  input type.
 */

VALUE
img_rint(VALUE obj)
{
	RUBY_VIPS_UNARY(im_rint);
}

/*
 *  call-seq:
 *     im.ceil -> image
 *
 *  For each pixel, find the smallest integral value not less than. Copy for
 *  integer types. Output type == input type.
 */

VALUE
img_ceil(VALUE obj)
{
	RUBY_VIPS_UNARY(im_ceil);
}

/*
 *  call-seq:
 *     linreg(x)     -> image
 *     linreg(*args) -> image
 *
 * Function to find perform pixelwise linear regression on an array of
 * single band images. The output is a seven-band double image.
 *
 * <i>x</i> is the position of each image (pixel value is Y).
 */

VALUE
img_s_linreg(int argc, VALUE *argv, VALUE obj)
{
    vipsImg *in;
    IMAGE **ins;
    double *vips_xs;
    VALUE cur_img;
    int i;
	OutPartial(new, data_new, im_new);
    
    vips_xs = IM_ARRAY(im_new, argc, double);
    ins = IM_ARRAY(im_new, argc + 1, IMAGE*);

    ins[argc] = NULL; /* takes a NULL terminated array of IMAGE pointers */

    for (i = 0; i < argc; i++) {
        cur_img = RARRAY_PTR(argv[i])[0];
		img_add_dep(data_new, cur_img);

        Data_Get_Struct(cur_img, vipsImg, in);
        ins[i] = in->in;

        vips_xs[i] = NUM2DBL(RARRAY_PTR(argv[i])[1]);
    }

    if (im_linreg(ins, im_new, vips_xs))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.point(interpolator_sym, x, y, band) -> number
 *
 *  Find the value at (@x, @y) in given band of image.
 *  Non-integral values are calculated using the supplied interpolator, e.g.
 *  :bilinear.
 *
 *  To get a list of available interpolators, look at
 *      VIPS::Interpolator::INTERPOLATORS.keys
 */

VALUE
img_point(VALUE obj, VALUE itrp_sym, VALUE x, VALUE y, VALUE band)
{
    double out;
    VipsInterpolate *itrp_vips = interp_lookup(itrp_sym);
	GetImg(obj, data, im);

    if (im_point(im, itrp_vips, NUM2DBL(x), NUM2DBL(y), NUM2INT(band), &out))
        vips_lib_error();

    return DBL2NUM(out);
}

/*
 *  call-seq:
 *     im.pow(c, ...) -> image
 *
 *  Tansforms each pixel value in the input image to value ** <i>c</i> in the
 *  output image. It detects division by zero, setting those pixels to zero in
 *  the output. Beware: it does this silently!
 *
 *  If one constant <i>c</i> is given, that constant is used for each image
 *  band. If more than one value is given, it must have the same number of
 *  elements as there are bands in the image, and one element is used for each
 *  band.
 */

VALUE
img_pow(int argc, VALUE *argv, VALUE obj)
{
	double *c;
	int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = IM_ARRAY(im_new, argc, double);

	for (i = 0; i < argc; i++)
		c[i] = NUM2DBL(argv[i]);

	if (im_powtra_vec(im, im_new, argc, c))
		vips_lib_error();

	return new;
}

/*
 *  call-seq:
 *     im.expn(c, ...) -> image
 *
 *  Transforms each pixel value of the input image to <i>c</i> ** value in the
 *  output image. It detects division by zero, setting those pixels to zero in
 *  the output. Beware: it does this silently!
 *
 *  If one constant <i>c</i> is given, that constant is used for each image
 *  band. If more than one value is given, it must have the same number of
 *  elements as there are bands in the image, and one element is used for each
 *  band.
 */

VALUE
img_expn(int argc, VALUE *argv, VALUE obj)
{
	double *c;
	int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	c = IM_ARRAY(im_new, argc, double);

	for (i = 0; i < argc; i++)
		c[i] = NUM2DBL(argv[i]);

	if (im_expntra_vec(im, im_new, argc, c))
		vips_lib_error();

	return new;
}

/*
 *  call-seq:
 *     im.log -> image
 *
 *  For each pixel, calculate the natural logarithm. The output type is float,
 *  unless the input is double, in which case the output is double. Non-complex
 *  images only.
 */

VALUE
img_log(VALUE obj)
{
	RUBY_VIPS_UNARY(im_logtra);
}

/*
 *  call-seq:
 *     im.log10 -> image
 *
 *  For each pixel, calculate the base 10 logarithm. The output type is float,
 *  unless the input is double, in which case the output is double. Non-complex
 *  images only.
 */

VALUE
img_log10(VALUE obj)
{
	RUBY_VIPS_UNARY(im_log10tra);
}

/*
 *  call-seq:
 *     im.sin -> image
 *
 *  For each pixel, calculate the sine. Angles are expressed in degrees. The
 *  output type is float, unless the input is double, in which case the output
 *  is double. Non-complex images only.
 */

VALUE
img_sin(VALUE obj)
{
	RUBY_VIPS_UNARY(im_sintra);
}

/*
 *  call-seq:
 *     im.cos -> image
 *
 *  For each pixel, calculate the cosine. Angles are expressed in degrees. The
 *  output type is float, unless the input is double, in which case the output
 *  is double. Non-complex images only.
 */

VALUE
img_cos(VALUE obj)
{
	RUBY_VIPS_UNARY(im_costra);
}

/*
 *  call-seq:
 *     im.tan -> image
 *
 *  For each pixel, calculate the tangent. Angles are expressed in degrees. The
 *  output type is float, unless the input is double, in which case the output
 *  is double. Non-complex images only.
 */

VALUE
img_tan(VALUE obj)
{
	RUBY_VIPS_UNARY(im_tantra);
}

/*
 *  call-seq:
 *     im.asin -> image
 *
 *  For each pixel, calculate the arc or inverse sine. Angles are expressed in
 *  degrees. The output type is float, unless the input is double, in which case
 *  the output is double. Non-complex images only.
 */

VALUE
img_asin(VALUE obj)
{
	RUBY_VIPS_UNARY(im_asintra);
}

/*
 *  call-seq:
 *     im.acos -> image
 *
 *  For each pixel, calculate the arc or inverse cosine. Angles are expressed in
 *  degrees. The output type is float, unless the input is double, in which case
 *  the output is double. Non-complex images only.
 */

VALUE
img_acos(VALUE obj)
{
	RUBY_VIPS_UNARY(im_acostra);
}

/*
 *  call-seq:
 *     im.atan -> image
 *
 *  For each pixel, calculate the arc or inverse tangent. Angles are expressed
 *  in degrees. The output type is float, unless the input is double, in which
 *  case the output is double. Non-complex images only.
 */

VALUE
img_atan(VALUE obj)
{
	RUBY_VIPS_UNARY(im_atantra);
}

/*
 *  call-seq:
 *     im.cross_phase(other_image) -> image
 *
 *  Find the phase of the cross power spectrum of two complex images, expressed
 *  as a complex image where the modulus of each pixel is one.
 */

VALUE
img_cross_phase(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_cross_phase);
}
