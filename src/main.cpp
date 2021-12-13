#include <iostream>
#include "tekscope.hpp"

int main()
{
  brildaq::nivisa::TekScope scope;   

  scope.enableProfiling();

  scope.startProfiler("devtest");  sleep(1);
  
  scope.stopProfiler("devtest");

  scope.startProfiler("devtest");  sleep(2);

  scope.stopProfiler("devtest");

   scope.startProfiler("devtest");  sleep(1);
  
  scope.stopProfiler("devtest");

  scope.startProfiler("devtest");  sleep(2);

  scope.stopProfiler("devtest");

  auto stat = scope.getProfilerStat("devtest");

  return 0;
}
