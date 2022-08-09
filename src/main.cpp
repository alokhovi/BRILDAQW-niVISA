#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include <iterator>
#include <chrono>
#include <stdlib.h> 
//#include <boost/dynamic_bitset.hpp>

#include "tekscope.hpp"
#include "tekscopecfg.hpp"


int main()
{
  brildaq::nivisa::TekScope    scope;

  brildaq::nivisa::TekScopeCfg scopeCfg;   

  // Setting up configuration parameters (to be implemented later via XDAQ interface)

  // scopeCfg.connectionParameters.connectionString = "TCPIP::127.0.0.1::4000::SOCKET";
  scopeCfg.connectionParameters.connectionString = "TCPIP::10.176.62.25::4000::SOCKET";
  scopeCfg.connectionParameters.timeout          = 5000;
  scopeCfg.connectionParameters.exclusiveLock    = true;

  scopeCfg.channelConfigurationParameters[0].ONOFF = true;
  scopeCfg.channelConfigurationParameters[1].ONOFF = true;//turn on channel 2
  scopeCfg.channelConfigurationParameters[2].ONOFF = true;
  scopeCfg.channelConfigurationParameters[3].ONOFF = true;//turn on channel 4
  scopeCfg.channelConfigurationParameters[4].ONOFF = true;
  scopeCfg.channelConfigurationParameters[5].ONOFF = true;
  scopeCfg.channelConfigurationParameters[6].ONOFF = true;
  scopeCfg.channelConfigurationParameters[7].ONOFF = true;//turn on channel 8

  scopeCfg.channelConfigurationParameters[0].VSCALE = "1";
  scopeCfg.channelConfigurationParameters[1].VSCALE = "1";//set ch2 vertical scale
  scopeCfg.channelConfigurationParameters[2].VSCALE = "0.5";
  scopeCfg.channelConfigurationParameters[3].VSCALE = "0.5";
  scopeCfg.channelConfigurationParameters[4].VSCALE = "0.5";
  scopeCfg.channelConfigurationParameters[5].VSCALE = "0.2";
  scopeCfg.channelConfigurationParameters[6].VSCALE = "0.2";
  scopeCfg.channelConfigurationParameters[7].VSCALE = "0.25";//set ch8 vertical scale

  
  scopeCfg.globalParams.TSCALE = "10e-6";
  scopeCfg.globalParams.TRIGSOURCE[0] = "8";
  scopeCfg.globalParams.TRIGSOURCE[1] = "-0.440";

  //scope.enableProfiling();

  //scope.startProfiler("devtest"); 


  
  //brildaq::nivisa::Status status = scope.connect(ViString("TCPIP::10.176.62.25::4000::SOCKET"),800 );

  auto & cc = scopeCfg.connectionParameters;

  brildaq::nivisa::Status status = scope.connect(const_cast<ViString>(cc.connectionString.c_str()),cc.timeout,cc.exclusiveLock);

  if ( status.first != VI_SUCCESS )
  {
    std::cout << "Connection failed: " << status.second << std::endl; return -1;
  }

  //print ID
  brildaq::nivisa::Data data = scope.query( const_cast<ViString>("*IDN?") );
  std::cout << data.second << std::endl;

  /*
  std::bitset<8> b1;
  std::bitset<8> b2;
  boost::dynamic_bitset<> num;
  for(int i = 6; i<2506; i+=2){
    b1 = std::bitset<8>(form[i]);
    b2 = std::bitset<8>(form[i+1]);
    num = boost::dynamic_bitset<>(b1.to_string() + b2.to_string());
    //std::cout << "bin : " << num << " || dec : " << binaryToInteger(num) << " || index : " << i << std::endl;
    int val = binaryToInteger(num);
    std::cout << val * scaleVal*5/32767 << " V" << std::endl;
  }*/

  //scope.asciiWaveformReadout("4");

  /*scope.write(const_cast<ViString>(":HORIZONTAL:MODE MANUAL"));
  scope.write(const_cast<ViString>(":HORIZONTAL:MODE:Samplerate 500e9"));
  scope.write(const_cast<ViString>(":HORIZONTAL:MODE:Recordlength 5e7"));*/
  
  /*

  duration<double, std::milli> ms_double = t2 - t1;

  //std::cout << ms_int.count() << "ms\n";
  std::cout << "Waveform Aqcuisiton Time : " << ms_double.count() << "ms\n";
  
  
  std::vector<float> form;
  
  for(int i = 1; i <= brildaq::nivisa::NM_OF_TEKSCOPE_CHANNELS; i++){
    std::ofstream outfile("channelData/CH" + std::to_string(i) + ".txt");
    form = forms[i];
    for(std::size_t j = 0; j < form.size(); j++){
      outfile << std::to_string(form[j]) << std::endl;
    }
  }
  */
  //scope.resetScope();
  //scope.baseConfig(scopeCfg.globalParams, scopeCfg.channelConfigurationParameters);
  //scope.write(const_cast<ViString>("HORizontal:POSition 5"));

  using std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  using std::chrono::duration;
  using std::chrono::milliseconds;

  duration<double, std::milli> ms_double;
  auto t1 = high_resolution_clock::now();
  auto t2 = high_resolution_clock::now();

  std::ofstream Binaryoutfile("aqTimeData/BinaryaqTimes.txt");

  int end = 900;
  
  for(int i = 0; i < end; i++)
  {
    if(((i+1) % 10) == 0) std::cout << std::to_string((double)(i+1)*100/end) << "%" << std::endl;
    
    
    t1 = high_resolution_clock::now();
    scope.readWaveform();
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;
    Binaryoutfile << ms_double.count() << std::endl;
    //std::cout << "Binary Waveform Aqcuisiton Time : " << ms_double.count() << "ms\n";
    scope.query(const_cast<ViString>("*OPC?"));
  }

  /*brildaq::nivisa::Waveform waves = scope.readWaveform();
  ms_double = t2 - t1;
  //outfile << ms_double.count() << std::endl;
  std::cout << "Binary Waveform Aqcuisiton Time : " << ms_double.count() << "ms\n";
  //sleep(50);
  scope.query(const_cast<ViString>("*OPC?"));*/

  /*
  boost::optional<std::map<int, std::vector<float> > > temp = waves.second;
  std::map<int, std::vector<float>> forms = static_cast<std::map<int, std::vector<float>>> (*temp);
  //forms[0];
  //std::cout << std::to_string(forms[6][0]) << std::endl;
  std::vector<float> form;
  for(int i = 1; i<=brildaq::nivisa::NM_OF_TEKSCOPE_CHANNELS; i++){
    std::ofstream outfile("channelData/BinaryTestCH" + std::to_string(i) + ".txt");
    form = forms[i];
    int end = form.size();
    for(std::size_t j = 0; j < end; j++){
      outfile << std::to_string(form[j]) << std::endl;
    }
  }*/
  


  //std::cout << "\n" << std::endl;
  //scope.write(const_cast<ViString>(":DATa:SOUrce CH3"));
  //form = scope.ReadWaveform();
  /*
  std::ofstream outfile("channelData/BinaryTestCH" + channel + ".txt");
  int end = form.size();
  for(std::size_t j = 0; j < end; j++){
    outfile << std::to_string(form[j]) << std::endl;
    //std::cout << std::to_string(form[j]) << std::endl;
  }*/

  //status = scope.write(const_cast<ViString>(":Data:Source CH3"));
  //std::cout << data.second << std::endl;
  
  /*
  using std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  using std::chrono::duration;
  using std::chrono::milliseconds;

  //status = scope.write(const_cast<ViString>("acquire:state 0"));
  //status = scope.write(const_cast<ViString>("acquire:stopafter sequence"));
  //status = scope.write(const_cast<ViString>("acquire:state 1"));
  std::vector<float> f;
  duration<double, std::milli> ms_double;
  auto t1 = high_resolution_clock::now();
  auto t2 = high_resolution_clock::now();

  std::ofstream ASCIIoutfile("aqTimeData/ASCIIaqTimes.txt");
  std::ofstream Binaryoutfile("aqTimeData/BinaryaqTimes.txt");

  std::map<int, std::vector<float>> forms;
  std::map<int, std::vector<float>> formsASC;

  int end = 1;
  
  for(int i = 0; i < end; i++)
  {
    if(((i+1) % 10) == 0) std::cout << std::to_string((double)(i+1)*100/end) << "%" << std::endl;
    
    
    t1 = high_resolution_clock::now();
    forms = scope.readWaveformBinary();
    //std::cout << forms[0][5] << std::endl;
    //f = scope.ReadWaveform();
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;
    //Binaryoutfile << ms_double.count() << std::endl;
    std::cout << "Binary Waveform Aqcuisiton Time : " << ms_double.count() << "ms\n";
    //sleep(50);
    

    
    t1 = high_resolution_clock::now();
    //f = scope.asciiWaveformReadout("4");
    formsASC = scope.readWaveformAscii();
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;
    //ASCIIoutfile << ms_double.count() << std::endl;
    //std::cout << ms_double.count() << std::endl;

    //ms_double = t2 - t1;
    std::cout << "ASCII Waveform Aqcuisiton Time : " << ms_double.count() << "ms\n";
  }*/

  //printf("Hi");


  /*
  for(std::size_t i; i < f.size(); i++){
    std::cout << std::to_string(f[i]) << std::endl;
  }*/



  //std::vector<float> form;

  /*
  //std::cout << "BINARY" << std::endl;
  for(int i = 1; i <= brildaq::nivisa::NM_OF_TEKSCOPE_CHANNELS; i++){
    std::ofstream outfile("channelData/BinaryCH" + std::to_string(i) + ".txt");
    //std::cout << "CH" << i << std::endl;
    form = forms[i];
    for(std::size_t j = 0; j < form.size(); j++){
    //for(std::size_t j = 0; j < 3; j++){
      outfile << std::to_string(form[j]) << std::endl;
    }
  }
  /*
  //std::cout << "ASCII" << std::endl;
  for(int i = 1; i <= brildaq::nivisa::NM_OF_TEKSCOPE_CHANNELS; i++){
    std::ofstream outfile("channelData/ASCIICH" + std::to_string(i) + ".txt");
    //std::cout << "CH" << i << std::endl;
    form = formsASC[i];
    for(std::size_t j = 0; j < form.size(); j++){
    //for(std::size_t j = 0; j < 3; j++){
      outfile << std::to_string(form[j]) << std::endl;
    }
  }*/

  
  scope.write(const_cast<ViString>("ACQUIRE:STOPAFTER RUNSTOP"));
  scope.write(const_cast<ViString>("ACQUIRE:STATE ON"));
/*
  std::vector<std::string> results = scope.getMeasurementResults("1");
  for(std::size_t i = 0; i < results.size(); i++){
    std::cout << results[i] << std::endl;
  }
*/
  //scope.write(const_cast<ViString>("HEADER 1"));
  //data = scope.query("WFMOUTPRE?");
  //std::cout << data.second << std::endl;

  //SAVE THIS
  scope.disconnect();

  //scope.stopProfiler("devtest");

  return 0;
}