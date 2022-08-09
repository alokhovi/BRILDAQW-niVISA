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

std::vector<float> Interface::ReadWaveform() noexcept
{
    assert(_isConnected);     

    ViStatus              status;
    float                 yoffset, ymult;
    int                   byt_nr;
    ViChar                buffer[256];
    std::vector<float>    form;
    int                   recordLen;
    std::size_t           offset;
    brildaq::nivisa::Data data;
    //ViUInt32              read;


    viSetAttribute(_instrumentSession, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);
    viSetAttribute(_instrumentSession, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);
    
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

    // Get the ymult to help calculate the vertical values.
    status = viQueryf(_instrumentSession, "WFMOutpre:YMULT?\n", "%f", &ymult);
    //printf("YMULT: %f\n",ymult);
    if (status < VI_SUCCESS) goto error;

    //set aqcuisition start and end
    status = viPrintf(_instrumentSession, "data:start %d;:data:stop %d\n", 0, recordLen);
    if (status < VI_SUCCESS) goto error;

    /*this->write(const_cast<ViString>("Header on"));
    data = this->query(const_cast<ViString>("WFMOUTPRE?"));
    this->write(const_cast<ViString>("Header off"));
    std::cout << data.second << std::endl;*/
    
    // Request the curve
    status = viPrintf(_instrumentSession, "CURVE?\n");
    if (status < VI_SUCCESS) goto error;

    //status = viFlush(_instrumentSession, VI_WRITE_BUF | VI_READ_BUF_DISCARD);
    //if (status < VI_SUCCESS) goto error;
    
    //calculate offset to ignore the header of the CURVE? query
    offset = 2 + std::to_string(recordLen).size();
    assert(recordLen + offset < MAX_FORMATTED_BUFFER_SIZE);
    
    _buffer[0] = VI_NULL;
    
    //turn off termination character so that all points are transferred
    viSetAttribute(_instrumentSession, VI_ATTR_TERMCHAR_EN, VI_FALSE);
    //viSetAttribute(_instrumentSession, VI_ATTR_TERMCHAR_EN, VI_TRUE);

    
    //read in Curve
    //status = viRead(_instrumentSession,(ViBuf)_buffer.get(), recordLen + offset, &read);
    status = viRead(_instrumentSession,(ViBuf)_buffer.get(), recordLen + offset, VI_NULL);
    //printf("Points Read: %d\n",read - offset);
    assert(status == VI_SUCCESS_MAX_CNT);
    
    //transfer curve from buffer to output variabel
    for(size_t i = offset; i < recordLen + offset; i++){
        form.push_back((((double) _buffer[i]) - yoffset) * ymult);
    }
    

    status = viSetAttribute(_instrumentSession, VI_ATTR_TERMCHAR_EN, VI_TRUE);

    //status = viFlush(_instrumentSession, VI_WRITE_BUF | VI_READ_BUF_DISCARD);
    //if (status < VI_SUCCESS) goto error;

    //std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    return form;

error:
    //Report error and clean up
    viStatusDesc(_instrumentSession, status, buffer);
    fprintf(stderr, "Failure: %s\n", buffer);
    //status = viSetAttribute(_instrumentSession, VI_ATTR_TERMCHAR_EN, VI_TRUE);
    return form;
}