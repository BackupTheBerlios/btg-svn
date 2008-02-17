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

#include <libtorrent/identify_client.hpp>

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

         // TODO: check alert about moved storage.
         ti->handle.move_storage(_destination_dir);

         BTG_MEXIT(logWrapper(), "moveToDirectory", result);

         return result;
      }

      bool Context::moveToSeedingDir(t_int const _torrent_id)
      {
         BTG_MENTER(logWrapper(), 
                    "moveToSeedingDir", "id = " << _torrent_id);
         bool result = false;

         torrentInfo* torrentinfo           = getTorrentInfo(_torrent_id);

         if (torrentinfo == 0)
            {
               BTG_MEXIT(logWrapper(), "moveToSeedingDir", result);
               return result;
            }

         libtorrent::torrent_info const& ti = torrentinfo->handle.get_torrent_info();

         if (dataPresentInSeedDir(ti))
            {
               // There is no need to move the data.
               // The torrent's data is already in the seed directory.

               result = true;
               BTG_MEXIT(logWrapper(), "moveToSeedingDir", result);
               return result;
            }

         result = moveToDirectory(_torrent_id, seedDir_);

         BTG_MEXIT(logWrapper(), "moveToSeedingDir", result);
         return result;
      }

      bool Context::moveToDestinationDir(t_int const _torrent_id)
      {
         BTG_MENTER(logWrapper(), 
                    "moveToDestinationDir", "id = " << _torrent_id);
         
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
         
         bool result = moveToDirectory(_torrent_id, outputDir_);

         BTG_MEXIT(logWrapper(), "moveToDestinationDir", result);
         return result;
      }

      bool Context::removeFastResumeData(t_int const _torrent_id)
      {
         BTG_MENTER(logWrapper(), "removeFastResumeData", "id = " << _torrent_id);
         bool status = false;

         std::string filename;
         if (this->getFilename(_torrent_id, filename))
            {
               if (this->hasFastResumeData(filename))
                  {
                     std::string fastResumeFilename = tempDir_ + GPD->sPATH_SEPARATOR() + 
                        filename + this->fastResumeFileNameEnd;
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
         if (this->getFilename(_torrent_id, filename))
            {
               filename = tempDir_ + GPD->sPATH_SEPARATOR() + filename;
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

      bool Context::find_file(boost::filesystem::path const& dir_path,
                              std::string const& file_name)
      {
         using namespace boost::filesystem;
         BTG_MENTER(logWrapper(), "find_file", "file name = " << file_name);

         if (!exists(dir_path))
            {
               BTG_MEXIT(logWrapper(), "find_file", false);
               return false;
            }

         // Default construction yields past-the-end.
         directory_iterator end_itr;

         for (directory_iterator itr(dir_path);
              itr != end_itr;
              ++itr )
            {
               if (is_directory(*itr))
                  {
                     if (find_file(*itr, file_name))
                        {
                           BTG_MEXIT(logWrapper(), "find_file", true);
                           return true;
                        }
                  }
               else if (itr->leaf() == file_name) // see below
                  {
                     BTG_MEXIT(logWrapper(), "find_file", true);
                     return true;
                  }
            }
         BTG_MEXIT(logWrapper(), "find_file", false);
         return false;
      }

      bool Context::dataPresentInSeedDir(libtorrent::torrent_info const& _torrent_info)
      {
         BTG_MENTER(logWrapper(), "dataPresentInSeedDir(torrent_info)", "");

         bool status = true;

         libtorrent::torrent_info::file_iterator iter;

         // Find and count the files.
         for (iter = _torrent_info.begin_files();
              iter != _torrent_info.end_files();
              iter++)
            {
               libtorrent::file_entry const& fe = *iter;
	      
               // The full path.
               boost::filesystem::path file_path = fe.path;
	      
               // Only the file name.
               std::string filename = file_path.leaf();
	      
               // Get the first directory name, if it exists.
               if (!file_path.branch_path().empty())
                  {
                     file_path = file_path.branch_path();
                  }

               // Fix of bug #11575 / #12055.
               // Check if the found file path is a directory.
               // If its not, then its just files in the seed
               // directory.
               
               try
                  {
                     if (boost::filesystem::exists(seedDir_ / file_path) &&
                         boost::filesystem::is_directory(seedDir_ / file_path))
                        {
                           file_path = seedDir_ / file_path;
                        }
                     else
                        {
                           file_path = seedDir_;
                        }
                  }
               catch(std::exception& e)
                  {
                     BTG_ERROR_LOG(logWrapper(), 
                                   "Got exception from boost::filesystem, is_directory: " << 
                                   e.what());
                     status = false;
                  }

               if (!status)
                  {
                     BTG_MEXIT(logWrapper(), "dataPresentInSeedDir(torrent_info)", status);
                     return status;
                  }

               BTG_MNOTICE(logWrapper(), "Using directory '" << file_path.string() << "'");
               BTG_MNOTICE(logWrapper(), "Checking file '" << filename << "'");
	      
               try
                  {
                     if (!find_file(boost::filesystem::path(file_path), filename))
                        {
                           status = false;
                           break;
                        }
                  }
               catch(std::exception& e)
                  {
                     BTG_ERROR_LOG(logWrapper(), 
                                   "Got exception from find_file: " << e.what());
                     status = false;
                     break;
                  }
            }

         BTG_MEXIT(logWrapper(), "dataPresentInSeedDir(torrent_info)", status);
         return status;
      }

      bool Context::entryToInfo(libtorrent::entry const& _input,
                                libtorrent::torrent_info & _output) const
      {
         BTG_MENTER(logWrapper(), "entryToInfo", "");

         bool status = false;

         libtorrent::torrent_info tinfo;
         try
            {
               _output = libtorrent::torrent_info(_input);
               status  = true;
            }
         catch (std::exception& e)
            {
               BTG_ERROR_LOG(logWrapper(), "libtorrent exception: " << e.what() );
            }

         if (!status)
            {
               // The input is incorrect.
               BTG_MEXIT(logWrapper(), "entryToInfo", status);
               return status;
            }

         BTG_MEXIT(logWrapper(), "entryToInfo", status);
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
                     std::string filename = _directory + GPD->sPATH_SEPARATOR() + (*cpyIter);
                     output.push_back(filename);
                  }

               // Convert the names to a full path.
               for (cpyIter = unique_directories.begin();
                    cpyIter != unique_directories.end();
                    cpyIter++)
                  {
                     std::string dirname = _directory + GPD->sPATH_SEPARATOR() + (*cpyIter);
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

      bool Context::writeResumeData()
      {
         BTG_MENTER(logWrapper(), "writeResumeData", "");

         bool op_status = false;

         std::map<t_int, torrentInfo*>::iterator tii;
         for (tii = torrents.begin(); tii != torrents.end(); tii++)
            {
               torrentInfo *ti       = tii->second;

               // Get the fast resume data.
               libtorrent::entry torrent_entry = ti->handle.write_resume_data();

               // The name to which the resume data is saved to.
               std::string filename;
               filename = tempDir_ + GPD->sPATH_SEPARATOR() + ti->filename + 
                  this->fastResumeFileNameEnd;

               // Output file.
               std::ofstream out;

#if HAVE_IOS_BASE
               out.open(filename.c_str(), std::ios_base::out);
#else
               out.open(filename.c_str(), std::ios::out);
#endif

               if (!out.is_open())
                  {
                     return op_status;
                  }

               try
                  {
                     // Encode to file.
                     libtorrent::bencode(
                                         std::ostream_iterator<char>(out),
                                         torrent_entry
                                         );
                  }
               catch (libtorrent::invalid_encoding & e)
                  {
                     BTG_ERROR_LOG(logWrapper(), "libtorrent exception: " << e.what() );
                     return op_status;
                  }

               BTG_MNOTICE(logWrapper(), "wrote fast resume data for '" << filename << "'");
               out.close();
            }

         op_status = true;

         BTG_MEXIT(logWrapper(), "writeResumeData", op_status);
         return op_status;
      }

      bool Context::hasFastResumeData(std::string const& _torrent_filename) const
      {
         // Check if the fast resume data actually exists.
         std::string fastResumeFilename = tempDir_ + GPD->sPATH_SEPARATOR() + 
            _torrent_filename + this->fastResumeFileNameEnd;
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

      bool Context::entryToFiles(libtorrent::entry const& _input,
                                 std::vector<std::string> & _output) const
      {
         BTG_MENTER(logWrapper(), "entryToFiles", "");

         bool status = true;
         try
            {
               libtorrent::torrent_info tinfo(_input);
               libtorrent::torrent_info::file_iterator iter;

               for (iter = tinfo.begin_files();
                    iter != tinfo.end_files();
                    iter++)
                  {
                     libtorrent::file_entry const& fe = *iter;
                     BTG_MNOTICE(logWrapper(), fe.path.string());

                     _output.push_back(fe.path.string());
                  }
            }
         catch (std::exception& e)
            {
               BTG_ERROR_LOG(logWrapper(), "libtorrent exception: " << e.what() );
               status = false;
            }

         BTG_MEXIT(logWrapper(), "entryToFiles", status);
         return status;
      }

      /// Compare two directories, return true if the first directory
      /// is the longest path of the two arguments.
      bool dirname_compare(std::string const& _s1,
                           std::string const& _s2)
      {
         return (_s1.length() > _s2.length());
      }

   } // namespace daemon
} // namespace btg
