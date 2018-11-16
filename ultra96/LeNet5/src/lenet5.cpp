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

int max(DTYPE *in,layer l)
{
    int index = 0;
    DTYPE tmp=in[0];
    for (int i = 1; i < l.oc; i++)
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
	blob         = (DTYPE*)sds_alloc(Lenet[data].ic*Lenet[data].ih*Lenet[data].iw*sizeof(DTYPE));
	conv1_output = (DTYPE*)sds_alloc(Lenet[conv1].oc*Lenet[conv1].oh*Lenet[conv1].ow*sizeof(DTYPE));
	conv1_weight = (DTYPE*)sds_alloc(Lenet[conv1].ic*Lenet[conv1].oc*Lenet[conv1].k*Lenet[conv1].k*sizeof(DTYPE));
	conv1_bias   = (DTYPE*)sds_alloc(Lenet[conv1].oc*sizeof(DTYPE));
	pool1_output = (DTYPE*)sds_alloc(Lenet[pool1].oc*Lenet[pool1].oh*Lenet[pool1].ow*sizeof(DTYPE));
	conv2_output = (DTYPE*)sds_alloc(Lenet[conv2].oc*Lenet[conv2].oh*Lenet[conv2].ow*sizeof(DTYPE));
	conv2_weight = (DTYPE*)sds_alloc(Lenet[conv2].ic*Lenet[conv2].oc*Lenet[conv2].k*Lenet[conv2].k*sizeof(DTYPE));
	conv2_bias   = (DTYPE*)sds_alloc(Lenet[conv2].oc*sizeof(DTYPE));
	pool2_output = (DTYPE*)sds_alloc(Lenet[pool2].oc*Lenet[pool2].oh*Lenet[pool2].ow*sizeof(DTYPE));
	ip1_output   = (DTYPE*)sds_alloc(Lenet[ip1].oc*Lenet[ip1].oh*Lenet[ip1].ow*sizeof(DTYPE));
	ip1_weight   = (DTYPE*)sds_alloc(Lenet[ip1].ic*Lenet[ip1].oc*Lenet[ip1].k*Lenet[ip1].k*sizeof(DTYPE));
	ip1_bias     = (DTYPE*)sds_alloc(Lenet[ip1].oc*sizeof(DTYPE));
	ip2_output   = (DTYPE*)sds_alloc(Lenet[ip2].oc*Lenet[ip2].oh*Lenet[ip2].ow*sizeof(DTYPE));
	ip2_weight   = (DTYPE*)sds_alloc(Lenet[ip2].ic*Lenet[ip2].oc*Lenet[ip2].k*Lenet[ip2].k*sizeof(DTYPE));
	ip2_bias     = (DTYPE*)sds_alloc(Lenet[ip2].oc*sizeof(DTYPE));
}


int Lenet5(DTYPE* input)
{
    int label;
    ctr.start();
    CONV1POOL1CONV2POOL2(input,pool2_output,conv1_weight,conv2_weight,conv1_bias,conv2_bias);
    ctr.stop();
    //printf("CONV1POOL1CONV2POOL2 costs %f ms\n",(float)ctr.avg_cpu_cycles()/(float)clock_frequency);
    check_data(pool2_output,Lenet[pool2]);

    ctr.reset();
    ctr.start();
    convolution(pool2_output, ip1_output, ip1_weight, ip1_bias, Lenet[ip1],1);
    ctr.stop();
    //printf("ip1 costs %f ms\n",(float)ctr.avg_cpu_cycles()/(float)clock_frequency);
    check_data(ip1_output,Lenet[ip1]);

    ctr.reset();
    ctr.start();
    convolution(ip1_output, ip2_output, ip2_weight, ip2_bias, Lenet[ip2],0);
    ctr.stop();
    //printf("ip2 costs %f ms\n",(float)ctr.avg_cpu_cycles()/(float)clock_frequency);

    //for (int i=0;i<10;i++) printf("%f, ",ip2_output[i]);
    label = max(ip2_output, Lenet[ip2]);
    return label;
}

