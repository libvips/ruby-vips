#include "ruby_vips.h"

/* vips-7.26.3 and earlier 7.26 (used on Ubuntu 12.04) had a broken
 * vips_class_map_all(). ruby_vips.h turns on NEED_ruby_vips_class_map_all if
 * we need to define our own, working version. 
 */
#ifdef NEED_ruby_vips_class_map_all
static void *
ruby_vips_type_map( GType base, VipsTypeMap2 fn, void *a, void *b )
{
	GType *child;
	guint n_children;
	unsigned int i;
	void *result;

	child = g_type_children( base, &n_children );
	result = NULL;
	for( i = 0; i < n_children && !result; i++ )
		result = fn( child[i], a, b );
	g_free( child );

	return( result );
}

void *
ruby_vips_class_map_all( GType type, VipsClassMap fn, void *a )
{
	void *result;

	if( !G_TYPE_IS_ABSTRACT( type ) ) {
		if( (result = fn( 
			VIPS_OBJECT_CLASS( g_type_class_ref( type ) ), a )) )
			return( result );
	}

	if( (result = vips_type_map( type, 
		(VipsTypeMap2) ruby_vips_class_map_all, fn, a )) )
		return( result );

	return( NULL );
}
#endif /*NEED_ruby_vips_class_map_all*/

VALUE mVIPS, eVIPSError;

void
vips_lib_error()
{
    VALUE vips_error = rb_str_new2(im_error_buffer());
    im_error_clear();
    rb_raise(eVIPSError, "VIPS error: %s", RSTRING_PTR(vips_error));
}

static VALUE
vips_s_version_string()
{
    return rb_str_new2(im_version_string());
}

static VALUE
vips_s_version_array()
{
    VALUE arr = rb_ary_new2(3);
    int i;

    for(i = 0; i < 3; i++)
        rb_ary_push(arr, INT2FIX(im_version(i)));

    return arr;
}

/*
 *  call-seq:
 *     VIPS.debug_info -> string
 *
 *  This will print internal debugging information from VIPS, including memory
 *  allocation information.
 */
static VALUE
vips_s_debug_info(VALUE obj)
{
    im__print_all();
    return Qnil;
}

/*
 * Build a call to im_init_world() and pass command line options to vips. This
 * sets some library wide options.
 */

static void
init_vips_library()
{
    GOptionContext *context;
    GError *error = NULL;
    int i, argc;
    char **argv;
    VALUE argv_0, argv_v;

    argv_0 = rb_gv_get("0");

    if (NIL_P(argv_0))
        im_init_world("");
    else
        im_init_world(RSTRING_PTR(argv_0));

    argv_v = rb_const_get(rb_mKernel, rb_intern("ARGV"));

    if (!NIL_P(argv_v))
    {
        argc = RARRAY_LEN(argv_v) + 1;
        argv = ALLOC_N(char*, argc);

        argv[0] = RSTRING_PTR(argv_0);

        for (i=0; i < argc - 1; i++)
            argv[i+1] = RSTRING_PTR(RARRAY_PTR(argv_v)[i]);

        context = g_option_context_new("- ruby-vips");
        g_option_context_set_ignore_unknown_options(context, TRUE);

        g_option_context_add_group(context, im_get_option_group());

        g_option_context_parse(context, &argc, &argv, &error);
        g_option_context_free(context);

        xfree(argv);
    }

    /* We use the vips7 interface, so the vips8 cache will not help us.
     * Disable it and save 100mb or so of memory in vips-7.28 and later.
     */
#if IM_MAJOR_VERSION > 7 || IM_MINOR_VERSION >= 28
    vips_cache_set_max_mem( 0 );
#endif
}

/*
 * The VIPS module namespaces all ruby-vips objects.
 */

void
Init_vips_ext()
{
    init_vips_library();

    mVIPS = rb_define_module("VIPS");
    eVIPSError = rb_define_class_under(mVIPS, "Error", rb_eStandardError);

    rb_define_singleton_method(mVIPS, "debug_info", vips_s_debug_info, 0);

    /* Vips Library version string */
    rb_define_const(mVIPS, "LIB_VERSION", vips_s_version_string());

    /* Vips Library version as a 3-element array */
    rb_define_const(mVIPS, "LIB_VERSION_ARRAY", vips_s_version_array());

    init_Header();
    init_Image();
    init_Mask();
    init_Interpolator();
    init_Writer();
    init_Reader();
}
