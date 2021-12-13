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

      class TekScope : public Interface
      {
      public:

        void enableProfiling()  { _profilingEnabled = true; }
        void desableProfiling() { _profilingEnabled = false; }

        virtual Waveform readWaveform();

        int dumpProfile(const std::string & action, const std::string & fileName, bool verbose = false, const std::string & delimiter = ", ") const;

        std::pair<long, long long> getProfilerStat(const std::string & action) const;

        virtual ~TekScope();

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