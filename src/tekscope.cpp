#include <iostream>
#include <fstream>
#include <numeric>
#include <cstring>
#include <string>
#include <thread>

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
                of << "Action " << action << " was not profiled." << std::endl;
                
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


Status TekScope::wait(std::chrono::milliseconds timeout) noexcept
{
    auto beginning = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());

    while ( std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()-beginning) < timeout )
    {
        auto data = query(const_cast<ViString>("BUSY?"));

        if ( data.first >= VI_SUCCESS )
        {
            try 
            {
                if ( std::stoi(data.second)  ) 
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                else 
                {
                    return std::make_pair(data.first,boost::none);
                }
            }
            catch(const std::exception & ex )
            {
                return std::make_pair(VI_ERROR_SYSTEM_ERROR,std::string("Query returned string of wrong format: ") + ex.what());
            }
        }
    }
    return std::make_pair(VI_ERROR_RSRC_BUSY,std::string("The scope is still busy afeter timeout"));
}

Waveform TekScope::readWaveform()
{
    return std::make_pair(VI_SUCCESS,boost::none);
}

Data TekScope::Dir(const ViString & directory)
{
    auto data = query(const_cast<ViString>("FILESystem:CWD?"));

    if ( data.first >= VI_SUCCESS )
    {
        std::cout << data.second << std::endl;
    }

    return std::make_pair(VI_SUCCESS,"");
}

TekScope::~TekScope()
{

}

/*
The following methods all send commands or queries to the scope for their given task
they return the relevant datatype
*/

Data TekScope::resetScope()
{
    return this->query(const_cast<ViString>("*RST;:*OPC?"));
}

Status TekScope::channelState(std::string channel, std::string state)
{
    std::string command = "DISPLAY:WAVEVIEW1:CH" + channel + ":STATE " + state;
    return this->write( const_cast<ViString>(command.c_str()) );
}

Status TekScope::verticalScale(std::string channel, std::string voltsPerDivision)
{
    std::string command = "DISplay:WAVEView1:CH" + channel + ":VERTical:SCAle " + voltsPerDivision;
    return this->write( const_cast<ViString>(command.c_str()));
}

Status TekScope::timeScale(std::string secsPerDivision)
{
    std::string command = "HORizontal:SCAle " + secsPerDivision;
    return this->write( const_cast<ViString>(command.c_str()));
}

Status TekScope::triggerType(std::string type)
{
    std::string command = ":TRIGger:A:TYPe " + type;
    return this->write( const_cast<ViString>(command.c_str()));
}

Status TekScope::triggerSource(std::string channel)
{
    std::string command = ":TRIGger:A:EDGE:SOUrce CH" + channel;
    return this->write( const_cast<ViString>(command.c_str()));
}

Status TekScope::triggerSlopeType(std::string type)
{
    std::string command = ":TRIGger:A:EDGE:SLOpe " + type;
    return this->write( const_cast<ViString>(command.c_str()));
}

Status TekScope::setHalfTrigger()
{
    return this->write( const_cast<ViString>(":TRIGger:A SETLevel") );
}

Status TekScope::setTriggerLevel(std::string channel, std::string voltageLevel)
{
    std::string command = ":TRIGger:A:LEVEL:CH" + channel + " " + voltageLevel;
    return this->write( const_cast<ViString>(command.c_str()) );
}

Data TekScope::checkReady()
{
    return this->query( const_cast<ViString>("*OPC?") );
}

Status TekScope::baseConfig(GlobalConfigurationParams globalParams, ChannelConfiguration channelConfigurationParameters[])
{
    brildaq::nivisa::Status status;
    brildaq::nivisa::Data   data;

    data = this->resetScope();//reset the scope

    for(int i = 0; i < NM_OF_TEKSCOPE_CHANNELS; i++){
        if(channelConfigurationParameters[i].ONOFF){
            std::string channel = std::to_string(channelConfigurationParameters[i].ID); //get the channel# from the ID
            status = this->channelState(channel,"1");
            status = this->verticalScale(channel,channelConfigurationParameters[i].VSCALE);
        }
        else if(i==0 && !channelConfigurationParameters[i].ONOFF){//turn off the first channel if needed
            status = this->channelState(std::to_string(channelConfigurationParameters[i].ID),"0");
        }
    }

    status = this->timeScale(globalParams.TSCALE);
    status = this->triggerType(globalParams.TRIGTYPE);
    status = this->triggerSource(globalParams.TRIGSOURCE[0]);
    status = this->triggerSlopeType(globalParams.SLOPETYPE);
    status = this->setTriggerLevel(globalParams.TRIGSOURCE[0],globalParams.TRIGSOURCE[1]);
    data = this->checkReady();

    return status; /*this is a bad return value*/
}