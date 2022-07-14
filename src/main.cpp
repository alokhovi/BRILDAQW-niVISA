#include <iostream>
#include <string>
#include <bitset>
//#include <boost/dynamic_bitset.hpp>

#include "tekscope.hpp"
#include "tekscopecfg.hpp"
/*
int to_int(boost::dynamic_bitset<> bnNum){//convert binary numbery to signed int
  int len = bnNum.size();
  int num = -(int)bnNum[len-1] * std::pow(2,len-1);
  for(int i = 0; i<(len-1); i++){
    num += std::pow(2,i)*(int)bnNum[i];
  }
  return num;
}*/

int main()
{
  brildaq::nivisa::TekScope    scope;

  brildaq::nivisa::TekScopeCfg scopeCfg;   

  // Setting up configuration parameters (to be implemented later via XDAQ interface)

  // scopeCfg.connectionParameters.connectionString = "TCPIP::127.0.0.1::4000::SOCKET";
  scopeCfg.connectionParameters.connectionString = "TCPIP::10.176.62.25::4000::SOCKET";
  scopeCfg.connectionParameters.timeout          = 5000;
  scopeCfg.connectionParameters.exclusiveLock    = true;

  scopeCfg.channelConfigurationParameters[1].ONOFF = true;//turn on channel 2
  scopeCfg.channelConfigurationParameters[3].ONOFF = true;//turn on channel 4
  scopeCfg.channelConfigurationParameters[7].ONOFF = true;//turn on channel 8
  scopeCfg.channelConfigurationParameters[1].VSCALE = "0.25";//set ch2 vertical scale
  scopeCfg.channelConfigurationParameters[7].VSCALE = "0.25";//set ch8 vertical scale
  scopeCfg.channelConfigurationParameters[3].VSCALE = "0.5";//set ch8 vertical scale

  
  scopeCfg.globalParams.TSCALE = "100e-9";
  scopeCfg.globalParams.TRIGSOURCE[0] = "4";
  scopeCfg.globalParams.TRIGSOURCE[1] = "-0.640";

  //scopeCfg.globalParams.TRIGSOURCE[0] = "8"; //trigger on channel 8 (which is actually the default value)
  //scopeCfg.globalParams.TRIGSOURCE[1] = "-0.440"; //set trigger level
  //std::cout << scopeCfg.globalParams.VSCALE << std::endl;
  //std::cout << scopeCfg.globalParams.TSCALE << std::endl;

  scope.enableProfiling();

  scope.startProfiler("devtest"); 
  //std::cout << scopeCfg.channelConfigurationParameters[0].name << std::endl;


  
  //brildaq::nivisa::Status status = scope.connect(ViString("TCPIP::10.176.62.25::4000::SOCKET"),800 );

  auto & cc = scopeCfg.connectionParameters;

  brildaq::nivisa::Status status = scope.connect(const_cast<ViString>(cc.connectionString.c_str()),cc.timeout,cc.exclusiveLock);

  if ( status.first != VI_SUCCESS )
  {
    std::cout << "Connection failed: " << status.second << std::endl; return -1;
  }

  brildaq::nivisa::Data data = scope.query( const_cast<ViString>("*IDN?") );
  std::cout << data.first << " - "  << data.second << std::endl;
  std::cout << data.second << std::endl;
  
  //scope.baseConfig(scopeCfg.globalParams, scopeCfg.channelConfigurationParameters);

  int time = 100 * std::pow(10,-6);
  int rate = std::pow(10,6);
  int samples = time * rate;
  std::string command;

  std::vector<std::string> res;
  res = scope.getMeasurementResults("3");
  
  for(int i=0;i<5;i++){
    std::cout << res[i] << std::endl;
  }
  

  /*scope.write(const_cast<ViString>("HEADER 0"));
  data = scope.query(const_cast<ViString>("MEASUrement:MEAS3:RESUlts:ALLAcqs:MEAN?"));
  std::cout << "Mean: " << data.second << std::endl;
  data = scope.query(const_cast<ViString>("MEASUrement:MEAS3:RESUlts:ALLAcqs:POPulation?"));
  std::cout << "Population: " << data.second << std::endl;
  data = scope.query(const_cast<ViString>("MEASUrement:MEAS3:RESUlts:ALLAcqs:STDDev?"));
  std::cout << "STD: " << data.second << std::endl;*/

  //scope.write(const_cast<ViString>(":HORIZONTAL:MODE MANUAL"));

  /*
  scope.write(const_cast<ViString>(":HORIZONTAL:MODE AUTOMATIC"));
  command = "HORizontal:SAMPLERate " + rate;
  std::cout << "rate : " << rate;
  scope.write(const_cast<ViString>("HORizontal:SAMPLERate 1e5"));//c//500e9
  data = scope.query(const_cast<ViString>("HORizontal:SAMPLERate?"));
  std::cout << "SAMPLE RATE: " << data.second << std::endl;
  scope.write(const_cast<ViString>("HORizontal:RECOrdlength 1000"));
  data = scope.query(const_cast<ViString>("HORizontal:RECOrdlength?"));
  std::cout << "RECORD LENGTH: " << data.second << std::endl;
  */
  
  //scope.binIn();
  /*
  std::string form = scope.getForm("4","2","1","1250");
  std::bitset<8> b1 = std::bitset<8>(form[6]);
  std::bitset<8> b2 = std::bitset<8>(form[7]);
  //std::bitset<16> num = std::bitset<16>(b1.to_string() + b2.to_string());
  boost::dynamic_bitset<> num = boost::dynamic_bitset<>(b1.to_string() + b2.to_string());
  std::cout << b1 << " : " << b2 << std::endl;
  //boost::dynamic_bitset<> num = boost::dynamic_bitset<>(b1.to_string());
  std::cout << num << std::endl;
  std::cout << "toInt : " << to_int(num) << std::endl;
  std::cout << "BintoInt : " << binaryToInteger(num) << std::endl;*/
  
  
  /*
  std::string form = scope.getForm("1","2","1","5e7");
  std::bitset<8> b1;
  std::bitset<8> b2;
  boost::dynamic_bitset<> num;
  for(int i = 6; i<1000; i+=2){
    b1 = std::bitset<8>(form[i]);
    b2 = std::bitset<8>(form[i+1]);
    num = boost::dynamic_bitset<>(b1.to_string() + b2.to_string());
    //std::cout << "bin : " << num << " || dec : " << binaryToInteger(num) << " || index : " << i << std::endl;
    std::cout << binaryToInteger(num) << std::endl;
  }
  */

  //data = scope.query(const_cast<ViString>("ACQuire:MAXSamplerate?"));
  //scope.query(const_cast<ViString>("*OPC?"));
  //std::cout << "MAX: " << data.second << std::endl;
  
  //scope.resetScope();

  //scope.Dir();

  scope.disconnect();

  scope.stopProfiler("devtest");

  return 0;
}