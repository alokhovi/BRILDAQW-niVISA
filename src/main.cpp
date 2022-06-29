#include <iostream>
#include <string>
#include <bitset>

#include "tekscope.hpp"
#include "tekscopecfg.hpp"


int main()
{
  brildaq::nivisa::TekScope    scope;

  brildaq::nivisa::TekScopeCfg scopeCfg;   

  // Setting up configuration parameters (to be implemented later via XDAQ interface)

  // scopeCfg.connectionParameters.connectionString = "TCPIP::127.0.0.1::4000::SOCKET";
  scopeCfg.connectionParameters.connectionString = "TCPIP::10.176.62.25::4000::SOCKET";
  scopeCfg.connectionParameters.timeout          = 5000;
  scopeCfg.connectionParameters.exclusiveLock    = true;

  scopeCfg.channelConfigurationParameters[1].ONOFF = true;//turn on channel 2
  scopeCfg.channelConfigurationParameters[2].ONOFF = true;//turn on channel 3
  scopeCfg.channelConfigurationParameters[7].ONOFF = true;//turn on channel 8
  scopeCfg.channelConfigurationParameters[1].VSCALE = "0.25";//set ch2 vertical scale
  scopeCfg.channelConfigurationParameters[7].VSCALE = "0.25";//set ch8 vertical scale

  //scopeCfg.globalParams.VSCALE = "10";
  scopeCfg.globalParams.TSCALE = "100e-9";
  scopeCfg.globalParams.TRIGSOURCE[0] = "8"; //trigger on channel 8 (which is actually the default value)
  scopeCfg.globalParams.TRIGSOURCE[1] = "-0.440"; //set trigger level

  //std::cout << scopeCfg.globalParams.VSCALE << std::endl;
  std::cout << scopeCfg.globalParams.TSCALE << std::endl;

  scope.enableProfiling();

  scope.startProfiler("devtest"); 
  std::cout << scopeCfg.channelConfigurationParameters[0].name << std::endl;


  
  //brildaq::nivisa::Status status = scope.connect(ViString("TCPIP::10.176.62.25::4000::SOCKET"),800 );

  auto & cc = scopeCfg.connectionParameters;

  brildaq::nivisa::Status status = scope.connect(const_cast<ViString>(cc.connectionString.c_str()),cc.timeout,cc.exclusiveLock);

  if ( status.first != VI_SUCCESS )
  {
    std::cout << "Connection failed: " << status.second << std::endl; return -1;
  }

  brildaq::nivisa::Data data = scope.query( const_cast<ViString>("*IDN?") );
  std::cout << data.first << " - "  << data.second << std::endl;
  std::cout << data.second << std::endl;
  
  scope.baseConfig(scopeCfg.globalParams, scopeCfg.channelConfigurationParameters);
  data = scope.query(const_cast<ViString>("HORizontal:SAMPLERate?"));
  std::cout << "SAMPLE RATE: " << data.second << std::endl;
  //scope.binIn();
  //scope.resetScope();

  //scope.Dir();

  scope.disconnect();

  scope.stopProfiler("devtest");

  return 0;
}
/*
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

    
    for(int i=14;i<305;i++){
      std::cout << std::bitset<8>(strres[i]) << " ------ " << (int)strres[i] << std::endl;
    }
    
    // Off Channels//

    viWrite(vi, (ViBuf)"SELECT:CH2 OFF",15, &actual);

    // Close session //

    viClose(vi);
    viClose(defaultRM);
    return 0;
}*/

/*/
/* TCPIP TEKTRONIX SCOPE BRIL CMS*/
/*
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

  viWrite(vi, (ViBuf)":DATa:STOP 900",16, &actual);
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

  for(int i=14;i<305;i++){
      std::cout << std::bitset<8>(strres[i]) << " ------ " << (int)strres[i] << std::endl;
  }

  // Off Channels//

  viWrite(vi, (ViBuf)"SELECT:CH2 OFF",15, &actual);

  // Close session //

  viClose(vi);
  viClose(defaultRM);
  return 0;

}*/