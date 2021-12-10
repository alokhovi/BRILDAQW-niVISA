#include <iostream>


#include "interface.hpp"
#include "tekscope.hpp"

using namespace brildaq::nivisa;

Status   Interface::connect(const ViString & resource)
{
    return std::make_pair(VI_SUCCESS,boost::none);
}

void Interface::disconnect()
{
    
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
