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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <bcore/type.h>

#include <string>
#include <vector>
#include <map>

#include <external/ini/cdatafile.h>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup core
          */
         /** @{ */

         /// Base class for reading and writing configuration files.
         class Configuration
            {
            public:
               /// Constructor.
               Configuration(std::string const& _filename);

               /// Read the file.
               /// @param [in] _force Force reading of the file.
               virtual bool read(bool const _force = false);

               /// Write the file.
               /// @param [in] _force Force saving of the file.
               virtual bool write(bool const _force = false);

               /// Return true if the configuration was modified.
               virtual bool modified() const { return data_modified; };

               /// Get a string describing the syntax of this file.
               virtual std::string getSyntax() const = 0;

               /// Get the textual description of an error.
               virtual bool getErrorDescription(std::string & _errordescription) const;

               /// Destructor.
               virtual ~Configuration();
            protected:
               /// The filename.
               std::string const   filename;

               /// Ini file handler.
               ::CDataFile*        inifile;

               /// Flag indicating that the file was opened.
               bool                opened;

               /// Flag inidicating that the configuration was
               /// modified, so it needs to be written back to file.
               bool                data_modified;
               
               /// If true, it indicates that an error occured and a
               /// textual error description was set.
               bool                errorDescriptionSet;
               /// If an error occured this member contains a textual
               /// error description.
               std::string         errorDescription;

               /// Store an error message concerning an operation on
               /// section:key.
               virtual void setErrorDescription(std::string const& _operation,
                                                std::string const& _section,
                                                std::string const& _key);

               /// Set an error description.
               /// Only the first error will be logged, the following
               /// will be ignored.
               virtual void setErrorDescription(std::string const& _errordescription);

               /// Used for printing syntax. Format the header.
               void formatHeader(std::string & _output) const;

               /// Used for printing syntax. Format a section.
               void formatSection(std::string const& _section,
                                  std::string & _output) const;

               /// Used for printing syntax. Format a key and values.
               void formatKey(std::string const& _key,
                              std::string const& _descr,
                              std::vector<std::string> const& _values,
                              std::string & _output) const;

                /// Read the value (on/off) of a switch.
                void readSwitch(std::string const& _key,
                                std::string const& _section,
                                std::string const& _switchOn,
                                std::string const& _switchOff,
                                bool & _value);
                
                /// Write the value (on/off) of a switch.
                bool writeSwitch(std::string const& _key,
                               std::string const& _section,
                               std::string const& _description,
                               std::string const& _switchOn,
                               std::string const& _switchOff,
                               bool const _value);
            private:
               /// Copy constructor.
               Configuration(Configuration const& _c);
               /// Assignment operator.
               Configuration& operator=(Configuration const& _c);
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif // CONFIGURATION_H

