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
      typedef std::pair<ViStatus,boost::optional<std::string> >                         Status;
      typedef std::pair<ViStatus,boost::optional<std::string> >                         Data;

      class Interface
      {
      private:

        bool      _isConnected            = false;
        ViSession _defaultResourceManager = 0;

      public:

        virtual Status   connect(const ViString & resource, bool exclusiveLock = false);
        virtual void     disconnect();
        virtual Status   write(const ViString & command);
        virtual Data     read();
        virtual Data     query(const ViString & command);
        
        virtual ~Interface() { if (_isConnected) disconnect(); }

        bool     isConnected() const    { return _isConnected; }

        ViSession getDefaultRM() const { return _defaultResourceManager; }

      protected:

        ViSession _instrumentSession      = 0;
      };
    }
}
#endif  