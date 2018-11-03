#include "lenet5.h"

void conv(DTYPE *in, DTYPE *out, DTYPE *weight, DTYPE *bias, layer net, int relu)
{
    for (int oh = 0; oh < net.oh; oh++)
    {
        for (int ow = 0; ow < net.ow; ow++)
        {
            for (int od = 0; od < net.od; od++)
            {
                DTYPE odata = 0;
                int outcnt = od * net.oh*net.ow + oh * net.ow + ow;
                for (int id = 0; id < net.id; id++)
                {
                    for (int wh = 0; wh < net.kernel; wh++)
                    {
                        for (int ww = 0; ww < net.kernel; ww++)
                        {
                            DTYPE ret = 0;

                            int xind = ow*net.stride - net.pad + ww;
                            int yind = oh*net.stride - net.pad + wh;

                            int incnt = id * net.ih*net.iw + yind *net.iw + xind;
                            int weicnt = od * net.id*net.kernel*net.kernel + id * net.kernel* net.kernel + wh * net.kernel + ww;

                            if ((xind < 0) || (xind >(net.iw - 1)) || (yind < 0) || (yind >(net.ih - 1)))
                                ret = 0;
                            else
                                ret = in[incnt];
                            ret *= weight[weicnt];
                            odata += ret;
                        }
                    }
                }
                odata += bias[od];
                if (relu)
                {
                    if (odata<0)odata = 0;
                }
                out[outcnt] = odata;
            }
        }
    }
}

void CONV1POOL1CONV2POOL2(DTYPE* in,DTYPE* out,DTYPE* conv1_w,DTYPE* conv2_w,DTYPE* conv1_b,DTYPE* conv2_b)
{
#pragma HLS INTERFACE m_axi depth=16 port=conv2_b offset=slave bundle=IO
#pragma HLS interface s_axilite port=conv2_b bundle=s_axi_AXILiteS
#pragma HLS INTERFACE m_axi depth=6 port=conv1_b offset=slave bundle=IO
#pragma HLS interface s_axilite port=conv1_b bundle=s_axi_AXILiteS
#pragma HLS INTERFACE m_axi depth=2400 port=conv2_w offset=slave bundle=IO
#pragma HLS interface s_axilite port=conv2_w bundle=s_axi_AXILiteS
#pragma HLS INTERFACE m_axi depth=150 port=conv1_w offset=slave bundle=IO
#pragma HLS interface s_axilite port=conv1_w bundle=s_axi_AXILiteS
#pragma HLS INTERFACE m_axi depth=256 port=out offset=slave bundle=IO
#pragma HLS interface s_axilite port=out bundle=s_axi_AXILiteS
#pragma HLS INTERFACE m_axi depth=784 port=in offset=slave bundle=IO
#pragma HLS interface s_axilite port=in bundle=s_axi_AXILiteS
#pragma HLS interface s_axilite port=return bundle=s_axi_AXILiteS
    DTYPE IBUFF[16][28*28];
#pragma HLS RESOURCE variable=IBUFF core=RAM_2P_BRAM
#pragma HLS ARRAY_PARTITION variable=IBUFF complete dim=1
    DTYPE OBUFF[16][24*24];
#pragma HLS RESOURCE variable=OBUFF core=RAM_2P_BRAM
#pragma HLS ARRAY_PARTITION variable=OBUFF complete dim=1
    DTYPE CONV1_WBUFF[1*6][5*5];
#pragma HLS RESOURCE variable=CONV1_WBUFF core=RAM_2P_BRAM
#pragma HLS ARRAY_PARTITION variable=CONV1_WBUFF block factor=6 dim=1
    DTYPE CONV2_WBUFF[6*16][5*5];
#pragma HLS RESOURCE variable=CONV2_WBUFF core=RAM_2P_BRAM
#pragma HLS ARRAY_PARTITION variable=CONV2_WBUFF block factor=24 dim=1
    DTYPE CONV1_BBUFF[6];
#pragma HLS ARRAY_PARTITION variable=CONV1_BBUFF complete dim=0
    DTYPE CONV2_BBUFF[16];
#pragma HLS ARRAY_PARTITION variable=CONV2_BBUFF complete dim=0
    Load_IBUFF:
    for(int i=0;i<28*28;i++)
    {
#pragma HLS PIPELINE
        IBUFF[0][i]=in[i];
    }
    CLR_OBUFF1:
    for(int i=0;i<24*24;i++)
    {
#pragma HLS PIPELINE
        for(int tm=0;tm<16;tm++)
        {
            OBUFF[tm][i]=0;
        }
    }
    Load_CONV1_WBUFF:
    for(int tmtn=0;tmtn<1*6;tmtn++)
    {
        for(int i=0;i<5*5;i++)
        {
#pragma HLS PIPELINE
            CONV1_WBUFF[tmtn][i]=conv1_w[tmtn*5*5+i];
        }
    }
    Load_CONV2_WBUFF:
    for(int tmtn=0;tmtn<6*16;tmtn++)
    {
        for(int i=0;i<5*5;i++)
        {
#pragma HLS PIPELINE
            CONV2_WBUFF[tmtn][i]=conv2_w[tmtn*5*5+i];
        }
    }
    Load_CONV1_BBUFF:
    for(int i=0;i<6;i++)
    {
#pragma HLS PIPELINE
        CONV1_BBUFF[i]=conv1_b[i];
    }
    Load_CONV2_BBUFF:
    for(int i=0;i<16;i++)
    {
#pragma HLS PIPELINE
        CONV2_BBUFF[i]=conv2_b[i];
    }
    CONV1:
    for (int kh = 0; kh < 5; kh++)
    {
        for (int kw = 0; kw < 5; kw++)
        {
            for (int tc = 0; tc < 24; tc++)
            {
                for (int tr = 0; tr < 24; tr++)
                {
#pragma HLS PIPELINE
                    int iw = tc + kw;
                    int ih = tr + kh;
                    for (int tm = 0; tm < 6; tm++)
                    {
                        DTYPE odatatmp = OBUFF[tm][tr*24 + tc];
                        DTYPE odata = 0;
#pragma HLS RESOURCE variable=odata core=DSP_Macro
                        odata += CONV1_WBUFF[tm][kh* 5 + kw] * IBUFF[0][28*ih + iw];
                        OBUFF[tm][tr*24 + tc] = odata + odatatmp;
                    }
                }
            }
        }
    }
    Add_Bias1:
    for(int i=0;i<24*24;i++)
    {
#pragma HLS PIPELINE
        for(int tm=0;tm<6;tm++)
        {
            OBUFF[tm][i]=OBUFF[tm][i]+CONV1_BBUFF[tm];
        }
    }
    CLR_IBUFF1:
    for(int i=0;i<12*12;i++)
    {
#pragma HLS PIPELINE
        for(int tm=0;tm<6;tm++)
            {
            IBUFF[tm][i]=-340000000;
        }
    }
    POOL1:
    for (int kh = 0; kh < 2; kh++){
        for (int kw = 0; kw < 2; kw++){
        	for (int ow = 0; ow < 12; ow++){
                for (int oh = 0; oh < 12; oh++){
#pragma HLS PIPELINE
                    int iw = ow*2 + kw;
                    int ih = oh*2 + kh;
                    for (int od = 0; od < 6; od++)
                    {
                        DTYPE odatatmp = IBUFF[od][oh*12 + ow];
                        DTYPE odata = OBUFF[od][ih*24 + iw];
                        if (odata>odatatmp)	IBUFF[od][oh*12+ ow] = odata;
                    }}}}}
    CLR_OBUFF:
    for(int i=0;i<8*8;i++)
    {
#pragma HLS PIPELINE
    	for(int tm=0;tm<16;tm++)
    	{
            OBUFF[tm][i]=0;
        }
    }
    CONV2:
    for (int kh = 0; kh < 5; kh++)
    {
        for (int kw = 0; kw < 5; kw++)
        {
            for (int tc = 0; tc < 8; tc++)
            {
                for (int tr = 0; tr < 8; tr++)
                {
#pragma HLS PIPELINE
                    int iw = tc + kw;
                    int ih = tr + kh;
                    int idx = 12*ih + iw;
                    for (int tm = 0; tm < 16; tm++)
                    {
                        DTYPE odatatmp = OBUFF[tm][tr*8 + tc];
                        DTYPE odata = 0;
#pragma HLS RESOURCE variable=odata core=DSP_Macro
                        for (int id = 0; id < 6; id++)
                            odata += CONV2_WBUFF[tm*6 + id][kh* 5 + kw] * IBUFF[id][idx];
                        OBUFF[tm][tr*8 + tc] = odata + odatatmp;
                    }
                }
            }
        }
    }
    Add_Bias2:
    for(int i=0;i<8*8;i++)
    {
#pragma HLS PIPELINE
        for(int tm=0;tm<16;tm++)
        {
            OBUFF[tm][i]=OBUFF[tm][i]+CONV2_BBUFF[tm];
        }
    }
    CLR_IBUFF2:
    for(int i=0;i<4*4;i++)
    {
#pragma HLS PIPELINE
        for(int tm=0;tm<16;tm++)
        {
            IBUFF[tm][i]=-3400000000;
        }
    }
    POOL2:
    for (int kh = 0; kh < 2; kh++)
    {
        for (int kw = 0; kw < 2; kw++)
        {
            for (int ow = 0; ow < 4; ow++)
            {
                for (int oh = 0; oh < 4; oh++)
                {
#pragma HLS PIPELINE
                    int iw = ow*2 + kw;
                    int ih = oh*2 + kh;
                    for (int od = 0; od < 16; od++)
                    {
                        DTYPE odatatmp = IBUFF[od][oh*4 + ow];
                        DTYPE odata = OBUFF[od][ih*8 + iw];
                        if (odata>odatatmp)	IBUFF[od][oh*4 + ow] = odata;
                    }
                }
            }
        }
    }
    COPY_IBUFF:
    for(int i=0;i<4*4;i++)
    {
#pragma HLS PIPELINE
        for(int tm=0;tm<16;tm++)
        {
            OBUFF[tm][i]=IBUFF[tm][i];
        }
    }
    EXPORT_OBUFF:
    for(int i=0;i<4*4;i++)
    {
        for(int tm=0;tm<16;tm++)
        {
#pragma HLS PIPELINE
            out[tm*4*4+i]=OBUFF[tm][i];
        }
    }

}

void CONV1(DTYPE* in,DTYPE* out,DTYPE* weight,DTYPE* bias)
{
    DTYPE IBUFF[28*28];
#pragma HLS RESOURCE variable=IBUFF core=RAM_2P_BRAM
    DTYPE OBUFF[6][24*24];
#pragma HLS ARRAY_PARTITION variable=OBUFF complete dim=1
#pragma HLS RESOURCE variable=OBUFF core=RAM_2P_BRAM
    DTYPE WBUFF[1*6][5*5];
#pragma HLS ARRAY_PARTITION variable=WBUFF complete dim=1
#pragma HLS RESOURCE variable=WBUFF core=RAM_1P_BRAM
    DTYPE BBUFF[6];
#pragma HLS ARRAY_PARTITION variable=BBUFF complete dim=0
    Load_IBUFF:
    for(int i=0;i<28*28;i++)
    {
#pragma HLS PIPELINE
        IBUFF[i]=in[i];
    }
    CLR_OBUFF:
    for(int i=0;i<24*24;i++)
    {
#pragma HLS PIPELINE
        for(int tm=0;tm<6;tm++)
        {
            OBUFF[tm][i]=0;
        }
    }
    Load_WBUFF:
    for(int tmtn=0;tmtn<1*6;tmtn++)
    {
        for(int i=0;i<5*5;i++)
        {
#pragma HLS PIPELINE
            WBUFF[tmtn][i]=weight[tmtn*5*5+i];
        }
    }
    Load_BUFF:
    for(int i=0;i<6;i++)
    {
#pragma HLS PIPELINE
        BBUFF[i]=bias[i];
    }
    CONV:
    for (int kh = 0; kh < 5; kh++){
        for (int kw = 0; kw < 5; kw++){
            for (int tc = 0; tc < 24; tc++){
                for (int tr = 0; tr < 24; tr++)
                {
#pragma HLS PIPELINE
                    int iw = tc + kw;
                    int ih = tr + kh;
                    for (int tm = 0; tm < 6; tm++)
                    {
                        DTYPE odatatmp = OBUFF[tm][tr*24 + tc];
                        DTYPE odata = 0;
                            odata += WBUFF[tm][kh* 5 + kw] * IBUFF[28*ih + iw];
                        OBUFF[tm][tr*24 + tc] = odata + odatatmp;
                    }}}}}
    Add_Bias:
    for(int i=0;i<24*24;i++)
    {
#pragma HLS PIPELINE
        for(int tm=0;tm<6;tm++)
        {
            OBUFF[tm][i]=OBUFF[tm][i]+BBUFF[tm];
        }
    }
    Export_OBUFF:
    for(int i=0;i<24*24;i++)
    {
        for(int tm=0;tm<6;tm++)
        {
#pragma HLS PIPELINE
            out[tm*24*24+i]=OBUFF[tm][i];
        }
    }
}
void CONV2(DTYPE* in,DTYPE* out,DTYPE* weight,DTYPE* bias)
{
    DTYPE IBUFF[6][12*12];
#pragma HLS ARRAY_PARTITION variable=IBUFF complete dim=1
    DTYPE OBUFF[16][8*8];
#pragma HLS ARRAY_PARTITION variable=OBUFF complete dim=1
    DTYPE WBUFF[6*16][5*5];
#pragma HLS RESOURCE variable=WBUFF core=RAM_2P_BRAM
#pragma HLS ARRAY_PARTITION variable=WBUFF block factor=12 dim=1
    DTYPE BBUFF[16];
#pragma HLS ARRAY_PARTITION variable=BBUFF complete dim=0
    Load_IBUFF:
    for(int i=0;i<12*12;i++)
    {
        for(int tn=0;tn<6;tn++)
        {
#pragma HLS PIPELINE
            IBUFF[tn][i]=in[tn*12*12+i];
        }
    }
    CLR_OBUFF:
    for(int i=0;i<8*8;i++)
    {
#pragma HLS PIPELINE
        for(int tm=0;tm<16;tm++)
        {
        	OBUFF[tm][i]=0;
        }
    }
    Load_WBUFF:
    for(int tmtn=0;tmtn<6*16;tmtn++)
    {
        for(int i=0;i<5*5;i++)
        {
#pragma HLS PIPELINE
            WBUFF[tmtn][i]=weight[tmtn*5*5+i];
        }
    }
    Load_BBUFF:
    for(int i=0;i<16;i++)
    {
#pragma HLS PIPELINE
        BBUFF[i]=bias[i];
    }
    CONV:
    for (int kh = 0; kh < 5; kh++){
        for (int kw = 0; kw < 5; kw++){
            for (int tc = 0; tc < 8; tc++){
                for (int tr = 0; tr < 8; tr++)
                {
#pragma HLS PIPELINE
                    int iw = tc + kw;
                    int ih = tr + kh;
                    int idx = 12*ih + iw;
                    for (int tm = 0; tm < 16; tm++)
                    {
                        DTYPE odatatmp = OBUFF[tm][tr*8 + tc];
                        DTYPE odata = 0;
                        for (int id = 0; id < 6; id++)
                            odata += WBUFF[tm*6 + id][kh* 5 + kw] * IBUFF[id][idx];
                        OBUFF[tm][tr*8 + tc] = odata + odatatmp;
                    }}}}}
    Add_Bias:
    for(int i=0;i<8*8;i++)
    {
#pragma HLS PIPELINE
        for(int tm=0;tm<16;tm++)
        {
            OBUFF[tm][i]=OBUFF[tm][i]+BBUFF[tm];
        }
    }
    Export_OBUFF:
    for(int i=0;i<8*8;i++)
    {
        for(int tm=0;tm<16;tm++)
        {
#pragma HLS PIPELINE
            out[tm*8*8+i]=OBUFF[tm][i];
        }
    }
}
