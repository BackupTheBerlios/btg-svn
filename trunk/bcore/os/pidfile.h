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
#include <bcore/type.h>

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

               typedef t_int pid_type;

               /// Represents a pid file used by a BTG application.
               class PIDFile
                  {
                  public:
                     /// Constructor.
                     /// Calls create().
                     PIDFile(const char * _fname = NULL);
                     
                     /// Creates pidfile, checks process existance.
                     /// @param [in] _fname PID-file name.
                     void create(const char * _fname);
                     
                     /// Return error flag (file creation error).
                     bool error() const { return m_error; }
                     
                     /// Return exists flag (another process exists with PID in PID-file).
                     bool exists() const { return m_exists; }
                     
                     /// Return existing (in current PID-file) or current process PID.
                     pid_type pid() const { return m_pid; }
                     
                     /// Clear internal state (reset flags, closes file).
                     void clear();
                     
                     /// Write current PID to PID-file.
                     void write();
                     
                     /// Destructor.
                     virtual ~PIDFile();
                  private:
                     /// Stream used for reading/writing the PID file.
                     btg::core::os::fstream m_pidfile;
                     /// file creation error.
                     bool m_error; 
                     /// process already exists.
                     bool m_exists; 
                     /// current or existing(from file) PID.
                     pid_type m_pid; 
                     /// PID-file name.
                     std::string m_fname; 
                  };

               /** @} */

            } // namespace os
      } // namespace core
} // namespace btg

#endif /*PIDFILE_H_*/
