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
         void cliHandler::CPRI_init(std::string const& _filename)
         {
            
         }

         void cliHandler::CPRI_pieceUploaded(t_uint _number, t_uint _parts)
         {

         }

         void cliHandler::CPRI_error(std::string const& _error)
         {
            setError(_error);
         }

         void cliHandler::CPRI_wait(std::string const& _msg)
         {
            
         }

         void cliHandler::CPRI_success(std::string const& _filename)
         {
            setOutput("Created '" + _filename + "'");
         }

      } // namespace cli
   } // namespace UI
} // namespace btg

