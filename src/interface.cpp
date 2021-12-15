#include <iostream>


#include "interface.hpp"
#include "tekscope.hpp"

using namespace brildaq::nivisa;

Status Interface::connect(const ViString & resource, bool exclusiveLock)
{
    if ( _isConnected )
    {
        return std::make_pair(VI_SUCCESS,boost::none);
    }

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

    assert (viSetAttribute (_instrumentSession, VI_ATTR_TMO_VALUE,800)   ==  VI_SUCCESS);

    _isConnected = true; return std::make_pair(VI_SUCCESS,boost::none);
}

void Interface::disconnect()
{
    if (_defaultResourceManager > 0 ) viClose(_defaultResourceManager);

    _instrumentSession = 0; _defaultResourceManager = 0;

    _isConnected = false;
}

Data  Interface::write(const ViString & command)
{
    return std::make_pair(VI_SUCCESS,boost::none);
}

Status   Interface::read()
{
    return std::make_pair(VI_SUCCESS,boost::none);
}

Status   Interface::query(const ViString & command)
{
    return std::make_pair(VI_SUCCESS,boost::none);
}
