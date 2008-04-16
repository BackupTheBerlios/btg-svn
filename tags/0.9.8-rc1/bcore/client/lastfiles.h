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

#ifndef LAST_FILES_H
#define LAST_FILES_H

#include <bcore/client/clientdynconfig.h>
#include <bcore/logable.h>

namespace btg
{
   namespace core
      {
         namespace client
            {

               /// Opened files history.
               class lastFiles: public Logable
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _logwrapper Pointer used to send logs to.
                     /// @param [in] cc          The dynamic client configuration data object
                     lastFiles(LogWrapperType _logwrapper,
                               clientDynConfig & _CDC);

                     /// Return true if the list was modified.
                     bool modified() const { return modified_; };

                     /// Get the list of last opened files.
                     const t_strList& get() const { return lastFiles_; }

                     /// Add a filename to the filename list.
                     void add(std::string const& _filename);

                     /// Remove a filename from the filename list.
                     void remove(std::string const& _filename);

                     /// Destructor.
                     virtual ~lastFiles();
                  
                  protected:
                     /// Sets data modification flag
                     /// @param [in] bMod true if data modified
                     void set_modified(bool const bMod);

                  private:
                     /// Dynamic configuration object
                     clientDynConfig & CDC_;

                     /// Indicates if the list was modified.
                     bool modified_;

                     /// List of last opened files.
                     /// (agregated by clientDynConfig now)
                     t_strList & lastFiles_;
                  };
            } // namespace client
      } // namespace core
} // namespace btg

#endif // LAST_FILES_H
