#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include <iterator>
#include <chrono>
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
  std::cout << data.second << std::endl;
  
  //scope.baseConfig(scopeCfg.globalParams, scopeCfg.channelConfigurationParameters);

  //BELOW THIS DELETE

  /*
  float scaleVal = 0.5;
  std::string form = scope.getForm("4","1","1","1250");
  double pnt;
  for(int i = 6; i < 1256; i++){
    pnt = (((double) form[i]) - 0) * 0.04;
    printf("%ld",pnt);
  }
  */

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
  //scope.channelState("4","1");

  std::string command;
  std::string channel = "4";
/*
  command = "SELECT:CH" + channel + " ON"; //turn on channel
  scope.write(const_cast<ViString>(command.c_str()));*/

  command = ":DATa:SOUrce CH" + channel; //turn on channel
  scope.write(const_cast<ViString>(command.c_str()));

  //status = scope.write(const_cast<ViString>(":Data:Source CH3"));
  //std::cout << data.second << std::endl;

  using std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  using std::chrono::duration;
  using std::chrono::milliseconds;

  status = scope.write(const_cast<ViString>("acquire:state 0"));
  status = scope.write(const_cast<ViString>("acquire:stopafter sequence"));
  status = scope.write(const_cast<ViString>("acquire:state 1"));
  std::vector<float> f;
  duration<double, std::milli> ms_double;
  auto t1 = high_resolution_clock::now();
  auto t2 = high_resolution_clock::now();

  std::ofstream ASCIIoutfile("aqTimeData/ASCIIaqTimes.txt");
  std::ofstream Binaryoutfile("aqTimeData/BinaryaqTimes.txt");

  int end = 4500;
  /*
  for(int i = 0; i < end; i++)
  {
    if(((i+1) % 10) == 0) std::cout << std::to_string((double)(i+1)*100/end) << "%" << std::endl;
    
    t1 = high_resolution_clock::now();
    //std::map<int, std::vector<float>> forms = scope.readWaveformBinary();
    f = scope.ReadWaveform();
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;
    Binaryoutfile << ms_double.count() << std::endl;
    //std::cout << "Binary Waveform Aqcuisiton Time : " << ms_double.count() << "ms\n";

    t1 = high_resolution_clock::now();
    f = scope.asciiWaveformReadout("4");
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;
    ASCIIoutfile << ms_double.count() << std::endl;

    //ms_double = t2 - t1;
    //std::cout << "ASCII Waveform Aqcuisiton Time : " << ms_double.count() << "ms\n";
  }*/

  //printf("Hi");


  /*
  for(std::size_t i; i < f.size(); i++){
    std::cout << std::to_string(f[i]) << std::endl;
  }*/


/*
  std::vector<float> form;
  for(int i = 1; i <= brildaq::nivisa::NM_OF_TEKSCOPE_CHANNELS; i++){
    std::ofstream outfile("channelData/CH" + std::to_string(i) + ".txt");
    form = forms[i];
    for(std::size_t j = 0; j < form.size(); j++){
      outfile << std::to_string(form[j]) << std::endl;
    }
  }
*/
  
  scope.write(const_cast<ViString>("ACQUIRE:STOPAFTER RUNSTOP"));
  scope.write(const_cast<ViString>("ACQUIRE:STATE ON"));

  std::vector<std::string> results = scope.getMeasurementResults("1");
  for(std::size_t i = 0; i < results.size(); i++){
    std::cout << results[i] << std::endl;
  }


  //SAVE THIS
  scope.disconnect();

  //scope.stopProfiler("devtest");

  return 0;
}



/*
double* ReadWaveform(ViSession vi, long* elements) noexcept
{
    ViStatus status;
    float yoffset, ymult;
    ViChar buffer[256];
    ViChar c;
    long count, i;
    double* ptr = NULL;

    assert(elements != NULL);

    status = viSetAttribute(vi, VI_ATTR_WR_BUF_OPER_MODE,VI_FLUSH_ON_ACCESS);
    status = viSetAttribute(vi, VI_ATTR_RD_BUF_OPER_MODE,VI_FLUSH_ON_ACCESS);

    // Turn headers off, this makes parsing easier
    status = viPrintf(vi, "header off\n");
    if (status < VI_SUCCESS) goto error;

    // Get record length value
    status = viQueryf(vi, "hor:reco?\n", "%ld", elements);
    if (status < VI_SUCCESS) goto error;

    // Make sure start, stop values for curve query match the
    // full record length
    status = viPrintf(vi, "data:start %d;data:stop %d\n", 0,(*elements)-1);
    if (status < VI_SUCCESS) goto error;

    // Get the yoffset to help calculate the vertical values.
    status = viQueryf(vi, "WFMOUTPRE:YOFF?\n", "%f", &yoffset);
    if (status < VI_SUCCESS) goto error;

    // Get the ymult to help calculate the vertical values.
    status = viQueryf(vi, "WFMOutpre:YMULT?\n", "%f", &ymult);
    if (status < VI_SUCCESS) goto error;

    // Request 8-bit binary data on the curve query
    status = viPrintf(vi, "DATA:ENCDG RIBINARY;WIDTH 1\n");
    if (status < VI_SUCCESS) goto error;

    // Request the curve
    status = viPrintf(vi, "CURVE?\n");
    if (status < VI_SUCCESS) goto error;

    // Always flush if a viScanf follows a viPrintf or
    // viBufWrite.
    status = viFlush(vi, VI_WRITE_BUF | VI_READ_BUF_DISCARD);
    if (status < VI_SUCCESS) goto error;

    // Get first char and validate
    status = viScanf(vi, "%c", &c);
    if (status < VI_SUCCESS) goto error;
    assert(c == '#');

    // Get width of element field.
    status = viScanf(vi, "%c", &c);
    if (status < VI_SUCCESS) goto error;
    assert(c >= '0' && c <= '9');

    // Read element characters
    count = c - '0';
    for (i = 0; i < count; i++) {
        status = viScanf(vi, "%c", &c);
        if (status < VI_SUCCESS) goto error;
        assert(c >= '0' && c <= '9');
    }

    // Read waveform into allocated storage
    ptr = (double*) malloc(*elements*sizeof(double));
    for (i = 0; i < *elements; i++) {
        status = viScanf(vi, "%c", &c);
        if (status < VI_SUCCESS) goto error;
        ptr[i] = (((double) c) - yoffset) * ymult;
    }

    status = viFlush(vi, VI_WRITE_BUF | VI_READ_BUF_DISCARD);
    if (status < VI_SUCCESS) goto error;

    return ptr;

error:
    // Report error and clean up
    viStatusDesc(vi, status, buffer);
    fprintf(stderr, "failure: %s\n", buffer);
    if (ptr != NULL) free(ptr);
    return NULL;
}

// This program reads a waveform from a Tektronix
// TDS scope and writes the floating point values to
// stdout.
int main(int argc, char* argv[]){
  ViSession        rm = VI_NULL, vi = VI_NULL;
  ViStatus status;
  ViChar           buffer[256];
  double*          wfm = NULL;
  long             elements, i;

  // Open a default session
  status = viOpenDefaultRM(&rm);
  if (status < VI_SUCCESS) goto error;

  // Open the GPIB device at primary address 1, GPIB board 8
  status = viOpen(rm, "GPIB8::1::INSTR", VI_NULL, VI_NULL,&vi);
  if (status < VI_SUCCESS) goto error;

  // Read waveform and write it to stdout
  wfm = ReadWaveform(vi, &elements);
  if (wfm != NULL) {
    for (i = 0; i < elements; i++) {
      printf("%f\n", wfm[i]);
    }
  }

  // Clean up
  if (wfm != NULL) free(wfm);
  viClose(vi); // Not needed, but makes things a bit more understandable
  viClose(rm);

  return 0;

error:
  // Report error and clean up
  viStatusDesc(vi, status, buffer);
  fprintf(stderr, "failure: %s\n", buffer);
  if (rm != VI_NULL) viClose(rm);
  if (wfm != NULL) free(wfm);
  return 1;
}*/