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

      const auto NM_OF_TEKSCOPE_CHANNELS = 8;

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

      protected:

        ViSession _instrumentSession      = 0;
      };
    }
}
#endif  