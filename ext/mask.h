#ifndef RUBY_VIPS_MASK_H
#define RUBY_VIPS_MASK_H

extern VALUE cVIPSMask;

struct _vipsMask {
  INTMASK *imask;
  DOUBLEMASK *dmask;
};
typedef struct _vipsMask vipsMask;

VALUE mask_alloc(VALUE);
VALUE imask2rb(INTMASK*);
VALUE dmask2rb(DOUBLEMASK*);
void mask_arg2mask(VALUE, INTMASK**, DOUBLEMASK**);

#endif
