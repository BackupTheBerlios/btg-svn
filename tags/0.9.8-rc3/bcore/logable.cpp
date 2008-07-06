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

#include "logable.h"

namespace btg
{
   namespace core
   {

      Logable::Logable(LogWrapperType _logwrapper)
         : logwrapper_(_logwrapper)
      {

      }

      /// Get the logwrapper.
      LogWrapperType Logable::logWrapper() const
      {
         return logwrapper_;
      }

      Logable::Logable(Logable const& _logable)
         : logwrapper_(_logable.logwrapper_)
      {

      }

      Logable& Logable::operator= (Logable const& _logable)
      {
         bool status = (_logable == *this);

         if (!status)
            {
               logwrapper_ = _logable.logwrapper_;
            }

         return *this;
      }

      bool Logable::operator== (Logable const& _logable) const
      {
         return (_logable.logwrapper_ == logwrapper_);
      }

      /// Destructor.
      Logable::~Logable() 
      {
      }

   } // namespace core
} // namespace btg

