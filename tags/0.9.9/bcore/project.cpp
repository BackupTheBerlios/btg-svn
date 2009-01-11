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

typedef std::vector<std::string> t_Path;
typedef t_Path::value_type     t_PathElem;
typedef t_Path::const_iterator t_PathCI;

// Paths that can contain *.ini-files.
static const t_PathElem config_paths[] =
   {
      t_PathElem("~/.btg/"),
      t_PathElem(/*PREFIX*/ "/etc/btg/"),
      t_PathElem("/etc/btg/"),
      t_PathElem("~/"),
      t_PathElem(/*PREFIX*/ "/etc/"),
      t_PathElem("/etc/")
   };
static t_int const config_paths_size = sizeof(config_paths)/sizeof(t_PathElem);

namespace btg
{
   namespace core
   {
      std::string getClientConfig();
      t_int getVersionNumber(t_uint const _n);

      std::string projectDefaults::sPROJECT_NAME()
      {
         return "btg";
      }

      std::string projectDefaults::sCLI_CLIENT()
      {
         return "btg*cli";
      }

      std::string projectDefaults::sGUI_CLIENT()
      {
         return "btgui";
      }

      std::string getClientConfig()
      {
         std::string client_config("client.ini");
         
         static const t_Path cp(&config_paths[0], &config_paths[config_paths_size]);

         // Search for client config file
         for (t_PathCI iter = cp.begin();
              iter != cp.end();
              iter++)
            {
               std::string cc = *iter + client_config;
               btg::core::os::fileOperation::resolvePath(cc);
               if (btg::core::os::fileOperation::check(cc))
                  {
                     client_config = cc;
               break;
                  }
            }

         return client_config;
      }

      std::string projectDefaults::sCLI_CONFIG()
      {
         return getClientConfig();
      }


      std::string projectDefaults::sCLI_DYNCONFIG()
      {
         std::string client_dynconfig("~/.btg/client.dynconfig");
         return client_dynconfig;
      }

      std::string projectDefaults::sGUI_CONFIG()
      {
         return getClientConfig();
      }

      std::string projectDefaults::sDAEMON()
      {
         return "btgdaemon";
      }

      std::string projectDefaults::sVERSION()
      {
         return std::string(BTG_EXPAND(BTG_VERSION));
      }

      std::string projectDefaults::sREVISION()
      {
         return std::string(BTG_EXPAND(BTG_REV));
      }
      
      std::string projectDefaults::sFULLVERSION()
      {
         std::string v(BTG_EXPAND(BTG_VERSION));
         std::string r(BTG_EXPAND(BTG_REV));
         
         if (r.size())
            {
               v += ", ";
               v += r;
            }

         return v;
      }

      t_int getVersionNumber(t_uint const _n)
      {
         t_int version = 0;

         std::string v(BTG_EXPAND(BTG_VERSION));

         t_strList s = Util::splitLine(v, ".");

         if (s.size() > _n)
            {
               t_uint buffer = 0;
               if (Util::stringToUInt(s[_n], buffer))
                  {
                     version = buffer;
                  }
            }

         return version;
      }

      t_int projectDefaults::iMAJORVERSION()
      {
         return getVersionNumber(0);
      }

      t_int projectDefaults::iMINORVERSION()
      {
         return getVersionNumber(1);
      }

      t_int projectDefaults::iREVISIONVERSION()
      {
         return getVersionNumber(2);
      }

      std::string projectDefaults::sBUILD()
      {
         return std::string(BTG_EXPAND(BTG_BUILD));
      }

      std::string projectDefaults::sHiddenPrefix()
      {
         return ".";
      }

      std::string projectDefaults::sPATH_SEPARATOR()
      {
         return "/";
      }

      std::string projectDefaults::sDAEMON_CONFIG()
      {
         std::string deamon_config("daemon.ini");

         static const t_Path dp(&config_paths[0], &config_paths[config_paths_size]);

         // Search for daemon config file
         for (t_PathCI iter = dp.begin();
              iter != dp.end();
              iter++)
            {
               std::string dc = *iter + deamon_config;
               btg::core::os::fileOperation::resolvePath(dc);
               if (btg::core::os::fileOperation::check(dc))
                  {
                     deamon_config = dc;
                     break;
                  }
            }

         return deamon_config;
      }

      t_uint projectDefaults::iMAXLASTFILES()
      {
         return 20;
      }

      t_uint projectDefaults::iMAXLASTURLS()
      {
         return 20;
      }

   } // namespace core
} // namespace btg
