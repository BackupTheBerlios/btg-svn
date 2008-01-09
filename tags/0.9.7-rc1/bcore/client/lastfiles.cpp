#include "lastfiles.h"

// #include <bcore/t_string.h>
#include <bcore/util.h>

#include <fstream>

namespace btg
{
   namespace core
   {
      namespace client
      {
         using namespace btg::core;
         using namespace btg::core::logger;

         lastFiles::lastFiles(LogWrapperType _logwrapper,
                              clientDynConfig & cc)
            : Logable(_logwrapper),
              m_cc(cc),
              data_modified_(false),
              lastFiles_(cc.lastFiles_)
         {
         }

         t_strList lastFiles::getLastFiles() const
         {
            return lastFiles_;
         }

         void lastFiles::addLastFile(std::string const& _filename)
         {
            t_strListCI strIter = find(lastFiles_.begin(), lastFiles_.end(), _filename);
            if (strIter == lastFiles_.end())
               {
                  BTG_NOTICE(logWrapper(), "Adding '" << _filename << "'.");
                  lastFiles_.push_back(_filename);
                  set_modified(true);
               }

            // Only keep a certain number of elements.
            while (lastFiles_.size() > GPD->iMAXLASTFILES())
               {
                  lastFiles_.erase(lastFiles_.begin());
                  set_modified(true);
               }
         }

         void lastFiles::removeLastFile(std::string const& _filename)
         {
            t_strListI strIter = find(lastFiles_.begin(), lastFiles_.end(), _filename);
            if (strIter != lastFiles_.end())
               {
                  lastFiles_.erase(strIter);
                  set_modified(true);
               }
         }

         void lastFiles::setLastFiles(t_strList const& _lastfiles)
         {
            lastFiles_ = _lastfiles;

            // Only keep a certain number of elements.
            while (lastFiles_.size() > GPD->iMAXLASTFILES())
               {
                  lastFiles_.erase(lastFiles_.begin());
               }

            set_modified(true);
         }

         lastFiles::~lastFiles()
         {
         }
         
         void lastFiles::set_modified(bool const bMod)
         {
            data_modified_ = bMod;
            m_cc.set_modified(bMod);
         }

      } // namespace client
   } // namespace core
} // namespace btg

