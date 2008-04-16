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

#include "pidfile.h"

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

namespace btg
{
   namespace core
   {
      namespace os
      {
         
         PIDFile::PIDFile(const char * _fname)
         {
            create(_fname);
         }
         
         void PIDFile::create(const char * _fname)
         {
            clear();
            if (!_fname)
               return;
            m_fname = _fname;
            m_pidfile.clear();
            m_pidfile.open(_fname);
            if (m_pidfile.is_open())
            {
               int pid;
               m_pidfile >> pid;
               if (m_pidfile.fail())
               {
                  m_pidfile.clear();
                  m_pidfile.seekp(0);
                  m_pidfile.truncate(0);
               }
               else
               {
                  if (
                     kill(pid,0)!=-1 // killed successfully
                     || errno == EPERM // process is present, but as other UID
                  )
                  {
                     m_exists = true;
                     m_pid = pid;
                  }
                  else
                  {
                     m_pidfile.seekp(0);
                     m_pidfile.truncate(0);
                  }
               }
            }
            else
            {
               m_pidfile.clear();
               m_pidfile.open(_fname, std::ios_base::out);
               if (!m_pidfile.is_open())
               {
                  m_error = true;
               }
            }
         }
         
         void PIDFile::clear()
         {
            m_pidfile.close();
            m_pidfile.clear();
            m_error = false;
            m_exists = false;
            m_pid = 0;
            m_fname = 0;
         }
         
         void PIDFile::write()
         {
            if (!m_fname)
               return;
            m_pidfile.seekp(0);
            m_pidfile << static_cast<pid_type>(getpid());
            m_pidfile.truncate(); // also do flush
         }
         
         PIDFile::~PIDFile()
         {
            if (m_error || m_exists)
               return;
            if (m_pidfile.is_open())
            {
               m_pidfile.flush();
               m_pidfile.truncate(0);
            }
            if (m_fname)
            {
               unlink(m_fname); // try to unlink, but probably fail
            }
         }
         
      } // namespace os
   } // namespace core
} // namespace btg
