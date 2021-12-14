cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 14)

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/config/version.txt" VERSION_STRING)

string(REGEX MATCHALL "([^.]+)" VERSION_NUMBERS ${VERSION_STRING})

list(LENGTH VERSION_NUMBERS N)

if( "${N}" LESS 3 )
   message(FATAL_ERROR, "${CMAKE_CURRENT_SOURCE_DIR}/config/version.txt Wrong version string format. It should be as 1.0.0")  
   return()
endif()

list(GET VERSION_NUMBERS    0 N )
set(BRILDAQNIVISA_MAJOR_VERSION ${N})
list(GET VERSION_NUMBERS    1 N )
set(BRILDAQNIVISA_MINOR_VERSION ${N})
list(GET VERSION_NUMBERS    2 N )
set(BRILDAQNIVISA_PATCH_VERSION ${N})

set(BRILDAQNIVISA_VERSION ${BRILDAQNIVISA_MAJOR_VERSION}.${BRILDAQNIVISA_MINOR_VERSION}.${BRILDAQNIVISA_PATCH_VERSION})

message(STATUS "The project version: ${BRILDAQNIVISA_VERSION}")

set(BOOST_ROOT /opt/boost_1_78_0)

project(brildaq_nivisa)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

file(GLOB SOURCES "src/*.cpp")

configure_file("${PROJECT_SOURCE_DIR}/config/version.h.in" "${PROJECT_SOURCE_DIR}/include/version.hpp" @ONLY)

include(CTest) 

enable_testing()

add_library(brildaqnivisa ${SOURCES})

# extra libs (NI-VIS, etc) are added to link time at the botom otf the file in target_link_libraries(brildaqnivisa....

target_include_directories(brildaqnivisa PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(brildaqnivisa PRIVATE ${Boost_INCLUDE_DIR})

add_executable(tekscope_devtest test/tekscope_devtest.cpp)

target_include_directories(tekscope_devtest PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(tekscope_devtest PRIVATE ${Boost_INCLUDE_DIR})

target_link_libraries(tekscope_devtest brildaqnivisa  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(tekscope src/main.cpp)

target_include_directories(tekscope PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(tekscope PRIVATE ${Boost_INCLUDE_DIR})

target_link_libraries(tekscope brildaqnivisa  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_definitions (-DBOOST_TEST_DYN_LINK)

if(APPLE)
   find_library(VISA_LIBRARY VISA)

   MARK_AS_ADVANCED(VISA_LIBRARY)

   set(NI_VISA "${VISA_LIBRARY}/Versions/A/Headers")

   if(EXISTS "${NI_VISA}")
      target_include_directories(brildaqnivisa    PRIVATE /Library/Frameworks/VISA.framework/Versions/A/Headers)
      target_include_directories(tekscope_devtest PRIVATE /Library/Frameworks/VISA.framework/Versions/A/Headers)
      target_include_directories(tekscope         PRIVATE /Library/Frameworks/VISA.framework/Versions/A/Headers)

      set(EXTRA_LIBS ${VISA_LIBRARY})
      
      message("-- NI-VISA softwasre is installed and linked with the project.")  
   else()
      message(FATAL_ERROR, "${NI_VISA} forlder doesn't exist. NI-VISA softwasre is required.")  
      return()
   endif()
endif (APPLE)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   set(NI_VISA /usr/include/ni-visa)
   if(EXISTS "${NI_VISA}")
      target_include_directories(brildaqnivisa    PRIVATE /usr/include/ni-visa)
      target_include_directories(tekscope_devtest PRIVATE /usr/include/ni-visa)
      target_include_directories(tekscope         PRIVATE /usr/include/ni-visa)
      message(STATUS, "NI-VISA softwasre is installed.")  
   else()
      message(FATAL_ERROR, "${NI_VISA} forlder doesn't exist. NI-VISA softwasre is required.")  
      return()
   endif()
endif()

target_link_libraries(brildaqnivisa  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${EXTRA_LIBS})

#
# ctest BOOST Unit Test invocation
#
add_test(TEKSCOPE tekscope_devtest)

set_tests_properties(TEKSCOPE PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=message")

