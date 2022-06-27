#include <iostream>
#include <cstring>

#include <chrono>
#include <thread>

#include "interface.hpp"
#include "tekscope.hpp"

using namespace brildaq::nivisa;

Interface::Interface() : _buffer(new char [MAX_FORMATTED_BUFFER_SIZE])
{

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

std::string Interface::resetScope()
{
    return this->query(const_cast<ViString>("*RST;:*OPC?")).second;
}

std::string Interface::baseConfig()
{
    brildaq::nivisa::Data       data;
    data = this->query( const_cast<ViString>("*RST;:*OPC?") );
    std::cout << "RESETTING SCOPE: " << data.second << std::endl;
    
    data = this->query( const_cast<ViString>("DISPLAY:WAVEVIEW1:CH8:STATE 1;:*OPC?") );
    std::cout << "OPENING CH8: " << data.second << std::endl;
    
    data = this->query( const_cast<ViString>("DISplay:WAVEView1:CH8:VERTical:SCAle 0.5;:*OPC?") );
    std::cout << "SETTING CH8 SCALE: " << data.second << std::endl;

    data = this->query( const_cast<ViString>("HORizontal:SCAle 100e-9;:*OPC?") );
    std::cout << "SETTING HORIZONTAL SCALE: " << data.second << std::endl;

    data = this->query( const_cast<ViString>(":TRIGger:A:TYPe EDGE;:*OPC?") );
    std::cout << "SETTING EDGE TRIGGER: " << data.second << std::endl;

    data = this->query( const_cast<ViString>(":TRIGger:A:EDGE:SOUrce CH8;:*OPC?") );
    std::cout << "SETTING EDGE SOURCE CH8: " << data.second << std::endl;

    data = this->query( const_cast<ViString>(":TRIGger:A:LOWerthreshold:CH8 -1.2;:*OPC?") );
    data = this->query( const_cast<ViString>(":TRIGger:A SETLevel;:*OPC?") );
    std::cout << "SETTING TRIGGER THRESHOLD: " << data.second << std::endl;

    data = this->query( const_cast<ViString>(":TRIGger:A:EDGE:SLOpe FALL;:*OPC?") );
    std::cout << "SETTING EDGE TYPE: " << data.second << std::endl;

    data = this->query( const_cast<ViString>(":TRIGger:STATE?;:*OPC?") );
    std::cout << "REQUESTING TRIGGER STATE: " << data.second << std::endl;

    data = this->query( const_cast<ViString>("DISPLAY:WAVEVIEW1:CH2:STATE 1;:*OPC?") );
    std::cout << "OPENING CH2: " << data.second << std::endl;

    data = this->query( const_cast<ViString>(":TRIGger:A:EDGE:SOUrce CH2;:*OPC?") );
    std::cout << "SETTING EDGE SOURCE CH2: " << data.second << std::endl;

    data = this->query( const_cast<ViString>(":TRIGger:A SETLevel;:*OPC?") );
    std::cout << "SETTING TRIGGER THRESHOLD: " << data.second << std::endl;

    return data.second;
}
