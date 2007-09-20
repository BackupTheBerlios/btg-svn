#include "lastfiles.h"

// #include <bcore/t_string.h>
#include <bcore/util.h>
#include <bcore/os/fileop.h>

#include <fstream>

namespace btg
{
   namespace core
   {
      namespace client
      {
         using namespace btg::core;

         using namespace btg::core;
         using namespace btg::core::logger;

         lastFiles::lastFiles(std::string const& _filename)
            : filename_(_filename),
              data_modified_(false),
              lastFiles_(0)
         {
         }

         bool lastFiles::load()
         {
            bool status = false;

            // Open file for reading.
            std::ifstream file;

#if HAVE_IOS_BASE
            file.open(filename_.c_str(), std::ios_base::in);
#else
            file.open(filename_.c_str(), std::ios::in);
#endif

            if (!file.is_open())
               {
                  BTG_NOTICE("Unable to open '" << filename_ << "'.");
                  return status;
               }

            status = true;

            std::string input;

            std::string errorMessage;

            while (!file.eof())
               {
                  getline (file, input);
                  if (input.size() > 0)
                     {
                        if (btg::core::os::fileOperation::check(
                                                                input,
                                                                errorMessage,
                                                                false))
                           {
                              lastFiles_.push_back(input);
                           }
                     }
               }

            file.close();

            // Only keep a certain number of elements.
            while (lastFiles_.size() > GPD->iMAXLASTFILES())
               {
                  lastFiles_.erase(lastFiles_.begin());
               }

            return status;
         }

         bool lastFiles::save()
         {
            bool status = false;

            BTG_NOTICE("Saving last files.");

            if (lastFiles_.size() == 0)
               {
                  BTG_NOTICE("Nothing to save.");

                  status = true;
                  return status;
               }

            // Open file for writing.
            std::ofstream file;

#if HAVE_IOS_BASE
            file.open(filename_.c_str(), std::ios_base::out);
#else
            file.open(filename_.c_str(), std::ios::out);
#endif

            if (!file.is_open())
               {
                  BTG_NOTICE("Unable to open '" << filename_ << "'.");
                  return status;
               }

            status = true;

            t_strListCI iter;
            for (iter = lastFiles_.begin();
                 iter != lastFiles_.end();
                 iter++)
               {
                  file << *iter << std::endl;

                  if (file.fail())
                     {
                        BTG_NOTICE("Saving file failed.");
                        status = false;
                        break;
                     }
               }

            if (!status)
               {
                  file.flush();
                  file.close();
                  data_modified_ = false;
               }

            return status;
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
                  BTG_NOTICE("Adding '" << _filename << "'.");
                  lastFiles_.push_back(_filename);
                  data_modified_ = true;
               }

            // Only keep a certain number of elements.
            while (lastFiles_.size() > GPD->iMAXLASTFILES())
               {
                  lastFiles_.erase(lastFiles_.begin());
                  data_modified_ = true;
               }
         }

         void lastFiles::removeLastFile(std::string const& _filename)
         {
            t_strListI strIter = find(lastFiles_.begin(), lastFiles_.end(), _filename);
            if (strIter != lastFiles_.end())
               {
                  lastFiles_.erase(strIter);
                  data_modified_ = true;
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

            data_modified_ = true;
         }

         lastFiles::~lastFiles()
         {

         }

      } // namespace client
   } // namespace core
} // namespace btg

