#include "ruby_vips.h"
#include "image.h"

static VALUE
img_fwfft(VALUE obj)
{
	RUBY_VIPS_UNARY(im_fwfft);
}

static VALUE
img_invfft(VALUE obj)
{
	RUBY_VIPS_UNARY(im_invfft);
}

static VALUE
img_rotquad(VALUE obj)
{
	RUBY_VIPS_UNARY(im_rotquad);
}

static VALUE
img_invfftr(VALUE obj)
{
	RUBY_VIPS_UNARY(im_invfftr);
}

#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 22

static VALUE
img_s_create_fmask_ideal_highpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im, NUM2INT(x), NUM2INT(y), VIPS_MASK_IDEAL_HIGHPASS,
		NUM2DBL(frequency_cutoff)))
        vips_lib_error();

    return new;	
}

static VALUE
img_s_create_fmask_ideal_lowpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im, NUM2INT(x), NUM2INT(y),
		VIPS_MASK_IDEAL_LOWPASS, NUM2DBL(frequency_cutoff)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_butterworth_highpass(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoff, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_BUTTERWORTH_HIGHPASS, NUM2DBL(order),
		NUM2DBL(frequency_cutoff), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_butterworth_lowpass(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoff, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_BUTTERWORTH_LOWPASS, NUM2DBL(order),
		NUM2DBL(frequency_cutoff), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_gauss_highpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_GAUSS_HIGHPASS,
		NUM2DBL(frequency_cutoff), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_gauss_lowpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_GAUSS_LOWPASS,
		NUM2DBL(frequency_cutoff), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_ideal_ringpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE width)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_IDEAL_RINGPASS,
		NUM2DBL(frequency_cutoff), NUM2DBL(width)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_ideal_ringreject(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE width)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_IDEAL_RINGREJECT,
		NUM2DBL(frequency_cutoff), NUM2DBL(width)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_butterworth_ringpass(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoff, VALUE width, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_BUTTERWORTH_RINGPASS,
		NUM2DBL(order), NUM2DBL(frequency_cutoff), NUM2DBL(width),
		NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_butterworth_ringreject(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoff, VALUE width, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_BUTTERWORTH_RINGREJECT,
		NUM2DBL(order), NUM2DBL(frequency_cutoff), NUM2DBL(width),
		NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_gauss_ringpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE width, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_GAUSS_RINGPASS,
		NUM2DBL(frequency_cutoff), NUM2DBL(width), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_gauss_ringreject(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE width, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_GAUSS_RINGREJECT,
		NUM2DBL(frequency_cutoff), NUM2DBL(width), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_ideal_bandpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoffx, VALUE frequency_cutoffy, VALUE radius)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_IDEAL_BANDPASS,
		NUM2DBL(frequency_cutoffx), NUM2DBL(frequency_cutoffy),
		NUM2DBL(radius)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_ideal_bandreject(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoffx, VALUE frequency_cutoffy, VALUE radius)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_IDEAL_BANDREJECT,
		NUM2DBL(frequency_cutoffx), NUM2DBL(frequency_cutoffy),
		NUM2DBL(radius)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_butterworth_bandpass(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoffx, VALUE frequency_cutoffy, VALUE radius,
	VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_BUTTERWORTH_BANDPASS,
		NUM2DBL(order), NUM2DBL(frequency_cutoffx), NUM2DBL(frequency_cutoffy),
		NUM2DBL(radius), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_butterworth_bandreject(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoffx, VALUE frequency_cutoffy, VALUE radius,
	VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_BUTTERWORTH_BANDREJECT,
		NUM2DBL(order), NUM2DBL(frequency_cutoffx), NUM2DBL(frequency_cutoffy),
		NUM2DBL(radius), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_gauss_bandpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoffx, VALUE frequency_cutoffy, VALUE radius,
	VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_GAUSS_BANDPASS,
		NUM2DBL(frequency_cutoffx), NUM2DBL(frequency_cutoffy), NUM2DBL(radius),
		NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_gauss_bandreject(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoffx, VALUE frequency_cutoffy, VALUE radius,
	VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_GAUSS_BANDREJECT,
		NUM2DBL(frequency_cutoffx), NUM2DBL(frequency_cutoffy), NUM2DBL(radius),
		NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

static VALUE
img_s_create_fmask_fractal_flt(VALUE obj, VALUE x, VALUE y,
	VALUE fractal_dimension)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_FRACTAL_FLT,
		NUM2DBL(fractal_dimension)))
        vips_lib_error();

    return new;	
}

#endif

static VALUE
img_freqflt(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_freqflt);
}

static VALUE
img_disp_ps(VALUE obj)
{
	RUBY_VIPS_UNARY(im_disp_ps);
}

static VALUE
img_phasecor_fft(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_phasecor_fft);
}

static VALUE
img_s_fractsurf(VALUE obj, VALUE size, VALUE frd)
{
	OutPartial(new, data, im);

    if (im_fractsurf(im, NUM2INT(size), NUM2DBL(frd)))
        vips_lib_error();

    return new;
}

void
init_freq_filt(void)
{
    rb_define_method(cVIPSImage, "fwfft", img_fwfft, 0);
    rb_define_method(cVIPSImage, "invfft", img_invfft, 0);
	rb_define_method(cVIPSImage, "rotquad", img_rotquad, 0);
    rb_define_method(cVIPSImage, "invfftr", img_invfftr, 0);

	#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 22
    rb_define_singleton_method(cVIPSImage, "create_fmask_ideal_highpass", img_s_create_fmask_ideal_highpass, 3);
    rb_define_singleton_method(cVIPSImage, "create_fmask_ideal_lowpass", img_s_create_fmask_ideal_lowpass, 3);
    rb_define_singleton_method(cVIPSImage, "create_fmask_butterworth_highpass", img_s_create_fmask_butterworth_highpass, 5);
    rb_define_singleton_method(cVIPSImage, "create_fmask_butterworth_lowpass", img_s_create_fmask_butterworth_lowpass, 5);
    rb_define_singleton_method(cVIPSImage, "create_fmask_gauss_highpass", img_s_create_fmask_gauss_highpass, 4);
    rb_define_singleton_method(cVIPSImage, "create_fmask_gauss_lowpass", img_s_create_fmask_gauss_lowpass, 4);

    rb_define_singleton_method(cVIPSImage, "create_fmask_ideal_ringpass", img_s_create_fmask_ideal_ringpass, 4);
    rb_define_singleton_method(cVIPSImage, "create_fmask_ideal_ringreject", img_s_create_fmask_ideal_ringreject, 4);
    rb_define_singleton_method(cVIPSImage, "create_fmask_butterworth_ringpass", img_s_create_fmask_butterworth_ringpass, 6);
    rb_define_singleton_method(cVIPSImage, "create_fmask_butterworth_ringreject", img_s_create_fmask_butterworth_ringreject, 6);
    rb_define_singleton_method(cVIPSImage, "create_fmask_gauss_ringpass", img_s_create_fmask_gauss_ringpass, 5);
    rb_define_singleton_method(cVIPSImage, "create_fmask_gauss_ringreject", img_s_create_fmask_gauss_ringreject, 5);

    rb_define_singleton_method(cVIPSImage, "create_fmask_ideal_bandpass", img_s_create_fmask_ideal_bandpass, 5);
    rb_define_singleton_method(cVIPSImage, "create_fmask_ideal_bandreject", img_s_create_fmask_ideal_bandreject, 5);
    rb_define_singleton_method(cVIPSImage, "create_fmask_butterworth_bandpass", img_s_create_fmask_butterworth_bandpass, 7);
    rb_define_singleton_method(cVIPSImage, "create_fmask_butterworth_bandreject", img_s_create_fmask_butterworth_bandreject, 7);
    rb_define_singleton_method(cVIPSImage, "create_fmask_gauss_bandpass", img_s_create_fmask_gauss_bandpass, 6);
    rb_define_singleton_method(cVIPSImage, "create_fmask_gauss_bandreject", img_s_create_fmask_gauss_bandreject, 6);

    rb_define_singleton_method(cVIPSImage, "create_fmask_fractal_flt", img_s_create_fmask_fractal_flt, 3);
	#endif

    rb_define_method(cVIPSImage, "freqflt", img_freqflt, 1);
    rb_define_method(cVIPSImage, "disp_ps", img_disp_ps, 0);
    rb_define_method(cVIPSImage, "phasecor_fft", img_phasecor_fft, 1);

    rb_define_singleton_method(cVIPSImage, "fractsurf", img_s_fractsurf, 2);
}

