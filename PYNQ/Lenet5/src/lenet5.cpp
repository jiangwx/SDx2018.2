#include "lenet5.h"
layer Lenet[7] = {
        { "data",   28,28,1,28,28,1,0,0,0},
        { "conv1",  28,28,1,24,24,6,5,1,0},
        { "pool1",  24,24,6,12,12,6,2,2,0},
        { "conv2",  12,12,6,8,8,16,5,1,0 },
        { "pool2",  8,8,16,4,4,16,2,2,0 },
        { "ip1",    4,4,16,1,1,120,4,4,0 },
        { "ip2",    1,1,120,1,1,10,1,1,0 },
};
sds_utils::perf_counter ctr;
DTYPE* blob;
DTYPE* conv1_output;
DTYPE* conv1_weight;
DTYPE* conv1_bias;
DTYPE* pool1_output;
DTYPE* conv2_output;
DTYPE* conv2_weight;
DTYPE* conv2_bias;
DTYPE* pool2_output;
DTYPE* ip1_output;
DTYPE* ip1_weight;
DTYPE* ip1_bias;
DTYPE* ip2_output;
DTYPE* ip2_weight;
DTYPE* ip2_bias;

int max(DTYPE *in,layer Lenet)
{
    int index = 0;
    DTYPE tmp=in[0];
    for (int i = 1; i < Lenet.od; i++)
    {
        if (in[i] > tmp)
        {
            index = i;
            tmp = in[i];
        }
    }
    return index;
}


void load_weight()
{
    load_wb(Lenet[conv1],conv1_weight,conv1_bias);
    load_wb(Lenet[conv2],conv2_weight,conv2_bias);
    load_wb(Lenet[ip1],ip1_weight,ip1_bias);
    load_wb(Lenet[ip2],ip2_weight,ip2_bias);
}
void Lenet5_init()
{
	blob         = (DTYPE*)sds_alloc(Lenet[data].id*Lenet[data].ih*Lenet[data].iw*sizeof(DTYPE));
	conv1_output = (DTYPE*)sds_alloc(Lenet[conv1].od*Lenet[conv1].oh*Lenet[conv1].ow*sizeof(DTYPE));
	conv1_weight = (DTYPE*)sds_alloc(Lenet[conv1].id*Lenet[conv1].od*Lenet[conv1].kernel*Lenet[conv1].kernel*sizeof(DTYPE));
	conv1_bias   = (DTYPE*)sds_alloc(Lenet[conv1].od*sizeof(DTYPE));
	pool1_output = (DTYPE*)sds_alloc(Lenet[pool1].od*Lenet[pool1].oh*Lenet[pool1].ow*sizeof(DTYPE));
	conv2_output = (DTYPE*)sds_alloc(Lenet[conv2].od*Lenet[conv2].oh*Lenet[conv2].ow*sizeof(DTYPE));
	conv2_weight = (DTYPE*)sds_alloc(Lenet[conv2].id*Lenet[conv2].od*Lenet[conv2].kernel*Lenet[conv2].kernel*sizeof(DTYPE));
	conv2_bias   = (DTYPE*)sds_alloc(Lenet[conv2].od*sizeof(DTYPE));
	pool2_output = (DTYPE*)sds_alloc(Lenet[pool2].od*Lenet[pool2].oh*Lenet[pool2].ow*sizeof(DTYPE));
	ip1_output   = (DTYPE*)sds_alloc(Lenet[ip1].od*Lenet[ip1].oh*Lenet[ip1].ow*sizeof(DTYPE));
	ip1_weight   = (DTYPE*)sds_alloc(Lenet[ip1].id*Lenet[ip1].od*Lenet[ip1].kernel*Lenet[ip1].kernel*sizeof(DTYPE));
	ip1_bias     = (DTYPE*)sds_alloc(Lenet[ip1].od*sizeof(DTYPE));
	ip2_output   = (DTYPE*)sds_alloc(Lenet[ip2].od*Lenet[ip2].oh*Lenet[ip2].ow*sizeof(DTYPE));
	ip2_weight   = (DTYPE*)sds_alloc(Lenet[ip2].id*Lenet[ip2].od*Lenet[ip2].kernel*Lenet[ip2].kernel*sizeof(DTYPE));
	ip2_bias     = (DTYPE*)sds_alloc(Lenet[ip2].od*sizeof(DTYPE));
}


int Lenet5(DTYPE* input)
{
    int label;
    //ctr.start();
    CONV1POOL1CONV2POOL2(input,pool2_output,conv1_weight,conv2_weight,conv1_bias,conv2_bias);
    //ctr.stop();
    //printf("CONV1POOL1CONV2POOL2 costs %llu cycles\n",ctr.avg_cpu_cycles());

    //ctr.reset();
    //ctr.start();
    conv(pool2_output, ip1_output, ip1_weight, ip1_bias, Lenet[ip1],1);
    //ctr.stop();
    //printf("ip1 costs %llu cycles\n",ctr.avg_cpu_cycles());

    //ctr.reset();
    //ctr.start();
    conv(ip1_output, ip2_output, ip2_weight, ip2_bias, Lenet[ip2],0);
    //ctr.stop();
    //printf("ip2 costs %llu cycles\n",ctr.avg_cpu_cycles());

    //check_data(ip2_output,Lenet[ip2]);
    label = max(ip2_output, Lenet[ip2]);
    return label;
}
/*
int Lenet5(DTYPE* input)
{
    int label;
    ctr.start();
    CONV1(input,conv1_output,conv1_weight,conv1_bias);
    ctr.stop();
    printf("CONV1 costs %llu cycles\n",ctr.avg_cpu_cycles());

    ctr.reset();
    ctr.start();
    maxpool(conv1_output,pool1_output,Lenet[pool1]);
    ctr.stop();
    printf("POOL1 costs %llu cycles\n",ctr.avg_cpu_cycles());

    ctr.reset();
    ctr.start();
    CONV2(pool1_output,conv2_output,conv2_weight,conv2_bias);
    ctr.stop();
    printf("CONV2 costs %llu cycles\n",ctr.avg_cpu_cycles());

    ctr.reset();
    ctr.start();
    maxpool(conv2_output,pool2_output,Lenet[pool2]);
    ctr.stop();
    printf("POOL2 costs %llu cycles\n",ctr.avg_cpu_cycles());

    ctr.reset();
    ctr.start();
    conv(pool2_output, ip1_output, ip1_weight, ip1_bias, Lenet[ip1],1);
    ctr.stop();
    printf("ip1 costs %llu cycles\n",ctr.avg_cpu_cycles());

    ctr.reset();
    ctr.start();
    conv(ip1_output, ip2_output, ip2_weight, ip2_bias, Lenet[ip2],0);
    ctr.stop();
    printf("ip2 costs %llu cycles\n",ctr.avg_cpu_cycles());

    check_data(ip2_output,Lenet[ip2]);
    label = max(ip2_output, Lenet[ip2]);
    return label;
}*/

