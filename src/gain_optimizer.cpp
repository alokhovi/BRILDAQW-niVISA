#include <iostream>
#include <fstream>
#include <numeric>
#include <cstring>
#include <string>
#include <thread>
#include <bitset>
#include <sstream>
#include <cmath>
#include <vector>
#include <regex>
#include <stdlib.h>


#include "tekscope.hpp"
using namespace brildaq::nivisa;

 
//add by rnagasak                                                                 
std::string TekScope::getFormAscii(std::string channel, std::string byteNum, std::string start, std::string stop)
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

  this->write(const_cast<ViString>(":WFMOutpre:ENCdg ASCii")); //set Ascii encoding format                                                                       
  this->write(const_cast<ViString>(":HEADer 0")); //turn off headers            

 command = ":WFMOutpre:BYT_Nr " + byteNum;
 this->write(const_cast<ViString>(command.c_str()));

 data = this->query(":Curve?");
 //data = this->query(":WAVfrm??");                                            

 std::string form = data.second;
 this->write(const_cast<ViString>(":HEADer 1")); //turn headers back on        
 return form;
}

void TekScope::gain_optimizer(double occupancy)
{
  brildaq::nivisa::Status status;
  brildaq::nivisa::Data   data;
  std::string command;
  std::string form;
  int channel;

  double vertical_scale;
  double offset;
  double position;
  std::string vertical_scale_str;
  std::string offset_str;
  std::string position_str;
  float yoffset;
  int n = 0;

  //std::cout << "==========" << std::endl;
  data = this->query(const_cast<ViString>(":WFMOutpre?"));
  //std::cout << data.second << std::endl;

  //std::cout << "=== initial waveform ===" << std::endl;
  std::map<int, std::vector<float>>   allChannels;

  this->query(const_cast<ViString>("*OPC?"));

  for(int j = 1; j <= brildaq::nivisa::NM_OF_TEKSCOPE_CHANNELS; j++){
    command = "Data:source CH" + std::to_string(j);
    //std::cout << "-------- " << command << " --------" << std::endl;
    this->write(const_cast<ViString>(command.c_str()));
      command = "CH" + std::to_string(j) + ":OFFSet?";
      data = this->query( const_cast<ViString>(command.c_str()));

      if (data.second == "1")
	{
	  n += 1;
	  std::cout << "n = " << n << std::endl;
	  std::cout << "===== " << j << " Ch =====" << std::endl;
	  std::cout << "offset string = " << data.second << std::endl;
	  command = "Data:source CH" + std::to_string(j);
	  //std::cout << "-------- " << command << " --------" << std::endl;
	  this->write(const_cast<ViString>(command.c_str()));
	  command = "CH" + std::to_string(j) + ":OFFSet?";
	  data = this->query( const_cast<ViString>(command.c_str()));
	  std::cout << "offset string = " << data.second << std::endl;
	  yoffset = std::stof(data.second);
	  std::cout << "offset float = " << yoffset << std::endl;
	}
      else
	{
	  yoffset = std::stof(data.second);
	}
      allChannels.insert(std::pair<int, std::vector<float>>(j,this->ReadWaveform((float)yoffset)));
    this->query(const_cast<ViString>("*OPC?"));
    //printf("\n");
  }

  // for (const auto& n : allChannels)
  //   {
  //     if (n.first == 5)
  // 	{
  //     std::cout << n.first << " = ";
  //     for (int i=0; i<n.second.size(); ++i)
  // 	{
  // 	  std::cout << n.second[i] << ",";
  // 	}
  //     std::cout << '\n';
  // 	}
  //   }
  // std::cout << '\n';

  //std::string v0;
 
  for (const auto& n :allChannels)
    {
      channel = n.first;
      //inital settings  
      //std::cout << "=== initial settings ===" << std::endl;

      //std::cout << "=== " << std::to_string(channel) << "Ch ===" << std::endl; 
      // command = "DISplay:WAVEView1:CH" + std::to_string(channel) + ":VERTical:SCAle?";
      // data = this->query( const_cast<ViString>(command.c_str()));
      // std::cout << data.second << std::endl;
      // v0 = data.second[0];
      // if (v0 == ":")
      // 	{
      // std::cout << data.second << std::endl;
  
      // for (int i=38; i<data.second.size(); ++i)
      //    {
      //      vertical_scale_str += data.second[i];
      //    }
      // 	}
      // else
      // 	{
      // vertical_scale_str = data.second;
      //}
      // vertical_scale = std::stod(vertical_scale_str);
      //std::cout << "==========" << std::endl;
      //std::cout << "vertical scale = " << vertical_scale << std::endl;
      //command = "CH" + std::to_string(channel) + ":OFFSet?";

      //data = this->query( const_cast<ViString>(command.c_str()));
      // for (int i=12; i<data.second.size(); ++i)
      //   {
      //     offset_str += data.second[i];
      //   }

      //offset_str = data.second;
      //offset = std::stod(offset_str);
      //std::cout << "==========" << std::endl;
      //std::cout << "offset = " << offset << std::endl;

      //command = "DISplay:WAVEView1:CH" + std::to_string(channel) + ":VERTical:POSition?";
      //data = this->query( const_cast<ViString>(command.c_str()));
      // for (int i=41; i<data.second.size(); ++i)
      //   {
      //     position_str += data.second[i];
      //   }

      //position_str = data.second; 
      //position = std::stod(position_str);
      //std::cout << "==========" << std::endl;
      //std::cout << "position = " << position << std::endl;

 
      // std::cout << "=== " << channel << " Ch ===" << std::endl; 
      // std::cout << "=== Max and Min ===" << std::endl;
      auto max = max_element(std::begin(n.second), std::end(n.second));
      auto min = min_element(std::begin(n.second), std::end(n.second));

      // std::cout << "Max = " << *max << std::endl;
      // std::cout << "Min = " << *min << std::endl;

      // std::cout << "=== new vertical scale ===" << std::endl;
      // double zoom_rate = occupancy*(10*vertical_scale/((double)*max-(double)*min));
      double vertical_scale = ((double)*max-(double)*min)/(10.0*occupancy);
      // std::cout << vertical_scale << std::endl;
      command = "DISplay:WAVEView1:CH" + std::to_string(channel) + ":VERTical:SCAle " + std::to_string(vertical_scale);
      this->write( const_cast<ViString>(command.c_str()));

      // std::cout << "=== new offset ===" << std::endl;
      offset = ((double)*max+(double)*min)/2;
      // std::cout << offset << std::endl;
      command = "CH" + std::to_string(channel) + ":OFFSet " + std::to_string(offset);
      this->write( const_cast<ViString>(command.c_str()));

      //std::cout << "=== new settings ===" << std::endl;
      //data = this->query(const_cast<ViString>(":WFMOutpre?"));
      //std::cout << data.second << std::endl;
    }  
}

std::vector<int> str_vec(std::string form)
{
  form = std::regex_replace(form, std::regex(","), " ");
  std::stringstream iss(form);

  int number;
  std::vector<int> vector;
  while (iss >> number)
    {
      vector.push_back(number);
    }

  return vector;
}

void TekScope::run_scope_calibration()
{
  std::string command;
  brildaq::nivisa::Status status;
  brildaq::nivisa::Data data;

  std::cout << "Running (builtin) scope calibration procedure" << std::endl;
  std::cout << "This takes 5 minutes per channel" << std::endl;

  command = "CALibrate:INTERNal";
  this->write(const_cast<ViString>(command.c_str()));

  //This should return zero if all went well.
  data = this->query(const_cast<ViString>("*CAL?"));

   if (data.second != "0")
    {
      std::cout << "Scope calibration procedure failed " << std::endl;
    }
  else
    {
      std::cout << "Done running scope calibration procedure" << std::endl;
    }
}
