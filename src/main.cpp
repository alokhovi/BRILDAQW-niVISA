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
  scopeCfg.channelConfigurationParameters[3].ONOFF = true;//turn on channel 4
  scopeCfg.channelConfigurationParameters[7].ONOFF = true;//turn on channel 8
  scopeCfg.channelConfigurationParameters[1].VSCALE = "0.25";//set ch2 vertical scale
  scopeCfg.channelConfigurationParameters[7].VSCALE = "0.25";//set ch8 vertical scale
  scopeCfg.channelConfigurationParameters[3].VSCALE = "0.5";//set ch8 vertical scale

  //scopeCfg.globalParams.VSCALE = "10";
  scopeCfg.globalParams.TSCALE = "100e-9";
  //scopeCfg.globalParams.TRIGSOURCE[0] = "8"; //trigger on channel 8 (which is actually the default value)
  //scopeCfg.globalParams.TRIGSOURCE[1] = "-0.440"; //set trigger level
  scopeCfg.globalParams.TRIGSOURCE[0] = "4";
  scopeCfg.globalParams.TRIGSOURCE[1] = "-0.640";
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
  
  //scope.baseConfig(scopeCfg.globalParams, scopeCfg.channelConfigurationParameters);
  data = scope.query(const_cast<ViString>("HORizontal:SAMPLERate?"));
  std::cout << "SAMPLE RATE: " << data.second << std::endl;
  data = scope.query(const_cast<ViString>("HORizontal:RECOrdlength?"));
  std::cout << "RECORD LENGTH: " << data.second << std::endl;
  
  //scope.binIn();
  std::string form = scope.getForm("4","1","1","1250");
  for(int i=14;i<1250;i++){
      std::cout << (int)form[i] << std::endl;
    }
  //scope.resetScope();

  //scope.Dir();

  scope.disconnect();

  scope.stopProfiler("devtest");

  return 0;
}