#ifndef _JF_CONFIG_H_
#define _JF_CONFIG_H_

#include <hls_stream.h>
#include <hls_video.h>
#include <stdint.h>
// use HLS stream library for easy use of AXI-stream interface
/* config width and height */
#define WIDTH 	1280
#define HEIGHT	720

typedef uint8_t PIXEL;
typedef ap_uint<32> PIXEL4;
typedef ap_uint<64> PIXEL8;
typedef ap_uint<128> PIXEL16;

#pragma SDS data mem_attribute("bgra":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("gray":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern("bgra":SEQUENTIAL,"gray":SEQUENTIAL)
#pragma SDS data zero_copy("bgra"[0:rows*cols],"gray"[0:rows*cols])
void jf_rgb2gray(PIXEL4* bgra, PIXEL* gray, int rows, int cols);

#pragma SDS data mem_attribute("gray":NON_CACHEABLE|PHYSICAL_CONTIGUOUS,"binary":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern("gray":SEQUENTIAL,"binary":SEQUENTIAL)
#pragma SDS data zero_copy("gray"[0:rows*cols],"binary"[0:rows*cols])
void jf_threshold(PIXEL* gray, PIXEL* binary,int rows, int cols, PIXEL threshold, PIXEL maxval);

#pragma SDS data mem_attribute("gray":NON_CACHEABLE|PHYSICAL_CONTIGUOUS,"dilate":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern("gray":SEQUENTIAL,"dilate":SEQUENTIAL)
#pragma SDS data zero_copy("gray"[0:rows*cols],"dilate"[0:rows*cols])
void jf_dilate(PIXEL* gray, PIXEL* dilate,int rows, int cols);

#pragma SDS data mem_attribute("bgra":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("sub":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern("bgra":SEQUENTIAL,"sub":SEQUENTIAL)
#pragma SDS data zero_copy("bgra"[0:rows*cols],"sub"[0:rows*cols])
void jf_subtract(PIXEL4* bgra, PIXEL* sub, int rows, int cols);

#pragma SDS data mem_attribute("bgra":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("binary":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern("bgra":SEQUENTIAL,"binary":SEQUENTIAL)
#pragma SDS data zero_copy("bgra"[0:rows*cols],"binary"[0:rows*cols])
void jf_bright_pre(PIXEL4* bgra, PIXEL* binary, int rows, int cols, PIXEL threshold, PIXEL maxval);

#pragma SDS data mem_attribute("bgra":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("dilate":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern("bgra":SEQUENTIAL,"dilate":SEQUENTIAL)
#pragma SDS data zero_copy("bgra"[0:rows*cols],"dilate"[0:rows*cols])
void jf_color_pre(PIXEL4* bgra, PIXEL* dilate, int rows, int cols, PIXEL threshold, PIXEL maxval);

#endif


