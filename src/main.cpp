#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
//#include <boost/dynamic_bitset.hpp>

#include "tekscope.hpp"
#include "tekscopecfg.hpp"

/*void measureDelays()
{
  brildaq::nivisa::Data data;
  scope.write(const_cast<ViString>("HEADER 1"));
  //scope.write(const_cast<ViString>("MEASUrement:ADDMEAS DELAY"));
  data = scope.query(const_cast<ViString>("MEASUrement:MEAS1:SOUrce2?"));
  std::cout << data.second << std::endl;
  return;
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

  scopeCfg.channelConfigurationParameters[0].ONOFF = true;
  scopeCfg.channelConfigurationParameters[1].ONOFF = true;//turn on channel 2
  scopeCfg.channelConfigurationParameters[2].ONOFF = true;
  scopeCfg.channelConfigurationParameters[3].ONOFF = true;//turn on channel 4
  scopeCfg.channelConfigurationParameters[4].ONOFF = true;
  scopeCfg.channelConfigurationParameters[5].ONOFF = true;
  scopeCfg.channelConfigurationParameters[6].ONOFF = true;
  scopeCfg.channelConfigurationParameters[7].ONOFF = true;//turn on channel 8

  scopeCfg.channelConfigurationParameters[0].VSCALE = "0.5";
  scopeCfg.channelConfigurationParameters[1].VSCALE = "0.5";//set ch2 vertical scale
  scopeCfg.channelConfigurationParameters[2].VSCALE = "0.5";
  scopeCfg.channelConfigurationParameters[3].VSCALE = "0.5";
  scopeCfg.channelConfigurationParameters[4].VSCALE = "0.5";
  scopeCfg.channelConfigurationParameters[5].VSCALE = "0.5";
  scopeCfg.channelConfigurationParameters[6].VSCALE = "0.5";
  scopeCfg.channelConfigurationParameters[7].VSCALE = "0.25";//set ch8 vertical scale

  
  scopeCfg.globalParams.TSCALE = "100e-9";
  scopeCfg.globalParams.TRIGSOURCE[0] = "4";
  scopeCfg.globalParams.TRIGSOURCE[1] = "-0.640";

  scopeCfg.globalParams.TRIGSOURCE[0] = "8"; //trigger on channel 8 (which is actually the default value)
  scopeCfg.globalParams.TRIGSOURCE[1] = "-0.440"; //set trigger level

  scope.enableProfiling();

  //scope.startProfiler("devtest"); 


  
  //brildaq::nivisa::Status status = scope.connect(ViString("TCPIP::10.176.62.25::4000::SOCKET"),800 );

  auto & cc = scopeCfg.connectionParameters;

  brildaq::nivisa::Status status = scope.connect(const_cast<ViString>(cc.connectionString.c_str()),cc.timeout,cc.exclusiveLock);

  if ( status.first != VI_SUCCESS )
  {
    std::cout << "Connection failed: " << status.second << std::endl; return -1;
  }

  brildaq::nivisa::Data data = scope.query( const_cast<ViString>("*IDN?") );
  //std::cout << data.first << " - "  << data.second << std::endl;
  std::cout << data.second << std::endl;
  
  //scope.baseConfig(scopeCfg.globalParams, scopeCfg.channelConfigurationParameters);

  

  /*int time = 100 * std::pow(10,-6);
  int rate = std::pow(10,6);
  int samples = time * rate;
  
  scope.write(const_cast<ViString>(":HORIZONTAL:MODE AUTOMATIC"));
  command = "HORizontal:SAMPLERate " + rate;
  std::cout << "rate : " << rate;
  scope.write(const_cast<ViString>("HORizontal:SAMPLERate 1e5"));//c//500e9
  data = scope.query(const_cast<ViString>("HORizontal:SAMPLERate?"));
  std::cout << "SAMPLE RATE: " << data.second << std::endl;
  scope.write(const_cast<ViString>("HORizontal:RECOrdlength 1000"));
  data = scope.query(const_cast<ViString>("HORizontal:RECOrdlength?"));
  std::cout << "RECORD LENGTH: " << data.second << std::endl;*/

  /*
  std::vector<std::string> res;
  res = scope.getMeasurementResults("1");

  std::cout << "MEAN : " << res[0] << std::endl;
  std::cout << "STD : " << res[0] << std::endl;
  std::cout << "MAX : " << res[0] << std::endl;
  std::cout << "MIN : " << res[0] << std::endl;
  std::cout << "POP : " << res[0] << std::endl;

  for(int i=0;i<5;i++){
    std::cout << res[i] << std::endl;
  }
  */
  /*
  data = scope.query(const_cast<ViString>(":HORIZONTAL:MODE?"));
  std::cout << data.second << std::endl;
  data = scope.query(const_cast<ViString>("HORizontal:SAMPLERate?"));
  std::cout << data.second << std::endl;
  data = scope.query(const_cast<ViString>("HORizontal:RECOrdlength?"));
  std::cout << data.second << std::endl;
  data = scope.query(const_cast<ViString>("WFMOutpre:BN_Fmt?"));
  std::cout << data.second << std::endl;
  */
  /*
  scope.write(const_cast<ViString>("HEADER 1"));
  data = scope.query(const_cast<ViString>("DISplay:WAVEView1:CH4:VERTical:SCAle?"));
  std::cout << data.second << std::endl;
  std::size_t pos = data.second.find(" "); //look for space
  std::string scale = data.second.substr(pos); //return scale
  double scaleVal = std::stod(scale); //convert scale to double
  
  //for(int j = 0; j < 10; j++){
  //std::ofstream outfile("dataRuns/run" + std::to_string(j) + ".txt");

  
  std::string form = scope.getForm("4","2","1","1250");
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

  /* ASCII Readout
  status = scope.write(const_cast<ViString>(":Data:Source CH4"));
  if(status.first < VI_SUCCESS){
    std::cout << "Data source error" << std::endl;
  }
  else {
    std::cout << "Data source connection success!" << std::endl;
  }

  status = scope.write(const_cast<ViString>(":Data:Start 1"));
  if(status.first < VI_SUCCESS){
    std::cout << "Data start error" << std::endl;
  }
  else {
    std::cout << "Data start success!" << std::endl;
  }

  status = scope.write(const_cast<ViString>(":Data:Stop 1250"));
  if(status.first < VI_SUCCESS){
    std::cout << "Data stop error" << std::endl;
  }
  else {
    std::cout << "Data stop success!" << std::endl;
  }

  status = scope.write(const_cast<ViString>(":WFMOutpre:ENCdg Ascii"));
  if(status.first < VI_SUCCESS){
    std::cout << "Encoding error" << std::endl;
  }
  else {
    std::cout << "Encoding success!" << std::endl;
  }
  
  
  data = scope.query(const_cast<ViString>(":WFMOutpre?"));
  if(data.first < VI_SUCCESS){
    std::cout << "Outpre error" << std::endl;
  }
  else{
    std::cout << data.second << std::endl;
  }
  

  status = scope.write(const_cast<ViString>("acquire:state 0"));
  if(status.first < VI_SUCCESS){
    std::cout << "state 0 error" << std::endl;
  }
  else {
    std::cout << "state 0 success!" << std::endl;
  }

  status = scope.write(const_cast<ViString>("acquire:stopafter sequence"));
  if(status.first < VI_SUCCESS){
    std::cout << "stopafter error" << std::endl;
  }
  else {
    std::cout << "stopafter success!" << std::endl;
  }

  status = scope.write(const_cast<ViString>("acquire:state 1"));
  if(status.first < VI_SUCCESS){
    std::cout << "state 1 error" << std::endl;
  }
  else {
    std::cout << "state 1 success!" << std::endl;
  }

  data = scope.query(const_cast<ViString>("*OPC?"));
  if(status.first < VI_SUCCESS){
    std::cout << "OPC error" << std::endl;
  }
  else {
    std::cout << "OPC success!" << std::endl;
  }

  data = scope.query(const_cast<ViString>(":CURVE?"));
  if(status.first < VI_SUCCESS){
    std::cout << "curve error" << std::endl;
  }
  else {
    std::cout << "curve success!" << std::endl;
  }
  std::cout << data.second << std::endl;
  */
  

  /*
  status = scope.write(const_cast<ViString>(":Data:Source CH4"));
  if(status.first < VI_SUCCESS){
    std::cout << "Data source error" << std::endl;
  }
  else {
    std::cout << "Data source connection success!" << std::endl;
  }

  status = scope.write(const_cast<ViString>(":Data:Start 1"));
  if(status.first < VI_SUCCESS){
    std::cout << "Data start error" << std::endl;
  }
  else {
    std::cout << "Data start success!" << std::endl;
  }

  status = scope.write(const_cast<ViString>(":Data:Stop 1250"));
  if(status.first < VI_SUCCESS){
    std::cout << "Data stop error" << std::endl;
  }
  else {
    std::cout << "Data stop success!" << std::endl;
  }

  status = scope.write(const_cast<ViString>(":WFMOutpre:ENCdg BINARY"));
  if(status.first < VI_SUCCESS){
    std::cout << "Encoding error" << std::endl;
  }
  else {
    std::cout << "Encoding success!" << std::endl;
  }
  
  status = scope.write(const_cast<ViString>(":Header 0"));
  if(status.first < VI_SUCCESS){
    std::cout << "Header error" << std::endl;
  }
  else{
    std::cout << "Header success!" << std::endl;
  }
  
  status = scope.write(const_cast<ViString>("acquire:state 0"));
  if(status.first < VI_SUCCESS){
    std::cout << "state 0 error" << std::endl;
  }
  else {
    std::cout << "state 0 success!" << std::endl;
  }

  status = scope.write(const_cast<ViString>("acquire:stopafter sequence"));
  if(status.first < VI_SUCCESS){
    std::cout << "stopafter error" << std::endl;
  }
  else {
    std::cout << "stopafter success!" << std::endl;
  }

  status = scope.write(const_cast<ViString>("acquire:state 1"));
  if(status.first < VI_SUCCESS){
    std::cout << "state 1 error" << std::endl;
  }
  else {
    std::cout << "state 1 success!" << std::endl;
  }

  data = scope.query(const_cast<ViString>("*OPC?"));
  if(status.first < VI_SUCCESS){
    std::cout << "OPC error" << std::endl;
  }
  else {
    std::cout << "OPC success!" << std::endl;
  }

  data = scope.query(const_cast<ViString>(":CURVE?"));
  if(status.first < VI_SUCCESS){
    std::cout << "curve error" << std::endl;
  }
  else {
    std::cout << "curve success!" << std::endl;
  }

  std::string form = data.second;
  std::bitset<8> b1;
  std::bitset<8> b2;
  boost::dynamic_bitset<> num;

  for(int i = 6; i<20; i+=2){
    b1 = std::bitset<8>(form[i]);
    b2 = std::bitset<8>(form[i+1]);
    num = boost::dynamic_bitset<>(b1.to_string() + b2.to_string());
    std::cout << "bin : " << num << " || dec : " << binaryToInteger(num) << " || index : " << i << std::endl;
    int val = binaryToInteger(num);
    std::cout << val * 0.5*5/32767 << " V" << std::endl;
  }
  */


  //data = scope.query(const_cast<ViString>(":Data:Start"));
  //data = scope.query(const_cast<ViString>(":Data:Source CH4"));
  
  //scope.resetScope();
  //scope.query("*OPC?");
  
  /*
  data = scope.query(const_cast<ViString>("WFMOutpre?"));
  std::cout << data.second << std::endl;
  */
  
  //data = scope.query(const_cast<ViString>("MEASU:MEAS1:SUBGROUP:RESUlts:CURRentacq:MEAN?"));
  //std::cout << data.second << std::endl;
  scope.asciiWaveformReadout("4");
  scope.disconnect();

  //scope.stopProfiler("devtest");

  return 0;
}