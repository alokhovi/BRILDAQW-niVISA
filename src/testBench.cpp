/*/
/* TCPIP TEKTRONIX SCOPE BRIL CMS*/

#include <stdio.h>

#include <stdlib.h>

#include <ni-visa/visa.h> 

#define MAX_CNT 200 

#define MAX_CNT1 1000

int main(void)
{

    // VISA variables //

    static unsigned char  strres [1000];
    static ViStatus status;
    static ViSession defaultRM, vi;
    static ViUInt32 retCount;
    static ViUInt32 actual;
    //ViChar buffer[MAX_CNT]; 

    // Open session to TCPIP device  //

    viOpenDefaultRM(&defaultRM);
    viSetAttribute (defaultRM, VI_ATTR_TMO_VALUE,1600);
    viOpen(defaultRM,"TCPIP::10.176.62.25::INSTR",VI_NULL,VI_NULL,&vi);
    viSetAttribute (vi, VI_ATTR_TMO_VALUE,100);

    // Data Source & Instrument Identification //

    viWrite(vi, (ViBuf)"SELECT:CH1 ON",14, &actual); 
    viWrite(vi, (ViBuf)":DATa:SOUrce:AVAILable?",24, &actual);
    viRead(vi,strres, MAX_CNT1, &retCount);
    printf("\nTektronix MSO58LP - Receiving  Data\n \n");
    printf("Data Source Available: %s\n", strres);

    viWrite(vi, (ViBuf)"*IDN?", 6, &actual); 
    viRead(vi,strres, MAX_CNT, &retCount);
    printf("Settings Scope: %s\n", strres);

    //////////////// Adquiring Data CH1 ////////////////////

    // Settings //

    viWrite(vi, (ViBuf)":DATa:SOUrce CH1",17, &actual);
    viRead(vi,strres, MAX_CNT,  &retCount);
    printf("Receving Data CH1: %s\n", strres);

    viWrite(vi, (ViBuf)":DATa:START 1",14, &actual);
    viRead(vi,strres, MAX_CNT,  &retCount);

    viWrite(vi, (ViBuf)":DATa:STOP 2500",16, &actual);
    viRead(vi,strres, MAX_CNT,  &retCount);

    viWrite(vi, (ViBuf)":WFMOutpre:ENCdg BINARY",24, &actual); // Binary Data Receiving
    viRead(vi,strres, MAX_CNT,  &retCount);

    viWrite(vi, (ViBuf)":WFMOutpre:BYT_Nr 1",20, &actual);
    viRead(vi,strres, MAX_CNT,  &retCount);

    viWrite(vi, (ViBuf)":HEADer 1",10, &actual);
    viRead(vi,strres, MAX_CNT,  &retCount);

    viWrite(vi,(ViBuf)":VERBOSE 1", 11,&actual);
    viRead(vi,strres, MAX_CNT, &retCount);

    // Settings for Curve Query //

    viWrite(vi, (ViBuf)":WFMOutpre?",12, &actual);
    viRead(vi,strres, MAX_CNT1,  &retCount);
    printf("%s\n", strres);

    // Query Data //

    viWrite(vi, (ViBuf)":CURVE?",8, &actual);
    viRead(vi,strres, MAX_CNT1,  &retCount);
    printf("%s\n ", strres);

    // Off Channels//

    viWrite(vi, (ViBuf)"SELECT:CH2 OFF",15, &actual);

    // Close session //

    viClose(vi);
    viClose(defaultRM);
    return 0;
}