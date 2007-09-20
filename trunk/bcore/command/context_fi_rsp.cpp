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
 * $Id: context_fi_rsp.cpp,v 1.5.4.10 2006/08/05 23:21:09 wojci Exp $
 */

#include "context_fi_rsp.h"
#include <bcore/helpermacro.h>
#include <iostream>

namespace btg
{
   namespace core
   {

      contextFileInfoResponseCommand::contextFileInfoResponseCommand()
         : contextCommand(Command::CN_CFILEINFORSP, contextCommand::UNDEFINED_CONTEXT),
           fileinfolist(0)
      {
         // fileinfolist is left undefined.
      }

      contextFileInfoResponseCommand::contextFileInfoResponseCommand(t_int const _context_id,
                                                                     t_fileInfoList const& _fileinfolist)
         : contextCommand(Command::CN_CFILEINFORSP, _context_id), 
           fileinfolist(_fileinfolist)
      {
      }

      bool contextFileInfoResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         contextCommand::serialize(_e);
         BTG_RCHECK(_e->status());

         // Serialize the list.

         // First write the size of the list.
         t_int list_size = fileinfolist.size();
         _e->setParamInfo("size of the contained list of file information objects", true);
         _e->intToBytes(&list_size);
         BTG_RCHECK(_e->status());


         _e->setParamInfo("list of file information objects", true);
         // Then serialize the contents of the list.
         t_fileInfoListCI iter;
         for (iter=fileinfolist.begin(); iter != fileinfolist.end(); iter++)
            {
               iter->serialize(_e);
               BTG_RCHECK(_e->status());
            }

         return true;
      }

      bool contextFileInfoResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         contextCommand::deserialize(_e);
         BTG_RCHECK(_e->status());

         // The size of the list.
         t_int list_size = 0;

         _e->setParamInfo("size of the contained list of file information objects", true);
         _e->bytesToInt(&list_size);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("list of file information objects", true);

         // The contents.
         for (t_int list_counter = 0; list_counter < list_size; list_counter++)
            {
               fileInformation fi;
               fi.deserialize(_e);
               BTG_RCHECK(_e->status());
               fileinfolist.push_back(fi);
            }

         return true;
      }

      t_fileInfoList contextFileInfoResponseCommand::getFileInfoList() const
      {
         return fileinfolist;
      }

      contextFileInfoResponseCommand::~contextFileInfoResponseCommand()
      {

      }

   } // namespace core
} // namespace btg
