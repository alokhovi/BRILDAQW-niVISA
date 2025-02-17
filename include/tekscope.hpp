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
          bool        ONOFF; //select channel on off
          std::string VSCALE; //set vertical scale of channel
          std::string name; //name of channel
      };

      struct GlobalConfigurationParams
      {
          std::string TSCALE = "1"; //time scale in secs/division
          std::string TRIGTYPE = "EDGE"; //trigger type
          std::string SLOPETYPE = "FALL"; //slope fall type
          std::string TRIGSOURCE[2] = {"8","-1"}; //default trigger source on ORBITMAIN

      };

      class TekScope : public Interface
      {
      public:

        void enableProfiling()  { _profilingEnabled = true; }
        void desableProfiling() { _profilingEnabled = false; }

        virtual Waveform readWaveform();

	//virtual std::map<int, std::vector<float>> readWaveform();

        virtual std::map<int, std::vector<float>> readWaveformAscii();

        virtual std::map<int, std::vector<float>> readWaveformBinary();

        Data Dir(const ViString & directory = nullptr);

        Status reset();

        Status wait(std::chrono::milliseconds timeout) noexcept;

        int dumpProfile(const std::string & action, const std::string & fileName, bool verbose = false, const std::string & delimiter = ", ") const;

        std::pair<long, long long> getProfilerStat(const std::string & action) const;

	virtual void  run_scope_calibration();// run scope build-in calibration

        virtual ~TekScope();

        virtual Data resetScope();//reset the scope to blank settings
        
        virtual Status terminateChannels(); //set 50 OHM terminations on all of the channels 

        virtual Status channelState(std::string channel, std::string state);//turn on="1" or off="0" a channel 

        virtual Status verticalScale(std::string channel, std::string voltsPerDivision);//adjust the vertical scale of a channel

        virtual Data getVerticalScale(std::string channel);//get vertical scale of a channel

	virtual Status verticalPosition(std::string channel, std::string position);//sdjust the position, range is -5.0 to 5.0

virtual Data getVerticalPosition(std::string channel);//get position, range is -5.0 to 5.0

	virtual Status verticalOffset(std::string channel, std::string volts);//adjust the offset, unit is volt

virtual Data getVerticalOffset(std::string channel);//get offset, unit is volt

	virtual Status verticalAutoset(std::string on_off);//whether enable or not

	virtual Status verticalOptimize(std::string resolution, std::string visiility);

        virtual Status timeScale(std::string secsPerDivision);//adjust the entire horizontal sclae

        virtual Status triggerType(std::string type);//select trigger type ("EDGE" typically)

        virtual Status triggerSource(std::string channel);//select the channel source for the A trigger

        virtual Status triggerSlopeType(std::string type);//select "RISE" or "FALL"(ing) edge for the trigger

        virtual Status setHalfTrigger(); //set the trigger level to 50%

        virtual Status setTriggerLevel(std::string channel, std::string voltageLevel); //set trigger voltage cutoff "LOW" or "UPP"
        
        virtual Data checkReady(); //wait for scope to finish all past commands

        virtual Status baseConfig(GlobalConfigurationParams globalParams, ChannelConfiguration channelConfigurationParameters[]); 
        //setup the scope to the base configuration parameters

        virtual std::string binIn();

        virtual std::string getForm(std::string channel, std::string byteNum, std::string start, std::string stop);

  	//add by Ren
	virtual std::string getFormAscii(std::string channel, std::string byteNum, std::string start, std::string stop);

        virtual std::vector<std::string> getMeasurementResults(std::string measurementID);

        virtual void measureDelays();

        virtual std::vector<float> asciiWaveformReadout(std::string channel);

	//add by Ren
	virtual void gain_optimizer(double occupancy); 

	virtual Status pause(float second);

	//add by jae
	virtual std::map<std::string, std::vector<float>> zeroCrossingTimes();

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

// add by Ren
std::vector<int> str_vec(std::string form);

#endif  
