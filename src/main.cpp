#include <iostream>
#include <string>
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

  scopeCfg.channelConfigurationParameters[1].ONOFF = true;//turn on channel 1
  scopeCfg.channelConfigurationParameters[1].VSCALE = "0.25";
  scopeCfg.channelConfigurationParameters[2].ONOFF = true;//turn on channel 3
  scopeCfg.channelConfigurationParameters[7].ONOFF = true;//turn on channel 8
  scopeCfg.channelConfigurationParameters[7].VSCALE = "0.5";

  //scopeCfg.globalParams.VSCALE = "10";
  scopeCfg.globalParams.TSCALE = "100e-9";
  scopeCfg.globalParams.TRIGSOURCE[0] = "8"; //trigger on channel 8 (which is actually the default value)
  scopeCfg.globalParams.TRIGSOURCE[1] = "-1.2"; //set trigger level

  //std::cout << scopeCfg.globalParams.VSCALE << std::endl;
  std::cout << scopeCfg.globalParams.TSCALE << std::endl;

  scope.enableProfiling();

  scope.startProfiler("devtest"); 
  std::cout << scopeCfg.channelConfigurationParameters[0].name << std::endl;


  
  //brildaq::nivisa::Status status = scope.connect(ViString("TCPIP::10.176.62.25::4000::SOCKET"),800 /* ms */);

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
  //scope.resetScope()

  //scope.Dir();

  scope.disconnect();

  scope.stopProfiler("devtest");

  return 0;
}
