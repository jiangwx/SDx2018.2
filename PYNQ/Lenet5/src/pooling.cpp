#include "lenet5.h"

void maxpool(DTYPE *in, DTYPE *out, layer net)
{
    for (int od = 0; od < net.od; od++)
    {
        for (int oh = 0; oh < net.oh; oh++)
        {
            for (int ow = 0; ow < net.ow; ow++)
            {
                float odata=-3400000000;
                int outcnt = od * net.oh * net.ow + oh * net.ow + ow;
                for (int wh = 0; wh < net.kernel; wh++)
                {
                    for (int ww = 0; ww < net.kernel; ww++)
                    {
                        float ret;

                        int xind = ow*net.stride + ww - net.pad;
                        int yind = oh*net.stride + wh - net.pad;

                        int incnt = od * net.ih*net.iw + yind *net.iw + xind;

                        if ((xind < 0) || (xind > (net.iw - 1)) || (yind < 0) || (yind > (net.ih - 1)))	ret = 0;
                        else ret = in[incnt];

                        if (ret > odata) odata = ret;
                    }
                }
                out[outcnt] = odata;
            }
        }
    }
}

void avgpool(DTYPE *in, DTYPE *out, layer net)
{
    for (int od = 0; od < net.od; od++)
    {
        for (int oh = 0; oh < net.oh; oh++)
        {
            for (int ow = 0; ow < net.ow; ow++)
            {
                DTYPE odata = 0;
                int outcnt = od * net.oh * net.ow + oh * net.ow + ow;
                for (int wh = 0; wh < net.kernel; wh++)
                {
                    for (int ww = 0; ww < net.kernel; ww++)
                    {
                        DTYPE ret = 0;

                        int xind = ow*net.stride + ww - net.pad;
                        int yind = oh*net.stride + wh - net.pad;

                        int incnt = od * net.ih*net.iw + yind *net.iw + xind;

                        if ((xind < 0) || (xind > (net.iw - 1)) || (yind < 0) || (yind > (net.ih - 1)))	ret = 0;
                        else ret = in[incnt];

                        odata += ret;
                    }
                }
                out[outcnt] = odata/(net.kernel*net.kernel);
            }
        }
    }
}
