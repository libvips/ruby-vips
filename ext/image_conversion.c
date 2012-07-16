#include "ruby_vips.h"
#include "image.h"
#include "mask.h"
#include "header.h"

static ID id_black, id_extend, id_repeat, id_mirror, id_white;

/*
 *  call-seq:
 *     im.to_mask -> mask
 *
 *  Make a mask from an image. All images are cast to band format :DOUBLE
 *  before processing. There are two cases for handling bands:
 *
 *  If the image has a single band, Image#to_mask will write a mask the same
 *  size as the image.
 *
 *  If the image has more than one band, it must be one pixel high or wide. In
 *  this case the output mask uses that axis to represent band values.
 */

VALUE
img_to_mask(VALUE obj)
{
    vipsMask *msk;
    VALUE mask = mask_alloc(cVIPSMask);
	GetImg(obj, data, im);

    Data_Get_Struct(mask, vipsMask, msk);

    if (!(msk->dmask = im_vips2mask(im, "img_vips2mask")))
        vips_lib_error();

    return mask;
}

/*
 *  call-seq:
 *     im.dup -> image
 *
 *  Create a copy of an image.
 */

VALUE
img_dup(VALUE obj)
{
	RUBY_VIPS_UNARY(im_copy);
}

/*
 *  call-seq:
 *     im.copy_swap -> image
 *
 *  Copy an image, swapping byte order between little and big endian. This
 *  really does change image pixels and does not just alter the header.
 */

VALUE
img_copy_swap(VALUE obj)
{
	RUBY_VIPS_UNARY(im_copy_swap);
}

/*
 *  call-seq:
 *     im.copy_native -> image
 *
 *  Copy an image to native order, that is, the order for the executing program.
 */

VALUE
img_copy_native(VALUE obj, VALUE input_msb_first)
{
#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 22
    vipsImg *in, *out;
    int msb_first;

    VALUE new = img_spawn(obj);

    Data_Get_Struct(obj, vipsImg, in);
    Data_Get_Struct(new, vipsImg, out);

    msb_first = input_msb_first == Qtrue ? TRUE : FALSE;
    if( im_copy_native(in->in, out->in, msb_first) )
        vips_lib_error();

    return new;
#else
    rb_raise(eVIPSError, "Your version of vips doesn't support this operation");
#endif
}

/*
 *  call-seq:
 *     im.copy_file -> image
 *
 *  Copy an image to a disc file, then copy again to output. If the image is
 *  already a disc file, just copy straight through.
 *
 *  The file is automatically deleted when <i>image</i> is closed.
 */

VALUE
img_copy_file(VALUE obj)
{
    vipsImg *in, *out;

    VALUE new = img_spawn(obj);

    Data_Get_Struct(obj, vipsImg, in);
    Data_Get_Struct(new, vipsImg, out);

    if( im_copy_file(in->in, out->in) )
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.clip2fmt(fmt_symbol) -> image
 *
 *  Convert the image to <i>fmt_symbol</i> format. You can convert between any
 *  pair of formats. Floats are truncated (not rounded). Out of range values are
 *  clipped.
 *
 *  <i>fmt_symbol</i> can be any of: :NOTSET, :UCHAR, :CHAR, :USHORT, :SHORT,
 *  :UINT, :INT, :FLOAT, :COMPLEX, :DOUBLE, :DPCOMPLEX.
 */

VALUE
img_clip2fmt(VALUE obj, VALUE fmt)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_clip2fmt(im, im_new, header_id_to_band_fmt(SYM2ID(fmt))))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.scale -> image
 *
 *  Search the image for the maximum and minimum value, then return the image
 *  as unsigned 8-bit, scaled so that the maximum value is 255 and the
 *  minimum is zero.
 */

VALUE
img_scale(VALUE obj)
{
	RUBY_VIPS_UNARY(im_scale);
}

/*
 *  call-seq:
 *     im.msb([band]) -> image
 *
 *  Turn any integer image to 8-bit unsigned char by discarding all but the most
 *  significant byte. Signed values are converted to unsigned by adding 128.
 *
 *  This operator also works for LABQ coding.
 *
 *  If <i>band</i> is given, this will turn any integer image to a single-band'
 *  8-bit unsigned char by discarding all but the most significant byte from the
 *  selected band.
 */

VALUE
img_msb(int argc, VALUE *argv, VALUE obj)
{
	VALUE v_num;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	rb_scan_args(argc, argv, "01", &v_num);

	if (NIL_P(v_num)) {
		if (im_msb(im, im_new))
			vips_lib_error();
	} else if (im_msb_band(im, im_new, NUM2INT(v_num)))
		vips_lib_error();
	
	return new;
}

/*
 *  call-seq:
 *     im.c2amph -> image
 *
 *  Convert a complex image from rectangular to polar coordinates. Angles are
 *  expressed in degrees.
 */

VALUE
img_c2amph(VALUE obj)
{
	RUBY_VIPS_UNARY(im_c2amph);
}

/*
 *  call-seq:
 *     im.c2rect -> image
 *
 *  Convert a complex image from polar to rectangular coordinates. Angles are
 *  expressed in degrees.
 */

VALUE
img_c2rect(VALUE obj)
{
	RUBY_VIPS_UNARY(im_c2rect);
}

/*
 *  call-seq:
 *     im.ri2c(other_image) -> image
 *
 *  Compose two real images to make a complex image. If either image has band
 *  format :DOUBLE, the output image is band format :DPCOMPLEX. Otherwise the
 *  output image is :COMPLEX.
 *
 *  If the number of bands differs, one of the images must have one band. In
 *  this case, an n-band image is formed from the one-band image by joining n
 *  copies of the one-band image together, and then the two n-band images are
 *  operated upon.
 *
 *  The two input images are cast up to the smallest common type.
 */

VALUE
img_ri2c(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_ri2c);
}

/*
 *  call-seq:
 *     im.c2imag -> image
 *
 *  Extract the imaginary part of a complex image.
 */

VALUE
img_c2imag(VALUE obj)
{
	RUBY_VIPS_UNARY(im_c2imag);
}

/*
 *  call-seq:
 *     im.c2real -> image
 *
 *  Extract the real part of a complex image.
 */

VALUE
img_c2real(VALUE obj)
{
	RUBY_VIPS_UNARY(im_c2real);
}

/*
 *  call-seq:
 *     im.scaleps -> image
 *
 *  Scale a power spectrum. Transform with log10(1.0 + x ** 0.25)) + 0.5,
 *  then scale so max == 255.
 */

VALUE
img_scaleps(VALUE obj)
{
	RUBY_VIPS_UNARY(im_scaleps);
}

/*
 *  call-seq:
 *    im.falsecolour -> image
 *
 *  Turn a 1-band 8-bit image into a 3-band 8-bit image with a false colour
 *  map. The map is supposed to make small differences in brightness more
 *  obvious.
 */

VALUE
img_falsecolour(VALUE obj)
{
	RUBY_VIPS_UNARY(im_falsecolour);
}

/*
 *  call-seq:
 *     Image.gaussnoise(width, height, mean, sigma) -> image
 *
 *  Make a one band float image of gaussian noise with the specified
 *  distribution. The noise distribution is created by averaging 12 random
 *  numbers with the appropriate weights.
 */

VALUE
img_s_gaussnoise(VALUE obj, VALUE x, VALUE y, VALUE mean, VALUE sigma)
{
	OutPartial(new, data, im_new);

    if (im_gaussnoise(im_new, NUM2INT(x), NUM2INT(y), NUM2DBL(mean),
        NUM2DBL(sigma)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     Image.black(width, height, bands) -> image
 *
 *  Make a black unsigned char image of a specified size.
 */

VALUE
img_s_black(VALUE obj, VALUE width, VALUE height, VALUE bands)
{
	OutPartial(new, data, im_new);

    if (im_black(im_new, NUM2INT(width), NUM2INT(height), NUM2DBL(bands)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     Image.text(text, font, width, alignment, dpi) -> image
 *
 *  Draw the string <i>text</i> to an image. @out is a one-band 8-bit
 *  unsigned char image, with 0 for no text and 255 for text. Values inbetween
 *  are used for anti-aliasing.
 *
 *  <i>text</i> is the text to render as a UTF-8 string. It can contain Pango
 *  markup, for example "&lt;i&gt;The&lt;/i&gt;Guardian".
 *
 *  <i>font</i> is the font to render with, selected by fontconfig. Examples
 *  might be "sans 12" or perhaps "bitstream charter bold 10".
 *
 *  <i>width</i> is the maximum number of pixels across to draw within. If the
 *  generated text is wider than this, it will wrap to a new line. In this
 *  case, <i>alignment</i> can be used to set the alignment style for multi-line
 *  text. 0 means left-align, 1 centre, 2 right-align.
 *
 *  <i>dpi</i> sets the resolution to render at. "sans 12" at 72 dpi draws
 *  characters approximately 12 pixels high.
 */

VALUE
img_s_text(VALUE obj, VALUE text, VALUE font, VALUE width,
    VALUE alignment, VALUE dpi)
{
	OutPartial(new, data, im_new);

    if (im_text(im_new, StringValuePtr(text), StringValuePtr(font),
		NUM2INT(width), NUM2INT(alignment), NUM2INT(dpi)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.extract_band(band [,num_bands]) -> image
 *
 *  Extract the band given by <i>band</i> from the image. If <i>num_bands</i> is
 *  given, extract that many bands. Bands number from zero.
 *
 *    im.extract_band(0, 2) # => new image with bands 0 & 1 from original.
 *
 *  Extracting bands outside the input image will trigger an error.
 */

VALUE
img_extract_band(int argc, VALUE *argv, VALUE obj)
{
	VALUE v_start_band, v_num_bands;
	int num_bands;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	rb_scan_args(argc, argv, "11", &v_start_band, &v_num_bands);
	num_bands = NIL_P(v_num_bands) ? 1 : NUM2INT(v_num_bands);

	if (im_extract_bands(im, im_new, NUM2INT(v_start_band), num_bands))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.extract_area(left, top, width, height)                    -> image
 *     im.extract_area(left, top, width, height, band [,num_bands]) -> image
 *
 *  The first form will extract an area from an image.
 *
 *  The second form will extract an area and a number of bands from an image.
 *  Bands number from zero. Extracting outside the input image will trigger an
 *  error.
 */

VALUE
img_extract_area(int argc, VALUE *argv, VALUE obj)
{
	VALUE v_left, v_top, v_width, v_height, v_start_band, v_num_bands;
	int start_band, num_bands;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	rb_scan_args(argc, argv, "42", &v_left, &v_top, &v_width, &v_height,
		&v_start_band, &v_num_bands);

	/* Start band defaults to the first band */
	start_band = NIL_P(v_start_band) ? 0 : NUM2INT(v_start_band);
	
	/* Number of bands defaults to all bands or the selected band.
	 */
	if (NIL_P(v_num_bands))
		num_bands = NIL_P(v_start_band) ? im->Bands : 1;
	else
		num_bands = NUM2INT(v_num_bands);
	
    if (im_extract_areabands(im, im_new, NUM2INT(v_left), NUM2INT(v_top),
        NUM2INT(v_width), NUM2INT(v_height), start_band, num_bands))
        vips_lib_error();

    return new;  
}

/*
 *  call-seq:
 *     im.embed(type_sym, x, y, width, height) -> image
 *
 *  The opposite of Image#extract: embed an image within a larger image.
 *  <i>type_sym</i> controls what appears in the new pixels:
 *
 *  * :black - black pels (all bytes == 0)
 *  * :extend - extend pels from image edge
 *  * :repeat - repeat image
 *  * :mirror - mirror image
 *  * :white - white pixels (all bytes == 255)
 */

VALUE
img_embed(VALUE obj, VALUE type_v, VALUE x, VALUE y, VALUE width, VALUE height)
{
    ID type_id = SYM2ID(type_v);
    int type;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if      (type_id == id_black)  type = 0;
    else if (type_id == id_extend) type = 1;
    else if (type_id == id_repeat) type = 2;
    else if (type_id == id_mirror) type = 3;
    else if (type_id == id_white)  type = 4;
    else
        rb_raise(rb_eArgError, "Extend type must be :black, :extend, :repeat, :mirror, :white");

    if (im_embed(im, im_new, type, NUM2INT(x), NUM2INT(y), NUM2INT(width),
        NUM2INT(height)))
        vips_lib_error();

    return new;  
}

/*
 *  call-seq:
 *     im.tile_cache(tile_width, tile_height, max_tiles) -> image
 *
 * This operation behaves rather like copy between images, 
 * except that it keeps a cache of computed pixels. 
 * This cache is made of up to max_tiles tiles (a value of -1 for
 * means any number of tiles), and each tile is of size tile_width
 * by tile_height pixels. 
 */

VALUE
img_tile_cache(VALUE obj, VALUE tile_width, VALUE tile_height, VALUE max_tiles)
{
    GetImg(obj, data, im);
    OutImg(obj, new, data_new, im_new);

    if (im_tile_cache(im, im_new,
	NUM2INT(tile_width), NUM2INT(tile_height), NUM2INT(max_tiles)))
        vips_lib_error();

    return new;  
}

/*
 *  call-seq:
 *     im.bandjoin(other_image, ...) -> image
 *
 *  Join a set of images together, bandwise. If the images have n and m bands,
 *  then the output image will have n + m bands, with the first n coming from
 *  the first image and the last m from the second.
 *
 *  The images must be the same size. The input images are cast up to the
 *  smallest common type.
 */

VALUE
img_bandjoin(int argc, VALUE *argv, VALUE obj)
{
	vipsImg *im_t;
    IMAGE **ins;
    int i;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	if(argc == 0)
		rb_raise(rb_eArgError, "Need at least one argument");

    ins = IM_ARRAY(im_new, argc + 1, IMAGE*);
	ins[0] = im;

    for (i = 0; i < argc; i++) {
		img_add_dep(data_new, argv[i]);
        Data_Get_Struct(argv[i], vipsImg, im_t);
        ins[i + 1] = im_t->in;
    }

    if (im_gbandjoin(ins, im_new, argc + 1))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.insert_noexpand(other_image, x, y) -> image
 *
 *  Insert one image into another. <i>other_image</i> is inserted into *self* at
 *  position <i>x</i>, <i>y</i> relative to the top LH corner of *self*. The
 *  output image is the same size as *self*. <i>other_image</i> is clipped
 *  against the edges of *self*.
 *
 *  If the number of bands differs, one of the images must have one band. In
 *  this case, an n-band image is formed from the one-band image by joining n
 *  copies of the one-band image together, and then the two n-band images are
 *  operated upon.
 *
 *  The two input images are cast up to the smallest common type.
 */

VALUE
img_insert_noexpand(VALUE obj, VALUE obj2, VALUE x, VALUE y)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_insert_noexpand(im, im2, im_new, NUM2INT(x), NUM2INT(y)))
        vips_lib_error();

    return new;
}

static VALUE
img_insert_one(VALUE obj, VALUE obj2, VALUE x, VALUE y)
{
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_insert(im, im2, im_new, NUM2INT(x), NUM2INT(y)))
        vips_lib_error();

    return new;
}

static VALUE
img_insertset(int argc, VALUE *argv, VALUE obj, VALUE obj2)
{
	int i, *x, *y;
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    x = IM_ARRAY(im_new, argc - 1, int);
    y = IM_ARRAY(im_new, argc - 1, int);

    for(i = 1; i < argc; i++) {
        x[i - 1] = NUM2INT(RARRAY_PTR(argv[i])[0]);
        y[i - 1] = NUM2INT(RARRAY_PTR(argv[i])[1]);
    }

    if( im_insertset(im, im2, im_new, argc - 1, x, y) )
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.insert(other_image, x, y)             -> image
 *     im.insert(other_image, coordinates, ...) -> image
 *
 *  Insert <i>other_image</i> repeatedly into *self* at the positions listed in
 *  the <i>coordinates</i> arrays. These arrays should contain x and y
 *  coordinates:
 *
 *    im.insert(other_image, 100, 100) # => other_image inserted once at 100, 100
 *    im.insert(other_image, [12, 20], [32, 22]) # => inserted twice at given coordinates
 *  The output image is the same size as *self*. <i>other_image</i> is clipped
 *  against the edges of *self*.
 *
 *  This operation is fast for large @n, but will use a memory buffer the size
 *  of the output image. It's useful for things like making scatter plots.
 *
 *  If the number of bands differs, one of the images must have one band. In
 *  this case, an n-band image is formed from the one-band image by joining n
 *  copies of the one-band image together, and then the two n-band images are
 *  operated upon.
 *
 *  The two input images are cast up to the smallest common type.
 */

VALUE
img_insert(int argc, VALUE *argv, VALUE obj)
{
	VALUE obj2, x_or_ary, y_or_arys;

	rb_scan_args(argc, argv, "2*", &obj2, &x_or_ary, &y_or_arys);

	if (TYPE(x_or_ary) == T_ARRAY) {
		if (!NIL_P(y_or_arys))
			rb_ary_unshift(y_or_arys, x_or_ary);

		return img_insertset(RARRAY_LEN(y_or_arys), RARRAY_PTR(y_or_arys), obj,
			obj2);
	}

	if (RARRAY_LEN(y_or_arys) != 1)
		rb_raise(rb_eArgError,
			"Need two coordinates or an array of coordinates");

	return img_insert_one(obj, obj2, x_or_ary, RARRAY_PTR(y_or_arys)[0]);
}

/*
 *  call-seq:
 *     im.lrjoin(other_image) -> image
 *
 *  Join *self* and <i>other_image</i> together, left-right. If one is taller
 *  than the other, the output image will be as high as the smaller image.
 *
 *  If the number of bands differs, one of the images must have one band. In
 *  this case, an n-band image is formed from the one-band image by joining n
 *  copies of the one-band image together, and then the two n-band images are
 *  operated upon.
 *
 *  The two input images are cast up to the smallest common type.
 */

VALUE
img_lrjoin(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_lrjoin);
}

/*
 *  call-seq:
 *     im.tbjoin(other_image) -> image
 *
 *  Join *self* and <i>other_image</i> together, top-bottom. If one is taller
 *  than the other, the output image will be as high as the smaller image.
 *
 *  If the number of bands differs, one of the images must have one band. In
 *  this case, an n-band image is formed from the one-band image by joining n
 *  copies of the one-band image together, and then the two n-band images are
 *  operated upon.
 *
 *  The two input images are cast up to the smallest common type.
 */

VALUE
img_tbjoin(VALUE obj, VALUE obj2)
{
	RUBY_VIPS_BINARY(im_tbjoin);
}

/*
 *  call-seq:
 *     im.replicate(across, down) -> image
 *
 *  Replicate an image <i>across</i> times horizontally <i>down</i> times
 *  vertically.
 */

VALUE
img_replicate(VALUE obj, VALUE across, VALUE down)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_replicate(im, im_new, NUM2INT(across), NUM2INT(down)))
        vips_lib_error();

    return new;  
}

/*
 *  call-seq:
 *     im.grid(tile_height, across, down) -> image
 *
 *  Chop a tall thin image up into a set of tiles, lay the tiles out in a grid.
 *
 *  *self* should be a very tall, thin image containing a list of smaller
 *  images. Volumetric or time-sequence images are often laid out like this.
 *  This image is chopped into a series of tiles, each <i>tile_height</i> pixels'
 *  high and the width of *self*. The tiles are then rearranged into a grid
 *  <i>across</i> tiles across and <i>down</i> tiles down in row-major order.
 */

VALUE
img_grid(VALUE obj, VALUE tile_height, VALUE across, VALUE down)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_grid(im, im_new, NUM2INT(tile_height), NUM2INT(across),
		NUM2INT(down)))
        vips_lib_error();

    return new;  
}

/*
 *  call-seq:
 *     im.wrap(x, y) -> image
 *
 *  Slice an image up and move the segments about so that the pixel that was
 *  at 0, 0 is now at <i>x</i>, <i>y</i>.
 */

VALUE
img_wrap(VALUE obj, VALUE x, VALUE y)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_wrap(im, im_new, NUM2INT(x), NUM2INT(y)))
        vips_lib_error();

    return new;  
}

/*
 *  call-seq:
 *     im.fliphor -> image
 *
 *  Flips an image left-right.
 */

VALUE
img_fliphor(VALUE obj)
{
	RUBY_VIPS_UNARY(im_fliphor);
}

/*
 *  call-seq:
 *     im.flipver -> image
 *
 *  Flips an image top-bottom.
 */

VALUE
img_flipver(VALUE obj)
{
	RUBY_VIPS_UNARY(im_flipver);
}

/*
 *  call-seq:
 *     im.rot90 -> image
 *
 *  Rotate an image 90 degrees.
 */

VALUE
img_rot90(VALUE obj)
{
	RUBY_VIPS_UNARY(im_rot90);
}

/*
 *  call-seq:
 *     im.rot180 -> image
 *
 *  Rotate an image 180 degrees.
 */

VALUE
img_rot180(VALUE obj)
{
	RUBY_VIPS_UNARY(im_rot180);
}

/*
 *  call-seq:
 *     im.rot270 -> image
 *
 *  Rotate an image 270 degrees.
 */

VALUE
img_rot270(VALUE obj)
{
	RUBY_VIPS_UNARY(im_rot270);
}

/*
 *  call-seq:
 *     im.subsample(x [,y]) -> image
 *
 *  Subsample an image by an integer fraction. This is fast nearest-neighbour
 *  shrink.
 *
 *  If only one integer is given the width and height are subsampled equally.
 */

VALUE
img_subsample(int argc, VALUE *argv, VALUE obj)
{
    VALUE x, y;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	rb_scan_args(argc, argv, "11", &x, &y);
    if (NIL_P(y))
        y = x;
    
    if (im_subsample(im, im_new, NUM2INT(x), NUM2INT(y)))
        vips_lib_error();

    return new;  
}

/*
 *  call-seq:
 *     im.zoom(x [,y]) -> image
 *
 *  Zoom an image by repeating pixels. This is fast nearest-neighbour
 *  zoom.
 *
 *  If only one integer is given the width and height are zoomed equally.
 */

VALUE
img_zoom(int argc, VALUE *argv, VALUE obj)
{
    VALUE x, y;
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

	rb_scan_args(argc, argv, "11", &x, &y);
    if (NIL_P(y))
        y = x;

    if (im_zoom(im, im_new, NUM2INT(x), NUM2INT(y)))
        vips_lib_error();

    return new;
}

void
init_Image_conversion()
{
    id_black = rb_intern("black");
    id_extend = rb_intern("extend");
    id_repeat = rb_intern("repeat");
    id_mirror = rb_intern("mirror");
    id_white = rb_intern("white");
}

