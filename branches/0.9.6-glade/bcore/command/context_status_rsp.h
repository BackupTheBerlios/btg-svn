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

#ifndef COMMAND_CONTEXT_STATUS_RESPONSE_H
#define COMMAND_CONTEXT_STATUS_RESPONSE_H

#include "context.h"
#include <bcore/type_btg.h>
#include <bcore/dbuf.h>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup commands
          */
         /** @{ */

         /// Command sent by the daemon to inform the client about the status
         /// of a context.
         class contextStatusResponseCommand: public contextCommand
            {
            public:
               /// Default constructor.
               contextStatusResponseCommand();
               /// Constructor.
               /// @param [in] _context_id     The context ID which this status is for.
               /// @param [in] _status         The status.
               contextStatusResponseCommand(t_int const _context_id,
                                            Status const& _status);
               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);
               /// Get the status transported by this command.
               Status getStatus() const;
               /// Destructor.
               virtual ~contextStatusResponseCommand();
            private:
               /// The status.
               Status status;
            };


         /// Command sent by the daemon to inform the client about the status
         /// of all contexts.
         class contextAllStatusResponseCommand: public contextCommand
            {
            public:
               /// Default constructor.
               contextAllStatusResponseCommand();

               /// Constructor.
               /// @param [in] _context_id     The context ID which this status is for.
               /// @param [in] _status         List of Status objects.
               contextAllStatusResponseCommand(t_int const _context_id,
                                               t_statusList const& _status);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Return a list of Status objects.
               t_statusList getStatus() const;
               /// Destructor.
               virtual ~contextAllStatusResponseCommand();
            private:
               /// List of Status objects.
               t_statusList status;
            };

         /// Command sent by the daemon to inform the client about the status
         /// of multiple contexts.
         class contextMultipleStatusResponseCommand: public contextCommand
            {
            public:
               /// Default constructor.
               contextMultipleStatusResponseCommand();

               /// Constructor.
               /// @param [in] _status         List of Status objects.
               contextMultipleStatusResponseCommand(t_statusList const& _status);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Return a list of Status objects.
               t_statusList getStatus() const;

               /// Destructor.
               virtual ~contextMultipleStatusResponseCommand();
            private:
               /// List of Status objects.
               t_statusList status;
            };

         /** @} */
      } // namespace core
} // namespace btg

#endif
