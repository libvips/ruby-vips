#include "ruby_vips.h"

ID id_match_left, id_match_right, id_match_both, id_match_none;

/*
 *  call-seq:
 *     im.lrmerge(other_image, dx, dy [,mwidth]) -> image
 *
 *  Merge *self* as the reference image and <i>other_image</i> as the secondary
 *  image according to the values <i>dx</i> and <i>dy</i>. <i>dx</i> and
 *  <i>dy</i> give the displacement of <i>other_image</i> relative to *self*.
 *  The result is written to the output image.
 *
 *  The program carries out a smooth merge using a raised cosine function.
 *  Works for any image type, including LABPACK.
 *
 *  Pixels are treated with the value zero as "transparent", that is, zero
 *  pixels in the overlap area do not contribute to the merge. This makes it
 *  possible to join non-rectangular images.
 *
 *  The "mwidth" parameter limits the maximum width of the blend area. If not
 *  given, the width will be unlimited.
 */

VALUE
img_lrmerge(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, dx, dy, mwidth_v;
    int mwidth = -1;

    rb_scan_args(argc, argv, "31", &obj2, &dx, &dy, &mwidth_v);
    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_lrmerge(im, im2, im_new, NUM2INT(dx), NUM2INT(dy), mwidth))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.tbmerge(other_image, dx, dy [,mheight]) -> image
 *
 *  see Image#lrmerge .
 */

VALUE
img_tbmerge(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, dx, dy, mwidth_v;
    int mwidth = -1;

    rb_scan_args(argc, argv, "31", &obj2, &dx, &dy, &mwidth_v);
    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

    GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_tbmerge(im, im2, im_new, NUM2INT(dx), NUM2INT(dy), mwidth))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.lrmerge1(other_image, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2
 *       [,mwidth]) -> image
 * 
 * 1st order left-right merge.
 */

VALUE
img_lrmerge1(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, mwidth_v;
    int mwidth = -1;

    rb_scan_args(argc, argv, "91", &obj2, &xr1, &yr1, &xs1, &ys1, &xr2, &yr2,
        &xs2, &ys2, &mwidth_v);
    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_lrmerge1(im, im2, im_new, NUM2INT(xr1), NUM2INT(yr1), NUM2INT(xs1),
		NUM2INT(ys1), NUM2INT(xr2), NUM2INT(yr2), NUM2INT(xs2),	NUM2INT(ys2),
		mwidth))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.tbmerge1(other_image, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2
 *       [,mheight]) -> image
 *
 * 1st order top-bottom merge.
 */

VALUE
img_tbmerge1(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2, mwidth_v;
    int mwidth = -1;

    rb_scan_args(argc, argv, "91", &obj2, &xr1, &yr1, &xs1, &ys1, &xr2, &yr2,
        &xs2, &ys2, &mwidth_v);
    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_tbmerge1(im, im2, im_new, NUM2INT(xr1), NUM2INT(yr1), NUM2INT(xs1),
		NUM2INT(ys1), NUM2INT(xr2), NUM2INT(yr2), NUM2INT(xs2), NUM2INT(ys2),
		mwidth))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.lrmosaic(other_image, band, xref, yref, xsec, ysec,
 *       halfcorrelation=5, halfarea=14 [,balancetype] [,mwidth]) -> image
 *
 *  Mosaic *self* and <i>other_image</i> left-right.
 *
 *  In order to carry out mosaicing, the coordinates of one tie point are
 *  required.  The tie point is expected to be in the overlapping area and has
 *  coordinates (<i>xref</i>, <i>yref</i>) on *self*, and (<i>xsec</i>,
 *  <i>ysec</i>) on <i>other_image</i>. The tie-point is not used as a start
 *  point for the search, but is used to specify the overlap of the two images.
 *
 *  The function splits the overlap area into three parts (top, middle and
 *  bottom) and searches t*self* in each part for the 20 best high contrast
 *  points. These 60 points are then searched for in <i>other_image</i>, giving
 *  a set of 60 possible corrected vectors.
 *
 *  A straight line is fitted through the 60 vectors, and points discarded which
 *  lie a significant distance from the line. The line is then refitted to the
 *  remaining points, and the process repeated until either all remaining points
 *  lie on a straight line, or too many points have been discarded.
 *
 *  If a good straight line fit is found, *self* and <i>other_image</i> are
 *  joined. If no fit was found, the function fails with an error message. Note
 *  that this function detects rotation: if the straight line found requires
 *  <i>other_image</i> to be rotated, it also fails with an error message.
 *
 *  <i>halfcorrelationsize</i> - sets the size of the fragments of *self* for
 *  which the function searches sec. The actual window will be of size
 *  2 * <i>halfcorrelationsize</i> + 1. We recommend a value of 5.
 *
 *  <i>halfareasize</i> - sets the size of the area of sec that is searched. The
 *  The actual area searched will be of size 2 * <i>halfareasize</i> + 1. We
 *  recommend a value of 14.
 *
 *  <i>balancetype</i> - sets the style of the balancing the functions perform.
 *  Balancing finds the average value of pixels in the overlap area, and scales
 *  the left and right images so as to make the images match in average overlap.
 *
 *  * :balance_none - no balancing.
 *  * :balance_left - keep the left image unadjusted and adjust the contrast of
 *    the right image to match the left.
 *  * :balance_right - keep the right image unadjusted and scale the left image
 *    to match it.
 *  * :balance_both - adjust the contrast of both the left and right images to
 *    bring both averages to a middle value. The middle value chosen is weighted
 *    by the number of pixels in each image: large images will be adjusted less
 *    than small images.
 *
 *  Balancing is useful for mosaicing frames from photographic or video sources
 *  where exact colour control is impossible and exposure varies from frame to
 *  frame. Balancing is only allowed for uncoded uchar images.
 * 
 *  The <i>mwidth</i> parameter sets the maximum blend width, see Image#lrmerge.
 */

VALUE
img_lrmosaic(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, bandno, xref, yref, xsec, ysec, halfcorrelation_v, halfarea_v,
        balancetype_v, mwidth_v;
    ID balancetype_id;
    int mwidth = -1, halfcorrelation = 5, halfarea = 14, balancetype = 0;

    rb_scan_args(argc, argv, "64", &obj2, &bandno, &xref, &yref, &xsec, &ysec,
        &halfcorrelation_v, &halfarea_v, &balancetype_v, &mwidth_v);

    if (!NIL_P(halfcorrelation_v))
        halfcorrelation = NUM2INT(halfcorrelation_v);

    if (!NIL_P(halfarea_v))
        halfarea = NUM2INT(halfarea_v);

    if (!NIL_P(balancetype_v)) {
        balancetype_id = SYM2ID(balancetype_v);
 if (balancetype_id == id_match_none) balancetype = 0;
 else if (balancetype_id == id_match_left) balancetype = 1;
        else if (balancetype_id == id_match_right) balancetype = 2;
        else if (balancetype_id == id_match_both) balancetype = 3;
        else
            rb_raise(rb_eArgError, "Balance type must be nil, :match_left, :match_right, or :match_both");
    }

    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_lrmosaic(im, im2, im_new, NUM2INT(bandno), NUM2INT(xref),
		NUM2INT(yref), NUM2INT(xsec), NUM2INT(ysec), NUM2INT(halfcorrelation),
		NUM2INT(halfarea), NUM2INT(balancetype), NUM2INT(mwidth)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.tbmosaic(other_image, band, xref, yref, xsec, ysec,
 *       halfcorrelation=5, halfarea=14 [,balancetype] [,mheight]) -> image
 *
 *  Mosaic *self* and <i>other_image</i> top-bottom.
 *
 *  See Image#lrmosaic .
 */

VALUE
img_tbmosaic(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, bandno, xref, yref, xsec, ysec, halfcorrelation_v, halfarea_v,
        balancetype_v, mwidth_v;
    ID balancetype_id;
    int mwidth = -1, halfcorrelation = 5, halfarea = 14, balancetype = 0;

    rb_scan_args(argc, argv, "64", &obj2, &bandno, &xref, &yref, &xsec, &ysec,
        &halfcorrelation_v, &halfarea_v, &balancetype_v, &mwidth_v);

    if (!NIL_P(halfcorrelation_v))
        halfcorrelation = NUM2INT(halfcorrelation_v);

    if (!NIL_P(halfarea_v))
        halfarea = NUM2INT(halfarea_v);

    if (!NIL_P(balancetype_v)) {
        balancetype_id = SYM2ID(balancetype_v);

        if (balancetype_id == id_match_left) balancetype = 1;
        else if (balancetype_id == id_match_right) balancetype = 2;
        else if (balancetype_id == id_match_both) balancetype = 3;
        else
            rb_raise(rb_eArgError, "Balance type must be nil, :match_left, :match_right, or :match_both");
    }

    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

    GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_tbmosaic(im, im2, im_new, NUM2INT(bandno), NUM2INT(xref),
        NUM2INT(yref), NUM2INT(xsec), NUM2INT(ysec), halfcorrelation, halfarea,
        balancetype, mwidth))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.lrmosaic1(other_image, band, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2,
 *       halfcorrelation=5, halfarea=14 [,balancetype] [,mwidth]) -> image
 *
 *  1st order left-right mosaic.
 */

VALUE
img_lrmosaic1(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, bandno, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2,
        halfcorrelation_v, halfarea_v, balancetype_v, mwidth_v;
    ID balancetype_id;
    int mwidth = -1, halfcorrelation = 5, halfarea = 14, balancetype = 0;

    rb_scan_args(argc, argv, "95", &obj2, &bandno, &xr1, &yr1, &xs1, &ys1, &xr2,
        &yr2, &xs2, &ys2, &halfcorrelation_v, &halfarea_v, &balancetype_v,
        &mwidth_v);
    
    if (argc < 10)
        rb_raise(rb_eArgError, "Need at least 10 arguments.");

    if (!NIL_P(halfcorrelation_v))
        halfcorrelation = NUM2INT(halfcorrelation_v);

    if (!NIL_P(halfarea_v))
        halfarea = NUM2INT(halfarea_v);

    if (!NIL_P(balancetype_v)) {
        balancetype_id = SYM2ID(balancetype_v);

        if (balancetype_id == id_match_left) balancetype = 1;
        else if (balancetype_id == id_match_right) balancetype = 2;
        else if (balancetype_id == id_match_both) balancetype = 3;
        else
            rb_raise(rb_eArgError, "Balance type must be nil, :match_left, :match_right, or :match_both");
    }

    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_lrmosaic1(im, im2, im_new, NUM2INT(bandno), NUM2INT(xr1),
		NUM2INT(yr1), NUM2INT(xs1), NUM2INT(ys1), NUM2INT(xr2), NUM2INT(yr2),
		NUM2INT(xs2), NUM2INT(ys2), halfcorrelation, halfarea, balancetype,
        mwidth))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.tbmosaic1(other_image, band, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2,
 *       halfcorrelation=5, halfarea=14 [,balancetype] [,mheight]) -> image
 *
 *  1st order top-bottom mosaic.
 */

VALUE
img_tbmosaic1(int argc, VALUE *argv, VALUE obj)
{
    VALUE obj2, bandno, xr1, yr1, xs1, ys1, xr2, yr2, xs2, ys2,
        halfcorrelation_v, halfarea_v, balancetype_v, mwidth_v;
    ID balancetype_id;
    int mwidth = -1, halfcorrelation = 5, halfarea = 14, balancetype = 0;

    rb_scan_args(argc, argv, "95", &obj2, &bandno, &xr1, &yr1, &xs1, &ys1, &xr2,
        &yr2, &xs2, &ys2, &halfcorrelation_v, &halfarea_v, &balancetype_v,
        &mwidth_v);

    if (argc < 10)
        rb_raise(rb_eArgError, "Need at least 10 arguments.");

    if (!NIL_P(halfcorrelation_v))
        halfcorrelation = NUM2INT(halfcorrelation_v);

    if (!NIL_P(halfarea_v))
        halfarea = NUM2INT(halfarea_v);

    if (!NIL_P(balancetype_v)) {
        balancetype_id = SYM2ID(balancetype_v);

        if (balancetype_id == id_match_left) balancetype = 1;
        else if (balancetype_id == id_match_right) balancetype = 2;
        else if (balancetype_id == id_match_both) balancetype = 3;
        else
            rb_raise(rb_eArgError, "Balance type must be nil, :match_left, :match_right, or :match_both");
    }

    if (!NIL_P(mwidth_v))
        mwidth = NUM2INT(mwidth_v);

	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);
	OutImg2(obj, obj2, new, data_new, im_new);

    if (im_tbmosaic1(im, im2, im_new, NUM2INT(bandno), NUM2INT(xr1),
		NUM2INT(yr1), NUM2INT(xs1), NUM2INT(ys1), NUM2INT(xr2), NUM2INT(yr2),
		NUM2INT(xs2), NUM2INT(ys2), halfcorrelation, halfarea, balancetype,
        mwidth))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.global_balance(gamma) -> image
 *
 *  Takes an image assembled with the mosaicing functions, take it apart, and
 *  reassemble it, globally optimising the image balance. This is useful for
 *  assembling image mosaics from sources where the exposure is uncontrolled and
 *  may vary from tile to tile --- such as video, or photographic sources.
 *
 *  The function finds a set of factors, one for each of the input images, and
 *  scales each image by its factor before reassembling. The factors are chosen
 *  so as to minimise the average grey-level difference between neighboring
 *  images at their overlaps.  Trivial overlaps (where the width and height of
 *  the overlap are both less than 20 pixels) are ignored.
 *
 *  The <i>gamma</i> parameter is the gamma of the image input system. It is
 *  used during brightness adjustment. Set to 1.0 to disable gamma, to 1.6 for a
 *  typical IR vidicon camera, or 2.3 for a typical video camera.
 *
 *  It relies on information left by the mosaicing functions in ".desc" files.
 *  If the ".desc" file of the input image has been corrupted, or is strangely
 *  complicated, or if any of the original input images have been moved or
 *  deleted, the function can fail.
 *
 *  The function will fail for mosaics larger than about 7 by 7 frames, since it
 *  will run out of file descriptors (UNIX sets a limit of 256 per process). To
 *  balance larger mosaics, just assemble them in 7x7 sections, balancing and
 *  saving each part in turn, before loading, assembling and balancing the final
 *  image. The function can also fail if there are significant mosaicing errors.
 */

VALUE
img_global_balance(VALUE obj, VALUE gamma)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_global_balance(im, im_new, NUM2DBL(gamma)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.global_balancef(gamma) -> image
 *
 *  Works as Image#global_balance, but outputs a float rather than a uchar
 *  image. This lets you adjust the range of the image manually, if the
 *  automatically-found scales are causing burn-out.
 */

VALUE
img_global_balancef(VALUE obj, VALUE gamma)
{
	GetImg(obj, data, im);
	OutImg(obj, new, data_new, im_new);

    if (im_global_balancef(im, im_new, NUM2DBL(gamma)))
        vips_lib_error();

    return new;
}

/*
 *  call-seq:
 *     im.correl(other_image, xref, yref, xsec, ysec, hwindowsize,
 *       hsearchsize) -> correlation, x, y
 *
 *  Find position of <i>other_image</i> within *self*. Search around point
 *  <i>xsec</i>, <i>ysec</i> for the best match for the area around <i>xref</i>,
 *  <i>yref</i>. Search an area of size <i>hsearchsize</i> for an of size
 *  <i>hwindowsize</i>.
 *
 *  Return a new value for xsec, ysec and the correlation at that point.
 */

VALUE
img_correl(VALUE obj, VALUE obj2, VALUE xref, VALUE yref, VALUE xsec,
	VALUE ysec, VALUE hwindowsize, VALUE hsearchsize)
{
    int x, y;
	double correlation;
	GetImg(obj, data, im);
	GetImg(obj2, data2, im2);

    if (im_correl(im, im2, NUM2INT(xref), NUM2INT(yref), NUM2INT(xsec),
		NUM2INT(ysec), NUM2INT(hwindowsize), NUM2INT(hsearchsize), &correlation,
		&x, &y))
        vips_lib_error();

    return rb_ary_new3(3, DBL2NUM(correlation), INT2NUM(x), INT2NUM(y));
}

/*
 *  call-seq:
 *     im.align_bands -> image
 *
 *  Brute force align the bands of an image.
 */

VALUE
img_align_bands(VALUE obj)
{
	RUBY_VIPS_UNARY(im_align_bands);
}

/*
 *  call-seq:
 *     im.maxpos_subpel -> x, y
 *
 *  This function implements "Extension of Phase Correlation to Subpixel
 *  Registration" by H. Foroosh, from IEEE trans. Im. Proc. 11(3), 2002.
 *
 *  If the best three matches in the correlation are aranged:
 *
 *    02   or   01
 *    1         2
 *
 *  then we return a subpixel match using the ratio of correlations in the
 *  vertical and horizontal dimension.
 *
 *  ( xs[0], ys[0] ) is the best integer alignment
 *  ( xs[ use_x ], ys[ use_x ] ) is equal in y and (+/-)1 off in x
 *  ( xs[ use_y ], ys[ use_y ] ) is equal in x and (+/-)1 off in y
 *
 *  Alternatively if the best four matches in the correlation are aranged in
 *  a square:
 *
 *    01  or  03  or  02  or  03
 *    32      12      31      21
 *
 *  then we return a subpixel match weighting with the sum the two on each
 *  side over the sum of all four, but only if all four of them are very
 *  close to the best, and the fifth is nowhere near.
 *
 *  This alternative method is not described by Foroosh, but is often the
 *  case where the match is close to n-and-a-half pixels in both dimensions.
 */

VALUE
img_maxpos_subpel(VALUE obj)
{
    double x, y;
	GetImg(obj, data, im);

    if (im_maxpos_subpel(im, &x, &y))
        vips_lib_error();

    return rb_ary_new3(2, DBL2NUM(x), INT2NUM(y));
}

void
init_Image_mosaicing(void)
{
 id_match_none = rb_intern("match_none");
    id_match_left = rb_intern("match_left");
    id_match_right = rb_intern("match_right");
    id_match_both = rb_intern("match_both");
}
