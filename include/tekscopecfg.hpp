#ifndef __BRILDAQ_NIVISA_TEKSCOPECFG__
#define __BRILDAQ_NIVISA_TEKSCOPECFG__

#include <boost/optional.hpp>

#include "tekscope.hpp"

#include <map>
#include <vector>
#include <visa.h>
  
namespace brildaq
{
    namespace nivisa
    {
        const auto NM_OF_TEKSCOPE_CHANNELS = 8;

        struct TekScopeCfg
        {
            struct ConnectionParameters
            {
                std::string     connectionString;
                ViAttrState     timeout;   // ms
                bool            exclusiveLock;
            
            } connectionParameters;

            ChannelConfiguration channelConfigurationParameters[NM_OF_TEKSCOPE_CHANNELS] = 
            {
                {1,false,"BPTX1"},{2,false,"BPTX2"},{3,false},{4,false},{5,false},{6,false},{7,false},{8,false}
            };
        };
    }
}
#endif  