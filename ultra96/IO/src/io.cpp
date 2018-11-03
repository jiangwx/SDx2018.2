/*
 * io.cpp
 *
 *  Created on: Nov 24, 2017
 *      Author: lulugay
 */
#include "io.h"


void mpsoc_io(DTYPE_IO *in,DTYPE_IO *out)
{

#pragma HLS DATA_PACK variable=in struct_level
#pragma HLS DATA_PACK variable=out struct_level

	DTYPE input_buffer[Tn][Length];
#pragma HLS ARRAY_PARTITION variable=input_buffer complete dim=1
	DTYPE output_buffer[Tn][Length];
#pragma HLS ARRAY_PARTITION variable=output_buffer complete dim=1
	for(int i=0;i<Length;i++)
	{
#pragma HLS PIPELINE
		DTYPE_IO tmp_in=in[i];
		for(int j=0;j<Tn;j++)
		{
			input_buffer[j][i]=tmp_in.data[j];
		}
	}
	for(int i=0;i<Length;i++)
	{
#pragma HLS PIPELINE
		for(int j=0;j<Tn;j++)
		{
			output_buffer[j][i]=input_buffer[j][i];
    	}
    }
	DTYPE_IO tmp_out;
	for(int i=0;i<Length;i++)
	{
#pragma HLS PIPELINE
		for(int j=0;j<Tn;j++)
		{
			tmp_out.data[j]=output_buffer[j][i];
		}
		out[i]=tmp_out;
	}
}

