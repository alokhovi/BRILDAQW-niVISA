#include <iostream>
#include "tekscope.hpp"

int main()
{
  brildaq::nivisa::TekScope scope;   

  scope.enableProfiling();

  scope.startProfiler("devtest"); 
  
  brildaq::nivisa::Status status = scope.connect(ViString("TCPIP::10.176.62.25::4000::SOCKET"),800 /* ms */);

  brildaq::nivisa::Data data = scope.query( const_cast<ViString>("*IDN?") );

  std::cout << data.first << " - "  << data.second << std::endl;

  scope.disconnect();

  scope.stopProfiler("devtest");

  return 0;
}
