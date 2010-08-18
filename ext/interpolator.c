#include "ruby_vips.h"

static VALUE cVIPSInterpolator;
static ID id_INTERPOLATORS;

VipsInterpolate*
interp_lookup(VALUE itrp_sym)
{
	VALUE rb_interp;
	VipsInterpolate *itrp;
	VALUE hVIPSInterpolators = rb_const_get(cVIPSInterpolator,
		id_INTERPOLATORS);

    rb_interp = rb_hash_aref(hVIPSInterpolators, itrp_sym);
	Data_Get_Struct(rb_interp, VipsInterpolate, itrp);

	return itrp;
}

static void*
interp_register(VipsInterpolateClass *itrp, void *hash)
{
    VipsInterpolate *inst;
    VipsObjectClass *klass = (VipsObjectClass*) itrp;;
    VALUE obj, hVIPSInterpolators = (VALUE)hash;

    inst = vips_interpolate_new(klass->nickname);
    obj = Data_Wrap_Struct(cVIPSInterpolator, 0, 0, inst);

    rb_hash_aset(hVIPSInterpolators, ID2SYM(rb_intern(klass->nickname)), obj);

	return NULL;
}

static void
interp_register_builtin()
{
	VALUE hVIPSInterpolators = rb_hash_new();

    /* Hash of available interpolators. Keys are the symbols, and values are
     * interpolator objects.
     */
    rb_define_const(cVIPSInterpolator, "INTERPOLATORS", hVIPSInterpolators);

	vips_class_map_concrete_all(
        g_type_from_name( "VipsInterpolate" ),
        (void *) interp_register,
        (void *) hVIPSInterpolators
    );
}


/*
 *  call-seq:
 *     interp.nickname -> string
 *
 *  Retrieve the internally used nickname of the interpolator.
 */

static VALUE
interp_nickname(VALUE obj)
{
    VipsObject *v_obj;
    Data_Get_Struct(obj, VipsObject, v_obj);
    return rb_str_new2(v_obj->nickname);
}

/*
 *  call-seq:
 *     interp.description -> string
 *
 *  Retrieve the description of the interpolator.
 */

static VALUE
interp_description(VALUE obj)
{
    VipsObject *v_obj;
    Data_Get_Struct(obj, VipsObject, v_obj);
    return rb_str_new2(v_obj->description);
}

/*
 *  VIPS Interpolators determine how color values will be estimated when an
 *  image is modified.
 *
 *  This class provides information on which interpolators are available to
 *  VIPS.
 */

void
init_Interpolator()
{
    cVIPSInterpolator = rb_define_class_under(mVIPS, "Interpolator", rb_cObject);

    rb_define_method(cVIPSInterpolator, "nickname", interp_nickname, 0);
    rb_define_method(cVIPSInterpolator, "description", interp_description, 0);

	id_INTERPOLATORS = rb_intern("INTERPOLATORS");
	
	interp_register_builtin();

#if 0
    VALUE mVIPS = rb_define_module("VIPS");
#endif
}
