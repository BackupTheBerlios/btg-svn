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
         
         class OptionBase: public Printable
         {
         public:
            enum Option
            {
               SS             = 0, // Session saving enabled.
               PERIODIC_SS    = 1, // Periodic session saving enabled.
               UPNP           = 2, // UPNP enabled.
               DHT            = 3, // DHT enabled.
               ENCRYPTION     = 4, // Encryption enabled.
               URL            = 5, // URL loading enabled.
               SELECTIVE_DL   = 6, // Selective download.
               OPTION_7       = 7,
               OPTION_8       = 8,
               OPTION_9       = 9,
               OPTION_10      = 10,
               OPTION_11      = 11,
               OPTION_12      = 12,
               OPTION_13      = 13,
               OPTION_14      = 14,
               OPTION_15      = 15,
               OPTION_16      = 16,
               OPTION_17      = 17,
               OPTION_18      = 18,
               OPTION_19      = 19,
               OPTION_20      = 20,
               OPTION_21      = 21,
               OPTION_22      = 22,
               OPTION_23      = 23,
               OPTION_24      = 24,
               OPTION_25      = 25,
               OPTION_26      = 26,
               OPTION_27      = 27,
               OPTION_28      = 28,
               OPTION_29      = 29,
               OPTION_30      = 30,
               OPTION_31      = 31
            };
         public:
            /// Default constructor.
            OptionBase();

            /// Constructor.
            OptionBase(t_byte const _mayor, 
                       t_byte const _minor, 
                       t_byte const _revision);
            
            virtual bool getOption(OptionBase::Option const _option) const;

            virtual void getVersion(t_byte & _mayor,
                                    t_byte & _minor,
                                    t_byte & _revision) const;

            std::string toString() const;
         protected:
            t_byte ver_mayor;
            t_byte ver_minor;
            t_byte ver_revision;
            
            /// Options.
            t_byte bytes[4];
         };
         /** @} */
         
      } // namespace core
} // namespace btg

#endif // COPTS_H
