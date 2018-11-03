#include "lenet5.h"

void innerproduct(DTYPE *in, DTYPE *out, DTYPE *weight, DTYPE *bias, layer gnet)
{
	for (int od = 0; od < gnet.od; od++)
	{
		DTYPE odata = 0;
		for (int id = 0; id < gnet.id; id++)
		{
			odata += weight[od * gnet.id + id] * in[id];
		}
		out[od] = odata+bias[od];
	}
}