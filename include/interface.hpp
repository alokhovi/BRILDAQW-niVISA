#ifndef __BRILDAQ_NIVISA_INTERFACE__
#define __BRILDAQ_NIVISA_INTERFACE__

#include <boost/optional.hpp>

#include <map>
#include <memory>
#include <vector>
#include <visa.h>
  
namespace brildaq
{
    namespace nivisa
    {
      constexpr  uint32_t  MAX_FORMATTED_BUFFER_SIZE = 268435456; // 256 MB

      constexpr  uint16_t  MAX_NUMBER_OF_QUERY_TRIES = 4;

      constexpr  uint8_t   LINEFEED_CHAR             = '\n';

      using Status = std::pair<ViStatus,boost::optional<std::string> >;

      using Data   = std::pair<ViStatus,std::string>;

      class Interface
      {
      private:

        bool      _isConnected            = false;

        bool      _isSocket               = false;
        
        ViSession _defaultResourceManager = 0;

        std::unique_ptr<char []>      _buffer;

      public:

        Interface();

        virtual Status   connect(const ViString & resource, ViAttrState timeout, bool exclusiveLock = false) noexcept;
        //connect to the scope through the Nivisa protocol
        
        virtual void     disconnect() noexcept; //disconnect from the scope
        
        virtual Data     query(const ViString & command) noexcept; //send query to scope and return response

        virtual Status   write(const ViString & command) noexcept; //send command to scope
        
        virtual ~Interface() { if (_isConnected) disconnect(); }

        bool     isConnected() const    { return _isConnected; }

        ViSession getDefaultRM() const { return _defaultResourceManager; }

        virtual std::string baseConfig(); //setup the scope to the base configuration parameters

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


      protected:

        ViSession _instrumentSession      = 0;
      };
    }
}
#endif  