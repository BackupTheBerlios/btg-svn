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

               /// The types of commands which are implemented and
               /// their responses.
               /// 
               /// \note Do _not_ change the numeric IDs of existing
               /// commands. Only add new commands to this enum.
               /// 
               /// Remember to add new commands to command.cpp and
               /// externalization.cpp too. If not updated, the
               /// btg::core unittest will fail.
               enum commandType
                  {
                     /* Global commands. */
                     CN_GINITCONNECTION = 1,  //!< Init connection.
                     CN_GSETUP          = 10, //!< Setup command.
                     CN_GSETUPRSP       = 11, //!< Setup response command.
                     CN_GKILL           = 12, //!< Bring down the daemon.
                     CN_GLIMIT          = 13, //!< Set global limits.
                     CN_GLIMITSTAT      = 14, //!< Request the current global limit.
                     CN_GLIMITSTATRSP   = 15, //!< Response to request for the current global limit.
                     CN_GUPTIME         = 16, //!< Get daemon uptime.
                     CN_GUPTIMERSP      = 17, //!< Daemon uptime response.
                     CN_GLIST           = 18, //!< List request.
                     CN_GLISTRSP        = 19, //!< List request response.
                     /* Context commands.*/
                     CN_CCREATEWITHDATA = 40, //!< Create a context, using embedded data.
                     CN_CLAST           = 41, //!< Request the context ID of the last created context.
                     CN_CLASTRSP        = 42, //!< Context ID of the last created context.
                     CN_CSTART          = 43, //!< Start a context.
                     CN_CSTOP           = 44, //!< Stop a context.
                     CN_CABORT          = 45, //!< Abort a context.
                     CN_CSTATUS         = 46, //!< Request status information about a context.
                     CN_CSTATUSRSP      = 47, //!< Status information about one context.
                     CN_CALLSTATUSRSP   = 48, //!< Status information about all contexts.
                     CN_CMSTATUS        = 49, //!< Status information, a number of contexts.
                     CN_CMSTATUSRSP     = 50, //!< Status information, a number of contexts - response.
                     CN_CFILEINFO       = 51, //!< Request file info information about a context.
                     CN_CFILEINFORSP    = 52, //!< File info information about a context.
                     CN_CALLFILEINFORSP = 53, //!< File info information about all contexts.
                     CN_CCLEAN          = 54, //!< Request to clean finished contexts.
                     CN_CCLEANRSP       = 55, //!< Clean response.
                     CN_CLIMIT          = 56, //!< Limit a context.
                     CN_CLIMITSTATUS    = 57, //!< Limit status.
                     CN_CLIMITSTATUSRSP = 58, //!< Limit status response.
                     CN_CPEERS          = 59, //!< Request a list of peers.
                     CN_CPEERSRSP       = 60, //!< Response containing a list of peers.
                     CN_CSETFILES       = 61, //!< Set the files (inside torrent) to download.
                     CN_CGETFILES       = 62, //!< Get the selected files.
                     CN_CGETFILESRSP    = 63, //!< Response containing the selected files
                     CN_CGETTRACKERS    = 64, //!< Get the list of used trackers.
                     CN_CGETTRACKERSRSP = 65, //!< List of trackers used for a context.
                     CN_CMOVE           = 66, //!< Move a context from one session to another.
                     CN_CCREATEFROMURL  = 67, //!< Create context from an URL.
                     CN_CCREATEFROMURLRSP = 68, //!< Create context from an URL, response.
                     CN_CURLSTATUS        = 69, //!< Request URL download status.
                     CN_CURLSTATUSRSP     = 70, //!< URL download status response.

                     /* General commands. */ 
                     CN_ERROR           = 100, //!< General error.
                     CN_ACK             = 101, //!< General acknowlegment.
                     /* Session commands. */ 
                     CN_SATTACH         = 120, //!< Attach to a running session.
                     CN_SDETACH         = 121, //!< Detach from a running session.
                     CN_SERROR          = 122, //!< Daemon uses this command to tell clients that their session is invalid.
                     CN_SINFO           = 123, //!< Get information about a session.
                     CN_SINFORSP        = 124, //!< Information about a session.
                     CN_SQUIT           = 125, //!< Quit the current session.
                     CN_SLIST           = 126, //!< Get a list of sessions.
                     CN_SLISTRSP        = 127, //!< Response that lists sessions.
                     CN_SNAME           = 128, //!< Get current session name.
                     CN_SNAMERSP        = 129, //!< Current session name.
                     CN_SSETNAME        = 130, //!< Set current session name.
                     CN_MOREAD          = 131, //!< Only read only access. Usefull for writing monitor applets.
                     CN_MOWRITE         = 132, //!< Default write mode.

                     CN_VERSION         = 133, //!< Get daemon version and settings.
                     CN_VERSIONRSP      = 134, //!< Get daemon version and settings response.

                     CN_UNDEFINED       = 254  //!< Undefined command.
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


