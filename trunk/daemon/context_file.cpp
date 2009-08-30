/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * $Id$
 */

#include "context.h"

#include "modulelog.h"
#include <bcore/verbose.h>
#include <bcore/os/fileop.h>
#include <bcore/util.h>
#include "lt_version.h"
#include <libtorrent/identify_client.hpp>
#include <algorithm>

#include <boost/version.hpp>

namespace btg
{
   namespace daemon
   {
      using namespace btg::core;

      const std::string moduleName("ctn");

      bool dirname_compare(std::string const& _s1,
                           std::string const& _s2);

      bool Context::moveToDirectory(t_int const _torrent_id, 
                                    std::string const& _destination_dir)
      {
         BTG_MENTER(logWrapper(), 
                    "moveToDirectory", "id = " << _torrent_id << ", destination = " << _destination_dir);

         bool result = true;

         torrentInfo *ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               return false;
            }

#if BTG_LT_0_14
         ti->handle.move_storage(_destination_dir);
#endif // lt version
         BTG_MEXIT(logWrapper(), "moveToDirectory", result);

         return result;
      }

      bool Context::moveToWorkingDir(t_int const _torrent_id)
      {
         BTG_MENTER(logWrapper(), 
                    "moveToWorkingDir", "id = " << _torrent_id);
         
         bool result = false;

         if (torrent_storage[_torrent_id] == tsWork)
         {
            result = true;
            // already in work
            BTG_MEXIT(logWrapper(), "moveToWorkingDir", result);
            return result;
         }

         result = moveToDirectory(_torrent_id, workDir_);

         if (result)
            {
               torrent_storage[_torrent_id] = tsWork;
            }

         BTG_MEXIT(logWrapper(), "moveToWorkingDir", result);
         return result;
      }
      
      bool Context::moveToSeedingDir(t_int const _torrent_id)
      {
         BTG_MENTER(logWrapper(), 
                    "moveToSeedingDir", "id = " << _torrent_id);
         
         bool result = false;

         if (torrent_storage[_torrent_id] == tsSeed)
         {
            result = true;
            // already in seed
            BTG_MEXIT(logWrapper(), "moveToSeedingDir", result);
            return result;
         }
         
         result = moveToDirectory(_torrent_id, seedDir_);

         if (result)
            {
               torrent_storage[_torrent_id] = tsSeed;
            }

         BTG_MEXIT(logWrapper(), "moveToSeedingDir", result);
         return result;
      }

      bool Context::moveToDestinationDir(t_int const _torrent_id)
      {
         BTG_MENTER(logWrapper(), 
                    "moveToDestinationDir", "id = " << _torrent_id);

         bool result = false;
         
         if (torrent_storage[_torrent_id] == tsDest)
         {
            result = true;
            // already in dest
            BTG_MEXIT(logWrapper(), "moveToDestinationDir", result);
            return result;
         }
         
         // remove unselected files (probably they aren't completely downloaded)
         selectedFileEntryList aSelectedFileEntryList;
         if (getSelectedFiles(_torrent_id, aSelectedFileEntryList))
         {
            std::vector<selectedFileEntry> vSelectedFileEntry = aSelectedFileEntryList.files();
            for (std::vector<selectedFileEntry>::const_iterator iSelectedFileEntry = vSelectedFileEntry.begin(); \
               iSelectedFileEntry != vSelectedFileEntry.end(); ++iSelectedFileEntry)
            {
               if (!iSelectedFileEntry->selected())
               {
                  btg::core::os::fileOperation::remove(seedDir_ + "/" + iSelectedFileEntry->filename());
                  // just for sure
                  btg::core::os::fileOperation::remove(workDir_ + "/" + iSelectedFileEntry->filename());
               }
            }
         }
         else
         {
            BTG_MERROR(logWrapper(),"getSelectedFiles");
            // but still continue
         }
         
         result = moveToDirectory(_torrent_id, outputDir_);
         
         if (result)
            torrent_storage[_torrent_id] = tsDest;

         BTG_MEXIT(logWrapper(), "moveToDestinationDir", result);
         return result;
      }

      bool Context::removeFastResumeData(t_int const _torrent_id)
      {
         BTG_MENTER(logWrapper(), "removeFastResumeData", "id = " << _torrent_id);
         bool status = false;

         std::string filename;
         if (getFilename(_torrent_id, filename))
            {
               if (hasFastResumeData(filename))
                  {
                     std::string fastResumeFilename = tempDir_ + 
                        projectDefaults::sPATH_SEPARATOR() + 
                        filename + fastResumeFileNameEnd;
                     status = btg::core::os::fileOperation::remove(fastResumeFilename);
                  }
            }

         BTG_MEXIT(logWrapper(), "removeFastResumeData", status);
         return status;
      }

      bool Context::removeTorrentFile(t_int const _torrent_id)
      {
         bool status = false;
         BTG_MENTER(logWrapper(), "removeTorrentFile", "id = " << _torrent_id);

         std::string filename;
         if (getFilename(_torrent_id, filename))
            {
               filename = tempDir_ + projectDefaults::sPATH_SEPARATOR() + filename;
               status = btg::core::os::fileOperation::remove(filename);
            }

         BTG_MEXIT(logWrapper(), "removeTorrentFile", status);
         return status;
      }

#if BTG_OPTION_EVENTCALLBACK
      void Context::removeFileInfoDetails(t_fileInfoList const& _source, 
                                          t_strList & _destination)
      {
         // Find unique top directories.
         t_fileInfoListCI iter;
         for (iter = _source.begin();
              iter != _source.end();
              iter++)
            {
               std::string path = iter->getFilename();

               t_strList parts = btg::core::Util::splitLine(path, "/");
               if (parts.size() == 0)
                  {
                     // No slashes.
                     _destination.push_back(path);
                  }
               else
                  {
                     // Slashes, add topdir if its not already added.
                     std::string topdir = parts[0];

                     t_strListCI iter = std::find(_destination.begin(), _destination.end(), topdir);

                     if (iter == _destination.end())
                        {
                           _destination.push_back(topdir);
                        }
                  }
            }
      }
#endif // BTG_OPTION_EVENTCALLBACK

      bool Context::dataPresentInSeedDir(libtorrent::torrent_info const& _torrent_info)
      {
         BTG_MENTER(logWrapper(), "dataPresentInSeedDir(torrent_info)", "");

         bool status = false;

         libtorrent::torrent_info::file_iterator iter;

         // Try to find any file.
         for (iter = _torrent_info.begin_files();
              iter != _torrent_info.end_files();
              iter++)
            {
               libtorrent::file_entry const& fe = *iter;
         
               // The full path.
               boost::filesystem::path const& file_path = fe.path;
               
               if (boost::filesystem::exists(seedDir_ / file_path))
                  {
                     // Check whether we have the same directory as file name in torrent.
                     // If so, we can't use the seedDir anyway.
                     if (boost::filesystem::is_directory(seedDir_ / file_path))
                        {
                           BTG_MNOTICE(logWrapper(), "dataPresentInSeedDir: torrent file '"
                                       << file_path.string() << "' is a directory, can't use seedDir.");
                        }
                     else
                        {
                           status = true;
                           
                           BTG_MNOTICE(logWrapper(), "dataPresentInSeedDir: found torrent file '"
                                       << file_path.string() << "'.");
                        }
                     break;
                  }
            }

         BTG_MEXIT(logWrapper(), "dataPresentInSeedDir(torrent_info)", status);
         return status;
      }

      bool Context::getListOfEntities(t_int const _torrent_id,
                                      std::string const& _directory,
                                      std::vector<std::string> & _entities)
      {
         BTG_MENTER(logWrapper(), "getListOfEntities", "_torrent_id = " << _torrent_id);

         bool status = false;

         if (!configured)
            {
               BTG_MEXIT(logWrapper(), "getListOfEntities", status);
               return status;
            }

         torrentInfo* ti;
         if ((ti = getTorrentInfo(_torrent_id)) == 0)
            {
               BTG_MEXIT(logWrapper(), "getListOfEntities", status);
               return status;
            }

         libtorrent::torrent_info const& info = ti->handle.get_torrent_info();

         // 1. Find all files.
         // 2. Find all unique directories.
         // At this point, incomplete relative paths are used.
         // They are extended later.
         std::vector<std::string> output;

         std::vector<std::string> unique_directories;
         std::vector<std::string> files;

         libtorrent::torrent_info::file_iterator fileIter;

         for (fileIter = info.begin_files();
              fileIter != info.end_files();
              fileIter++)
            {
               boost::filesystem::path const& p = fileIter->path.string();

               files.push_back(p.string());

               // Check if the directory in which the file resides in
               // is unique.

               boost::filesystem::path dir = p.branch_path();

               while (!dir.empty())
                  {
                     std::string br = dir.string();

                     std::vector<std::string>::const_iterator uniqueIter = std::find(
                                                                                     unique_directories.begin(),
                                                                                     unique_directories.end(),
                                                                                     br);
                     if (uniqueIter == unique_directories.end())
                        {
                           unique_directories.push_back(br);
                        }

                     dir = dir.branch_path();
                  }
            }

         if (unique_directories.size() > 0)
            {
               // Sort the directory by using the directory length.
               std::sort(unique_directories.begin(), unique_directories.end(), dirname_compare);
            }

         if ((unique_directories.size() > 0) || (files.size() > 0))
            {
               // Files or directories .. or both found.

               std::vector<std::string>::const_iterator cpyIter;

               // Convert the name to a full path.
               for (cpyIter = files.begin();
                    cpyIter != files.end();
                    cpyIter++)
                  {
                     std::string filename = _directory + projectDefaults::sPATH_SEPARATOR() + (*cpyIter);
                     output.push_back(filename);
                  }

               // Convert the names to a full path.
               for (cpyIter = unique_directories.begin();
                    cpyIter != unique_directories.end();
                    cpyIter++)
                  {
                     std::string dirname = _directory + projectDefaults::sPATH_SEPARATOR() + (*cpyIter);
                     output.push_back(dirname);
                  }

               status = true;
            }

         if (status)
            {
               // Generated a list of unique files and/or directories.
               // Now check if the entities really exist.

               std::vector<std::string>::const_iterator checkIter;

               for (checkIter = output.begin();
                    checkIter != output.end();
                    checkIter++)
                  {
                     if (boost::filesystem::exists(*checkIter))
                        {
                           _entities.push_back(*checkIter);
                        }
                  }

               if (_entities.size() == 0)
                  {
                     status = false;
                  }
            }

         BTG_MEXIT(logWrapper(), "getListOfEntities", status);
         return status;
      }

      void Context::waitForResumeDataAlert()
      {
         bool finished = false;
         while (!finished)
         {
            libtorrent::alert const* al = torrent_session->wait_for_alert(libtorrent::seconds(1));

            if (al == 0)
            {
               continue;
            }

            std::auto_ptr<libtorrent::alert> sp_alert = torrent_session->pop_alert();
            libtorrent::alert* raw_alert = sp_alert.get();
            
            if (!raw_alert)
            {
               continue;
            }
            
            libtorrent::torrent_alert* alert = dynamic_cast<libtorrent::torrent_alert*>(raw_alert);

            // we aren't interested in alert, that doesn't contain torrent_handle
            if (alert != 0 )
            {
               if ((typeid(*alert) == typeid(libtorrent::save_resume_data_alert)) ||
                   (typeid(*alert) == typeid(libtorrent::save_resume_data_failed_alert))
                   )
                  {
                     handleAlert(sp_alert.get());
                     // Done waiting.
                     finished = true;
                     VERBOSE_LOG(logWrapper(), verboseFlag_, "Finished waiting for resume data.");
                  }
               else
                  {
                     // Save alert for processing later.
                     libtorrent::torrent_alert* a = dynamic_cast<libtorrent::torrent_alert*>(sp_alert.release());
                     saved_alerts_.push_back(a);
                  }
            }
         }
      }

      bool Context::writeResumeData()
      {
         BTG_MENTER(logWrapper(), "writeResumeData", "");

         bool op_status = false;

         // TODO: result checking
         std::for_each(torrent_ids.begin(), torrent_ids.end(), boost::bind(&Context::writeResumeData, this, _1));

         op_status = true;

         BTG_MEXIT(logWrapper(), "writeResumeData", op_status);
         return op_status;
      }

      bool Context::writeResumeData(t_int const _torrent_id)
      {
         BTG_MENTER(logWrapper(), "writeResumeData(torrent_id)", _torrent_id);

         bool op_status = false;

         torrentInfo *ti       = getTorrentInfo(_torrent_id);
         if (!ti)
         {
            BTG_ERROR_LOG(logWrapper(), "Context::writeResumeData(torrent_id): unknown torrent_id = " << _torrent_id);
            return op_status;
         }

#if (BTG_LT_0_14)
         // The actual writing is done using a callback (handleResumeDataAlert).
         ti->handle.save_resume_data();
         // Wait for this operation to complete.
         waitForResumeDataAlert();
#endif

         op_status = true;

         BTG_MEXIT(logWrapper(), "writeResumeData(torrent_id)", op_status);
         return op_status;
      }

      bool Context::hasFastResumeData(std::string const& _torrent_filename) const
      {
         // Check if the fast resume data actually exists.
         std::string fastResumeFilename = tempDir_ + projectDefaults::sPATH_SEPARATOR() + 
            _torrent_filename + fastResumeFileNameEnd;
         bool status = btg::core::os::fileOperation::check(fastResumeFilename);

         if (!status)
            {
               BTG_MNOTICE(logWrapper(), "no fast resume data for '" << _torrent_filename << "'");
            }
         else
            {
               boost::intmax_t filesize = 
                  boost::filesystem::file_size(boost::filesystem::path(fastResumeFilename));
               if(filesize == 0)
                  {
                     BTG_MNOTICE(logWrapper(), "no valid fast resume data for '" << _torrent_filename << "'");
                     status = false;
                  }
            }

         return status;
      }
#if BTG_LT_0_14
      bool Context::torrentInfoToFiles(libtorrent::torrent_info const& _tinfo,
                                       std::vector<std::string> & _output) const
      {
         BTG_MENTER(logWrapper(), "torrentInfoToFiles", "");

         bool status = true;
         try
            {
               libtorrent::torrent_info::file_iterator iter;

               for (iter = _tinfo.begin_files();
                    iter != _tinfo.end_files();
                    iter++)
                  {
                     libtorrent::file_entry const& fe = *iter;
                     BTG_MNOTICE(logWrapper(), fe.path.string());

                     _output.push_back(fe.path.string());
                  }
            }
         catch (std::exception& e)
            {
               BTG_ERROR_LOG(logWrapper(), "libtorrent exception (torrentInfoToFiles): " << e.what() );
               status = false;
            }

         if (_output.size() == 0)
         {
            // Torrents with no files are not ok.
            BTG_ERROR_LOG(logWrapper(), "torrentInfoToFiles, attempt to load torrent with no files in it.");
            status = false;
         }
         
         BTG_MEXIT(logWrapper(), "torrentInfoToFiles", status);
         return status;
      }
#endif

#if (BTG_LT_0_14)
      void Context::bitfieldToVector(libtorrent::bitfield const& _input, 
                                     std::vector<bool> & _output) const
      {
         _output.resize(_input.size());
         std::copy(_input.begin(), _input.end(), _output.begin());
      }
#endif

      /// Compare two directories, return true if the first directory
      /// is the longest path of the two arguments.
      bool dirname_compare(std::string const& _s1,
                           std::string const& _s2)
      {
         return (_s1.length() > _s2.length());
      }

   } // namespace daemon
} // namespace btg
