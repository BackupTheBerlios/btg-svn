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

#include "configuration.h"
#include <bcore/logmacro.h>

std::string const WRITE_ERROR("Unable to write:");

namespace btg
{
   namespace core
   {

      Configuration::Configuration(LogWrapperType _logwrapper,
                                   std::string const& _filename)
         : Logable(_logwrapper),
           filename(_filename),
           inifile(0),
           opened(false),
           data_modified(false),
           errorDescriptionSet(false),
           errorDescription()
      {
         inifile = new ::CDataFile();
      }

      bool Configuration::read(bool const _force)
      {
         bool status = false;

         status  = inifile->Load(filename);

         if (!_force)
            {
               if (!status)
                  {
                     opened = false;
                  }
            }

         return status;
      }

      bool Configuration::write(bool const _force)
      {
         bool status = false;

         if (!inifile)
            {
               setErrorDescription(WRITE_ERROR + "internal error");
               return status;
            }

         if (!_force)
            {
               if (!opened)
                  {
                     setErrorDescription(WRITE_ERROR + "file not open");
                     // BTG_NOTICE("File not open.");
                     return status;
                  }
            }

         inifile->SetFileName(filename);
         status = inifile->Save();

         if (status)
            {
               inifile->Clear();
            }

         return status;
      }

      void Configuration::formatHeader(std::string & _output) const
      {
         _output += "INI file format:";
         _output += "\n";

      }

      void Configuration::formatSection(std::string const& _section,
                                        std::string & _output) const
      {
         std::string const sectionLabel("section: ");

         _output += sectionLabel;
         _output += "\"" + _section + "\"";
         _output += "\n";
      }

      void Configuration::formatKey(std::string const& _key,
                                    std::string const& _descr,
                                    std::vector<std::string> const& _values,
                                    std::string & _output) const
      {
         std::string const keyLabel  (" key  : ");
         std::string const valueLabel(" value: ");

         std::string const valueLabelBeg(" ( ");
         std::string const valueLabelEnd(" )");
         std::string const valueSep("|");

         _output += keyLabel;
         _output += "\"" + _key + "\"";
         _output += " (" + _descr + ")";
         _output += ".";

         _output += "\n";

         _output += valueLabel;

         t_uint counter = 0;
         std::vector<std::string>::const_iterator iter;
         for (iter = _values.begin();
              iter != _values.end();
              iter++)
            {
               _output += *iter;
               if (counter < (_values.size()-1))
                  {
                     _output += valueSep;
                  }
               counter++;
            }
         _output += ".";
         _output += "\n";
         _output += "\n";
      }

      void Configuration::setErrorDescription(std::string const& _operation, 
                                              std::string const& _section, 
                                              std::string const& _key)
      {
         std::string temp = 
            "Operation " + _operation + " on " + 
            _section + ":" + _key + " failed.";
         
         setErrorDescription(temp);
      }

      void Configuration::setErrorDescription(std::string const& _errordescription)
      {
         if (!errorDescriptionSet)
            {
               errorDescriptionSet = true;
               errorDescription    = _errordescription;
            }
      }

      bool Configuration::getErrorDescription(std::string & _errordescription) const
      {
         if (errorDescriptionSet)
            {
               _errordescription = errorDescription;
            }

         return errorDescriptionSet;
      }

       bool Configuration::writeSwitch(std::string const& _key,
                                       std::string const& _section,
                                       std::string const& _description,
                                       std::string const& _switchOn,
                                       std::string const& _switchOff,
                                       bool const _value)
       {
           std::string switchStr;
           bool status = true;
           
           if (_value)
               {
                   switchStr = _switchOn;
               }
            else
               {
                  switchStr = _switchOff;
               }

            if (!inifile->SetValue(_key,
                                   switchStr,
                                   _description,
                                   _section))
               {
                  setErrorDescription("write", _section, _key);
                  status = false;
               }
            
            return status;
       }
       
       void Configuration::readSwitch(std::string const& _key,
                                      std::string const& _section,
                                      std::string const& _switchOn,
                                      std::string const& _switchOff,
                                      bool & _value)
       {
           std::string switchStr = inifile->GetValue(_key, _section);

           if (switchStr.size() > 0)
           {
               if (switchStr == _switchOn)
               {
                   _value = true;
               }
               if (switchStr == _switchOff)
               {
                   _value = false;
               }
           }
           else
           {
               _value = false;
           }
       }
       
      Configuration::~Configuration()
      {
         // To avoid saving the file here, call a function which resets the
         // dirty bit used by the ini file implementation.
         // The inifile was saved, if needed in by the user of this
         // class calling the write function.
         inifile->Clear();

         delete inifile;
         inifile = 0;
      }

   } // namespace core
} // namespace btg

