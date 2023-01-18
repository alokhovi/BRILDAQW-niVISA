#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include <iterator>
#include <chrono>
#include <stdlib.h> 
//#include <boost/dynamic_bitset.hpp>

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

  scopeCfg.channelConfigurationParameters[0].ONOFF = true;
  scopeCfg.channelConfigurationParameters[1].ONOFF = true;//turn on channel 2
  scopeCfg.channelConfigurationParameters[2].ONOFF = true;
  scopeCfg.channelConfigurationParameters[3].ONOFF = true;//turn on channel 4
  scopeCfg.channelConfigurationParameters[4].ONOFF = true;
  scopeCfg.channelConfigurationParameters[5].ONOFF = true;
  scopeCfg.channelConfigurationParameters[6].ONOFF = true;
  scopeCfg.channelConfigurationParameters[7].ONOFF = true;//turn on channel 8

  scopeCfg.channelConfigurationParameters[0].VSCALE = "1";
  scopeCfg.channelConfigurationParameters[1].VSCALE = "1";//set ch2 vertical scale
  scopeCfg.channelConfigurationParameters[2].VSCALE = "0.5";
  scopeCfg.channelConfigurationParameters[3].VSCALE = "0.5";
  scopeCfg.channelConfigurationParameters[4].VSCALE = "0.5";
  scopeCfg.channelConfigurationParameters[5].VSCALE = "0.2";
  scopeCfg.channelConfigurationParameters[6].VSCALE = "0.2";
  scopeCfg.channelConfigurationParameters[7].VSCALE = "0.25";//set ch8 vertical scale

  
  scopeCfg.globalParams.TSCALE = "10e-6";
  scopeCfg.globalParams.TRIGSOURCE[0] = "8";
  scopeCfg.globalParams.TRIGSOURCE[1] = "-0.440";

  //scope.enableProfiling();

  //scope.startProfiler("devtest"); 


  
  //brildaq::nivisa::Status status = scope.connect(ViString("TCPIP::10.176.62.25::4000::SOCKET"),800 );

  auto & cc = scopeCfg.connectionParameters;

  brildaq::nivisa::Status status = scope.connect(const_cast<ViString>(cc.connectionString.c_str()),cc.timeout,cc.exclusiveLock);

  if ( status.first != VI_SUCCESS )
  {
    std::cout << "Connection failed: " << status.second << std::endl; return -1;
  }

  //print ID
  brildaq::nivisa::Data data = scope.query( const_cast<ViString>("*IDN?") );
  std::cout << data.second << std::endl;

  scope.resetScope();
  scope.baseConfig(scopeCfg.globalParams, scopeCfg.channelConfigurationParameters);
  scope.write(const_cast<ViString>("HORizontal:POSition 5"));

  scope.write(const_cast<ViString>("WFMOUTPRE:BYT_NR 1"));
  scope.write(const_cast<ViString>("DATA:SOURCE CH7"));
  scope.write(const_cast<ViString>("header on"));
  data = scope.query(const_cast<ViString>("WFMOUTPRE?"));
  std::cout << data.second << std::endl;
  //scope.ReadWaveform();
  //scope.readWaveform();
  
  scope.write(const_cast<ViString>("ACQUIRE:STOPAFTER RUNSTOP"));
  scope.write(const_cast<ViString>("ACQUIRE:STATE ON"));

  scope.disconnect();

  //scope.stopProfiler("devtest");

  return 0;
}