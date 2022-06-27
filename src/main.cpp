#include <iostream>
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

  scope.enableProfiling();

  scope.startProfiler("devtest"); 

  
  //brildaq::nivisa::Status status = scope.connect(ViString("TCPIP::10.176.62.25::4000::SOCKET"),800 /* ms */);

  auto & cc = scopeCfg.connectionParameters;

  brildaq::nivisa::Status status = scope.connect(const_cast<ViString>(cc.connectionString.c_str()),cc.timeout,cc.exclusiveLock);

  if ( status.first != VI_SUCCESS )
  {
    std::cout << "Connection failed: " << status.second << std::endl; return -1;
  }

  /*
  brildaq::nivisa::Data data = scope.query( const_cast<ViString>("*IDN?") );


  std::cout << data.first << " - "  << data.second << std::endl;

  std::cout << data.second << std::endl;
  */

  //my code
  
  std::cout << "** Testing New Funtionality **" << std::endl;
  brildaq::nivisa::Data data;
  //brildaq::nivisa::Data data = scope.query( const_cast<ViString>("TRIGger?") );
  //std::cout << data.first << " bees "  << data.second << std::endl;
  
  std::cout << "------------ RESETTING SCOPE ------------" << std::endl;
  data = scope.query( const_cast<ViString>("*RST;:*OPC?") );
  std::cout << "DATA.FIRST --- " << data.first << std::endl;
  std::cout << "DATA.SECOND --- " << data.second << std::endl;

  std::cout << "\n\n------------ OPENING CH8 ------------" << std::endl;
  data = scope.query( const_cast<ViString>("DISPLAY:WAVEVIEW1:CH8:STATE 1;:*OPC?") );
  std::cout << "DATA.FIRST --- " << data.first << std::endl;
  std::cout << "DATA.SECOND --- " << data.second << std::endl;

  std::cout << "\n\n------------ SETTING CH8 SCALE ------------" << std::endl;
  data = scope.query( const_cast<ViString>("DISplay:WAVEView1:CH8:VERTical:SCAle 2.0;:*OPC?") );
  std::cout << "DATA.FIRST --- " << data.first << std::endl;
  std::cout << "DATA.SECOND --- " << data.second << std::endl;

  std::cout << "\n\n------------ SETTING EDGE TRIGGER ------------" << std::endl;
  data = scope.query( const_cast<ViString>(":TRIGger:A:TYPe EDGE;:*OPC?") );
  std::cout << "DATA.FIRST --- " << data.first << std::endl;
  std::cout << "DATA.SECOND --- " << data.second << std::endl;

  std::cout << "\n\n------------ SETTING EDGE SOURCE CH8 ------------" << std::endl;
  data = scope.query( const_cast<ViString>(":TRIGger:A:EDGE:SOUrce CH8;:*OPC?") );
  std::cout << "DATA.FIRST --- " << data.first << std::endl;
  std::cout << "DATA.SECOND --- " << data.second << std::endl;

  std::cout << "\n\n------------ SETTING TRIGGER THRESHOLD ------------" << std::endl;
  data = scope.query( const_cast<ViString>(":TRIGger:A:LOWerthreshold:CH8 1.2;:*OPC?") );
  std::cout << "DATA.FIRST --- " << data.first << std::endl;
  std::cout << "DATA.SECOND --- " << data.second << std::endl;
  
  std::cout << "\n\n------------ SETTING EDGE TYPE ------------" << std::endl;
  data = scope.query( const_cast<ViString>(":TRIGger:A:EDGE:SLOpe FALL;:*OPC?") );
  std::cout << "DATA.FIRST --- " << data.first << std::endl;
  std::cout << "DATA.SECOND --- " << data.second << std::endl;

  std::cout << "\n\n------------ REQUESTING TRIGGER STATE ------------" << std::endl;
  data = scope.query( const_cast<ViString>(":TRIGger:STATE?;:*OPC?") );
  std::cout << "DATA.FIRST --- " << data.first << std::endl;
  std::cout << "DATA.SECOND --- " << data.second << std::endl;
  std::cout << "**Finished**" << std::endl;
  /*
  status = scope.write( const_cast<ViString>("SAVE:IMAGe \"C:/Temp.png\"") );
  data = scope.query( const_cast<ViString>("*OPC?") );
  status = scope.write( const_cast<ViString>("FILESystem:READFile \"C:/Temp.png\"") );
  */
  //scope.query(const_cast<ViString>("*OPC?" ));

  //scope.Dir();

  scope.disconnect();

  scope.stopProfiler("devtest");

  return 0;
}
