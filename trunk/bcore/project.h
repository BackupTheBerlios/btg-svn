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

#ifndef PROJECT_H
#define PROJECT_H

#include "type.h"
#include <string>

/**
 * \defgroup project Project defines
 */
/** @{ */

/// If defined, all applications write debugging output.
#ifndef BTG_DEBUG
#define BTG_DEBUG 0
#endif

/// Handy macro to get the projectDefaults singleton.
#define GPD btg::core::projectDefaults::getInstance()

/// Normal exit code.
#define BTG_NORMAL_EXIT 0
/// Error exit code.
#define BTG_ERROR_EXIT  -1

namespace btg
{
   namespace core
      {
         /// Singleton for keeping defaults for this project.
         class projectDefaults
            {
            public:

               /// Get the singleton.
               static projectDefaults* getInstance();

               /// Kill the singleton.
               static void killInstance();

               /// The project name.
               std::string sPROJECT_NAME()   const;
               /// The CLI client name.
               std::string sCLI_CLIENT()     const;
               /// The default client confiuration file.
               std::string sCLI_CONFIG()     const;
               /// Default client dynamic config file name.
               std::string sCLI_DYNCONFIG() const;
               /// The GUI client name.
               std::string sGUI_CLIENT()     const;
               /// The default confiuration file.
               std::string sGUI_CONFIG()     const;
               /// The daemon name.
               std::string sDAEMON()         const;
               /// The version of this software.
               std::string sVERSION()        const;
               /// The SVN revision of this software.
               std::string sREVISION()       const;
               /// Get both version and revision (if present);
               std::string sFULLVERSION()    const;
               /// Identification: Major version.
               t_int iMAJORVERSION()          const;
               /// Identification: Minor version.
               t_int iMINORVERSION()          const;
               /// Identification: Revision version.
               t_int iREVISIONVERSION()       const;
               /// The build of this software.
               std::string sBUILD()          const;
               /// Hidden file/directory prefix.
               std::string sHiddenPrefix()   const;
               /// Newline.
               std::string sNEWLINE()        const;
               /// Newline.
               char        cNEWLINE()        const;
               /// Space.
               std::string sSPACE()          const;
               /// Space.
               char        cSPACE()          const;
               /// The character used to separate entities in a path.
               std::string sPATH_SEPARATOR() const;

               /// Used for generating random filenames.
               std::string sTEMPTEMPLATE0() const;
               /// Used for generating random filenames.
               std::string sTEMPTEMPLATE1() const;

               /// The default buffer size used for the messageQue class.
               t_uint      iBufferSize()     const;

               // Defaults:

               /// Default daemon config file.
               std::string sDAEMON_CONFIG() const;

               /// Default work dir.
               std::string sDEFAULT_WORK_DIR()    const;
               /// Default destination dir.
               std::string sDEFAULT_DEST_DIR()    const;
               /// Default que location.
               std::string sDEFAULT_QUE()         const;
               /// Default start of the port range.
               t_int       iDEFAULT_RANGE_START() const;
               /// Default end of the port range.
               t_int       iDEFAULT_RANGE_END()   const;
               /// Default leech mode.
               bool        bDEFAULT_LEECH_MODE()  const;
               /// Number of last opened files the clients should
               /// remember.
               t_uint      iMAXLASTFILES()        const;

            private:
               /// Constructor.
               projectDefaults();
               /// Destructor.
               ~projectDefaults();

               /// Pointer to the single instance of this class.
               static projectDefaults* instance;

               /// The name of this project.
               std::string const project_name;
               /// The name of the CLI client.
               std::string const cli_client_name;
               /// The name of the GUI client.
               std::string const gui_client_name;
               /// The name of the client config file.
               std::string       m_client_config;
               /// Client dynamic configuration data file name
               /// @see btg::core::client::clientDynConfig
               std::string       m_client_dynconfig;
               /// The name of the daemon.
               std::string const daemon_name;
               /// Version of this software.
               std::string const version;
               /// SVN revision of this software.
               std::string const revision;
               /// Identification: Major version.
               t_int majorVersion;
               /// Identification: Minor version.
               t_int minorVersion;
               /// Identification: Revision version.
               t_int revisionVersion;

               /// Build string.
               std::string const build;
               /// Newline.
               std::string const newline;
               /// Space.
               std::string const space;
               /// Path separator.
               std::string const path_separator;
               /// Prefix used for hiding files/directories.
               std::string const hidden_prefix;
               /// Character used instead of the path to an users home directory.
               std::string const home_char;

               /// The default buffer size used for the messageQue class.
               t_uint      const buffer_size;

               // Defaults:

               /// Default daemon config file.
               std::string m_deamon_config;

               /// Default work dir.
               std::string const  default_work_dir;
               /// Default destination dir.
               std::string const  default_dest_dir;
               /// Default message que file.
               std::string const  default_que;
               /// The start of the port range used.
               t_int       const  default_range_start;
               /// The end of the port range used.
               t_int       const  default_range_end;
               /// If leech mode is enabled.
               bool        const  default_leech_mode;
               /// Used for generating random filenames.
               std::string const  temptemplate0;
               /// Used for generating random filenames.
               std::string const  temptemplate1;
               /// Number of last opened files the clients should remember.
               t_uint      const  max_last_files;
            };

      } // namespace core
} // namespace btg

/** @} */

#endif
