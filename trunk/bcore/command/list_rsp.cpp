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

#include "list_rsp.h"
#include <bcore/t_string.h>
#include <bcore/helpermacro.h>

#include <iostream>

namespace btg
{
   namespace core
   {
      listCommandResponse::listCommandResponse()
         : Command(Command::CN_GLISTRSP),
           context_ids(0), 
           filenames(0)
      {}

      listCommandResponse::listCommandResponse(t_intList const& _context_ids, t_strList const& _filenames)
         : Command(Command::CN_GLISTRSP), 
           context_ids(_context_ids), 
           filenames(_filenames)
      {}

      std::string listCommandResponse::toString() const
      {
         std::string output("");

         t_strListCI ci = filenames.begin();
         for (t_intListCI i=context_ids.begin(); i != context_ids.end(); i++)
            {
               output += convertToString<t_int>(*i);
               output += ": ";
               output += *ci;
               output += "\n";
               ci++;
            }
         return output;
      }

      t_intList listCommandResponse::getIDs() const
      {
         return context_ids;
      }

      t_strList listCommandResponse::getFilanames() const
      {
         return filenames;
      }

      bool listCommandResponse::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( Command::serialize(_e) );

         _e->setParamInfo("list of context ids", true);
         BTG_RCHECK( _e->intListToBytes(&context_ids) );

         _e->setParamInfo("list of file names", true);
         BTG_RCHECK( _e->stringListToBytes(&filenames) );

         return true;
      }

      bool listCommandResponse::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( Command::deserialize(_e) );

         t_strList stringv;
         t_intList intv;

         _e->setParamInfo("list of context ids", true);
         BTG_RCHECK( _e->bytesToIntList(&intv) );

         context_ids = intv;
         _e->setParamInfo("list of file names", true);
         BTG_RCHECK( _e->bytesToStringList(&stringv) );

         filenames = stringv;

         return true;
      }

      listCommandResponse::~listCommandResponse()
      {

      }

   } // namespace core
} // namespace btg
