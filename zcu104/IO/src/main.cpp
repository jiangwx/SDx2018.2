/*
 * main.cpp
 *
 *  Created on: Nov 24, 2017
 *      Author: lulugay
 */

#include "io.h"
sds_utils::perf_counter hw_ctr;
int main()
{
	int err=0;
	DTYPE_IO* input=(DTYPE_IO*)sds_alloc(Length*sizeof(DTYPE_IO));
	DTYPE_IO* output=(DTYPE_IO*)sds_alloc(Length*sizeof(DTYPE_IO));

	for(int i=0;i<Length;i++)
	{
		for(int j=0;j<Tn;j++)
		{
			input[i].data[j]=i*Tn+j;
		}
	}
	hw_ctr.start();
	mpsoc_io(input,output);
	hw_ctr.stop();
	for(int i=0;i<Length;i++)
	{
		for(int j=0;j<Tn;j++)
		{
			if(input[i].data[j]!=output[i].data[j])err++;
		}
	}

	printf("error count %d\n",err);
	printf("cycles =%llu\n",hw_ctr.avg_cpu_cycles());
	return 0;
}
