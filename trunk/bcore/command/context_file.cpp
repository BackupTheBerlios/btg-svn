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
 * $Id: context_file.cpp,v 1.1.2.1 2007/06/06 22:04:31 wojci Exp $
 */

#include "context_file.h"
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {
      contextSetFilesCommand::contextSetFilesCommand()
         : contextCommand(Command::CN_CSETFILES, 
                          contextCommand::UNDEFINED_CONTEXT),
           files_()
      {

      }

      contextSetFilesCommand::contextSetFilesCommand(t_int const _context_id,
                                                     selectedFileEntryList const& _files)
         : contextCommand(Command::CN_CSETFILES,
                          _context_id),
           files_(_files)
      {
      }
               
      bool contextSetFilesCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( contextCommand::serialize(_e) );

         _e->setParamInfo("data: files", true);
         BTG_RCHECK( files_.serialize(_e) );
         BTG_RCHECK(_e->status());

         return true;
      }

      bool contextSetFilesCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         contextCommand::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("data: files", true);
         BTG_RCHECK( files_.deserialize(_e) );

         BTG_RCHECK(_e->status());

         return true;
      }

      selectedFileEntryList contextSetFilesCommand::getFiles() const
      {
         return files_;
      }

      contextSetFilesCommand::~contextSetFilesCommand()
      {

      }
         
      contextGetFilesCommand::contextGetFilesCommand()
         : contextCommand(Command::CN_CGETFILES, 
                          contextCommand::UNDEFINED_CONTEXT)
      {

      }

      contextGetFilesCommand::contextGetFilesCommand(t_int const _context_id)
         : contextCommand(Command::CN_CGETFILES, 
                          _context_id)
      {

      }

      contextGetFilesCommand::~contextGetFilesCommand()
      {
      }

      contextGetFilesResponseCommand::contextGetFilesResponseCommand()
         : contextCommand(Command::CN_CGETFILESRSP,
                          contextCommand::UNDEFINED_CONTEXT),
           files_()
      {

      }
      
      contextGetFilesResponseCommand::contextGetFilesResponseCommand(t_int const _context_id, 
                                                                     selectedFileEntryList const& _files)
         : contextCommand(Command::CN_CGETFILESRSP, 
                          _context_id),
           files_(_files)
      {
      }
               
      bool contextGetFilesResponseCommand::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( contextCommand::serialize(_e) );

         _e->setParamInfo("data: files", true);
         BTG_RCHECK( files_.serialize(_e) );

         BTG_RCHECK(_e->status());

         return true;
      }

      bool contextGetFilesResponseCommand::deserialize(btg::core::externalization::Externalization* _e)
      {
         contextCommand::deserialize(_e);
         BTG_RCHECK(_e->status());

         _e->setParamInfo("data: files", true);
         BTG_RCHECK( files_.deserialize(_e) );

         BTG_RCHECK(_e->status());

         return true;
      }

      selectedFileEntryList contextGetFilesResponseCommand::getFiles() const
      {
         return files_;
      }

      contextGetFilesResponseCommand::~contextGetFilesResponseCommand()
      {

      }

   } // namespace core
} // namespace btg
