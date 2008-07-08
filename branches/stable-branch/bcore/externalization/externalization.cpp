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

#include "externalization.h"
#include "ext_macro.h"
#include <bcore/command/command.h>

namespace btg
{
   namespace core
   {
      namespace externalization
      {
         /// Used for mapping command IDs to method names.
         typedef std::map<t_int, std::string> t_IdToName;
         /// Used for mapping command IDs to method names.
         typedef t_IdToName::value_type     t_IdToNameElem;
         /// Used for mapping command IDs to method names.
         typedef t_IdToName::const_iterator t_IdToNameCI;

         /// The method name array.
         static const t_IdToNameElem commandMethodNames[] =
            {
               // General:
               t_IdToNameElem(Command::CN_GINITCONNECTION, "general.initconnection"),
               t_IdToNameElem(Command::CN_GSETUP,          "general.setup"),
               t_IdToNameElem(Command::CN_GSETUPRSP,       "general.setuprsp"),
               t_IdToNameElem(Command::CN_GKILL,           "general.kill"),
               t_IdToNameElem(Command::CN_GLIMIT,          "general.limit"),
               t_IdToNameElem(Command::CN_GLIMITSTAT,      "general.limitstat"),
               t_IdToNameElem(Command::CN_GLIMITSTATRSP,   "general.limitstatrsp"),
               t_IdToNameElem(Command::CN_GUPTIME,         "general.uptime"),
               t_IdToNameElem(Command::CN_GUPTIMERSP,      "general.uptimersp"),
               t_IdToNameElem(Command::CN_GLIST,           "general.list"),
               t_IdToNameElem(Command::CN_GLISTRSP,        "general.listrsp"),
               // Context:
               t_IdToNameElem(Command::CN_CCREATEWITHDATA, "context.createwithdata"),
               t_IdToNameElem(Command::CN_CLAST,           "context.last"),
               t_IdToNameElem(Command::CN_CLASTRSP,        "context.lastrsp"),
               t_IdToNameElem(Command::CN_CCREATEWITHDATA, "context.createwithdata"),
               t_IdToNameElem(Command::CN_CSTART,          "context.start"),
               t_IdToNameElem(Command::CN_CSTOP,           "context.stop"),
               t_IdToNameElem(Command::CN_CABORT,          "context.abort"),
               t_IdToNameElem(Command::CN_CSTATUS,         "context.status"),
               t_IdToNameElem(Command::CN_CSTATUSRSP,      "context.statusrsp"),
               t_IdToNameElem(Command::CN_CALLSTATUSRSP,   "context.statusallrsp"),
               t_IdToNameElem(Command::CN_CMSTATUS,        "context.statusm"),
               t_IdToNameElem(Command::CN_CMSTATUSRSP,     "context.statusmrsp"),
               t_IdToNameElem(Command::CN_CFILEINFO,       "context.fileinfo"),
               t_IdToNameElem(Command::CN_CFILEINFORSP,    "context.fileinforsp"),
               t_IdToNameElem(Command::CN_CALLFILEINFORSP, "context.fileinfoallrsp"),
               t_IdToNameElem(Command::CN_CCLEAN,          "context.clean"),
               t_IdToNameElem(Command::CN_CCLEANRSP,       "context.cleanrsp"),
               t_IdToNameElem(Command::CN_CLIMIT,          "context.limit"),
               t_IdToNameElem(Command::CN_CLIMITSTATUS,    "context.limitstatus"),
               t_IdToNameElem(Command::CN_CLIMITSTATUSRSP, "context.limitstatusrsp"),
               t_IdToNameElem(Command::CN_CPEERS,          "context.peers"),
               t_IdToNameElem(Command::CN_CPEERSRSP,       "context.peersrsp"),
               t_IdToNameElem(Command::CN_CSETFILES,       "context.setfiles"),
               t_IdToNameElem(Command::CN_CGETFILES,       "context.getfiles"),
               t_IdToNameElem(Command::CN_CGETFILESRSP,    "context.getfilesrsp"),
               t_IdToNameElem(Command::CN_CGETTRACKERS,    "context.gettrackers"),
               t_IdToNameElem(Command::CN_CGETTRACKERSRSP, "context.gettrackersrsp"),
               t_IdToNameElem(Command::CN_CMOVE,           "context.move"),
               t_IdToNameElem(Command::CN_CCREATEFROMURL,      "context.createfromurl"),
               t_IdToNameElem(Command::CN_CCREATEFROMURLRSP,   "context.createfromurlrsp"),
               t_IdToNameElem(Command::CN_CCREATEFROMFILE,     "context.createfromfile"),
               t_IdToNameElem(Command::CN_CCREATEFROMFILERSP,  "context.createfromfilersp"),
               t_IdToNameElem(Command::CN_CCREATEFROMFILEPART, "context.createfromfilepart"),

               t_IdToNameElem(Command::CN_OPABORT,             "general.opabort"),
               t_IdToNameElem(Command::CN_OPSTATUS,            "general.opstatus"),
               t_IdToNameElem(Command::CN_OPSTATUSRSP,         "general.opstatusrsp"),

               // General:
               t_IdToNameElem(Command::CN_ERROR,           "general.error"),
               t_IdToNameElem(Command::CN_ACK,             "general.ack"),
               // Session:
               t_IdToNameElem(Command::CN_SATTACH,         "session.attach"),
               t_IdToNameElem(Command::CN_SDETACH,         "session.detach"),
               t_IdToNameElem(Command::CN_SQUIT,           "session.quit"),
               t_IdToNameElem(Command::CN_SERROR,          "session.error"),
               t_IdToNameElem(Command::CN_SINFO,           "session.info"),
               t_IdToNameElem(Command::CN_SINFORSP,        "session.inforsp"),
               t_IdToNameElem(Command::CN_SLIST,           "session.list"),
               t_IdToNameElem(Command::CN_SLISTRSP,        "session.listrsp"),
               t_IdToNameElem(Command::CN_SNAME,           "session.name"),
               t_IdToNameElem(Command::CN_SNAMERSP,        "session.namersp"),
               t_IdToNameElem(Command::CN_SSETNAME,        "session.setname"),
               t_IdToNameElem(Command::CN_MOREAD,          "session.romode"),
               t_IdToNameElem(Command::CN_MOWRITE,         "session.rwmode"),
               t_IdToNameElem(Command::CN_VERSION,         "session.version"),
               t_IdToNameElem(Command::CN_VERSIONRSP,      "session.versionrsp"),

               // Undefined.
               t_IdToNameElem(Command::CN_UNDEFINED,       "Undefined")
            };

         /// The size of the method names array.
         static t_int const commandMethodNames_size = sizeof(commandMethodNames)/sizeof(t_IdToNameElem);

         std::string Externalization::getCommandName(t_int const _type)
         {
            static const t_IdToName map(&commandMethodNames[0], &commandMethodNames[commandMethodNames_size]);

            if(map.find(_type) == map.end())
               {
                  return map.find(Command::CN_UNDEFINED)->second;
               }

            return map.find(_type)->second;
         }

         t_int Externalization::getCommandID(std::string const _methodName)
         {
            static const t_IdToName map(&commandMethodNames[0], &commandMethodNames[commandMethodNames_size]);

            t_int result;
            t_IdToNameCI iter;
            result = Command::CN_UNDEFINED;

            for(iter = map.begin(); iter != map.end(); iter++)
               {
                  if(_methodName == iter->second)
                     {
                        result = iter->first;
                        break;
                     }
               }

            return result;
         }

         Externalization::Externalization(LogWrapperType _logwrapper)
            : btg::core::Logable(_logwrapper),
              status_(false)
         {
         }

         Externalization::~Externalization()
         {
         }

         bool Externalization::status() const
         {
            return status_;
         }

         void Externalization::failture()
         {
            status_ = false;
         }

         void Externalization::success()
         {
            status_ = true;
         }

      } // namespace externalization
   } // namespace core
} // namespace btg
