#include <iostream>
#include <cstring>

#include <chrono>
#include <thread>
#include <bitset>

#include "interface.hpp"
#include "tekscope.hpp"

using namespace brildaq::nivisa;

Interface::Interface() : _buffer(new char [MAX_FORMATTED_BUFFER_SIZE])
{

}

int binaryToInteger(boost::dynamic_bitset<> bnNum)//convert binary numbery to signed int via Two's Complement
{
  int len = bnNum.size();
  int num = -(int)bnNum[len-1] * std::pow(2,len-1);
  for(int i = 0; i<(len-1); i++){
    num += std::pow(2,i)*(int)bnNum[i];
  }
  return num;
}

Status Interface::connect(const ViString & resource, ViAttrState timeout, bool exclusiveLock) noexcept
{
    assert( !_isConnected );
    
    ViStatus status= viOpenDefaultRM(&_defaultResourceManager);

    if (status < VI_SUCCESS) 
    {
		return std::make_pair(status,std::string("Cannot open default NI-VISA resource manager"));
    }

    ViAccessMode access = VI_NULL; if ( exclusiveLock ) access = VI_EXCLUSIVE_LOCK;

    status = viOpen(_defaultResourceManager, resource, access, VI_NULL, &_instrumentSession);

    if (status == VI_ERROR_TMO)
    {
        return std::make_pair(status,std::string("Timeout to open the resource: ")+resource);
    }
    if (status <  VI_SUCCESS) 
    {
		return std::make_pair(status,std::string("Cannot open the resource: ")+resource);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    assert (viSetAttribute (_instrumentSession, VI_ATTR_TMO_VALUE,timeout)   ==  VI_SUCCESS);

    _buffer[0] = VI_NULL;

	status = viGetAttribute(_instrumentSession, VI_ATTR_RSRC_NAME, _buffer.get());

	if (0 == strcmp(&_buffer[strlen(_buffer.get()) - strlen("SOCKET")], "SOCKET")) 
    {
        _isSocket = true;

    	viSetAttribute(_instrumentSession, VI_ATTR_TERMCHAR,    LINEFEED_CHAR);
        viSetAttribute(_instrumentSession, VI_ATTR_TERMCHAR_EN, VI_TRUE      );
	}

    viSetAttribute(_instrumentSession, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);

    _isConnected = true; return std::make_pair(VI_SUCCESS,boost::none);
}

void Interface::disconnect()  noexcept
{
    if (_defaultResourceManager > 0 ) viClose(_defaultResourceManager);

    _instrumentSession = 0; _defaultResourceManager = 0;

    _isConnected = false;
}

Data Interface::query(const ViString & command)  noexcept
{
    assert(_isConnected);     

    assert(command[strlen(command)-1] == '?'); // not a query command

    ViStatus status; ViUInt32 read;

    std::string lc(command);

    if ( _isSocket )
    {
        lc += LINEFEED_CHAR;
    }

    for( uint16_t nTry=0; nTry < MAX_NUMBER_OF_QUERY_TRIES; nTry++ )
    {
        _buffer[0] = VI_NULL;

        status = viWrite(_instrumentSession, (ViBuf)lc.c_str(), lc.length(), &read); 

        if (status <  VI_SUCCESS) 
        {
            if ( nTry == MAX_NUMBER_OF_QUERY_TRIES-1 )
            {
                return std::make_pair(status,std::string("Writing the following query command has failed: ") + command);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        else
        {
            assert(lc.length() == read); break;
        }
    }

    for( uint16_t nTry=0; nTry < MAX_NUMBER_OF_QUERY_TRIES; nTry++ )
    {
        _buffer[0] = VI_NULL;

        status = viRead(_instrumentSession,(ViBuf)_buffer.get(), MAX_FORMATTED_BUFFER_SIZE, &read);

        assert(read < MAX_FORMATTED_BUFFER_SIZE);

        if (status <  VI_SUCCESS) 
        {
            if ( nTry == MAX_NUMBER_OF_QUERY_TRIES-1 )
            {
                viStatusDesc(_instrumentSession, status, _buffer.get());

                return std::make_pair(status,std::string("Reading has failed: ")+_buffer.get());
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        else
        {
            //
            // in case of SOCKET, the status will be VI_SUCCESS_TERM_CHAR
            //
            auto lf=strchr(_buffer.get(),LINEFEED_CHAR);  if (lf) lf[0] = 0;

            return        std::make_pair(status,std::string(_buffer.get()));
        }
    }
    disconnect(); return std::make_pair(VI_ERROR_CONN_LOST,"") ; 
}

Status Interface::write(const ViString & command)  noexcept
{
    assert(_isConnected);     

    ViStatus status; ViUInt32 read;

    std::string lc(command);

    if ( _isSocket )
    {
        lc += LINEFEED_CHAR;
    }

    for( uint16_t nTry=0; nTry < MAX_NUMBER_OF_QUERY_TRIES; nTry++ )
    {
        _buffer[0] = VI_NULL;

        status = viWrite(_instrumentSession, (ViBuf)lc.c_str(), lc.length(), &read); 

        if (status <  VI_SUCCESS) 
        {
            if ( nTry == MAX_NUMBER_OF_QUERY_TRIES-1 )
            {
                return std::make_pair(status,std::string("Writing the following query command has failed: ") + command);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        else
        {
            assert(lc.length() == read); return std::make_pair(VI_SUCCESS,boost::none);
        }
    }
    disconnect(); return std::make_pair(VI_ERROR_CONN_LOST,boost::none) ; 
}

/*std::vector<float> Interface::ReadWaveform() noexcept
{
    assert(_isConnected);     

    ViStatus            status; 
    //ViUInt32            read;
    float               yoffset, ymult;
    ViChar              buffer[256];
    //ViChar              c;
    //long                count, i;
    std::vector<float>  form;
    int                 recordLen;


    status = viSetAttribute(_instrumentSession, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);
    status = viSetAttribute(_instrumentSession, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);

    status = viSetBuf(_instrumentSession,VI_READ_BUF, 256);
    if (status < VI_SUCCESS) goto error;
    
    //turn off headers
    status = viPrintf(_instrumentSession, "header off\n");
    if (status < VI_SUCCESS) goto error;

    // Get record length value
    status = viQueryf(_instrumentSession, "HORizontal:RECOrdlength?\n", "%u", &recordLen);
    //printf("%u\n",recordLen);
    if (status < VI_SUCCESS) goto error;

    //set aqcuisition start and end
    status = viPrintf(_instrumentSession, "data:start %d;data:stop %d\n", 0,recordLen);
    if (status < VI_SUCCESS) goto error;

    // Get the yoffset to help calculate the vertical values.
    status = viQueryf(_instrumentSession, "WFMOUTPRE:YOFF?\n", "%f", &yoffset);
    //printf("%f\n",yoffset);
    if (status < VI_SUCCESS) goto error;

    // Get the ymult to help calculate the vertical values.
    status = viQueryf(_instrumentSession, "WFMOutpre:YMULT?\n", "%f", &ymult);//might have a rounding issue here
    //printf("%f\n",ymult);
    if (status < VI_SUCCESS) goto error;


    // Request 8-bit binary data on the curve query
    status = viPrintf(_instrumentSession, "DATA:ENCDG RIBINARY;WIDTH 1\n");
    if (status < VI_SUCCESS) goto error;

    //std::cout << std::to_string(sizeof(_buffer)) << std::endl;

    // Request the curve
    status = viPrintf(_instrumentSession, "CURVE?\n");
    if (status < VI_SUCCESS) goto error;

    //flush before the scanf
    status = viFlush(_instrumentSession, VI_WRITE_BUF | VI_READ_BUF_DISCARD);
    if (status < VI_SUCCESS) goto error;

    status = viBufRead(_instrumentSession,(ViBuf) _buffer.get(), MAX_FORMATTED_BUFFER_SIZE, VI_NULL);
    if (status < VI_SUCCESS) goto error;

    double pnt;
    for(int i = 6; i < recordLen + 6; i++){
        form.push_back((((double) _buffer[i]) - yoffset) * ymult);
    }

    status = viFlush(_instrumentSession, VI_WRITE_BUF | VI_READ_BUF_DISCARD);
    if (status < VI_SUCCESS) goto error;

    return form;

error:
    // Report error and clean up
    viStatusDesc(_instrumentSession, status, buffer);
    fprintf(stderr, "Failure: %s\n", buffer);
    return form;
}*/


std::vector<float> Interface::ReadWaveform() noexcept
{
    assert(_isConnected);     

    ViStatus            status;
    float               yoffset, ymult;
    int                 byt_nr;
    ViChar              buffer[256];
    char                waveformBuffer[2000];
    std::vector<float>  form;
    int                 dataIndex, recordLen;
    std::size_t         offset;
    char                buf[2000][2];

    brildaq::nivisa::Data data;


    status = viSetAttribute(_instrumentSession, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);
    status = viSetAttribute(_instrumentSession, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);

    status = viSetBuf(_instrumentSession,VI_READ_BUF, 256);
    if (status < VI_SUCCESS) goto error;
    
    //turn off headers
    status = viPrintf(_instrumentSession, "header off\n");
    if (status < VI_SUCCESS) goto error;

    // Get record length value
    status = viQueryf(_instrumentSession, "HORizontal:RECOrdlength?\n", "%u", &recordLen);
    //printf("RecordLen: %u\n",recordLen);
    if (status < VI_SUCCESS) goto error;

    // Get the yoffset to help calculate the vertical values.
    status = viQueryf(_instrumentSession, "WFMOUTPRE:YOFF?\n", "%f", &yoffset);
    //printf("YOFF: %f\n",yoffset);
    if (status < VI_SUCCESS) goto error;

    
    // Get the byte number of acquisition
    status = viQueryf(_instrumentSession, "WFMOutpre:BYT_NR?\n", "%d", &byt_nr);
    //printf("BYT_NR: %d\n",byt_nr);
    if (status < VI_SUCCESS) goto error;

    // Request n-bit binary data on the curve query
    status = viPrintf(_instrumentSession, "DATA:ENCDG RIBINARY;:WIDTH %d\n", byt_nr);
    if (status < VI_SUCCESS) goto error;

    //status = viPrintf(_instrumentSession, "DATA:ENCDG RIBINARY;");
    //if (status < VI_SUCCESS) goto error;

    // Get the ymult to help calculate the vertical values.
    status = viQueryf(_instrumentSession, "WFMOutpre:YMULT?\n", "%f", &ymult);
    //printf("YMULT: %f\n",ymult);
    if (status < VI_SUCCESS) goto error;

    //set aqcuisition start and end
    status = viPrintf(_instrumentSession, "data:start %d;:data:stop %d\n", 0, recordLen);
    if (status < VI_SUCCESS) goto error;

    //this->write(const_cast<ViString>("Header On"));
    //data = this->query(const_cast<ViString>(":WFMOutpre?"));
    //std::cout << data.second << std::endl;
    //this->write(const_cast<ViString>("Header Off"));

    // Request the curve
    status = viPrintf(_instrumentSession, "CURVE?\n");
    if (status < VI_SUCCESS) goto error;

    //flush before the scanf
    status = viFlush(_instrumentSession, VI_WRITE_BUF | VI_READ_BUF_DISCARD);
    if (status < VI_SUCCESS) goto error;

    status = viBufRead(_instrumentSession, (ViBuf) _buffer.get(), MAX_FORMATTED_BUFFER_SIZE, VI_NULL);
    if (status < VI_SUCCESS) goto error;

    offset = 2 + std::to_string(recordLen).size();

    for(size_t i = offset; i < recordLen + offset; i++){
        form.push_back((((double) _buffer[i]) - yoffset) * ymult);
    }


    return form;

    //printf("%s\n",_buffer.get());
    //printf("%c\n",_buffer[5]);

    /*for(int i = 6; i < 100 ; i++){
        std::cout << (double) _buffer[i] << std::endl;
    }*/

    //std::cout << std::bitset<8>(waveformBuffer[6]) << std::endl;

    //status = viBufRead(_instrumentSession,(ViBuf) waveformBuffer, 1024, VI_NULL);
    //if (status < VI_SUCCESS) goto error;

    
    /*for(int i = 6; i < recordLen + 6; i+=byt_nr){
        if(byt_nr == 2) std::cout << std::bitset<8>(_buffer[i]) << std::bitset<8>(_buffer[i+1]) << std::endl;
        if(byt_nr == 1) std::cout << std::bitset<8>(_buffer[i]) << std::endl;
    }*/

    /*for(int i = 6; i < sizeof(waveformBuffer); i++){
        std::cout << "i: " << std::to_string(i) << " | " << (double) waveformBuffer[i] << std::endl;
    }*/


    //std::cout << std::to_string(recordLen) << std::endl;
    
    /*for(int i = 6; i < recordLen + 6 ; i+=byt_nr){;
        if(byt_nr == 2) {
            std::cout << "i : " << std::to_string(i/2) << " | " 
            << binaryToInteger(boost::dynamic_bitset<>(std::bitset<8>(waveformBuffer[i]).to_string() 
            + std::bitset<8>(waveformBuffer[i+1]).to_string())) << " | B : " << std::bitset<8>(waveformBuffer[i]).to_string() 
            <<  " " << std::bitset<8>(waveformBuffer[i+1]).to_string() << std::endl;
        }
        else{
            std::cout << "i : " << std::to_string(i) << " | " << binaryToInteger(boost::dynamic_bitset<>(std::bitset<8>(_buffer[i]).to_string()))
                << " | B : " << std::bitset<8>(_buffer[i]).to_string() << std::endl;
        }
        //std::cout << "i : " << std::to_string(i) << " | " << binaryToInteger(boost::dynamic_bitset<>(std::bitset<8>(_buffer[i]).to_string())) << std::endl;
    }*/

    //if (status == VI_SUCCESS_MAX_CNT) printf("All Data Returned!!");

    //status = viFlush(_instrumentSession, VI_WRITE_BUF | VI_READ_BUF_DISCARD);
    //if (status < VI_SUCCESS) goto error;

    //double ADCval;
    //ADCval = binaryToInteger(boost::dynamic_bitset<>(std::bitset<8>(formBuffer[6]).to_string() 
    //    + std::bitset<8>(formBuffer[7]).to_string()));
    //std::cout << std::to_string(ADCval*1/32768) << std::endl;
    //std::cout << std::to_string(ymult) << std::endl;
    //std::cout << std::to_string(1.0/32768) << std::endl;
    //std::cout << std::to_string(sizeof(formBuffer)) << std::endl;

    //printf("%s\n",_buffer.get());
    //printf("%c\n",_buffer[5]);

    //double pnt;
    
    //std::cout << "RecLen : " << std::to_string(recordLen) << std::endl;
    /*for(int i = 6; i < recordLen + 6; i++){
        form.push_back((((double) _buffer[i]) - yoffset) * ymult);
        //std::cout << std::to_string(_buffer[i]) << " ---- " << ((((double) _buffer[i]) - yoffset) * ymult) << std::endl;
        //std::cout << "hi" << std::endl;
    }*/

    /*
    for(int i = 6; i < recordLen + 6; i++){
        form.push_back((((double) bTemp[i]) - yoffset) * ymult);
        if(i==6 || i==7) std::cout << std::to_string(bTemp[i]) << " ---- " << ((((double) bTemp[i]) - yoffset) * ymult) << std::endl;
    }*/

    //return form;

error:
    // Report error and clean up
    viStatusDesc(_instrumentSession, status, buffer);
    fprintf(stderr, "Failure: %s\n", buffer);
    //if (ptr != NULL) free(ptr);
    //disconnect();
    return form; //not the best return. Maybe I should switch to pointers
}