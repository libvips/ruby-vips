#include "ruby_vips.h"

VALUE cVIPSMask;

static void
mask_free(vipsMask *msk)
{
    if(msk->dmask)
        im_free_dmask(msk->dmask);

    if(msk->imask)
        im_free_imask(msk->imask);

    xfree(msk);
}

VALUE
mask_alloc(VALUE klass)
{
    vipsMask *msk;
    VALUE new = Data_Make_Struct(klass, vipsMask, 0, mask_free, msk);
    msk->imask = NULL;
    msk->dmask = NULL;

    return new;
}

static int
ary_is_int_2d(VALUE ary)
{
    Check_Type(ary, T_ARRAY);

    VALUE *row, *rows = RARRAY_PTR(ary);
    int i, j;
    for(i = 0; i < RARRAY_LEN(ary); i++) {
        Check_Type(rows[i], T_ARRAY);
        for(j = 0; j < RARRAY_LEN(rows[i]); j++) {
            row = RARRAY_PTR(rows[i]);
            if(TYPE(row[j]) != T_FIXNUM)
                return 0;
        }
    }

    return 1;
}

/* initialize an INTMASK from a ruby array */

static INTMASK*
mask_ary2imask(VALUE coeffs)
{
    INTMASK *msk;
    VALUE row, *rows = RARRAY_PTR(coeffs);
    int i, j, num_cols, num_rows = RARRAY_LEN(coeffs);

    num_cols = RARRAY_LEN(rows[0]);
    if( !(msk = im_create_imask("mask_initialize_imask", num_cols, num_rows)) )
        vips_lib_error();

    for(i = 0; i < num_rows; i++) {
        row = rows[i];
        for(j = 0; j < RARRAY_LEN(row) && j < num_cols; j++)
            msk->coeff[i * num_cols + j] = NUM2INT(RARRAY_PTR(row)[j]);
    }

    return msk;
}

static DOUBLEMASK*
mask_ary2dmask(VALUE coeffs)
{
    DOUBLEMASK *msk;
    VALUE row, *rows = RARRAY_PTR(coeffs);
    int i, j, num_cols, num_rows = RARRAY_LEN(coeffs);

    num_cols = RARRAY_LEN(rows[0]);
    if (!(msk = im_create_dmask("mask_ary2dmask", num_cols, num_rows)))
        vips_lib_error();

    for(i = 0; i < num_rows; i++) {
        row = rows[i];
        for(j = 0; j < RARRAY_LEN(row) && j < num_cols; j++)
            msk->coeff[i * num_cols + j] = NUM2DBL(RARRAY_PTR(row)[j]);
    }

    return msk;
}

/*
 *  call-seq:
 *     Mask.new(coeffs, scale=1, offset=0) -> mask
 *
 *  Create a new Mask object. <i>coeffs</i> is a two-dimensional array where
 *  every row must have the same length. Note that some methods require a mask
 *  where all values in <i>coeffs</i> are whole integers.
 */
static VALUE
mask_initialize(int argc, VALUE *argv, VALUE obj)
{
    VALUE coeffs, scale, offset;
    vipsMask *msk;
    Data_Get_Struct(obj, vipsMask, msk);

	rb_scan_args(argc, argv, "12", &coeffs, &scale, &offset);

    if (NIL_P(scale))
        scale = INT2NUM(1);

    if (NIL_P(offset))
        offset = INT2NUM(0);

    if(TYPE(scale) == T_FIXNUM && TYPE(offset) == T_FIXNUM &&
        ary_is_int_2d(coeffs)) {
        msk->imask = mask_ary2imask(coeffs);
        msk->imask->scale = NUM2INT(scale);
        msk->imask->offset = NUM2INT(offset);
    }

    msk->dmask = mask_ary2dmask(coeffs);
    msk->dmask->scale = NUM2DBL(scale);
    msk->dmask->offset = NUM2DBL(offset);

    return obj;
}

/*
 *  call-seq:
 *     msk.xsize -> number
 *
 *  Retrieve the number of columns in the mask.
 */

static VALUE
mask_xsize(VALUE obj)
{
    vipsMask *msk;
    Data_Get_Struct(obj, vipsMask, msk);
    return INT2FIX(msk->dmask->xsize);
}

/*
 *  call-seq:
 *     msk.ysize -> number
 *
 *  Retrieve the number of rows in the mask.
 */

static VALUE
mask_ysize(VALUE obj)
{
    vipsMask *msk;
    Data_Get_Struct(obj, vipsMask, msk);
    return INT2FIX(msk->dmask->ysize);
}

/*
 *  call-seq:
 *     msk.scale -> number
 *
 *  Retrieve the scale of the mask.
 */

static VALUE
mask_scale(VALUE obj)
{
    vipsMask *msk;
    Data_Get_Struct(obj, vipsMask, msk);
    return msk->imask ? INT2FIX(msk->imask->scale) : DBL2NUM(msk->dmask->scale);
}

/*
 *  call-seq:
 *     msk.offset -> number
 *
 *  Retrieve the offset of the mask.
 */

static VALUE
mask_offset(VALUE obj)
{
    vipsMask *msk;
    Data_Get_Struct(obj, vipsMask, msk);
    return msk->imask ? INT2FIX(msk->imask->offset) : DBL2NUM(msk->dmask->offset);
}

VALUE
imask2rb(INTMASK *msk)
{
    int i, j;
    VALUE row, rows = rb_ary_new2(msk->ysize);

    for(i = 0; i < msk->ysize; i++) {
        row = rb_ary_new2(msk->xsize);
        rb_ary_push(rows, row);
        for(j = 0; j < msk->xsize; j++) {
            rb_ary_push(row, INT2FIX(msk->coeff[i * msk->xsize + j]));
        }
    }

    return rows;
}

VALUE
dmask2rb(DOUBLEMASK *msk)
{
    int i, j;
    VALUE row, rows = rb_ary_new2(msk->ysize);

    for(i = 0; i < msk->ysize; i++) {
        row = rb_ary_new2(msk->xsize);
        rb_ary_push(rows, row);
        for(j = 0; j < msk->xsize; j++) {
            rb_ary_push(row, DBL2NUM(msk->coeff[i * msk->xsize + j]));
        }
    }

    return rows;
}

/*
 *  call-seq:
 *     msk.coeff -> array
 *
 *  Retrieve the two-dimensional array of coefficients for the mask.
 */

static VALUE
mask_coeff(VALUE obj)
{
    vipsMask *msk;
    Data_Get_Struct(obj, vipsMask, msk);
    if(msk->imask)
        return imask2rb(msk->imask);
    else
        return dmask2rb(msk->dmask);
}

/*
 *  call-seq:
 *     msk.int? -> true or false
 *
 *  Indicate whether all coefficients, the scale and the offset in the mask are
 *  integers. Some methods require an all-integer mask.
 */

static VALUE
mask_int_p(VALUE obj)
{
    vipsMask *msk;
    Data_Get_Struct(obj, vipsMask, msk);
    if(msk->imask)
        return Qtrue;

    return Qfalse;
}

void
mask_arg2mask(VALUE arg, INTMASK **imask, DOUBLEMASK **dmask)
{
    INTMASK *imask_t = NULL;
    DOUBLEMASK *dmask_t = NULL;
    vipsMask *data;
    const char *errstr;

    if (TYPE(arg) == T_ARRAY) {
        if (imask && ary_is_int_2d(arg))
            imask_t = mask_ary2imask(arg);
        else if (dmask)
            dmask_t = mask_ary2dmask(arg);
    } else if (CLASS_OF(arg) == cVIPSMask) {
        Data_Get_Struct(arg, vipsMask, data);

        if (imask)
            imask_t = data->imask;
        if (dmask)
            dmask_t = data->dmask;
    }

    if (!imask_t && !dmask_t) {
        if (imask && dmask)
            errstr = "Expected an array or a Mask";
        else
            errstr = "Expected an int array or an int Mask";

        rb_raise(rb_eArgError, errstr);
    }

    if (imask)
        *imask = imask_t;

    if (dmask)
        *dmask = dmask_t;
}

/*
 *  call-seq:
 *     msk.int? -> true or false
 *
 *  Create  a one-band, band format :DOUBLE image based on mask *self*.
 */

static VALUE
mask_to_image(VALUE obj)
{
    vipsMask *msk;

    OutImg(obj, new, data, im);
    Data_Get_Struct(obj, vipsMask, msk);

    if (im_mask2vips(msk->dmask, im))
        vips_lib_error();

    return new;
}

/*
 *  VIPS uses masks for various operations. A vips mask is a two-dimensional
 *  array with a scale and an offset.
 *
 *  All operations that accept a Mask object also accept an array, in which case
 *  scale defaults to 1 and offset to zero.
 *
 *  Some vips operations require that all values in the mask are integer values.
 *  These operations will raise an exception if given a mask that contains
 *  any float values.
 */

void
init_Mask( void )
{
    cVIPSMask = rb_define_class_under(mVIPS, "Mask", rb_cObject);

    rb_define_alloc_func(cVIPSMask, mask_alloc);
    rb_define_method(cVIPSMask, "initialize", mask_initialize, -1);
    rb_define_method(cVIPSMask, "xsize", mask_xsize, 0);
    rb_define_method(cVIPSMask, "ysize", mask_ysize, 0);
    rb_define_method(cVIPSMask, "scale", mask_scale, 0);
    rb_define_method(cVIPSMask, "offset", mask_offset, 0);
    rb_define_method(cVIPSMask, "coeff", mask_coeff, 0);
    rb_define_method(cVIPSMask, "int?", mask_int_p, 0);
    rb_define_method(cVIPSMask, "to_image", mask_to_image, 0);

#if 0
    VALUE mVIPS = rb_define_module("VIPS");
#endif
}

