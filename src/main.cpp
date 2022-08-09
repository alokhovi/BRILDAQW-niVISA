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

  //scope.resetScope();
  //scope.baseConfig(scopeCfg.globalParams, scopeCfg.channelConfigurationParameters);
  //scope.write(const_cast<ViString>("HORizontal:POSition 5"));

  /*using std::chrono::high_resolution_clock;
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
  }*/

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

  scope.write(const_cast<ViString>("WFMOUTPRE:BYT_NR 1"));
  scope.write(const_cast<ViString>("DATA:SOURCE CH7"));
  data = scope.query(const_cast<ViString>("WFMOUTPRE?"));
  std::cout << data.second << std::endl;
  scope.ReadWaveform();
  
  scope.write(const_cast<ViString>("ACQUIRE:STOPAFTER RUNSTOP"));
  scope.write(const_cast<ViString>("ACQUIRE:STATE ON"));

  scope.disconnect();

  //scope.stopProfiler("devtest");

  return 0;
}


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