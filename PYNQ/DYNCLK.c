#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

#define Dynclk_SIZE 0x10000
#define DYNCLK0 0x43c00000
#define DYNCLK1 0x43c10000
#define DYNCLK2 0x43c20000
#define DYNCLK3 0x43c30000

static const int MMCM[29]=
{
0x000,0x004,0x008,0x00C,
0x010,0x200,0x204,
0x208,0x20C,0x210,//CLKOUT0
0x214,0x218,0x21C,//CLKOUT1
0x220,0x224,0x228,//CLKOUT2
0x22C,0x230,0x234,//CLKOUT3
0x238,0x23C,0x240,//CLKOUT4
0x244,0x248,0x24C,//CLKOUT5
0x250,0x254,0x258,//CLKOUT6
0x25C
};

enum MMCM_Reg
{
Software_Reset,CLK_Status,Clock_Monitor,Interrupt_Status,
Interrupt_Enable,ClockConfig0,CLKFBOUT_PHASE,
CLKOUT0_DIVIDE,CLKOUT0_PHASE,CLKOUT0_DUTY,
CLKOUT1_DIVIDE,CLKOUT1_PHASE,CLKOUT1_DUTY,
CLKOUT2_DIVIDE,CLKOUT2_PHASE,CLKOUT2_DUTY,
CLKOUT3_DIVIDE,CLKOUT3_PHASE,CLKOUT3_DUTY,
CLKOUT4_DIVIDE,CLKOUT4_PHASE,CLKOUT4_DUTY,
CLKOUT5_DIVIDE,CLKOUT5_PHASE,CLKOUT5_DUTY,
CLKOUT6_DIVIDE,CLKOUT6_PHASE,CLKOUT6_DUTY,
LoadConfig
};

struct Reg_Params
{
	float Freq;
	float BOUT_MULT;
	int   DIVCLK_DIVIDE;
	float CLKOUT_DIVIDE;
	unsigned char  BOUT_MULT_I;//BOUT_MULT整数部分
	unsigned short BOUT_MULT_F;//BOUT_MULTС小数部分
	unsigned char  CLKOUT_DIVIDE_I;//BOUT_MULT整数部分
	unsigned short CLKOUT_DIVIDE_F;//BOUT_MULTС小数部分
	int ClkConfig0;//
	int CLKOUT;//
};

int Clk_Read_Reg(void* Dynclk_Addr,enum MMCM_Reg Offset)
{
	int Value;
	Value=*((unsigned*)(Dynclk_Addr + MMCM[Offset]));
	return Value;
}

int Clk_Write_Reg(void* Dynclk_Addr,enum MMCM_Reg Offset, int Value)
{
	*((unsigned*)(Dynclk_Addr + MMCM[Offset]))=Value;
	//printf("MMCM[%x]=%x\n",MMCM[Offset],Value);
	return 0;
}

struct Reg_Params ClkGetParams(float Freq)
{
	struct Reg_Params tmp;
	if (Freq < 20)
	{
		printf("Unsupported Frequency\n");
	}
	else if ((Freq >= 20) && (Freq < 50))
	{
		tmp.DIVCLK_DIVIDE = 5;
		tmp.CLKOUT_DIVIDE = 20;
		tmp.BOUT_MULT = Freq;
	}
	else if ((Freq >= 50) && (Freq < 100))
	{
		tmp.DIVCLK_DIVIDE = 4;
		tmp.CLKOUT_DIVIDE = 12.5;
		tmp.BOUT_MULT = Freq / 2;
	}
	else if ((Freq >= 100) && (Freq < 250))
	{
		tmp.DIVCLK_DIVIDE = 2;
		tmp.CLKOUT_DIVIDE = 12.5;
		tmp.BOUT_MULT = Freq / 4;
	}
	else
	{
		printf("Unsupported Frequency\n");
	}

	tmp.BOUT_MULT_I = (int)tmp.BOUT_MULT;
	tmp.BOUT_MULT_F = (int)((tmp.BOUT_MULT - tmp.BOUT_MULT_I) * 1000);
	tmp.CLKOUT_DIVIDE_I = (int)tmp.CLKOUT_DIVIDE;
	tmp.CLKOUT_DIVIDE_F = (int)((tmp.CLKOUT_DIVIDE - tmp.CLKOUT_DIVIDE_I) * 1000);
	tmp.ClkConfig0 = (tmp.BOUT_MULT_F << 15) + (tmp.BOUT_MULT_I<<8) + tmp.DIVCLK_DIVIDE;
	tmp.CLKOUT = (tmp.CLKOUT_DIVIDE_F << 8) + tmp.CLKOUT_DIVIDE_I;
	return tmp;
}

int Clk_Config_Freq(void* Dynclk_Addr, int Clk_id, float Freq)
{
	int Status;
	struct Reg_Params Clk_Config=ClkGetParams(Freq);

	Clk_Write_Reg(Dynclk_Addr,ClockConfig0,Clk_Config.ClkConfig0);
	Clk_Write_Reg(Dynclk_Addr,Clk_id*3+7,Clk_Config.CLKOUT);
	do
	{
		Status=Clk_Read_Reg(Dynclk_Addr,CLK_Status);
	}
	while(Status!=0x01);
	Clk_Write_Reg(Dynclk_Addr,LoadConfig,0x03);
}

int main(int argc, char* argv[])
{
	void* DYNCLK0_ptr;
	void* DYNCLK1_ptr;

	int Freq0 = atoi(argv[1]);
	int Freq1 = atoi(argv[2]);


	// open mem
	int mem_fd;
	mem_fd = open("/dev/mem", O_RDWR);
	if (mem_fd == -1) {
	printf("CANNOT open /dev/mem!\n");
	return -1;
	}
    
	DYNCLK0_ptr = mmap(NULL, Dynclk_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, DYNCLK0);
	DYNCLK1_ptr = mmap(NULL, Dynclk_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, DYNCLK1);

	clock_t time1, time2;
	time1 = clock();
	Clk_Config_Freq(DYNCLK0_ptr, 0, Freq0);
	Clk_Config_Freq(DYNCLK1_ptr, 0, Freq1);
	time2 = clock();

	printf("%d %d ", Freq0, Freq1);
 
	munmap(DYNCLK0_ptr, Dynclk_SIZE);
	munmap(DYNCLK1_ptr, Dynclk_SIZE);
}


