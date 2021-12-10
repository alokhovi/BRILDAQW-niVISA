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

        virtual Waveform readWaveform();

        virtual ~TekScope();

      public:

        // Histograming of the scope readout time
        void startProfiler(); void stopProfiler();
        // ----------------------------------------
        //       method name, dynamic histogram data
        std::map<std::string, std::map<long,unsigned long> > _profiling;

        std::chrono::nanoseconds  _beginning;


      };
    }
}
#endif  