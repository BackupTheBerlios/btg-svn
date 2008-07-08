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

/// Normal exit code.
#define BTG_NORMAL_EXIT 0
/// Error exit code.
#define BTG_ERROR_EXIT  -1

namespace btg
{
   namespace core
      {
         /// Defaults used by this project.
         class projectDefaults
            {
            public:
               /// The project name.
               static std::string sPROJECT_NAME();

               /// The CLI client name.
               static std::string sCLI_CLIENT();

               /// The default client confiuration file.
               static std::string sCLI_CONFIG();

               /// Default client dynamic config file name.
               static std::string sCLI_DYNCONFIG();

               /// The GUI client name.
               static std::string sGUI_CLIENT();

               /// The default confiuration file.
               static std::string sGUI_CONFIG();

               /// The daemon name.
               static std::string sDAEMON();

               /// The version of this software.
               static std::string sVERSION();

               /// The SVN revision of this software.
               static std::string sREVISION();

               /// Get both version and revision (if present);
               static std::string sFULLVERSION();

               /// Identification: Major version.
               static t_int iMAJORVERSION();

               /// Identification: Minor version.
               static t_int iMINORVERSION();

               /// Identification: Revision version.
               static t_int iREVISIONVERSION();

               /// The build of this software.
               static std::string sBUILD();

               /// Hidden file/directory prefix.
               static std::string sHiddenPrefix();

               /// The character used to separate entities in a path.
               static std::string sPATH_SEPARATOR();

               // Defaults:

               /// Default daemon config file.
               static std::string sDAEMON_CONFIG();

               /// Default start of the port range.
               static t_int       iDEFAULT_RANGE_START();

               /// Default end of the port range.
               static t_int       iDEFAULT_RANGE_END();

               /// Number of last opened files the clients should
               /// remember.
               static t_uint      iMAXLASTFILES();

               /// Number of last opened URLs the clients should
               /// remember.
               static t_uint      iMAXLASTURLS();
            };

      } // namespace core
} // namespace btg

/** @} */

#endif // PROJECT_H

