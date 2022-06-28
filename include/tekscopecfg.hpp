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
                {1,false,"1","BPTX1"},{2,false,"1","BPTX2"},{3,false,"1","ORBIT1"},{4,false,"1","ORBIT2"},{5,false,"1","BUNCH1"},
                {6,false,"1","BUNCH2"},{7,false,"1","BUNCHMAIN"},{8,false,"1","ORBITMAIN"}
            };

            GlobalConfigurationParams globalParams;
        };
    }
}
#endif  