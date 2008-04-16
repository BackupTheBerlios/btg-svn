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

#include "command.h"
#include <bcore/project.h>
#include <bcore/t_string.h>
#include <bcore/helpermacro.h>

extern "C"
{
#include <assert.h>
}

namespace btg
{
   namespace core
   {
      /// Used for mapping command IDs to names.
      typedef std::map<t_int, std::string> t_IdToName;
      /// Used for mapping command IDs to names.
      typedef t_IdToName::value_type     t_IdToNameElem;
      /// Used for mapping command IDs to names.
      typedef t_IdToName::const_iterator t_IdToNameCI;

      t_long const Command::INVALID_SESSION   = -1;

      /// The command names array.
      static const t_IdToNameElem commandNames[] =
         {
            // General:
            t_IdToNameElem(Command::CN_GINITCONNECTION, "(g) Init connection"),
            t_IdToNameElem(Command::CN_GSETUP,          "(g) Setup command"),
            t_IdToNameElem(Command::CN_GSETUPRSP,       "(g) Setup command response"),
            t_IdToNameElem(Command::CN_GKILL,           "(g) Kill daemon"),
            t_IdToNameElem(Command::CN_GLIMIT,          "(g) limit"),
            t_IdToNameElem(Command::CN_GLIMITSTAT,      "(g) Limit stat request"),
            t_IdToNameElem(Command::CN_GLIMITSTATRSP,   "(g) Limit stat response"),
            t_IdToNameElem(Command::CN_GUPTIME,         "(g) Daemon uptime request"),
            t_IdToNameElem(Command::CN_GUPTIMERSP,      "(g) Daemon uptime response"),
            t_IdToNameElem(Command::CN_GLIST,           "(g) List request"),
            t_IdToNameElem(Command::CN_GLISTRSP,        "(g) List request response"),
            t_IdToNameElem(Command::CN_CCREATEWITHDATA, "(g) Create context"),
            t_IdToNameElem(Command::CN_CLAST,           "(g) Last context"),
            t_IdToNameElem(Command::CN_CLASTRSP,        "(g) Last context response"),
            t_IdToNameElem(Command::CN_CSTART,          "(c) Start context"),
            t_IdToNameElem(Command::CN_CSTOP,           "(c) Stop context"),
            t_IdToNameElem(Command::CN_CABORT,          "(c) Abort context"),
            t_IdToNameElem(Command::CN_CSTATUS,         "(c) Status"),
            t_IdToNameElem(Command::CN_CSTATUSRSP,      "(c) Status response"),
            t_IdToNameElem(Command::CN_CALLSTATUSRSP,   "(c) Status all response"),
            t_IdToNameElem(Command::CN_CMSTATUS,        "(c) Status, multiple"),
            t_IdToNameElem(Command::CN_CMSTATUSRSP,     "(c) Status, multiple response"),
            t_IdToNameElem(Command::CN_CFILEINFO,       "(c) File info"),
            t_IdToNameElem(Command::CN_CFILEINFORSP,    "(c) File info response"),
            t_IdToNameElem(Command::CN_CALLFILEINFORSP, "(c) File info all response"),
            t_IdToNameElem(Command::CN_CCLEAN,          "(c) Clean"),
            t_IdToNameElem(Command::CN_CCLEANRSP,       "(c) Clean response"),
            t_IdToNameElem(Command::CN_CLIMIT,          "(c) Limit"),
            t_IdToNameElem(Command::CN_CLIMITSTATUS,    "(c) Limit status"),
            t_IdToNameElem(Command::CN_CLIMITSTATUSRSP, "(c) Limit status response"),
            t_IdToNameElem(Command::CN_CPEERS,          "(c) List peers"),
            t_IdToNameElem(Command::CN_CPEERSRSP,       "(c) List peers response"),
            t_IdToNameElem(Command::CN_CSETFILES,       "(c) Set files"),
            t_IdToNameElem(Command::CN_CGETFILES,       "(c) Get selected files"),
            t_IdToNameElem(Command::CN_CGETFILESRSP,    "(c) Get selected files response"),
            t_IdToNameElem(Command::CN_CGETTRACKERS,    "(c) Get used trackers"),
            t_IdToNameElem(Command::CN_CGETTRACKERSRSP, "(c) Get used trackers response"),
            t_IdToNameElem(Command::CN_CMOVE,           "(c) Move"),
            t_IdToNameElem(Command::CN_CCREATEFROMURL,      "(c) Create context from URL"),
            t_IdToNameElem(Command::CN_CCREATEFROMURLRSP,   "(c) Create context from URL response"),
            t_IdToNameElem(Command::CN_CCREATEURLABORT,     "(c) Abort creation of context from URL."),
            t_IdToNameElem(Command::CN_CURLSTATUS,          "(c) URL download status"),
            t_IdToNameElem(Command::CN_CURLSTATUSRSP,       "(c) URL download status response"),
            t_IdToNameElem(Command::CN_CCREATEFROMFILE,     "(c) Create context from file"),
            t_IdToNameElem(Command::CN_CCREATEFROMFILERSP,  "(c) Create context from file response"),
            t_IdToNameElem(Command::CN_CCREATEFFABORT,      "(c) Abort creation of context from file."),
            t_IdToNameElem(Command::CN_CCREATEFROMFILEPART, "(c) Create context from file, part of file"),
            t_IdToNameElem(Command::CN_CCRFILESTATUS,       "(c) File download status"),
            t_IdToNameElem(Command::CN_CCRFILESTATUSRSP,    "(c) File download status response"),

            // General:
            t_IdToNameElem(Command::CN_ERROR,           "(g) Error"),
            t_IdToNameElem(Command::CN_ACK,             "(g) Ack"),
            // Session:
            t_IdToNameElem(Command::CN_SATTACH,         "(s) Attach"),
            t_IdToNameElem(Command::CN_SDETACH,         "(s) Detach"),
            t_IdToNameElem(Command::CN_SQUIT,           "(s) Quit"),
            t_IdToNameElem(Command::CN_SERROR,          "(s) Error"),
            t_IdToNameElem(Command::CN_SINFO,           "(s) Info"),
            t_IdToNameElem(Command::CN_SINFORSP,        "(s) Info response"),

            t_IdToNameElem(Command::CN_SLIST,           "(s) List"),
            t_IdToNameElem(Command::CN_SLISTRSP,        "(s) List response"),
            t_IdToNameElem(Command::CN_SNAME,           "(s) Session name"),
            t_IdToNameElem(Command::CN_SNAMERSP,        "(s) Session name response"),
            t_IdToNameElem(Command::CN_SSETNAME,        "(s) Set session name"),
            t_IdToNameElem(Command::CN_MOREAD,          "(s) RO mode"),
            t_IdToNameElem(Command::CN_MOWRITE,         "(s) RW mode"),
            t_IdToNameElem(Command::CN_VERSION,         "(s) Version"),
            t_IdToNameElem(Command::CN_VERSIONRSP,      "(s) Version response"),
            // Undefined.
            t_IdToNameElem(Command::CN_UNDEFINED,       "Undefined")
         };

      /// The size of the command names array.
      static t_int const commandNames_size = sizeof(commandNames)/sizeof(t_IdToNameElem);

      Command::Command()
         : initialized(false), type(CN_UNDEFINED)
      {}

      Command::Command(t_int const _type)
         : initialized(true), type(_type)
      {}

      t_int Command::getType() const
      {
         return this->type;
      }

      std::string Command::getName() const
      {
         return Command::getName(this->type);
      }

      std::string Command::getName(t_int const _type)
      {
         static const t_IdToName map(&commandNames[0], &commandNames[commandNames_size]);

         t_IdToNameCI iter = map.find(_type);

         if (iter != map.end())
            {
               return iter->second;
            }

         iter = map.find(CN_UNDEFINED);

         return iter->second;
      }

      bool Command::serialize(btg::core::externalization::Externalization* _e) const
      {
         // Write the command id.
         _e->setParamInfo("command type", true);
         BTG_RCHECK( _e->setCommand(this->type));

         return true;
      }

      bool Command::deserialize(btg::core::externalization::Externalization* _e)
      {
         // Make sure that this is a valid command.
         // Read the command id.
         _e->setParamInfo("command type", true);
         BTG_RCHECK( _e->getCommand(this->type) );

         return true;
      }

      bool Command::isInitialized() const
      {
         if (this->type != CN_UNDEFINED)
            {
               return true;
            }
         return false;
      }

      std::string Command::toString() const
      {
         std::string output;
         if (this->isInitialized())
            {
               output = this->getName() + ".";
            }
         else
            {
               output = this->getName() + ", not initialized.";
            }
         return output;
      }

      bool Command::operator== (const Command* _command)
      {
         if ( (this->type == _command->getType()) )
            {
               return true;
            }
         else
            {
               return false;
            }

      }

      Command::~Command()
      {
         // Empty as there is nothing to delete.
      }

   } // namespace core
} // namespace btg
