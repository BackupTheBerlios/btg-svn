/*
 * btg Copyright (C) 2007 Roman Rybalko.
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

#ifndef CLIENTDYNCONFIG_H
#define CLIENTDYNCONFIG_H

#include <string>

#include <bcore/type.h>
#include <bcore/logable.h>

namespace btg
{
   namespace core
      {
         namespace client
            {
               class lastFiles;

               /**
                * \addtogroup gencli Generic client
                */
               /** @{ */
               
               /// Save and retrive various client dinamically-generated
               /// configuretion data (like client window position and dimensions, etc.)
               class clientDynConfig: public Logable
                  {
                     // Allow to handle some data to lastFiles
                     friend class lastFiles;
                  
                  public:
                     /// Constructor.
                     /// @param [in] _logwrapper Pointer used to send logs to.
                     /// @param [in] _file_name  Dynamic client config data file name
                     /// for store/load config data
                     clientDynConfig(LogWrapperType _logwrapper,
                                     std::string const& _file_name);

                     /// Destructor.
                     virtual ~clientDynConfig();

                     /// Load data from file
                     bool load();
                     
                     /// Store data to file
                     bool save();
                     
                     /// Return true if on-disk copy is different form in-memory
                     bool modified() const { return m_data_modified; }
                     
                     /// Set GUI window width and height
                     void set_gui_window_dimensions(int const width, int const height);
                     
                     /// Set GUI window position
                     void set_gui_window_position(int const x, int const y);
                     
                     /// Get GUI window width
                     int get_gui_window_width() const { return m_gui_window_width; }

                     /// Get GUI window height
                     int get_gui_window_height() const { return m_gui_window_height; }
                     
                     /// Get GUI window horizontal position
                     int get_gui_window_x() const { return m_gui_window_xpos; }
                     
                     /// Get GUI window vertical position
                     int get_gui_window_y() const { return m_gui_window_ypos; }
                     
                     /// Get GUI Client window maximized state
                     bool get_gui_window_maximized() const { return m_gui_window_maximized; }
                     
                     /// Set GUI Client window maximized state
                     void set_gui_window_maximized(bool const maxmin);
                  
                  protected:
                     /// Sets data modification flag
                     /// @param [in] bMod true if data modified
                     void set_modified(bool const bMod);
                  
                  // Internal data
                  private:
                     /// destination file name
                     std::string m_file_name;
                     
                     /// if data modified and need to write it
                     bool m_data_modified;
                     
                  // Configuration data
                  private:
                     /// GUI Client window width
                     int m_gui_window_width;
                     
                     /// GUI Clinet window height
                     int m_gui_window_height;
                     
                     /// GUI Client window horizontal position 
                     int m_gui_window_xpos;
                     
                     /// GUI Client window vertical position
                     int m_gui_window_ypos;
                     
                     /// true if GUI Client window maximized
                     bool m_gui_window_maximized;

                     /// List of last opened files. (handled by lastFiles)
                     t_strList lastFiles_;
                  };

               /** @} */

            } // namespace client
      } // namespace core
} // namespace btg

#endif
