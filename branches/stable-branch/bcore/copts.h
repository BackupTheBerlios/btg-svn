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

#ifndef COPTS_H
#define COPTS_H

#include <string>
#include <bcore/type.h>
#include <bcore/printable.h>
namespace btg
{
   namespace core
      {

         /**
          * \addtogroup core
          */
         /** @{ */
         
         /// Modify _output to contain a description of the compile
         /// time options used.
         void getCompileTimeOptions(std::string & _output);
         
         /// Contains a mask which details compile time options.
         class OptionBase: public Printable
         {
         public:
            /// Different options.
            enum Option
            {
               SS             = 0, /// Session saving enabled.
               PERIODIC_SS    = 1, /// Periodic session saving enabled.
               UPNP           = 2, /// UPNP enabled.
               DHT            = 3, /// DHT enabled.
               ENCRYPTION     = 4, /// Encryption enabled.
               URL            = 5, /// URL loading enabled.
               SELECTIVE_DL   = 6, /// Selective download.
               OPTION_7       = 7, /// Placeholder.
               OPTION_8       = 8, /// Placeholder.
               OPTION_9       = 9, /// Placeholder.
               OPTION_10      = 10, /// Placeholder.
               OPTION_11      = 11, /// Placeholder.
               OPTION_12      = 12, /// Placeholder.
               OPTION_13      = 13, /// Placeholder.
               OPTION_14      = 14, /// Placeholder.
               OPTION_15      = 15, /// Placeholder.
               OPTION_16      = 16, /// Placeholder.
               OPTION_17      = 17, /// Placeholder.
               OPTION_18      = 18, /// Placeholder.
               OPTION_19      = 19, /// Placeholder.
               OPTION_20      = 20, /// Placeholder.
               OPTION_21      = 21, /// Placeholder.
               OPTION_22      = 22, /// Placeholder.
               OPTION_23      = 23, /// Placeholder.
               OPTION_24      = 24, /// Placeholder.
               OPTION_25      = 25, /// Placeholder.
               OPTION_26      = 26, /// Placeholder.
               OPTION_27      = 27, /// Placeholder.
               OPTION_28      = 28, /// Placeholder.
               OPTION_29      = 29, /// Placeholder.
               OPTION_30      = 30, /// Placeholder.
               OPTION_31      = 31  /// Placeholder.
            };
         public:
            /// Default constructor.
            OptionBase();

            /// Constructor.
            OptionBase(t_byte const _mayor, 
                       t_byte const _minor, 
                       t_byte const _revision);
            
            /// Indicates if an option is set.
            virtual bool getOption(OptionBase::Option const _option) const;

            /// Get version of the daemon.
            virtual void getVersion(t_byte & _mayor,
                                    t_byte & _minor,
                                    t_byte & _revision) const;

            /// Convert an instance of this class to a string.
            std::string toString() const;
         protected:
            /// Version, mayor.
            t_byte ver_mayor;
            /// Version, minor.
            t_byte ver_minor;
            /// Version, revision.
            t_byte ver_revision;
            
            /// Options.
            t_byte bytes[4];
         };
         /** @} */
         
      } // namespace core
} // namespace btg

#endif // COPTS_H
