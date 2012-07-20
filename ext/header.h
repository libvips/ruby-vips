#ifndef RUBY_VIPS_HEADER_H
#define RUBY_VIPS_HEADER_H

extern VALUE mVIPSHeader;

VipsBandFmt header_id_to_band_fmt(VALUE);
void init_Header( void );

#endif
