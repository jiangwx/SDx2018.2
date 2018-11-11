#include "lenet5.h"

void fm2mm(float *fm, float *mm, layer l)
{
    for(int h=0;h<l.oh;h++)
    {
        for(int w=0;w<l.ow;w++)
        {
            for(int c=0;c<l.ic;c++)
            {
                for(int kh=0;kh<l.k;kh++)
                {
                    for(int kw=0;kw<l.k;kw++)
                    {

                        int fw = w*l.s - l.p + kw;//matrix coordinate to fm coordinate
                        int fh = h*l.s - l.p + kh;
                        int fm_index = c*l.ih*l.iw + fh*l.iw + fw;
                        int mm_index_t = (c*l.k*l.k + kh*l.k + kw)*l.ow*l.oh + h*l.ow + w;//transpose the matrix in this procedure

                        if((fw < 0)||(fh < 0)||(fw > (l.iw-1))||(fh > (l.ih-1)))
                            mm[mm_index_t]=0;
                        else
                            mm[mm_index_t]=fm[fm_index];
                    }
                }
            }
        }
    }
}

void add_bias(float *ifm, float *ofm, float *bias, layer l, int relu)
{
	float odata;
    for (int oc = 0; oc < l.oc; oc++)
    {
        for (int i = 0; i < l.ow*l.oh; i++)
        {
        	odata = ifm[oc*l.ow*l.oh + i] + bias[oc];
			if (relu)
			{
				if (odata<0) odata = 0;
			}
			ofm[oc*l.ow*l.oh + i] = odata;
        }
    }
}

void gemm(float *im, float *weight, float* om, layer l)
{
    const float alpha=1;
    const float beta=0;
    int M = l.oc;
    int N = l.ow*l.oh;
    int K = l.ic*l.k*l.k;
    int lda=K;//A的列
    int ldb=N;//B的列
    int ldc=N;//C的列

    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, alpha, weight, lda, im, ldb, beta, om, ldc);

}

void convolution_mm(float *ifm, float *ofm, float *weight, float *bias, layer l, int relu)
{
    float* om=(float*)calloc(l.oh*l.ow*l.oc, sizeof(float));
    if(l.k==1)
    {
        gemm(ifm,weight,om,l);
    }
    else
    {
        float* im=(float*)calloc(l.oh*l.ow*l.ic*l.k*l.k, sizeof(float));
        fm2mm(ifm,im,l);
        gemm(im,weight,om,l);
        free(im);
    }
    add_bias(om,ofm,bias,l,relu);
}

void fc(float *ifm, float *ofm, float *weight, float *bias, layer l, int relu)
{
    float* om=(float*)calloc(l.oh*l.ow*l.oc, sizeof(float));

    gemm(ifm,weight,om,l);

    add_bias(om,ofm,bias,l,relu);
}
