#include "lasturls.h"

#include <bcore/logmacro.h>

namespace btg
{
   namespace core
   {
      namespace client
      {
         using namespace btg::core;
         using namespace btg::core::logger;

         lastURLs::lastURLs(LogWrapperType _logwrapper, clientDynConfig & _CDC)
            : Logable(_logwrapper),
              CDC_(_CDC),
              modified_(false),
              URLs_(_CDC.lastURLs_),
              Files_(_CDC.lastURLFiles_)
         {
         }

         void lastURLs::add(std::string const& _url, std::string const& _filename)
         {
            if (find(URLs_.begin(), URLs_.end(), _url) == URLs_.end())
               {
                  BTG_NOTICE(logWrapper(), "Adding '" << _url << "' (" << _filename << ")" << ".");
                  URLs_.push_back(_url);
                  Files_.push_back(_filename);
                  set_modified(true);
               }

            // Only keep a certain number of elements.
            while (URLs_.size() > projectDefaults::iMAXLASTFILES())
               {
                  URLs_.erase(URLs_.begin());
                  Files_.erase(Files_.begin());
                  set_modified(true);
               }
         }

         void lastURLs::remove(std::string const& _url)
         {
            t_strListI ui,fi;
            ui = URLs_.begin();
            fi = Files_.begin();
            while(ui != URLs_.end())
            {
               if (*ui == _url)
               {
                  URLs_.erase(ui);
                  Files_.erase(fi);
                  set_modified(true);
                  break;
               }
               ++ui;
               ++fi;
            }
         }

         void lastURLs::set_modified(bool const bMod)
         {
            modified_ = bMod;
            CDC_.set_modified(bMod);
         }

      } // namespace client
   } // namespace core
} // namespace btg
