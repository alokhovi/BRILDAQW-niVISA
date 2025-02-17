#include <iostream>
#include <fstream>
#include <numeric>
#include <cstring>
#include <string>
#include <thread>
#include <sstream>
#include <cmath>
#include <vector>
#include <regex>

#include "tekscope.hpp"
using namespace brildaq::nivisa;
//#define MAX_CNT1 1000

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

Data TekScope::Dir(const ViString & directory)
{
    auto data = query(const_cast<ViString>("FILESystem:CWD?"));

    if ( data.first >= VI_SUCCESS )
    {
        std::cout << data.second << std::endl;
    }

    return std::make_pair(VI_SUCCESS,"");
}

Waveform TekScope::readWaveform()
//std::map<int, std::vector<float>> TekScope::readWaveform()
{

  std::string                         command;
  brildaq::nivisa::Data   data;
  std::map<int, std::vector<float>>   allChannels;
  float yoffset;

  this->write(const_cast<ViString>("acquire:state 0"));
  this->write(const_cast<ViString>("acquire:stopafter sequence"));
  this->write(const_cast<ViString>("acquire:state 1"));
  this->query(const_cast<ViString>("*OPC?"));
    
  for(int j = 1; j <= brildaq::nivisa::NM_OF_TEKSCOPE_CHANNELS; j++){
     command = "Data:source CH" + std::to_string(j);
    // std::cout << "-------- " << command << " --------" << std::endl;
    this->write(const_cast<ViString>(command.c_str()));

    command = "CH" + std::to_string(j) + ":OFFSet?";
    data = this->query( const_cast<ViString>(command.c_str()));
    yoffset = std::stof(data.second);
    //yoffset = 0.0;
    allChannels.insert(std::pair<int, std::vector<float>>(j,this->ReadWaveform(yoffset)));//edited by Ren
    this->query(const_cast<ViString>("*OPC?"));
    printf("\n");
  }

 //  for (const auto& n : allChannels)     
 //    {  
 // std::cout << n.first << " = ";                                           
 //      for (int i=0; i<n.second.size(); ++i)    
 //     {                                  
 //       std::cout << n.second[i] << ",";               
 //     }                                                            
 //      std::cout << '\n';                      
 //    }              
 //  std::cout << '\n';        
	
  return std::make_pair(VI_SUCCESS, allChannels);

  //return allChannels;

  error:
  return std::make_pair(0,boost::none);

}

std::map<int, std::vector<float>> TekScope::readWaveformBinary()
{
  std::map<int, std::vector<float>>   forms;
  //brildaq::nivisa::Status             status;
  brildaq::nivisa::Data               data;

  this->write(const_cast<ViString>("acquire:state 0"));

  this->write(const_cast<ViString>("acquire:stopafter sequence"));

  this->write(const_cast<ViString>("acquire:state 1"));

  data = this->query(const_cast<ViString>("WFMOUTPre:Byt_NR?"));
  std::cout << data.second << std::endl;

  /*std::string command;
    for(int i = 1; i <= brildaq::nivisa::NM_OF_TEKSCOPE_CHANNELS; i++)
    {
        command = ":DATa:SOUrce CH" + std::to_string(i);
        this->write(const_cast<ViString>(command.c_str()));
        forms.insert(std::pair<int, std::vector<float>>(i,this->ReadWaveform()));
        //printf("Channel %u Done\n",i);
	}*/
    
  return forms;

}

std::map<int, std::vector<float>> TekScope::readWaveformAscii()
{
    std::map<int, std::vector<float>> forms;
    brildaq::nivisa::Status status;

    status = this->write(const_cast<ViString>("acquire:state 0"));
    if(status.first < VI_SUCCESS){
        std::cout << "state 0 error" << std::endl;
    }

    status = this->write(const_cast<ViString>("acquire:stopafter sequence"));
    if(status.first < VI_SUCCESS){
        std::cout << "stopafter error" << std::endl;
    }

    status = this->write(const_cast<ViString>("acquire:state 1"));
    if(status.first < VI_SUCCESS){
        std::cout << "state 1 error" << std::endl;
    }

    //forms.insert(std::pair<int, std::vector<float>>(4,this->asciiWaveformReadout("4")));
    for(int i = 1; i <= brildaq::nivisa::NM_OF_TEKSCOPE_CHANNELS; i++)
    {
        forms.insert(std::pair<int, std::vector<float>>(i,this->asciiWaveformReadout(std::to_string(i))));
    }
    
    return forms;
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
    brildaq::nivisa::Data data = this->query(const_cast<ViString>("*RST;:*OPC?"));
    this->terminateChannels();
    return data;
}

Status TekScope::terminateChannels()
{
    std::string command;
    brildaq::nivisa::Status status;
    for(int i = 0; i < NM_OF_TEKSCOPE_CHANNELS; i++)//ensure 50OHM termination for all scopes
    {
        command = "CH" + std::to_string(i+1) + ":TERMINATION 50";
        status = this->write(const_cast<ViString>(command.c_str()));
    }
    return status;
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

Data TekScope::getVerticalScale(std::string channel)
{
    std::string command = "DISplay:WAVEView1:CH" + channel + ":VERTical:SCAle?";
    return this->query( const_cast<ViString>(command.c_str()));
}


Status TekScope::verticalPosition(std::string channel, std::string position)
{
  std::string command = "DISplay:WAVEView1:CH" + channel + ":VERTical:POSition " + position;
  return this->write( const_cast<ViString>(command.c_str()));
 }

Data TekScope::getVerticalPosition(std::string channel)
{
  std::string command = "DISplay:WAVEView1:CH" + channel + ":VERTical:POSition?";
  return this->query( const_cast<ViString>(command.c_str()));
 }

Status TekScope::verticalOffset(std::string channel, std::string volts)
{
  std::string command = "CH" + channel + ":OFFSet " + volts;
  return this->write( const_cast<ViString>(command.c_str()));
}

Data TekScope::getVerticalOffset(std::string channel)
{
  std::string command = "CH" + channel + ":OFFSet?";
  return this->query( const_cast<ViString>(command.c_str()));
}

Status TekScope::verticalAutoset(std::string on_off)
{
  std::string command = "AUTOSet:VERTical:ENAble" + on_off;
  return this->write( const_cast<ViString>(command.c_str()));
}

Status TekScope::verticalOptimize(std::string resolution, std::string visibility)
{
  std::string command = "AUTOSet:VERTical:OPTIMize" + resolution + visibility;
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

std::string TekScope::binIn()
{
    brildaq::nivisa::Status status;
    brildaq::nivisa::Data   data;
    /*
    static unsigned char  strres [6263];
    static ViStatus stat;
    static ViSession defaultRM, vi;
    static ViUInt32 retCount;
    static ViUInt32 actual;

    viOpenDefaultRM(&defaultRM);
    viSetAttribute (defaultRM, VI_ATTR_TMO_VALUE,1600);
    viOpen(defaultRM,"TCPIP::10.176.62.25::INSTR",VI_NULL,VI_NULL,&vi);
    viSetAttribute (vi, VI_ATTR_TMO_VALUE,100);*/

    //this->write(const_cast<ViString>("SELECT:CH1 ON"));
    //this->write(const_cast<ViString>("SELECT:CH4 ON"));
    this->channelState("4","1");
    data = this->query(const_cast<ViString>(":DATa:SOUrce:AVAILable?"));
    std::cout << "Data Source Available: " << data.second << std::endl;
    this->write(const_cast<ViString>(":DATa:SOUrce CH4"));

    //this->timeScale("0.5");
    data = this->query(const_cast<ViString>(":HORizontal:MODE?"));
    std::cout << "HORIZ MODE: " << data.second << std::endl;
    data = this->query(const_cast<ViString>(":HORizontal:RECOrdlength?"));
    std::cout << "HORIZ MODE LENGTH: " << data.second << std::endl;

    this->write(const_cast<ViString>(":DATa:START 1"));
    //this->write(const_cast<ViString>(":DATa:STOP 6250"));
    this->write(const_cast<ViString>(":DATa:STOP 1250"));
    this->write(const_cast<ViString>(":WFMOutpre:ENCdg BINARY"));
    this->write(const_cast<ViString>(":WFMOutpre:BYT_Nr 1"));
    //data = this->query(const_cast<ViString>("WFMOutpre:BIT_Nr?"));
    //std::cout << "BITS: " << data.second << std::endl;
    this->write(const_cast<ViString>(":HEADer 1"));
    this->write(const_cast<ViString>(":VERBOSE 1"));
    data = this->query(const_cast<ViString>(":WFMOutpre?"));
    std::cout << data.second << std::endl;

    
    //viWrite(vi, (ViBuf)":CURVE?",8, &actual);
    //viRead(vi,strres, 7000,  &retCount);
    //printf("%s\n ", strres);
    

    /*for(int i=14;i<6250;i++){
      if((int)strres[i] != 0 && (int)strres[i] != 1 && (int)strres[i] != 255){
        std::cout << std::bitset<8>(strres[i]) << " ------ " << (int)strres[i] << " ------ " << i << std::endl;
        }
      //std::cout << (int)strres[i] << std::endl;
    }*/

    data = this->query(":CURVE?");
    std::string temp = data.second;
    //std::cout << data.second[0] << std::endl;
    for(int i=14;i<1250;i++){
      /*if((int)temp[i] != 0 && (int)temp[i] != 1 && (int)temp[i] != 255){
        std::cout << std::bitset<8>(temp[i]) << " ------ " << (int)temp[i] << " ------ " << i << std::endl;
        }*/
      std::cout << (int)temp[i] << std::endl;
    }

    //std::string temp = std::string(strres);
    //std::cout << temp << std::endl;

    return temp;

}

std::string TekScope::getForm(std::string channel, std::string byteNum, std::string start, std::string stop)
{
    brildaq::nivisa::Status status;
    brildaq::nivisa::Data   data;
    std::string command;

    command = "SELECT:CH" + channel + " ON"; //turn on channel
    this->write(const_cast<ViString>(command.c_str()));

    command = ":DATa:SOUrce CH" + channel; //turn on channel
    this->write(const_cast<ViString>(command.c_str()));

    command = ":DATa:START " + start; //set start point for data collection
    this->write(const_cast<ViString>(command.c_str()));

    command = ":DATa:STOP " + stop; //set end point of data collection
    this->write(const_cast<ViString>(command.c_str()));

    this->write(const_cast<ViString>(":WFMOutpre:ENCdg BINARY")); //set binary encoding format
    this->write(const_cast<ViString>(":HEADer 0")); //turn off headers

    command = ":WFMOutpre:BYT_Nr " + byteNum; 
    this->write(const_cast<ViString>(command.c_str()));
    /*
    command = ":HORIZONTAL:MODE MANUAL"; //set end point of data collection
    this->write(const_cast<ViString>(command.c_str()));*/

    //data = this->query(const_cast<ViString>(":WFMOutpre:BYT_Nr?"));
    /*
    this->write(const_cast<ViString>("acquire:state 0"));
    this->write(const_cast<ViString>("acquire:stopafter SEQUENCE"));
    this->write(const_cast<ViString>("acquire:state 1"));
    this->query("*OPC?");
    */

    data = this->query(":CURVE?");
    std::string form = data.second;

    this->write(const_cast<ViString>(":HEADer 1")); //turn headers back on
    return form;
}

std::vector<std::string> TekScope::getMeasurementResults(std::string measurementID) //get the Mean,STD,Max,Min, and pop of a measurement
{
    std::vector<std::string> measurementValues; //vector to store the measurements
    brildaq::nivisa::Status status;
    brildaq::nivisa::Data   data;
    std::string command;

    std::string start = "1";
    std::string stop = "100";

    command = ":DATa:START " + start; //set start point for data collection
    this->write(const_cast<ViString>(command.c_str()));

    command = ":DATa:STOP " + stop; //set end point of data collection
    this->write(const_cast<ViString>(command.c_str()));

    command = "MEASUrement:MEAS" + measurementID + ":RESUlts:ALLAcqs:";

    this->write(const_cast<ViString>("HEADER 0"));
    data = this->query(const_cast<ViString>((command + "MEAN?").c_str())); //get the mean
    measurementValues.push_back(data.second);

    data = this->query(const_cast<ViString>((command + "STDdev?").c_str())); //get std
    measurementValues.push_back(data.second);

    data = this->query(const_cast<ViString>((command + "MAXimum?").c_str())); //get max value
    measurementValues.push_back(data.second);

    data = this->query(const_cast<ViString>((command + "MINimum?").c_str())); //get min value
    measurementValues.push_back(data.second);

    data = this->query(const_cast<ViString>((command + "POPUlation?").c_str())); //get population
    measurementValues.push_back(data.second);

    command = "MEASUrement:MEAS" + measurementID + ":RESUlts:CURRentacq:";

    data = this->query(const_cast<ViString>((command + "MEAN?").c_str())); //get the mean
    measurementValues.push_back(data.second);

    data = this->query(const_cast<ViString>((command + "STDdev?").c_str())); //get std
    measurementValues.push_back(data.second);

    data = this->query(const_cast<ViString>((command + "MAXimum?").c_str())); //get max value
    measurementValues.push_back(data.second);

    data = this->query(const_cast<ViString>((command + "MINimum?").c_str())); //get min value
    measurementValues.push_back(data.second);

    data = this->query(const_cast<ViString>((command + "POPUlation?").c_str())); //get population
    measurementValues.push_back(data.second);


    return measurementValues;
}

void TekScope::measureDelays()//turn on delay measurements
{
    brildaq::nivisa::Data data;
    this->write(const_cast<ViString>("HEADER 0"));

    this->write(const_cast<ViString>("MEASUrement:ADDMEAS DELAY"));
    this->write(const_cast<ViString>("MEASUrement:MEAS1:SOUrce CH3"));
    this->write(const_cast<ViString>("MEASUrement:MEAS1:SOUrce2 CH4"));
    std::cout << data.second << std::endl;
    return;
}

std::vector<float> TekScope::asciiWaveformReadout(std::string channel)
{
    std::vector<float> form; //this will hold the waveform of individual channel
    brildaq::nivisa::Status status;
    brildaq::nivisa::Data data;

    std::string command = ":Data:Source CH" + channel;
    status = this->write(const_cast<ViString>(command.c_str()));
    if(status.first < VI_SUCCESS){
        std::cout << "Data source error" << std::endl;
    }

    status = this->write(const_cast<ViString>(":Header 0"));
    if(status.first < VI_SUCCESS){
        std::cout << "Header error" << std::endl;
    }

    status = this->write(const_cast<ViString>(":Data:Start 1"));
    if(status.first < VI_SUCCESS){
        std::cout << "Data start error" << std::endl;
    }

    data = this->query(const_cast<ViString>("HORizontal:RECOrdlength?"));
    if(data.first < VI_SUCCESS){
        std::cout << "Record length error" << std::endl;
    }

    command = ":Data:Stop " + data.second;
    status = this->write(const_cast<ViString>(command.c_str()));
    if(status.first < VI_SUCCESS){
        std::cout << "Data stop error" << std::endl;
    }

    status = this->write(const_cast<ViString>(":WFMOutpre:ENCdg Ascii"));
    if(status.first < VI_SUCCESS){
        std::cout << "Encoding error" << std::endl;
    }

    command = "DISplay:WAVEView1:CH" + channel + ":VERTical:SCAle?";
    data = this->query(const_cast<ViString>(command.c_str()));
    if(data.first < VI_SUCCESS){
        std::cout << "Scale query error" << std::endl;
    }
    float verticalScale = std::stof(data.second); //save the vertical scale
    
    /*
    data = this->query(const_cast<ViString>(":WFMOutpre?"));
    if(data.first < VI_SUCCESS){
        std::cout << "Outpre error" << std::endl;
    }
    else{
        std::cout << data.second << std::endl;
    }*/

    data = this->query(const_cast<ViString>("*OPC?"));
    if(data.first < VI_SUCCESS){
        std::cout << "OPC error" << std::endl;
    }

    data = this->query(const_cast<ViString>(":CURVE?"));
    if(data.first < VI_SUCCESS){
        std::cout << "curve error" << std::endl;
    }
    
    //std::vector<int> vect;
    std::stringstream ss(data.second);

    for (int i; ss >> i;) {
        form.push_back((float)i * verticalScale * 5 / 32767); //convert to voltage value    
        if (ss.peek() == ',')
            ss.ignore();
    }

    //for(std::size_t i = 0; i < form.size(); i++)
        //std::cout << form[i] << std::endl;

    return form;
}

Status TekScope::pause(float second)
{
  std::cout << "===== pause =====" << std::endl; 
  std::string command = "PAUSe " + std::to_string(second);
  return this->write(const_cast<ViString>(command.c_str()));
}
//add by jae

std::map<std::string, std::vector<float>> TekScope::zeroCrossingTimes()
{
  std::map<std::string, std::vector<float>> times;
  /*float                                     voltageThreshold;

    scope.write(const_cast<ViString>("acquire:state 0"));
    scope.write(const_cast<ViString>("acquire:stopafter sequence"));
    scope.write(const_cast<ViString>("acquire:state 1"));



    scope.write(const_cast<ViString>("ACQUIRE:STOPAFTER RUNSTOP"));
    scope.write(const_cast<ViString>("ACQUIRE:STATE ON"));*/

  return times;
}
