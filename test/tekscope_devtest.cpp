#include <iostream>
#include "tekscope.hpp"

#define BOOST_TEST_MODULE brildaq::nivisa::TekScope test

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( connection_profiling_test )
{
  brildaq::nivisa::TekScope scope; 

  scope.connect(ViString("")); scope.disconnect();
  
  scope.enableProfiling();

  for( int i=0; i < 100; i++ )
  {  
    BOOST_TEST_MESSAGE("===Loop: i= " << i << "  of 100 ") ;

    scope.startProfiler("devtest");

    // test only in the CMS network
    
    brildaq::nivisa::Status status = scope.connect(ViString("TCPIP::10.176.62.25::INSTR"));

    if ( status.first != VI_SUCCESS )
    {
      BOOST_TEST_MESSAGE("===Stat: " << status.first << "  " << status.second) ;
    }
    BOOST_CHECK((status.first == VI_SUCCESS || status.first == VI_ERROR_TMO));

    scope.disconnect();

    scope.stopProfiler("devtest");
  }

  scope.dumpProfile("devtest", "profiling.csv", true);
}
