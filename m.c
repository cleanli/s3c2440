#define rGPFCON    (*(volatile unsigned *)0x56000050) //Port F control
#define rGPFDAT    (*(volatile unsigned *)0x56000054) //Port F data
#define rGPFUP     (*(volatile unsigned *)0x56000058) //Pull-up control F

typedef unsigned int U32;
void main(void)
{
    rGPFCON = 0x55aa;
    rGPFDAT =  0xff;
    return;
}
