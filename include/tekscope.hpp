#ifndef __BRILDAQ_NIVISA_TEKSCOPE__
#define __BRILDAQ_NIVISA_TEKSCOPE__

#include <map>
#include <chrono>

#include "interface.hpp"

namespace brildaq
{
    namespace nivisa
    {
      typedef std::pair<ViStatus,boost::optional<std::map<int, std::vector<float> > > > Waveform;

      struct ChannelConfiguration
      {
          uint8_t     ID; /* 1-8 */
          bool        ONOFF;
          std::string name;
      };

      class TekScope : public Interface
      {
      public:

        void enableProfiling()  { _profilingEnabled = true; }
        void desableProfiling() { _profilingEnabled = false; }

        virtual Waveform readWaveform();

        Data Dir(const ViString & directory = nullptr);

        Status reset();

        Status wait(std::chrono::milliseconds timeout) noexcept;

        int dumpProfile(const std::string & action, const std::string & fileName, bool verbose = false, const std::string & delimiter = ", ") const;

        std::pair<long, long long> getProfilerStat(const std::string & action) const;

        virtual ~TekScope();

        virtual Data resetScope();//reset the scope to blank settings

        virtual Status channelState(std::string channel, std::string state);//turn on="1" or off="0" a channel 

        virtual Status verticalScale(std::string channel, std::string voltsPerDivision);//adjust the vertical scale of a channel

        virtual Status timeScale(std::string secsPerDivision);//adjust the entire horizontal sclae

        virtual Status triggerType(std::string type);//select trigger type ("EDGE" typically)

        virtual Status triggerSource(std::string channel);//select the channel source for the A trigger

        virtual Status triggerSlopeType(std::string type);//select "RISE" or "FALL"(ing) edge for the trigger

        virtual Status setHalfTrigger(); //set the trigger level to 50%

        virtual Status setTriggerLevel(std::string channel, std::string voltageLevel); //set trigger voltage cutoff "LOW" or "UPP"
        
        virtual Data checkReady();

        virtual Status baseConfig(ChannelConfiguration channelConfigurationParameters[]); //setup the scope to the base configuration parameters

      public:

        // Histograming of the scope readout time
        void startProfiler(const std::string & action); 
        
        std::chrono::milliseconds stopProfiler(const std::string & action);

      private:

        bool _profilingEnabled = false;

        //       method name, dynamic histogram data
        std::map<std::string, std::map<std::chrono::milliseconds,long long> > _profiling;

        std::map<std::string, std::chrono::milliseconds> _beginning;

        void rebin(const std::string & action, std::chrono::milliseconds readoutTime);
      };
    }
}
#endif  