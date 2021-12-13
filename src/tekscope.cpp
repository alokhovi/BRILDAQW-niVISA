#include <iostream>
#include <fstream>
#include <numeric>

#include "tekscope.hpp"
using namespace brildaq::nivisa;

void TekScope::startProfiler(const std::string & action)
{
    if ( ! _profilingEnabled ) return;

    assert(!action.empty());

    _beginning[action] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
}

std::chrono::milliseconds TekScope::stopProfiler(const std::string & action)
{
    if ( ! _profilingEnabled ) return std::chrono::milliseconds::zero();

    assert(!action.empty());

    if ( _beginning.find(action) != _beginning.end() && _beginning.at(action) != std::chrono::milliseconds::zero() )
    {
        auto readoutTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()-_beginning[action]); 

        rebin(action, readoutTime);

        _beginning.at(action) = std::chrono::milliseconds::zero();

        return readoutTime;
    }
    return std::chrono::milliseconds::zero();
}

void TekScope::rebin(const std::string & action, std::chrono::milliseconds readoutTime)
{
    auto & hist = _profiling[action];

    if ( hist.find(readoutTime) == hist.end() )
    {
        hist.insert(std::pair<std::chrono::milliseconds,long long>(readoutTime,1));
    }
    else
    {
        hist[readoutTime]++;
    }
}

int TekScope::dumpProfile(const std::string & action, const std::string & fileName, bool verbose, const std::string & delimiter) const
{
    assert(!action.empty());   assert(!fileName.empty()); 
    
    if ( ! _profilingEnabled ) return 0;

    std::fstream of(fileName,std::ios::out); 
    
    if ( !of )
    {
        if (verbose ) std::cerr << "Cannot create/rewrite the file " << fileName << " (" << strerror(errno) << ")" << std::endl; 
        
        return errno;
    }
    else
    {
        of.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
        
        if ( _profiling.find(action) == _profiling.cend() )
        {
            try 
            {
                of << "Action " << action << "was not profiled." << std::endl;
                
            }
            catch (std::ifstream::failure e) 
            {
                std::cerr << "Exception: " << e.what() << " File: " << __FILE__ << ":" << __LINE__ << std::endl;
            }
            of.close(); return 0;            
        }

        try 
        {
            auto binIter     = _profiling.at(action).begin();

            while ( binIter != _profiling.at(action).end()  )
            {
                of << binIter->first.count() << delimiter << binIter->second << std::endl;

                ++binIter;
            }

        }
        catch (std::ifstream::failure e) 
        {
            std::cerr << "Exception: " << e.what() << " File: " << __FILE__ << ":" << __LINE__ << std::endl;
        }
        of.close(); return 0;
    }
}

std::pair<long, long long> TekScope::getProfilerStat(const std::string & action) const
{
    assert(!action.empty());  if ( ! _profilingEnabled ) return std::make_pair(0,0);

    auto hist = _profiling.find(action);

    if ( hist == _profiling.end()  ) return std::make_pair(0,0);

    auto histMap = hist->second;

    long long sum = std::accumulate(std::begin(histMap), std::end(histMap), 0LL,[](long long previous, decltype(*histMap.begin()) p){ return previous + p.second; });
    
    return std::make_pair(histMap.size(),sum);
}


Waveform TekScope::readWaveform()
{
    return std::make_pair(VI_SUCCESS,boost::none);
}

TekScope::~TekScope()
{

}