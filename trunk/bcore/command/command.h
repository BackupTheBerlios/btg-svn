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

#ifndef COMMAND_H
#define COMMAND_H

#include <bcore/project.h>
#include <bcore/serializable.h>
#include <bcore/printable.h>
#include <string>
#include <map>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// This class represents a generic command.
         /// All specific commands inherit from it.
         class Command: public Serializable, public Printable
            {
            public:
               /// Indicates that a session is invalid.
               static t_long const INVALID_SESSION;

               // The types of commands which are implemented and their responses.
               /// As the entries in this enum are also used for finding command
               /// names from a static array, its important that their values
               /// start at 1 and only increase by one for each entry.
               /// Remember to add new commands to command.cpp and externalization.cpp too!
               enum commandType
                  {
                     CN_GINITCONNECTION = 1, //!< Init connection.
                     CN_GSETUP,            //!< Setup command.
                     CN_GSETUPRSP,         //!< Setup response command.
                     CN_GKILL,             //!< Bring down the daemon.
                     CN_GLIMIT,            //!< Set global limits.
                     CN_GLIMITSTAT,        //!< Request the current global limit.
                     CN_GLIMITSTATRSP,     //!< Response to request for the current global limit.
                     CN_GUPTIME,           //!< Get daemon uptime.
                     CN_GUPTIMERSP,        //!< Daemon uptime response.
                     CN_GLIST,             //!< List request.
                     CN_GLISTRSP,          //!< List request response.
                     CN_CCREATEWITHDATA,   //!< Create a context, using embedded data.
                     CN_CLAST,             //!< Request the context ID of the last created context.
                     CN_CLASTRSP,          //!< Context ID of the last created context.
                     CN_CSTART,            //!< Start a context.
                     CN_CSTOP,             //!< Stop a context.
                     CN_CABORT,            //!< Abort a context.
                     CN_CSTATUS,           //!< Request status information about a context.
                     CN_CSTATUSRSP,        //!< Status information about one context.
                     CN_CALLSTATUSRSP,     //!< Status information about all contexts.
                     CN_CMSTATUS,          //!< Status information, a number of contexts.
                     CN_CMSTATUSRSP,       //!< Status information, a number of contexts - response.
                     CN_CFILEINFO,         //!< Request file info information about a context.
                     CN_CFILEINFORSP,      //!< File info information about a context.
                     CN_CALLFILEINFORSP,   //!< File info information about all contexts.
                     CN_CCLEAN,            //!< Request to clean finished contexts.
                     CN_CCLEANRSP,         //!< Response that informs the client that some contexts have been cleaned.
                     CN_CLIMIT,            //!< Limit a context.
                     CN_CLIMITSTATUS,      //!< Limit status.
                     CN_CLIMITSTATUSRSP,   //!< Limit status response.
                     CN_CPEERS,            //!< Request a list of peers.
                     CN_CPEERSRSP,         //!< Response containing a list of peers.
                     CN_CSETFILES,         //!< Set the files (inside torrent) to download.
                     CN_CGETFILES,         //!< Get the selected files.
                     CN_CGETFILESRSP,      //!< Response containing the selected files
                     CN_CGETTRACKERS,      //!< Get the list of used trackers.
                     CN_CGETTRACKERSRSP,   //!< List of trackers used for a context.
                     CN_ERROR,             //!< General error.
                     CN_ACK,               //!< General acknowlegment.
                     CN_SATTACH,           //!< Attach to a running session.
                     CN_SDETACH,           //!< Detach from a running session.
                     CN_SERROR,            //!< Daemon uses this command to tell clients that their session is invalid.
                     CN_SQUIT,             //!< Quit the current session.
                     CN_SLIST,             //!< Get a list of sessions.
                     CN_SLISTRSP,          //!< Response that lists sessions.
                     CN_SNAME,             //!< Get current session name.
                     CN_SNAMERSP,          //!< Current session name.
                     CN_SSETNAME,          //!< Set current session name.
                     CN_MOREAD,            //!< Only read only access. Usefull for writing monitor applets.
                     CN_MOWRITE,           //!< Default write mode.
                     CN_UNDEFINED          //!< Undefined command.
                  };

            public:
               /// The default constructor. Creates an undefined command.
               Command();

               /// Create a specific command.
               /// @param [in] _type The type of this command.
               Command(t_int const _type);

               /// Get the type of command.
               t_int getType() const;

               /// Get the textual name of this command.
               std::string getName() const;

               /// Get the textual name of a command.
               /// @param _type [in] Type of command.
               /// @return The name or "Name mismatch." if _type is not a command type.
               static std::string getName(t_int const _type);

               // Children must implement these functions:

               /// This is useful as the default constructor create invalid (in this context) objects.
               /// Those invalid objects are used for deserialization.
               bool isInitialized() const;

               /// The equality operator.
               virtual bool operator== (const Command* _command);

               virtual bool deserialize(btg::core::externalization::Externalization* _e);
               virtual bool serialize(btg::core::externalization::Externalization* _e) const;

               /// Get a string which describes this command and its arguments.
               virtual std::string toString() const;

               /// Destructor.
               virtual ~Command();

            protected:
               /// Used to indicate if an instance has been initialized.
               /// This means that it got a type.
               bool initialized;

               /// The type of this command.
               t_int type;
            };

         /* @} */

      } // namespace core
} // namespace btg



#endif


