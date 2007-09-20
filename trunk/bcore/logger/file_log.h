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
 * $Id: file_log.h,v 1.6.4.1 2006/04/05 18:44:10 wojci Exp $
 */

#ifndef FILE_LOGGER_H
#define FILE_LOGGER_H

#include "logger.h"
#include <fstream>

namespace btg
{
   namespace core
      {
         namespace logger
            {
               /**
                * \addtogroup logger
                */
               /** @{ */

               /// Write log to a file.
               class fileLogger : public logBuffer
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _filename The file name used for writing.
                     fileLogger(std::string _filename);
                     /// Open the logfile.
                     bool open();
                     /// Close the logfile.
                     bool close();
                     /// Destructor.
                     virtual ~fileLogger();
                  private:
                     /// The filename.
                     std::string filename;
                     /// Used for writing to the file.
                     std::ofstream file;
                     /// Write the buffer to the file.
                     void put_buffer();
                     /// Write a character to the file.
                     void put_char(int _char);
                  };

               /** @} */

            } // namespace logger
      } // namespace core
} // namespace btg

#endif
