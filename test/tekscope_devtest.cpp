#include <iostream>
#include "tekscope.hpp"

#define BOOST_TEST_MODULE brildaq::nivisa::TekScope test

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( instantiation_test )
{
  brildaq::nivisa::TekScope scope;

  BOOST_TEST( scope.isConnected() );

}

// int main()
// {
//   brildaq::nivisa::TekScope scope;   std::cout << scope.isConnected() << std::endl;
// }