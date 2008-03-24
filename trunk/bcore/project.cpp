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

#include "project.h"
#include <bcore/util.h>
#include <bcore/os/fileop.h>

#define BTG_STRINGIFY(x) #x
#define BTG_EXPAND(x)    BTG_STRINGIFY(x)

namespace btg
{
   namespace core
   {

      projectDefaults* projectDefaults::instance = 0;

      projectDefaults* projectDefaults::getInstance()
      {
         if (projectDefaults::instance == 0)
            {
               projectDefaults::instance = new projectDefaults();
            }

         return projectDefaults::instance;
      }

      void projectDefaults::killInstance()
      {
         if (projectDefaults::instance)
            {
               delete projectDefaults::instance;
               projectDefaults::instance = 0;
            }
      }

      projectDefaults::projectDefaults()
         : project_name("btg"),
           cli_client_name("btg*cli"),
           gui_client_name("btgui"),
           m_client_config("client.ini"),
           m_client_dynconfig("client.dynconfig"),
           daemon_name("btgdaemon"),
           version(BTG_EXPAND(BTG_VERSION)),
           revision(BTG_EXPAND(BTG_REV)),
           majorVersion(0),
           minorVersion(0),
           revisionVersion(0),
           build(BTG_EXPAND(BTG_BUILD)),
           newline("\n"),
           space(" "),
           path_separator("/"),
           hidden_prefix("."),
           buffer_size(512),
           m_deamon_config("daemon.ini"),
           default_work_dir("~/bt"),
           default_dest_dir("~/bt/finished"),
           default_que("~/.btg/daemon"),
           default_range_start(15000),
           default_range_end(15400),
           default_leech_mode(false),
           temptemplate0("/tmp/btg0"),
           temptemplate1("/tmp/btg1"),
           max_last_files(50)
      {
         // Paths that can contain *.ini-files
         static const char * config_paths[] = {
            // default locations
            "~/.btg/",
            PREFIX "/etc/btg/",
            "/etc/btg/",
            // extra locations
            "~/",
            PREFIX "/etc/",
            "/etc/",
            0
         };

         // Search for daemon config file
         for (int i = 0; config_paths[i]; ++i)
         {
            std::string daemon_config = config_paths[i] + m_deamon_config;
            btg::core::os::fileOperation::resolvePath(daemon_config);
            if (btg::core::os::fileOperation::check(daemon_config))
            {
               m_deamon_config = daemon_config;
               break;
            }
         }
         // else in current dir
         
         // Search for client config file
         for (int i = 0; config_paths[i]; ++i)
         {
            std::string client_config = config_paths[i] + m_client_config;
            btg::core::os::fileOperation::resolvePath(client_config);
            if (btg::core::os::fileOperation::check(client_config))
            {
               m_client_config = client_config;
               break;
            }
         }
         // else in current dir
         
         m_client_dynconfig = "~/.btg/" + m_client_dynconfig;

         // Convert the version string into:
         // majorVersion
         // minorVersion;
         // revisionVersion;

         t_strList s = Util::splitLine(version, ".");

         if (s.size() > 0)
            {
               t_uint buffer;
               t_strListCI iter = s.begin();

               if (Util::stringToUInt(*iter, buffer))
                  {
                     majorVersion = buffer;
                  }

               iter++;

               if (iter != s.end())
                  {
                     if (Util::stringToUInt(*iter, buffer))
                        {
                           minorVersion = buffer;
                        }
                  }

               iter++;

               if (iter != s.end())
                  {
                     if (Util::stringToUInt(*iter, buffer))
                        {
                           revisionVersion = buffer;
                        }

                  }
            }
      }

      projectDefaults::~projectDefaults()
      {
      }

      std::string projectDefaults::sPROJECT_NAME() const
      {
         return project_name;
      }

      std::string projectDefaults::sCLI_CLIENT() const
      {
         return cli_client_name;
      }

      std::string projectDefaults::sGUI_CLIENT() const
      {
         return gui_client_name;
      }

      std::string projectDefaults::sCLI_CONFIG() const
      {
         return m_client_config;
      }


      std::string projectDefaults::sCLI_DYNCONFIG() const
      {
         return m_client_dynconfig;
      }

      std::string projectDefaults::sGUI_CONFIG() const
      {
         return m_client_config;
      }

      std::string projectDefaults::sDAEMON() const
      {
         return daemon_name;
      }

      std::string projectDefaults::sVERSION() const
      {
         return version;
      }

      std::string projectDefaults::sREVISION() const
      {
         return revision;
      }
      
      std::string projectDefaults::sFULLVERSION() const
      {
         std::string temp(version);
         if (revision.size() > 0)
            {
               temp += ", ";
               temp += revision;
            }

         return temp;
      }

      t_int projectDefaults::iMAJORVERSION() const
      {
         return majorVersion;
      }

      t_int projectDefaults::iMINORVERSION() const
      {
         return minorVersion;
      }

      t_int projectDefaults::iREVISIONVERSION() const
      {
         return revisionVersion;
      }

      std::string projectDefaults::sBUILD() const
      {
         return build;
      }

      std::string projectDefaults::sHiddenPrefix() const
      {
         return hidden_prefix;
      }

      std::string projectDefaults::sNEWLINE() const
      {
         return newline;
      }

      char projectDefaults::cNEWLINE() const
      {
         return newline[0];
      }

      std::string projectDefaults::sSPACE() const
      {
         return space;
      }

      char projectDefaults::cSPACE() const
      {
         return space[0];
      }

      std::string projectDefaults::sPATH_SEPARATOR() const
      {
         return path_separator;
      }

      t_uint projectDefaults::iBufferSize() const
      {
         return buffer_size;
      }

      std::string projectDefaults::sDAEMON_CONFIG() const
      {
         return m_deamon_config;
      }

      std::string projectDefaults::sDEFAULT_WORK_DIR() const
      {
         return default_work_dir;
      }

      std::string projectDefaults::sDEFAULT_DEST_DIR() const
      {
         return default_dest_dir;
      }

      std::string projectDefaults::sDEFAULT_QUE() const
      {
         return default_que;
      }

      t_int projectDefaults::iDEFAULT_RANGE_START() const
      {
         return default_range_start;
      }

      t_int projectDefaults::iDEFAULT_RANGE_END() const
      {
         return default_range_end;
      }

      bool projectDefaults::bDEFAULT_LEECH_MODE() const
      {
         return default_leech_mode;
      }

      std::string projectDefaults::sTEMPTEMPLATE0() const
      {
         return temptemplate0;
      }

      std::string projectDefaults::sTEMPTEMPLATE1() const
      {
         return temptemplate1;
      }

      t_uint projectDefaults::iMAXLASTFILES() const
      {
         return max_last_files;
      }

   } // namespace core
} // namespace btg
