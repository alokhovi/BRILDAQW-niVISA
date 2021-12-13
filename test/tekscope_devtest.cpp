#include <iostream>
#include "tekscope.hpp"

#define BOOST_TEST_MODULE brildaq::nivisa::TekScope test

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( instantiation_test )
{
  brildaq::nivisa::TekScope scope; scope.enableProfiling();
  
  scope.startProfiler("devtest");  sleep(1);

  auto duration = scope.stopProfiler("devtest");

  scope.startProfiler("devtest");  sleep(2);

  duration = scope.stopProfiler("devtest");

  BOOST_TEST_MESSAGE("===Duration of the devtest " << duration.count() << " ms ===") ;

  BOOST_TEST( duration.count() >= 1000 ); 

  auto stat = scope.getProfilerStat("devtest");

  BOOST_TEST_MESSAGE("===Stat: " << stat.first << "  " << stat.second ) ;

  BOOST_TEST( (stat.first == 2 && stat.second == 2 )); 

}
