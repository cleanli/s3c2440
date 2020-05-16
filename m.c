#define rGPFCON    (*(volatile unsigned *)0x56000050) //Port F control
#define rGPFDAT    (*(volatile unsigned *)0x56000054) //Port F data
#define rGPFUP     (*(volatile unsigned *)0x56000058) //Pull-up control F

typedef unsigned int U32;
void dely(U32 tt);
void main(void)
{
    rGPFCON = 0x55aa;
    while(1){
        if(rGPFDAT & 0x10)
            rGPFDAT = 0;
        else
            rGPFDAT = 0xff;
        dely(1);
    }
    return;
}
void dely(U32 tt)
{
    U32 i;
    for(;tt>0;tt--)
    {
        for(i=0;i<1000000;i++){}
    }
}
