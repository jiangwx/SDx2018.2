#include "lenet5.h"
#define TEST 1
char synset[10]={'0','1','2','3','4','5','6','7','8','9'};
sds_utils::perf_counter hw_ctr;

layer input = { "data", 28,28,1,28,28,1,0,0,0};

int main(int argc, char **argv)
{
    DTYPE *blob = (DTYPE *)sds_alloc(sizeof(DTYPE)*IMG_W*IMG_H);
    Lenet5_init();
    load_weight();
    load_data(blob,input);
    hw_ctr.start();
    Lenet5(blob);
    hw_ctr.stop();
    //printf("LeNet5 costs %llu cycles\n",hw_ctr.avg_cpu_cycles());

}
