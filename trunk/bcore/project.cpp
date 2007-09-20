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
 * $Id: project.cpp,v 1.13.4.10 2006/12/09 01:28:07 wojci Exp $
 */

#include "project.h"
#include <bcore/util.h>

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

      using namespace std;

      projectDefaults::projectDefaults()
         : project_name("btg"),
           cli_client_name("btg*cli"),
           config_name(),
           lastfiles(),
           gui_client_name("btgui"),
           daemon_name("btgdaemon"),
           version(BTG_EXPAND(BTG_VERSION)),
           majorVersion(0),
           minorVersion(0),
           revisionVersion(0),
           build(BTG_EXPAND(BTG_BUILD)),
           nullchar('\0'),
           newline("\n"),
           space(" "),
           path_separator("/"),
           hidden_prefix("."),
           home_char("~"),
           home_env("HOME"),
           buffer_size(512),
           default_deamon_config("~/.btg/daemon.ini"),
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
         config_name = home_char + path_separator + hidden_prefix + project_name + path_separator + "client.ini";
         lastfiles   = config_name + ".last";

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

      string projectDefaults::sPROJECT_NAME() const
      {
         return project_name;
      }

      string projectDefaults::sCLI_CLIENT() const
      {
         return cli_client_name;
      }

      string projectDefaults::sGUI_CLIENT() const
      {
         return gui_client_name;
      }

      string projectDefaults::sCLI_CONFIG() const
      {
         return config_name;
      }

      std::string projectDefaults::sCLI_LASTFILES() const
      {
         return lastfiles;
      }

      string projectDefaults::sGUI_CONFIG() const
      {
         return config_name;
      }

      std::string projectDefaults::sGUI_LASTFILES() const
      {
         return lastfiles;
      }

      string projectDefaults::sDAEMON() const
      {
         return daemon_name;
      }

      string projectDefaults::sVERSION() const
      {
         return version;
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

      string projectDefaults::sBUILD() const
      {
         return build;
      }

      string projectDefaults::sHiddenPrefix() const
      {
         return hidden_prefix;
      }

      char projectDefaults::cNULL() const
      {
         return nullchar;
      }

      string projectDefaults::sNEWLINE() const
      {
         return newline;
      }

      char projectDefaults::cNEWLINE() const
      {
         return newline[0];
      }

      string projectDefaults::sSPACE() const
      {
         return space;
      }

      char projectDefaults::cSPACE() const
      {
         return space[0];
      }

      string projectDefaults::sPATH_SEPARATOR() const
      {
         return path_separator;
      }

      string projectDefaults::sHOME_CHAR() const
      {
         return home_char;
      }

      string projectDefaults::sHOME_ENV() const
      {
         return home_env;
      }

      const char* projectDefaults::cpHOME_ENV() const
      {
         return home_env.c_str();
      }

      t_uint projectDefaults::iBufferSize() const
      {
         return buffer_size;
      }

      std::string projectDefaults::sDEFAULT_DAEMON_CONFIG() const
      {
         return default_deamon_config;
      }

      std::string projectDefaults::sDEFAULT_WORK_DIR() const
      {
         return default_work_dir;
      }

      string projectDefaults::sDEFAULT_DEST_DIR() const
      {
         return default_dest_dir;
      }

      string projectDefaults::sDEFAULT_QUE() const
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
