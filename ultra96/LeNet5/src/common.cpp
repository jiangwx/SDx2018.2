#include "lenet5.h"

void cvMat2array(cv::Mat cvMat, DTYPE* array)
{
	int length = cvMat.cols*cvMat.rows;
	for(int i=0;i<length;i++)
	{
		array[i]=(float)cvMat.data[i];
	}

}

void load_wb(layer l, DTYPE *weight, DTYPE *bias)
{
    int wlen=l.ic*l.oc*l.k*l.k;
    int blen=l.oc;
    char nstr[512];
    sprintf(nstr, "/mnt/Lenet5/weight/%s.wt", l.name);
    FILE *fp = fopen(nstr, "rb");
    fread(weight, 1, wlen * sizeof(DTYPE), fp);

    sprintf(nstr, "/mnt/Lenet5/weight/%s.bs", l.name);
    fp = fopen(nstr, "rb");
    fread(bias, 1, blen * sizeof(DTYPE), fp);
    fclose(fp);
}
void load_data(DTYPE* data,layer l)
{
    char nstr[50];
    int len = l.oc*l.ow*l.oh;
    sprintf(nstr, "/mnt/Lenet5/blob/%s.blob", l.name);
    FILE *fp = fopen(nstr, "rb");
    fread(data, 1, len * sizeof(DTYPE), fp);
    fclose(fp);
}
void check_data(DTYPE *data,layer l)
{
    int len = l.oc*l.ow*l.oh;
    int err = 0;
    char nstr[50];

    DTYPE *tmp = (DTYPE *)malloc(sizeof(DTYPE)*len);
    sprintf(nstr, "/mnt/Lenet5/blob/%s.blob", l.name);
    FILE *fp = fopen(nstr, "rb");
    fread(tmp, 1, len * sizeof(DTYPE), fp);
    fclose(fp);

    for (int i = 0; i < len; i++)
    {
        if (((data[i] - tmp[i]) > CheckScale) || ((data[i] - tmp[i]) < -CheckScale))
        {
            err++;
            printf("data[%d]:correct = %f, wrong =%f\n",i,tmp[i],data[i]);
        }
    }

    if (err > 0) { printf("error cnt= %d, %s\n", err, l.name);}
    else { printf("correct %s \n", l.name);}
    free(tmp);
}
void show_data(DTYPE *data,layer l)
{
    char nstr[50];
    int len = l.oc*l.ow*l.oh;
    sprintf(nstr, "/mnt/Lenet5/blob/%s.blob", l.name);
    FILE *fp = fopen(nstr, "rb");
    fread(data, 1, len * sizeof(DTYPE), fp);
    fclose(fp);
    for(int i =0;i<l.ih;i++)
    {
    	for(int j=0;j<l.iw;j++)
    	{
    		printf("%.1f ",data[i*IMG_W+j]);
    	}
    	printf("\n");
    }
}
