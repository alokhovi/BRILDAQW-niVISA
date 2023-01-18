#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include <iterator>
#include <chrono>
//#include <boost/dynamic_bitset.hpp>         
#include "tekscope.hpp"
#include "tekscopecfg.hpp"
#include <string.h>
#include <stdlib.h>  
#include <algorithm>
#include <sstream>
#include <vector>
#include <regex>
#include <cmath>
#include <map>
#include <string_view>

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


 scopeCfg.channelConfigurationParameters[0].VSCALE = "0.5";
 scopeCfg.channelConfigurationParameters[1].VSCALE = "0.5";//set ch2 vertical scale
 scopeCfg.channelConfigurationParameters[2].VSCALE = "0.5";
 scopeCfg.channelConfigurationParameters[3].VSCALE = "0.5";
 scopeCfg.channelConfigurationParameters[4].VSCALE = "0.5";
 scopeCfg.channelConfigurationParameters[5].VSCALE = "0.5";
 scopeCfg.channelConfigurationParameters[6].VSCALE = "0.5";
 scopeCfg.channelConfigurationParameters[7].VSCALE = "0.5";//set ch8 vertical scale
 scopeCfg.globalParams.TSCALE = "10e-9";
 scopeCfg.globalParams.TRIGSOURCE[0] = "8";
 scopeCfg.globalParams.TRIGSOURCE[1] = "-0.440";
 
  // scope.triggerType("edge");
  // scope.triggerSlopeType("Rise");
  // scope.setTriggerLevel(channel, "-0.440");

  auto & cc = scopeCfg.connectionParameters;

  brildaq::nivisa::Status status = scope.connect(const_cast<ViString>(cc.connectionString.c_str()),cc.timeout,cc.exclusiveLock);

  if ( status.first != VI_SUCCESS )
    {
      std::cout << "Connection failed: " << status.second << std::endl; return -1;
    }



  brildaq::nivisa::Data data = scope.query( const_cast<ViString>("*IDN?") );
  std::cout << data.second << std::endl;

  scope.resetScope();
  scope.baseConfig(scopeCfg.globalParams, scopeCfg.channelConfigurationParameters);

  status = scope.write(const_cast<ViString>("acquire:state 0"));
  status = scope.write(const_cast<ViString>("acquire:stopafter sequence"));
  status = scope.write(const_cast<ViString>("acquire:state 1"));

  scope.run_scope_calibration();  

  scope.disconnect();

  return 0;
}
