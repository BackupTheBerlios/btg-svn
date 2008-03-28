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

#ifndef PIDFILE_H_
#define PIDFILE_H_

#include <bcore/os/fstream.h>

namespace btg
{
   namespace core
      {
         namespace os
            {
               /**
                * \addtogroup OS
                */
               /** @{ */

               class PIDFile
                  {
                     typedef int pid_type;
                     
                     btg::core::os::fstream m_pidfile;
                     bool m_error; ///< file creation error
                     bool m_exists; ///< process already exists
                     pid_type m_pid; ///< current or existing(from file) PID
                     const char * m_fname; ///< PID-file name
                  public:
                     /// Calls create()
                     PIDFile(const char * _fname = NULL);
                     
                     /// Creates pidfile, checks process existance
                     /// @param [in] _fname PID-file name
                     void create(const char * _fname);
                     
                     /// Return error flag (file creation error)
                     bool error() const { return m_error; }
                     
                     /// Return exists flag (another process exists with PID in PID-file)
                     bool exists() const { return m_exists; }
                     
                     /// Return existing (in current PID-file) or current process PID
                     pid_type pid() const { return m_pid; }
                     
                     /// Clear internal state (reset flags, closes file)
                     void clear();
                     
                     /// write current PID to PID-file
                     void write();
                     
                     virtual ~PIDFile();
                  };

               /** @} */

            } // namespace os
      } // namespace core
} // namespace btg

#endif /*PIDFILE_H_*/
