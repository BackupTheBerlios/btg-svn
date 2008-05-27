#include "passwd_auth.h"

#include <bcore/project.h>
#include <bcore/logmacro.h>
#include <bcore/util.h>

#include <fstream>

#include <daemon/modulelog.h>

namespace btg
{
   namespace daemon
   {
      namespace auth
      {

         char const commentChr = '#';

         const std::string moduleName("aut");

         passwordAuth::passwordAuth(btg::core::LogWrapperType _logwrapper,
                                    std::string const& _filename, 
                                    bool const _ignoreErrors)
            : Auth(_logwrapper),
              filename_(_filename),
              initialized_(true),
              users()
         {
            if (!read(users))
               {
                  if (!_ignoreErrors)
                     {
                        initialized_ = false;
                     }
               }
            else
               {
#if BTG_AUTH_DEBUG
                  BTG_MNOTICE(logWrapper(), "got " << users.size() << " user(s)");
#endif // BTG_AUTH_DEBUG
               }
         }

         bool passwordAuth::checkUser(std::string const& _username,
                                      std::string const& _passwordHash,
                                      btg::core::addressPort const& /*_addressport*/)
         {
            bool status = false;

            std::map<std::string, userData>::const_iterator iter = users.find(_username);

            if (iter != users.end())
               {
                  if (btg::core::Util::compareStringNoCase(iter->second.getHash(),_passwordHash))
                     {
                        status = true;
                     }
               }

            return status;
         }

         bool passwordAuth::getTempDir(std::string const& _username, std::string & _tempDir) const
         {
            bool status = false;

            std::map<std::string, userData>::const_iterator iter = users.find(_username);

            if (iter != users.end())
               {
                  _tempDir = iter->second.getTempDir();
                  status   = true;
               }

            return status;
         }

         bool passwordAuth::getWorkDir(std::string const& _username, std::string & _workDir) const
         {
            bool status = false;

            std::map<std::string, userData>::const_iterator iter = users.find(_username);

            if (iter != users.end())
               {
                  _workDir = iter->second.getWorkDir();
                  status   = true;
               }

            return status;
         }

         bool passwordAuth::getSeedDir(std::string const& _username, std::string & _seedDir) const
         {
            bool status = false;

            std::map<std::string, userData>::const_iterator iter = users.find(_username);

            if (iter != users.end())
               {
                  _seedDir = iter->second.getSeedDir();
                  status   = true;
               }

            return status;
         }


         bool passwordAuth::getDestDir(std::string const& _username, std::string & _destDir) const
         {
            bool status = false;

            std::map<std::string, userData>::const_iterator iter = users.find(_username);

            if (iter != users.end())
               {
                  _destDir = iter->second.getDestDir();
                  status   = true;
               }

            return status;
         }

         bool passwordAuth::getControlFlag(std::string const& _username, bool & _controlFlag) const
         {
            bool status = false;

            std::map<std::string, userData>::const_iterator iter = users.find(_username);

            if (iter != users.end())
               {
                  _controlFlag = iter->second.getControlFlag();
                  status       = true;
               }

            return status;
         }

         bool passwordAuth::getCallbackFile(std::string const& _username, std::string & _callbackFile) const
         {
            bool status = false;

            std::map<std::string, userData>::const_iterator iter = users.find(_username);

            if (iter != users.end())
               {
                  _callbackFile = iter->second.getCallback();
                  status        = true;
               }

            return status;
         }

         bool passwordAuth::initialized() const
         {
            return initialized_;
         }

         bool passwordAuth::reInit()
         {
            bool status = false;

            std::map<std::string, userData> dest;

            if (read(dest))
               {
                  status = true;
                  users  = dest;
               }

            return status;
         }

         bool passwordAuth::read(std::map<std::string, userData> & _dest)
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
                  return status;
               }

            status = true;

            std::string input;
            t_strList separated;

            while (!file.eof())
               {
                  getline(file, input);

                  if ((input.size() > 0) && (input[0] != commentChr))
                     {
                        separated = btg::core::Util::splitLine(input, ":");
                        if (separated.size() == 8)
                           {
                              std::string username    = separated[0];
                              std::string passwd_hash = separated[1];
                              std::string temp_dir    = separated[2];
                              std::string work_dir    = separated[3];
                              std::string seed_dir    = separated[4];
                              std::string dest_dir    = separated[5];

                              bool control_flag        = false;
                              if (separated[6] == "1")
                                 {
                                    control_flag = true;
                                 }

                              std::string callback    = separated[7];
#if BTG_AUTH_DEBUG
                              BTG_MNOTICE(logWrapper(),
                                          "read: username '" << username    << "', " <<
                                          "hash     '" << passwd_hash << "', " <<
                                          "temp_dir '" << temp_dir    << "', " <<
                                          "work_dir '" << work_dir    << "', " <<
                                          "seed_dir '" << seed_dir    << "', " <<
                                          "dest_dir '" << dest_dir    << "', " <<
                                          "control_flag '" << control_flag    << "', " <<
                                          "callback '" << callback << "'");
#endif // BTG_AUTH_DEBUG
                              userData ud(passwd_hash, temp_dir, work_dir, seed_dir, dest_dir, control_flag, callback);
                              std::pair<std::string, userData> p(username, ud);
                              _dest.insert(p);
                           }
                     }
               }

            file.close();

            if (_dest.size() == 0)
               {
#if BTG_AUTH_DEBUG
                  BTG_MNOTICE(logWrapper(), "no users");
#endif // BTG_AUTH_DEBUG
                  status = false;
               }

            return status;
         }

         bool passwordAuth::addUser(std::string const& _username, std::string const& _password,
                                    std::string const& _temp_dir, std::string const& _work_dir,
                                    std::string const& _seed_dir, std::string const& _dest_dir,
                                    bool const _controlFlag, std::string const& _callback
                                    )
         {
            bool status = false;

            if (GetUser(_username) == users.end())
               {
                  userData ud(_password, _temp_dir, _work_dir, _seed_dir, _dest_dir, _controlFlag, _callback);
                  std::pair<std::string, userData> p(_username, ud);
                  users.insert(p);
                  status = true;
               }

            return status;
         }

         bool passwordAuth::modUser(std::string const& _username, std::string const& _password,
                                    std::string const& _temp_dir, std::string const& _work_dir,
                                    std::string const& _seed_dir, std::string const& _dest_dir,
                                    bool const _controlFlag, std::string const& _callback)
         {
            bool status = false;

            std::map<std::string, userData>::iterator iter = GetUser(_username);

            if (iter != users.end())
               {
                  if (_password != "")
                     {
                        iter->second.setHash(_password);
                        status = true;
                     }

                  if (_temp_dir != "")
                     {
                        iter->second.setTempDir(_temp_dir);
                        status = true;
                     }

                  if (_work_dir != "")
                     {
                        iter->second.setWorkDir(_work_dir);
                        status = true;
                     }

                  if (_seed_dir != "")
                     {
                        iter->second.setSeedDir(_seed_dir);
                        status = true;
                     }

                  if (_dest_dir != "")
                     {
                        iter->second.setDestDir(_dest_dir);
                        status = true;
                     }

                  if (_controlFlag != iter->second.getControlFlag())
                     {
                        iter->second.setControlFlag(_controlFlag);
                        status = true;
                     }

                  if (_callback != "")
                     {
                        iter->second.setCallback(_callback);
                        status = true;
                     }
               }

            return status;
         }

         bool passwordAuth::delUser(std::string const& _username)
         {
            bool status = false;

            std::map<std::string, userData>::iterator iter = GetUser(_username);

            if (iter != users.end())
               {
                  users.erase(iter);
                  status = true;
               }

            return status;
         }

         bool passwordAuth::write()
         {
            bool status = false;

            // Open file for writing.
            std::ofstream file;

#if HAVE_IOS_BASE
            file.open(filename_.c_str(), std::ios_base::out);
#else
            file.open(filename_.c_str(), std::ios::out);
#endif

            if (!file.is_open())
               {
#if BTG_AUTH_DEBUG
                  BTG_MNOTICE(logWrapper(), "saving file '" << filename_ << "' failed");
#endif // BTG_AUTH_DEBUG
                  return status;
               }

            status = true;

            std::string line;

            std::map<std::string, userData>::const_iterator dataIter;

            for (dataIter = users.begin();
                 dataIter != users.end();
                 dataIter++)
               {
                  line =
                     dataIter->first               + ":" +
                     dataIter->second.getHash()    + ":" +
                     dataIter->second.getTempDir() + ":" +
                     dataIter->second.getWorkDir() + ":" +
                     dataIter->second.getSeedDir() + ":" +
                     dataIter->second.getDestDir() + ":";

                  if (dataIter->second.getControlFlag())
                     {
                        line += "1";
                     }
                  else
                     {
                        line += "0";
                     }
                  line += ":";
                  line += dataIter->second.getCallback();

                  file << line << std::endl;

                  if (file.fail())
                     {
#if BTG_AUTH_DEBUG
                        BTG_MNOTICE(logWrapper(), "saving file '" << filename_ << "' failed");
#endif // BTG_AUTH_DEBUG
                        status = false;
                        break;
                     }
               }

            if (!status)
               {
                  file.flush();
                  file.close();
               }

            return status;
         }

         std::map<std::string, userData>::iterator passwordAuth::GetUser(std::string const& _username)
         {
            return users.find(_username);
         }

         std::string passwordAuth::toString() const
         {
            std::string output;

            output += "[username] / [temp dir] / [work dir] / [seed dir] / [dest dir] / [callback]";
            output += "\n";

            std::map<std::string, userData>::const_iterator dataIter;

            for (dataIter = users.begin();
                 dataIter != users.end();
                 dataIter++)
               {
                  output += dataIter->first;
                  output += " ";
                  output += dataIter->second.getTempDir();
                  output += " ";
                  output += dataIter->second.getWorkDir();
                  output += " ";
                  output += dataIter->second.getSeedDir();
                  output += " ";
                  output += dataIter->second.getDestDir();
                  output += " ";
                  if (dataIter->second.getCallback() == Auth::callbackDisabled)
                     {
                        output += "disabled";
                     }
                  else
                     {
                        output += dataIter->second.getCallback();
                     }
                  output += " ";
                  output += "\n";
               }

            return output;
         }

         passwordAuth::~passwordAuth()
         {

         }

         /* */
         /* */
         /* */

         userData::userData(std::string const& _passwordHash,
                            std::string const& _tempDir,
                            std::string const& _workDir,
                            std::string const& _seedDir,
                            std::string const& _destDir,
                            bool const         _controlFlag,
                            std::string const& _callback
                            )
            : passwordHash_(_passwordHash),
              tempDir_(_tempDir),
              workDir_(_workDir),
              seedDir_(_seedDir),
              destDir_(_destDir),
              controlFlag_(_controlFlag),
              callback_(_callback)
         {
         }

         userData::userData(userData const& _userdata)
            : passwordHash_(_userdata.passwordHash_),
              tempDir_(_userdata.tempDir_),
              workDir_(_userdata.workDir_),
              seedDir_(_userdata.seedDir_),
              destDir_(_userdata.destDir_),
              controlFlag_(_userdata.controlFlag_),
              callback_(_userdata.callback_)
         {
         }

         bool userData::operator== (userData const& _userdata) const
         {
            bool status = true;

            if (
                (passwordHash_ != _userdata.passwordHash_) ||
                (tempDir_ != _userdata.tempDir_) ||
                (workDir_ != _userdata.workDir_) ||
                (seedDir_ != _userdata.seedDir_) ||
                (destDir_ != _userdata.destDir_) ||
                (controlFlag_ != _userdata.controlFlag_) ||
                (callback_ != _userdata.callback_)
                )
               {
                  status = false;
               }

            return status;
         }

         bool userData::operator!= (userData const& _userdata) const
         {
            bool status = false;
            bool eq     = (*this == _userdata);

            if (!eq)
               {
                  status = true;
               }

            return status;
         }

         userData& userData::operator= (userData const& _userdata)
         {
            bool eq = (*this == _userdata);

            if (!eq)
               {
                  passwordHash_ = _userdata.passwordHash_;
                  tempDir_      = _userdata.tempDir_;
                  workDir_      = _userdata.workDir_;
                  seedDir_      = _userdata.seedDir_;
                  destDir_      = _userdata.destDir_;
                  controlFlag_  = _userdata.controlFlag_;
                  callback_     = _userdata.callback_;
               }

            return *this;
         }

         userData::~userData()
         {

         }

      } // namespace auth
   } // namespace daemon
} // namespace btg
