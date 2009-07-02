/*
 * btg Copyright (C) 2008 Michael Wojciechowski.
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

#include "cli.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         void cliHandler::CPIF_begin(std::string const& _filename)
         {
            
         }

         void cliHandler::CPIF_begin(std::string const& _filename, 
                                     std::string const& _url)
         {

         }

         void cliHandler::CPIF_filePiece(t_uint _number, t_uint _parts)
         {

         }

         void cliHandler::CPIF_urlDlStatus(t_uint _total, t_uint _now, t_uint _speed)
         {

         }

         void cliHandler::CPIF_error(std::string const& _error)
         {
            setError(_error);
         }

         void cliHandler::CPIF_wait(std::string const& _msg)
         {
            
         }

         void cliHandler::CPIF_success(std::string const& _filename)
         {
            setOutput("Created '" + _filename + "'");
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

