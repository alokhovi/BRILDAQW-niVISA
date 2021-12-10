#include <iostream>
#include "tekscope.hpp"

using namespace brildaq::nivisa;

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

void TekScope::startProfiler()
{
    _beginning = std::chrono::high_resolution_clock::now().time_since_epoch();    
}

void TekScope::stopProfiler()
{
    auto elpased = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()-_beginning); 

    std::cout << elpased.count() << std::endl;
}

Waveform TekScope::readWaveform()
{
    return std::make_pair(VI_SUCCESS,boost::none);
}

TekScope::~TekScope()
{
    
}