#include "ruby_vips.h"
#include "image.h"
#include "image_freq_filt.h"

/*
 *  call-seq:
 *     im.fwfft -> image
 *
 *  Transform an image to Fourier space.
 *
 *  VIPS uses the fftw3 or fftw2 Fourier transform libraries if possible. If
 *  they were not available when VIPS was built, it falls back to its own
 *  FFT functions which are slow and only work for square images whose sides
 *  are a power of two.
 */

VALUE
img_fwfft(VALUE obj)
{
	RUBY_VIPS_UNARY(im_fwfft);
}

/*
 *  call-seq:
 *     im.invfft -> image
 *
 *  Transform an image from Fourier space to real space. The result is complex.
 *  If you are OK with a real result, use Image#invfftr instead, it's quicker.
 *
 *  VIPS uses the fftw3 or fftw2 Fourier transform libraries if possible. If
 *  they were not available when VIPS was built, it falls back to its own FFT
 *  functions which are slow and only work for square images whose sides are a
 *  power of two.
 */

VALUE
img_invfft(VALUE obj)
{
	RUBY_VIPS_UNARY(im_invfft);
}

/*
 *  call-seq:
 *     im.rotquad -> image
 *
 *  Rotate the quadrants of the image so that the point that was at the
 *  top-left is now in the centre. Handy for moving Fourier images to optical
 *  space.
 */

VALUE
img_rotquad(VALUE obj)
{
	RUBY_VIPS_UNARY(im_rotquad);
}

/*
 *  call-seq:
 *     im.invfftr -> image
 *
 *  Transform an image from Fourier space to real space, giving a real result.
 *  This is faster than Image#invfft, which gives a complex result.
 *
 *  VIPS uses the fftw3 or fftw2 Fourier transform libraries if possible. If
 *  they were not available when VIPS was built, it falls back to it's own
 *  FFT functions which are slow and only work for square images whose sides
 *  are a power of two.
 */

VALUE
img_invfftr(VALUE obj)
{
	RUBY_VIPS_UNARY(im_invfftr);
}

#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 22

/*
 *  call-seq:
 *     Image.fmask_ideal_lowpass(x, y, frequency_cutoff) -> image
 *
 *  This operation creates a one-band float image of the size <i>x</i> by
 *  <i>y</i>. The image must be square, and the sides must be a power of two.
 *  The image has values in the range [0, 1] and is typically used for
 *  multiplying against  frequency domain images to filter them.
 *
 *  All masks are created with the DC component at (0, 0), so you might want to
 *  rotate the quadrants with im_rotquad() before viewing. The DC pixel always
 *  has the value 1.0.
 *
 *  Unless noted below, all parameters are expressed as percentages, scaled to
 *  [0, 1].
 *
 *  * High-pass, low-pass masks: A high pass filter mask filters the low
 *    frequencies while allowing the high frequencies to get through.
 *    The reverse happens with a low pass filter mask.
 *
 *  * Ring-pass, ring-reject masks: A ring filter passes or rejects a range of
 *    frequencies. The range is specified by the <i>frequency_cutoff</i> and
 *    the <i>width</i>.
 *
 *  * Band-pass, band-reject masks: These masks are used to pass or remove
 *    spatial frequencies around a given frequency. The position of the
 *    frequency to pass or remove is given by <i>frequency_cutoffx</i> and
 *    <i>frequency_cutoffy</i>. The size of the region around the point is
 *    given by <i>radius</i>.
 *
 *  * Ideal filters: These filters pass or reject frequencies with a sharp
 *    cutoff at the transition.
 *
 *  * Butterworth filters: These filters use a Butterworth function to separate
 *    the frequencies (see Gonzalez and Wintz, Digital Image Processing, 1987).
 *    The shape of the curve is controlled by <i>order</i>: higher values give
 *    a sharper transition.
 *
 *  * Gaussian filters: These filters have a smooth Gaussian shape,
 *    controlled by <i>amplitude_cutoff</i>.
 */

VALUE
img_s_fmask_ideal_highpass(VALUE obj, VALUE x, VALUE y, VALUE frequency_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im, NUM2INT(x), NUM2INT(y), VIPS_MASK_IDEAL_HIGHPASS,
		NUM2DBL(frequency_cutoff)))
        vips_lib_error();

    return new;	
}

/*
 *  call-seq:
 *    Image.fmask_ideal_lowpass(x, y, frequency_cutoff) -> image
 *
 *  See Image.fmask_ideal_highpass
 *
 */

VALUE
img_s_fmask_ideal_lowpass(VALUE obj, VALUE x, VALUE y, VALUE frequency_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im, NUM2INT(x), NUM2INT(y),
		VIPS_MASK_IDEAL_LOWPASS, NUM2DBL(frequency_cutoff)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     Image.fmask_butterworth_highpass(x, y, order, frequency_cutoff,
 *       amplitude_cutoff) -> image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_butterworth_highpass(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoff, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_BUTTERWORTH_HIGHPASS, NUM2DBL(order),
		NUM2DBL(frequency_cutoff), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     Image.fmask_butterworth_lowpass(x, y, order, frequency_cutoff,
 *       amplitude_cutoff)
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_butterworth_lowpass(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoff, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_BUTTERWORTH_LOWPASS, NUM2DBL(order),
		NUM2DBL(frequency_cutoff), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     Image.fmask_gauss_highpass(x, y, frequency_cutoff, amplitude_cutoff) ->
 *       image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_gauss_highpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_GAUSS_HIGHPASS,
		NUM2DBL(frequency_cutoff), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     Image.fmask_gauss_lowpass(x, y, frequency_cutoff, amplitude_cutoff) ->
 *       image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_gauss_lowpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_GAUSS_LOWPASS,
		NUM2DBL(frequency_cutoff), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     Image.fmask_ideal_ringpass(x, y, frequency_cutoff, width) -> image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_ideal_ringpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE width)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_IDEAL_RINGPASS,
		NUM2DBL(frequency_cutoff), NUM2DBL(width)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     Image.fmask_ideal_ringreject(x, y, frequency_cutoff, width) -> image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_ideal_ringreject(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE width)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_IDEAL_RINGREJECT,
		NUM2DBL(frequency_cutoff), NUM2DBL(width)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     Image.fmask_butterworth_ringpass(x, y, order, frequency_cutoff,
 *       width, amplitude_cutoff) -> image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_butterworth_ringpass(VALUE obj, VALUE x, VALUE y,
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

/*
 *  call-seq:
 *     Image.fmask_butterworth_ringreject(x, y, order, frequency_cutoff,
 *       width, amplitude_cutoff) -> image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_butterworth_ringreject(VALUE obj, VALUE x, VALUE y,
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

/*
 *  call-seq:
 *     Image.fmask_gauss_ringpass(x, y, frequency_cutoff, width,
 *       amplitude_cutoff) -> image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_gauss_ringpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE width, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_GAUSS_RINGPASS,
		NUM2DBL(frequency_cutoff), NUM2DBL(width), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     Image.fmask_gauss_ringreject(x, y, frequency_cutoff, width,
 *       amplitude_cutoff) -> image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_gauss_ringreject(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE width, VALUE amplitude_cutoff)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_GAUSS_RINGREJECT,
		NUM2DBL(frequency_cutoff), NUM2DBL(width), NUM2DBL(amplitude_cutoff)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     Image.fmask_ideal_bandpass(x, y, frequency_cutoffx, frequency_cutoffy,
 *       radius) -> image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_ideal_bandpass(VALUE obj, VALUE x, VALUE y,
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

/*
 *  call-seq:
 *     Image.fmask_ideal_bandreject(x, y, frequency_cutoffx, frequency_cutoffy,
 *       radius)
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_ideal_bandreject(VALUE obj, VALUE x, VALUE y,
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

/*
 *  call-seq:
 *     Image.fmask_butterworth_bandpass(x, y, order, frequency_cutoffx,
 *       frequency_cutoffy, radius, amplitude_cutoff) -> image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_butterworth_bandpass(VALUE obj, VALUE x, VALUE y,
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

/*
 *  call-seq:
 *     Image.fmask_butterworth_bandreject(x, y, order, frequency_cutoffx,
 *       frequency_cutoffy, radius, amplitude_cutoff) -> image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_butterworth_bandreject(VALUE obj, VALUE x, VALUE y,
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

/*
 *  call-seq:
 *     Image.fmask_gaus_bandpass(x, y, frequency_cutoffx, frequency_cutoffy,
 *       radius, amplitude_cutoff) -> image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_gauss_bandpass(VALUE obj, VALUE x, VALUE y,
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

/*
 *  call-seq:
 *     Image.fmask_gauss_bandreject, x, y, frequency_cutoffx, frequency_cutoffy,
 *       radius, amplitude_cutoff) -> image
 *
 *  See Image.fmask_ideal_highpass
 */

VALUE
img_s_fmask_gauss_bandreject(VALUE obj, VALUE x, VALUE y,
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

/*
 *  call-seq:
 *     Image.fmask_fractal_flt(x, y, fractal_dimension) -> image
 *
 *  This mask is handy for filtering images of gaussian noise in order to create
 *  surfaces of a given fractal dimension. @fractal_dimension should be between
 *  2 and 3.
 */

VALUE
img_s_fmask_fractal_flt(VALUE obj, VALUE x, VALUE y,
	VALUE fractal_dimension)
{
	OutPartial(new, data, im);

    if (im_create_fmask(im,
		NUM2INT(x), NUM2INT(y), VIPS_MASK_FRACTAL_FLT,
		NUM2DBL(fractal_dimension)))
        vips_lib_error();

    return new;	
}

#else

VALUE
img_s_fmask_ideal_highpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_ideal_lowpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_butterworth_highpass(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoff, VALUE amplitude_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_butterworth_lowpass(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoff, VALUE amplitude_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_gauss_highpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE amplitude_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_gauss_lowpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE amplitude_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_ideal_ringpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE width)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_ideal_ringreject(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE width)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_butterworth_ringpass(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoff, VALUE width, VALUE amplitude_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_butterworth_ringreject(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoff, VALUE width, VALUE amplitude_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_gauss_ringpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE width, VALUE amplitude_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_gauss_ringreject(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoff, VALUE width, VALUE amplitude_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_ideal_bandpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoffx, VALUE frequency_cutoffy, VALUE radius)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_ideal_bandreject(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoffx, VALUE frequency_cutoffy, VALUE radius)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_butterworth_bandpass(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoffx, VALUE frequency_cutoffy, VALUE radius,
	VALUE amplitude_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_butterworth_bandreject(VALUE obj, VALUE x, VALUE y,
	VALUE order, VALUE frequency_cutoffx, VALUE frequency_cutoffy, VALUE radius,
	VALUE amplitude_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_gauss_bandpass(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoffx, VALUE frequency_cutoffy, VALUE radius,
	VALUE amplitude_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_gauss_bandreject(VALUE obj, VALUE x, VALUE y,
	VALUE frequency_cutoffx, VALUE frequency_cutoffy, VALUE radius,
	VALUE amplitude_cutoff)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

VALUE
img_s_fmask_fractal_flt(VALUE obj, VALUE x, VALUE y,
	VALUE fractal_dimension)
{
    rb_raise(eVIPSError, "Operation not supported with your version of VIPS.");
}

#endif

/*
 *  call-seq:
 *     im.freqflt(other_image) -> image
 *
 *  Filter an image in Fourier space.
 *
 *  *self* is transformed to Fourier space, multipled with <i>other_image</i>,
 *  then transformed back to real space. If *self* is already a complex image,
 *  just multiply then inverse transform.
 */

VALUE
img_freqflt(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_freqflt);
}

/*
 *  call-seq:
 *     im.disp_ps -> image
 *
 *  Make a displayable (ie. 8-bit unsigned int) power spectrum.
 *
 *  If *self* is non-complex, it is transformed to Fourier space. Then the
 *  absolute value is passed through Image#scaleps, and Image#rotquad.
 */

VALUE
img_disp_ps(VALUE obj)
{
	RUBY_VIPS_UNARY(im_disp_ps);
}

/*
 *  call-seq:
 *     im.phasecor_fft(other_image) -> image
 *
 *  Convert the two input images to Fourier space, calculate phase-correlation,
 *  back to real space.
 */

VALUE
img_phasecor_fft(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_phasecor_fft);
}

/*
 *  call-seq:
 *     Image.fractsurf(size, frd) -> image
 *
 *  Generate an image of size <i>size</i> and fractal dimension <i>frd</i>. The
 *  dimension should be between 2 and 3.
 */

VALUE
img_s_fractsurf(VALUE obj, VALUE size, VALUE frd)
{
	OutPartial(new, data, im);

    if (im_fractsurf(im, NUM2INT(size), NUM2DBL(frd)))
        vips_lib_error();

    return new;
}
