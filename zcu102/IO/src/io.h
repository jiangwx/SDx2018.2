#include <ap_int.h>
#include <ap_fixed.h>

#include "sds_utils.h"
#define Length 128
#define Tn 4
typedef ap_fixed<16, 7,AP_RND> DTYPE;
//typedef float DTYPE;
typedef struct {
	DTYPE data[Tn];
} DTYPE_IO;


#pragma SDS data zero_copy(in[0:Length],out[0:Length])
#pragma SDS data access_pattern(in:SEQUENTIAL,out:SEQUENTIAL)
#pragma SDS data data_mover(in:AXIDMA_SIMPLE, out:AXIDMA_SIMPLE)
void mpsoc_io(DTYPE_IO *in,DTYPE_IO *out);
