#ifndef _JF_BGR2GRAY_CONFIG_H_
#define _JF_BGR2GRAY_CONFIG_H_

// Parameters Description:
//         NUM_ROWS:            matrix height
//         WORD_PER_ROW:        number of words in a row
//         BLOCK_SIZE:          number of words in an array
#define NUM_ROWS   64
#define WORD_PER_ROW 64
#define BLOCK_SIZE (WORD_PER_ROW*NUM_ROWS)

#include <hls_stream.h>
// Default data type is integer
typedef int DTYPE;
// use HLS stream library for easy use of AXI-stream interface
/* config width and height */
#define WIDTH 	1280
#define HEIGHT	720

//typedef ap_uint<8> uint8;
typedef unsigned char uint8;

#pragma SDS data mem_attribute("b":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("g":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("r":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("gray":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern("b":SEQUENTIAL,"g":SEQUENTIAL,"r":SEQUENTIAL,"gray":SEQUENTIAL)
#pragma SDS data copy("b"[0:rows*cols],"g"[0:rows*cols],"r"[0:rows*cols],"gray"[0:rows*cols])
void jf_rgb2gray(uint8* b, uint8* g, uint8* r, uint8* gray, int rows, int cols);

#endif


