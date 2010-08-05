#include "ruby_vips.h"
#include "mask.h"

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

INTMASK*
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

DOUBLEMASK*
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

static VALUE
mask_initialize(VALUE obj, VALUE coeffs, VALUE scale, VALUE offset)
{
    vipsMask *msk;
    Data_Get_Struct(obj, vipsMask, msk);

    if(TYPE(scale) == T_FIXNUM && TYPE(offset) == T_FIXNUM &&
        ary_is_int_2d(coeffs)) {
        msk->imask = mask_ary2imask(coeffs);
        msk->imask->scale = NUM2INT(scale);
        msk->imask->offset = NUM2INT(offset);
    } else {
        msk->dmask = mask_ary2dmask(coeffs);
        msk->dmask->scale = NUM2DBL(scale);
        msk->dmask->offset = NUM2DBL(offset);
    }

    return obj;
}

static VALUE
mask_xsize(VALUE obj)
{
    vipsMask *msk;
    Data_Get_Struct(obj, vipsMask, msk);
    return msk->imask ? INT2FIX(msk->imask->xsize) : INT2FIX(msk->dmask->xsize);
}

static VALUE
mask_ysize(VALUE obj)
{
    vipsMask *msk;
    Data_Get_Struct(obj, vipsMask, msk);
    return msk->imask ? INT2FIX(msk->imask->ysize) : INT2FIX(msk->dmask->ysize);
}

static VALUE
mask_scale(VALUE obj)
{
    vipsMask *msk;
    Data_Get_Struct(obj, vipsMask, msk);
    return msk->imask ? INT2FIX(msk->imask->scale) : DBL2NUM(msk->dmask->scale);
}

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

static VALUE
mask_int_p(VALUE obj)
{
    vipsMask *msk;
    Data_Get_Struct(obj, vipsMask, msk);
    if(msk->imask)
        return Qtrue;

    return Qfalse;
}

static VALUE
mask_double_p(VALUE obj)
{
    vipsMask *msk;
    Data_Get_Struct(obj, vipsMask, msk);
    if(msk->dmask)
        return Qtrue;

    return Qfalse;
}

void
init_mask()
{
    cVIPSMask = rb_define_class_under(mVIPS, "Mask", rb_cObject);
    rb_define_alloc_func(cVIPSMask, mask_alloc);
    rb_define_method(cVIPSMask, "initialize", mask_initialize, 3);
    rb_define_method(cVIPSMask, "xsize", mask_xsize, 0);
    rb_define_method(cVIPSMask, "ysize", mask_ysize, 0);
    rb_define_method(cVIPSMask, "scale", mask_scale, 0);
    rb_define_method(cVIPSMask, "offset", mask_offset, 0);
    rb_define_method(cVIPSMask, "coeff", mask_coeff, 0);
    rb_define_method(cVIPSMask, "int?", mask_int_p, 0);
    rb_define_method(cVIPSMask, "double?", mask_double_p, 0);
}

