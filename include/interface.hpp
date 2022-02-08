#ifndef __BRILDAQ_NIVISA_INTERFACE__
#define __BRILDAQ_NIVISA_INTERFACE__

#include <boost/optional.hpp>

#include <map>
#include <vector>
#include <visa.h>
  
namespace brildaq
{
    namespace nivisa
    {
      constexpr  uint16_t  MAX_FORMATTED_BUFFER_SIZE = 4096;

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

        char      _buffer[MAX_FORMATTED_BUFFER_SIZE];

      public:

        virtual Status   connect(const ViString & resource, ViAttrState timeout, bool exclusiveLock = false) noexcept;
        
        virtual void     disconnect() noexcept;
        
        virtual Data     query(const ViString & command) noexcept;

        virtual Status   write(const ViString & command) noexcept;
        
        virtual ~Interface() { if (_isConnected) disconnect(); }

        bool     isConnected() const    { return _isConnected; }

        ViSession getDefaultRM() const { return _defaultResourceManager; }

      protected:

        ViSession _instrumentSession      = 0;
      };
    }
}
#endif  