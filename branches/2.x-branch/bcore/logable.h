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

#ifndef LOGABLE_H
#define LOGABLE_H

#include <bcore/logger/logger.h>

namespace btg
{
   namespace core
   {

      /**
       * \addtogroup core
       */
      /** @{ */

      typedef boost::shared_ptr<btg::core::logger::logWrapper> LogWrapperType;

      /// Interface: classes inheriting this class can log.
      class Logable
      {
      public:
         /// Constructor.
         Logable(LogWrapperType _logwrapper);

         /// Copy constructor.
         Logable(Logable const& _logable);

         /// Get the logwrapper.
         LogWrapperType logWrapper() const;

         /// The assignment operator.
         Logable& operator= (Logable const& _logable);

         /// The equality operator.
         bool operator== (Logable const& _logable) const;

         /// Destructor.
         ~Logable();
      protected:
         /// The logwrapper used.
         LogWrapperType logwrapper_;
      };

      /** @} */

   } // namespace core
} // namespace btg

#endif // PRINTABLE_H
