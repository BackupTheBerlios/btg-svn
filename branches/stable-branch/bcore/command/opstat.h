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
 * Id    : $Id$
 */

#ifndef COMMAND_OPSTAT_H
#define COMMAND_OPSTAT_H

#include <bcore/command/command.h>
#include <bcore/sbuf.h>
#include <string>

namespace btg
{
   namespace core
   {
      /**
       * \addtogroup commands
       */
      /** @{ */

      /// Get the operation status of a command.
      class opStatusCommand: public Command
      {
      public:
         /// Constructor.
         opStatusCommand();

         /// Constructor.
         /// @param [in] _id   The id of the command.
         /// @param [in] _type The type of the command.
         opStatusCommand(t_uint const _id, 
                         t_uint const _type);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained id.
         t_uint id() const
         {
            return id_;
         }

         /// Get the contained type.
         t_uint type() const
         {
            return type_;
         }

         /// Destructor.
         virtual ~opStatusCommand();
      private:
         /// The id of the url to load.
         t_uint id_;

         /// The contained type.
         t_uint type_;
      };

      /// Operation status of a command.
      class opStatusResponseCommand: public Command
      {
      public:
         /// Constructor.
         opStatusResponseCommand();

         /// Constructor.
         /// @param [in] _id     The ID of the operation the response is about.
         /// @param [in] _type   The type of the operation the response is about.
         /// @param [in] _status The status.
         opStatusResponseCommand(t_uint _id, 
                                 t_uint _type,
                                 t_uint _status);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained id.
         t_uint id() const
         {
            return id_;
         }
         
         /// Get the contained type.
         t_uint type() const
         {
            return type_;
         }

         /// Get the contained status.
         t_uint status() const
         {
            return status_;
         }
         
         /// Indicates if data is set.
         bool dataSet() const
         {
            return dataSet_;
         }

         /// Set the contained data, set flag indicating that data is
         /// set.
         void setData(std::string const& _data);
 
         /// Get the contained data, if any.
         std::string data() const
            {
               return data_;
            }

         /// Destructor.
         virtual ~opStatusResponseCommand();
      private:
         /// The id of the contained status
         t_uint id_;

         /// The contained type.
         t_uint type_;

         /// The contained status.
         t_uint status_;

         /// Indicates if data is set.
         bool dataSet_;

         /// The contained data, if any.
         std::string data_; 
      };

      /// Abort an operation identified by an id and type.
      class opAbortCommand: public Command
      {
      public:
         /// Constructor.
         opAbortCommand();

         /// Constructor.
         /// @param [in] _id Id of the operation to abort.
         /// @param [in] _type   The type of the operation the response is about.
         opAbortCommand(t_uint const _id, 
                        t_uint const _type);

         bool serialize(btg::core::externalization::Externalization* _e) const;
         bool deserialize(btg::core::externalization::Externalization* _e);

         /// Get the contained id.
         t_uint id() const
         {
            return id_;
         }

         /// Get the contained type.
         t_uint type() const
         {
            return type_;
         }

         /// Destructor.
         virtual ~opAbortCommand();
      private:
         /// The id of the operation to abort.
         t_uint id_;
         /// The type of abort.
         t_uint type_;
      };

      /** @} */

   } // namespace core
} // namespace btg

#endif // COMMAND_OPSTAT_H

