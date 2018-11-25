#ifndef _JF_CONFIG_H_
#define _JF_CONFIG_H_

#include <hls_stream.h>
#include <stdint.h>
// use HLS stream library for easy use of AXI-stream interface
/* config width and height */
#define WIDTH 	1280
#define HEIGHT	720

typedef uint8_t PIXEL;

#pragma SDS data mem_attribute("b":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("g":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("r":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("gray":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern("b":SEQUENTIAL,"g":SEQUENTIAL,"r":SEQUENTIAL,"gray":SEQUENTIAL)
#pragma SDS data zero_copy("b"[0:rows*cols],"g"[0:rows*cols],"r"[0:rows*cols],"gray"[0:rows*cols])
void jf_rgb2gray(PIXEL* b, PIXEL* g, PIXEL* r, PIXEL* gray, int rows, int cols);

#pragma SDS data mem_attribute("gray":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("binary":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern("gray":SEQUENTIAL,"binary":SEQUENTIAL)
#pragma SDS data zero_copy("gray"[0:rows*cols],"binary"[0:rows*cols])
void jf_threshold(PIXEL* gray, PIXEL* binary,int rows, int cols, PIXEL threshold, PIXEL maxval);

#endif


