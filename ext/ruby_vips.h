#ifndef RUBY_VIPS_H
#define RUBY_VIPS_H

#include "ruby.h"
#include "vips/vips.h"

extern VALUE mVIPS;
extern VALUE eVIPSError;

void vips_lib_error();

/* Backports from ruby 1.9.2 for ruby 1.8.7
 */

#ifndef DBL2NUM
#define DBL2NUM(dbl) rb_float_new(dbl)
#endif

#ifndef RARRAY_LENINT
#define RARRAY_LENINT(ary) RARRAY_LEN(ary)
#endif

#ifndef rb_str_new_cstr
#define rb_str_new_cstr(str) rb_str_new(str, (long)strlen(str))
#endif

#endif

