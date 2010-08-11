#ifndef RUBY_VIPS_IMAGE_H
#define RUBY_VIPS_IMAGE_H

#include "ruby_vips.h"

extern VALUE cVIPSImage;

struct _vipsImg {
  VipsImage *in;
  VALUE *deps;
  int deps_len;
};
typedef struct _vipsImg vipsImg;

void img_add_dep(vipsImg*, VALUE);
VALUE img_spawn(VALUE);
VALUE img_spawn2(VALUE, VALUE);
VALUE img_spawn3(VALUE, VALUE, VALUE);
VALUE img_alloc(VALUE);
VALUE img_init(VALUE, VipsImage*);
VALUE img_init_partial();
VALUE img_init_partial_anyclass(VALUE);
VipsBandFmt img_id_to_band_fmt(VALUE);

#define GetImg(obj, data, im) \
    vipsImg *data; \
    VipsImage *im; \
    Data_Get_Struct(obj, vipsImg, data); \
    im = data->in;

#define OutImg(obj, new, data_new, im_new) \
    VALUE new = img_spawn(obj); \
    GetImg(new, data_new, im_new);

#define OutImg2(obj, obj2, new, data_new, im_new) \
    VALUE new = img_spawn2(obj, obj2); \
    GetImg(new, data_new, im_new);

#define OutImg3(obj, obj2, obj3, new, data_new, im_new) \
    VALUE new = img_spawn3(obj, obj2, obj3); \
    GetImg(new, data_new, im_new);

#define OutPartial(new, data_new, im_new) \
    VALUE new = img_init_partial(); \
    GetImg(new, data_new, im_new);

#define RUBY_VIPS_UNARY(VIPS_METHOD) \
    do { \
        GetImg(obj, data, im); \
        OutImg(obj, new, data_new, im_new); \
\
        if (VIPS_METHOD(im, im_new)) \
            vips_lib_error(); \
\
        return new; \
    } while(0)

#define RUBY_VIPS_BINARY(VIPS_METHOD) \
    do { \
        GetImg(obj, data, im); \
        GetImg(obj2, data2, im2); \
        OutImg2(obj, obj2, new, data_new, im_new); \
\
        if (VIPS_METHOD(im, im2, im_new)) \
            vips_lib_error(); \
\
        return new; \
    } while(0)

#endif

