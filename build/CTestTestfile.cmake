# CMake generated Testfile for 
# Source directory: /nfshome0/jsledgei/brildaq-nivisa
# Build directory: /nfshome0/jsledgei/brildaq-nivisa/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(TEKSCOPE "tekscope_devtest")
SET_TESTS_PROPERTIES(TEKSCOPE PROPERTIES  ENVIRONMENT "BOOST_TEST_LOG_LEVEL=message")
