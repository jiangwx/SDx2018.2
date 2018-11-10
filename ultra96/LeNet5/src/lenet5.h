#ifndef __CONV__H__
#define __CONV__H__
typedef float DTYPE;
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <cstring>
#include <stdio.h>

#include "sds_lib.h"
#include "sds_utils.h"

#define CheckScale 0.01
#define IMG_W 28
#define IMG_H 28
#define PATH "/mnt"
struct layer
{
    char name[15];
    int iw, ih, id, ow, oh, od;
    int kernel, stride, pad;
};

enum Net_idx {data, conv1, pool1,conv2, pool2, ip1, ip2};
void check_data(DTYPE *data,layer net);
void load_wb(layer net, DTYPE *weight, DTYPE *bias);
void load_data(DTYPE* data,layer net);
void conv(DTYPE *in, DTYPE *out, DTYPE *weight, DTYPE *bias, layer net, int relu);
void maxpool(DTYPE *in, DTYPE *out, layer net);
/*
#pragma SDS data copy(in[0:28*28],out[0:6*24*24],weight[0:6*5*5],bias[0:6])
#pragma SDS data data_mover(in:AXIDMA_SIMPLE, out:AXIDMA_SIMPLE, weight:AXIDMA_SIMPLE, bias:AXIDMA_SIMPLE)
#pragma SDS data access_pattern(in:SEQUENTIAL, out:SEQUENTIAL, weight:SEQUENTIAL, bias:SEQUENTIAL)
#pragma SDS data sys_port(in:ACP,out:AFI,weight:ACP,bias:ACP)*/

#pragma SDS data zero_copy(in[0:28*28],out[0:6*24*24],weight[0:6*5*5],bias[0:6])
#pragma SDS data data_mover(in:AXIDMA_SIMPLE, out:AXIDMA_SIMPLE, weight:AXIDMA_SIMPLE, bias:AXIDMA_SIMPLE)
#pragma SDS data access_pattern(in:SEQUENTIAL, out:SEQUENTIAL, weight:SEQUENTIAL, bias:SEQUENTIAL)
#pragma SDS data sys_port(in:AFI,out:AFI,weight:AFI,bias:AFI)
void CONV1(DTYPE* in,DTYPE* out,DTYPE* weight,DTYPE* bias);

#pragma SDS data zero_copy(in[0:6*12*12],out[0:16*8*8],weight[0:6*16*5*5],bias[0:16])
#pragma SDS data data_mover(in:AXIDMA_SIMPLE, out:AXIDMA_SIMPLE, weight:AXIDMA_SIMPLE, bias:AXIDMA_SIMPLE)
#pragma SDS data access_pattern(in:SEQUENTIAL, out:SEQUENTIAL, weight:SEQUENTIAL, bias:SEQUENTIAL)
#pragma SDS data sys_port(in:AFI,out:AFI,weight:AFI,bias:AFI);
void CONV2(DTYPE* in,DTYPE* out,DTYPE* weight,DTYPE* bias);

#pragma SDS data zero_copy(in[0:28*28],out[0:16*4*4],conv1_w[0:6*5*5],conv2_w[0:6*16*5*5],conv1_b[0:6],conv2_b[0:16])
#pragma SDS data data_mover(in:AXIDMA_SIMPLE, out:AXIDMA_SIMPLE, conv1_w:AXIDMA_SIMPLE, conv2_w:AXIDMA_SIMPLE,conv1_b:AXIDMA_SIMPLE,conv2_b:AXIDMA_SIMPLE)
#pragma SDS data access_pattern(in:SEQUENTIAL, out:SEQUENTIAL,conv1_w:SEQUENTIAL, conv2_w:SEQUENTIAL,conv1_b:SEQUENTIAL,conv2_b:SEQUENTIAL)
#pragma SDS data sys_port(in:AFI,out:AFI,conv1_w:AFI,conv2_w:AFI,conv1_b:AFI,conv2_b:AFI)
void CONV1POOL1CONV2POOL2(DTYPE* in,DTYPE* out,DTYPE* conv1_w,DTYPE* conv2_w,DTYPE* conv1_b,DTYPE* conv2_b);

void load_weight();
void Lenet5_init();
int Lenet5(DTYPE* input);

#endif
