#include <iostream>
#include "tekscope.hpp"
#include "tekscopecfg.hpp"

int main()
{
  brildaq::nivisa::TekScope    scope;

  brildaq::nivisa::TekScopeCfg scopeCfg;   

  // Setting up configuration parameters (to be implemented later via XDAQ interface)

  // scopeCfg.connectionParameters.connectionString = "TCPIP::127.0.0.1::4000::SOCKET";
  scopeCfg.connectionParameters.connectionString = "TCPIP::10.176.62.25::4000::SOCKET";
  scopeCfg.connectionParameters.timeout          = 800;
  scopeCfg.connectionParameters.exclusiveLock    = true;

  scope.enableProfiling();

  scope.startProfiler("devtest"); 
  
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

  //scope.Dir();

  scope.disconnect();

  scope.stopProfiler("devtest");

  return 0;
}
