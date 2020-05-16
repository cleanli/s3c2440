#define rGPFCON    (*(volatile unsigned *)0x56000050) //Port F control
#define rGPFDAT    (*(volatile unsigned *)0x56000054) //Port F data
#define rGPFUP     (*(volatile unsigned *)0x56000058) //Pull-up control F

typedef unsigned int U32;
void dely(U32 tt);
int main(void)
{
   int delay_time = 5;
   rGPFCON = 0x55aa;
   while(1)
   {
	 rGPFDAT =  rGPFDAT & 0xffffffef;
	 rGPFDAT =  rGPFDAT | 0x000000e0;
     dely(delay_time);
	 rGPFDAT =  rGPFDAT & 0xffffffdf;
	 rGPFDAT =  rGPFDAT | 0x000000d0;
     dely(delay_time);
     rGPFDAT =  rGPFDAT & 0xffffffbf;
	 rGPFDAT =  rGPFDAT | 0x000000b0;
     dely(delay_time);
     rGPFDAT =  rGPFDAT & 0xffffff7f;
	 rGPFDAT =  rGPFDAT | 0x00000070;
     dely(delay_time);
     rGPFDAT =  rGPFDAT & 0xffffffbf;
	 rGPFDAT =  rGPFDAT | 0x000000b0;
     dely(delay_time);
	 rGPFDAT =  rGPFDAT & 0xffffffdf;
	 rGPFDAT =  rGPFDAT | 0x000000d0;
     dely(delay_time);
   }   
   return 0;
}
void dely(U32 tt)
{
   U32 i;
   for(;tt>0;tt--)
   {
     for(i=0;i<100000;i++){}
   }
}
   
