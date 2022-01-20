#include <iostream>
#include "tekscope.hpp"
#include <optional>

#define BOOST_TEST_MODULE brildaq::nivisa::TekScope test

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( tekscope_testing_suite )

brildaq::nivisa::TekScope scope;  

BOOST_AUTO_TEST_CASE( connection_profiling_test_socket )
{
  scope.enableProfiling();

  scope.connect(ViString(""),800 /* ms */); 

  for( int i=0; i < 100; i++ )
  {  
    BOOST_TEST_MESSAGE("===Loop: i= " << i << "  of 100 ") ;

    scope.startProfiler("devtest_socket");

    // test only in the CMS network
    
    brildaq::nivisa::Status status = scope.connect(ViString("TCPIP::10.176.62.25::4000::SOCKET"),800 /* ms */);
    
    BOOST_CHECK((status.first == VI_SUCCESS || status.first == VI_ERROR_TMO));

    status = scope.query( const_cast<ViString>("*IDN?") );

    if ( status.first != VI_SUCCESS )
    {
      BOOST_TEST_MESSAGE("===Stat: " << status.first << "  " << *status.second) ;
    }

    scope.disconnect();

    scope.stopProfiler("devtest_socket");
  }

  scope.dumpProfile("devtest_socket", "profiling_socket.csv", true);
}

BOOST_AUTO_TEST_CASE( connection_profiling_test_instr )
{
  scope.enableProfiling();
  
  scope.connect(ViString(""),800 /* ms */); 
  
  for( int i=0; i < 100; i++ )
  {  
    BOOST_TEST_MESSAGE("===Loop: i= " << i << "  of 100 ") ;

    scope.startProfiler("devtest_instr");

    // test only in the CMS network
    
    brildaq::nivisa::Status status = scope.connect(ViString("TCPIP::10.176.62.25::INSTR"),800 /* ms */);
    
    BOOST_CHECK((status.first == VI_SUCCESS || status.first == VI_ERROR_TMO));

    status = scope.query( const_cast<ViString>("*IDN?") );

    if ( status.first != VI_SUCCESS )
    {
      BOOST_TEST_MESSAGE("===Stat: " << status.first << "  " << *status.second) ;
    }

    scope.disconnect();

    scope.stopProfiler("devtest_instr");
  }

  scope.dumpProfile("devtest_instr", "profiling_instr.csv", true);
}

BOOST_AUTO_TEST_SUITE_END()
