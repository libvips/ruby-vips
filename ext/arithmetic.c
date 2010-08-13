#include "ruby_vips.h"
#include "image.h"
#include "mask.h"
#include "interpolator.h"

/* TODO: all these args are not pretty -- perhaps accept a hash, or break into
 * a multi step process with a block or a class.
 */ 
static VALUE
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

static VALUE 
img_stats(VALUE obj)
{
    DOUBLEMASK *ret;
	GetImg(obj, data, im);

    if (!(ret = im_stats(im)))
        vips_lib_error();

    return dmask2rb(ret);
}

static VALUE
img_max(VALUE obj) {
    double out;
	GetImg(obj, data, im);

    if (im_max(im, &out))
        vips_lib_error();

    return DBL2NUM(out);
}

static VALUE
img_min(VALUE obj) {
    double out;
	GetImg(obj, data, im);

    if (im_min(im, &out))
        vips_lib_error();

    return DBL2NUM(out);
}

static VALUE
img_avg(VALUE obj) {
    double out;
	GetImg(obj, data, im);
    
    if (im_avg(im, &out))
        vips_lib_error();

    return DBL2NUM(out);
}

static VALUE
img_deviate(VALUE obj) {
    double out;
	GetImg(obj, data, im);
    
    if (im_deviate(im, &out))
        vips_lib_error();

    return DBL2NUM(out);
}

static VALUE
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

static VALUE
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

static VALUE
img_minpos(int argc, VALUE *argv, VALUE obj) {
	VALUE v_num;

    rb_scan_args(argc, argv, "01", &v_num);
	if (NIL_P(v_num))
		return img_minpos_single(obj);
	else
		return img_minpos_n(obj, v_num);
}

static VALUE
img_bandmean(VALUE obj)
{
	RUBY_VIPS_UNARY(im_bandmean);
}

static VALUE
img_add(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_add);
}

static VALUE
img_subtract(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_subtract);
}

static VALUE
img_invert(VALUE obj)
{
	RUBY_VIPS_UNARY(im_invert);
}

static VALUE
img_lintra_single(VALUE obj, VALUE a, VALUE b)
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
img_lintra_mult(int argc, VALUE *argv, VALUE obj)
{
    double *a, *b;
    int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	a = IM_ARRAY(im_new, argc, double);
	b = IM_ARRAY(im_new, argc, double);

	for (i = 0; i < argc; i++) {
	    a[i] = NUM2DBL(RARRAY_PTR(argv[i])[0]);
	    b[i] = NUM2DBL(RARRAY_PTR(argv[i])[1]);
	}

	if (im_lintra_vec(argc, a, im, b, im_new))
	    vips_lib_error();

    return new;
}

static VALUE
img_lintra(int argc, VALUE *argv, VALUE obj)
{
    if (argc == 2 && TYPE(argv[0]) != T_ARRAY)
		return img_lintra_single(obj, argv[0], argv[1]);
	else
		return img_lintra_mult(argc, argv, obj);
}

static VALUE
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

static VALUE
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

static VALUE
img_divide(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_divide);
}

static VALUE
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

static VALUE
img_sign(VALUE obj)
{
	RUBY_VIPS_UNARY(im_sign);
}

static VALUE
img_abs(VALUE obj)
{
	RUBY_VIPS_UNARY(im_abs);
}

static VALUE
img_floor(VALUE obj)
{
	RUBY_VIPS_UNARY(im_floor);
}

static VALUE
img_rint(VALUE obj)
{
	RUBY_VIPS_UNARY(im_rint);
}

static VALUE
img_ceil(VALUE obj)
{
	RUBY_VIPS_UNARY(im_ceil);
}

/* TODO: change to an instance method */

static VALUE
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

static VALUE
img_point(VALUE obj, VALUE itrp_sym, VALUE x, VALUE y, VALUE band)
{
    double out;
    VipsInterpolate *itrp_vips = interp_lookup(itrp_sym);
	GetImg(obj, data, im);

    if (im_point(im, itrp_vips, NUM2DBL(x), NUM2DBL(y), NUM2INT(band), &out))
        vips_lib_error();

    return DBL2NUM(out);
}

static VALUE
img_powtra(int argc, VALUE *argv, VALUE obj)
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

static VALUE
img_expntra(int argc, VALUE *argv, VALUE obj)
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

static VALUE
img_logtra(VALUE obj)
{
	RUBY_VIPS_UNARY(im_logtra);
}

static VALUE
img_log10tra(VALUE obj)
{
	RUBY_VIPS_UNARY(im_log10tra);
}

static VALUE
img_sintra(VALUE obj)
{
	RUBY_VIPS_UNARY(im_sintra);
}

static VALUE
img_costra(VALUE obj)
{
	RUBY_VIPS_UNARY(im_costra);
}

static VALUE
img_tantra(VALUE obj)
{
	RUBY_VIPS_UNARY(im_tantra);
}

static VALUE
img_asintra(VALUE obj)
{
	RUBY_VIPS_UNARY(im_asintra);
}

static VALUE
img_acostra(VALUE obj)
{
	RUBY_VIPS_UNARY(im_acostra);
}

static VALUE
img_atantra(VALUE obj)
{
	RUBY_VIPS_UNARY(im_atantra);
}

static VALUE
img_cross_phase(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_cross_phase);
}

void
init_arithmetic(void)
{
	rb_define_method(cVIPSImage, "measure_area", img_measure_area, 7);
	rb_define_method(cVIPSImage, "stats", img_stats, 0);
	rb_define_method(cVIPSImage, "max", img_max, 0);
	rb_define_method(cVIPSImage, "min", img_min, 0);
	rb_define_method(cVIPSImage, "avg", img_avg, 0);
	rb_define_method(cVIPSImage, "deviate", img_deviate, 0);
	rb_define_method(cVIPSImage, "maxpos", img_maxpos, -1);
	rb_define_method(cVIPSImage, "minpos", img_minpos, -1);
	rb_define_method(cVIPSImage, "maxpos_avg", img_maxpos_avg, 0);
	rb_define_method(cVIPSImage, "bandmean", img_bandmean, 0);
	rb_define_method(cVIPSImage, "add", img_add, 1);
	rb_define_method(cVIPSImage, "subtract", img_subtract, 1);
	rb_define_method(cVIPSImage, "invert", img_invert, 0);
	rb_define_method(cVIPSImage, "lintra", img_lintra, -1);
	rb_define_method(cVIPSImage, "multiply", img_multiply, 1);
	rb_define_method(cVIPSImage, "divide", img_divide, 1);
	rb_define_method(cVIPSImage, "remainder", img_remainder, -1);
	rb_define_method(cVIPSImage, "recomb", img_recomb, 1);
	rb_define_method(cVIPSImage, "sign", img_sign, 0);
	rb_define_method(cVIPSImage, "abs", img_abs, 0);
	rb_define_method(cVIPSImage, "floor", img_floor, 0);
	rb_define_method(cVIPSImage, "rint", img_rint, 0);
	rb_define_method(cVIPSImage, "ceil", img_ceil, 0);
	rb_define_singleton_method(cVIPSImage, "linreg", img_s_linreg, -1);
	rb_define_method(cVIPSImage, "point", img_point, 4);
	rb_define_method(cVIPSImage, "powtra", img_powtra, -1);
	rb_define_method(cVIPSImage, "expntra", img_expntra, -1);
	rb_define_method(cVIPSImage, "logtra", img_logtra, 0);
	rb_define_method(cVIPSImage, "log10tra", img_log10tra, 0);
	rb_define_method(cVIPSImage, "sintra", img_sintra, 0);
	rb_define_method(cVIPSImage, "costra", img_costra, 0);
	rb_define_method(cVIPSImage, "tantra", img_tantra, 0);
	rb_define_method(cVIPSImage, "asintra", img_asintra, 0);
	rb_define_method(cVIPSImage, "acostra", img_acostra, 0);
	rb_define_method(cVIPSImage, "atantra", img_atantra, 0);
	rb_define_method(cVIPSImage, "cross_phase", img_cross_phase, 1);
}

