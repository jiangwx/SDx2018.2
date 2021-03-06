#include "lenet5.h"
void load_wb(layer net, DTYPE *weight, DTYPE *bias)
{
    int wlen=net.id*net.od*net.kernel*net.kernel;
    int blen=net.od;
    char nstr[512];
    sprintf(nstr, "/mnt/Lenet5/weight/%s.wt", net.name);
    FILE *fp = fopen(nstr, "rb");
    fread(weight, 1, wlen * sizeof(DTYPE), fp);

    sprintf(nstr, "/mnt/Lenet5/weight/%s.bs", net.name);
    fp = fopen(nstr, "rb");
    fread(bias, 1, blen * sizeof(DTYPE), fp);
    fclose(fp);
}
void load_data(DTYPE* data,layer net)
{
    char nstr[50];
    int len = net.od*net.ow*net.oh;
    sprintf(nstr, "/mnt/Lenet5/blob/%s.blob", net.name);
    FILE *fp = fopen(nstr, "rb");
    fread(data, 1, len * sizeof(DTYPE), fp);
    fclose(fp);
}
void generate_data(DTYPE* data,layer net)
{
	int len = net.od*net.ow*net.oh;
	for(int i=0;i<len;i++)
	{
		data[i]=(DTYPE)rand();
	}
}
void check_data(DTYPE *data,layer net)
{
    int len = net.od*net.ow*net.oh;
    int err = 0;
    char nstr[50];

    DTYPE *tmp = (DTYPE *)malloc(sizeof(DTYPE)*len);
    sprintf(nstr, "/mnt/Lenet5/blob/%s.blob", net.name);
    FILE *fp = fopen(nstr, "rb");
    fread(tmp, 1, len * sizeof(DTYPE), fp);
    fclose(fp);

    for (int i = 0; i < len; i++)
    {
        if (((data[i] - tmp[i]) > CheckScale) || ((data[i] - tmp[i]) < -CheckScale))
        {
            err++;
            printf("data[%d]:correct = %f, wrong =%f\n",i,(float)tmp[i],(float)data[i]);
        }
    }

    if (err > 0) { printf("error cnt= %d, %s\n", err, net.name);}
    else { printf("correct %s \n", net.name);}
    free(tmp);
}
