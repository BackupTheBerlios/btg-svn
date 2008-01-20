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

#ifndef NCONFIG_H
#define NCONFIG_H

#include <bcore/client/configuration.h>

#include "keys.h"

#include <string>
#include <vector>

// #include "colors.h"

namespace btg
{
   namespace UI
      {
         namespace cli
            {
	      /**
                * \addtogroup ncli
                */
               /** @{ */

               /// RGB color.
               struct rgbDef
               {
                  public:

                  /// Default constructor.
                  rgbDef() 
                  : r(0), g(0), b(0)
                  {};
		
                  /// RGB: r.
                  t_uint r;

                  /// RGB: g.
                  t_uint g;

                  /// RGB: b.
                  t_uint b;
               };

               /// Struct used to contain information about a color.
               struct colorDef
               {
                  /// Default constructor.
                  colorDef()
                     : background(), 
                       foreground(), 
                       flag_set(false),
                       label(0)
                  {}

                  public:

                  /// Background color.
                  rgbDef background;

                  /// Foreground color.
                  rgbDef foreground;

                  /// Indicates that this color was set.
                  bool   flag_set;

                  /// Label.
                  t_uint label;
               };

               /// Client configuration.
               class ncliConfiguration: public btg::core::client::clientConfiguration
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _logwrapper Pointer used to send logs to.
                     /// @param [in] _filename   The filename to use for reading/writing.
                     ncliConfiguration(btg::core::LogWrapperType _logwrapper,
                                       std::string const& _filename);

                     /// Read the configuration file.
                     bool read(bool const _force = false);

                     /// Write the configuration file.
                     bool write(bool const _force = false);

                     /// Get the syntax of the configuration file.
                     std::string getSyntax() const;

                     /// Get a key definition.
                     /// @returns True - key definition was read from config. False - otherwise.
                     bool getKey(keyMapping::KEYLABEL const _label, t_int & _value);

                     /// Get the color mappings read from the config file.
                     void getColors(std::vector<colorDef> & _colors);

                     /// Destructor.
                     virtual ~ncliConfiguration();
                  private:
                     /// Key.
                     t_int helpKey;
                     /// Key.
                     t_int detachKey;
                     /// Key.
                     t_int quitKey;
                     /// Key.
                     t_int quitscreenKey;
                     /// Key.
                     t_int loadKey;
                     /// Key.
                     t_int menuKey;
                     /// Key.
                     t_int limitKey;
                     /// Key.
                     t_int downKey;
                     /// Key.
                     t_int upKey;
                     /// Key.
                     t_int listStartKey;
                     /// Key.
                     t_int listEndKey;
                     /// Key.
                     t_int selectKey;
                     /// Key.
                     t_int markKey;
                     /// Key.
                     t_int markAllKey;
                     /// Key.
                     t_int nextKey;
                     /// Key.
                     t_int prevKey;
                     /// Key.
                     t_int deleteKey;

                     /// Color.
                     colorDef c_normal;
                     /// Color.
                     colorDef c_border;
                     /// Color.
                     colorDef c_file;
                     /// Color.
                     colorDef c_directory;
                     /// Color.
                     colorDef c_mark;
                     /// Color.
                     colorDef c_warning;
                     /// Color.
                     colorDef c_error;

                     /// Convert a string containing a key definition.
                     void convertKey(std::string const& _key, 
                                     t_int & _value);

                     /// Convert a string containing a color
                     /// definition ("r,g,b:r,g,b").
                     void convertColor(std::string const& _color,
                                       t_uint const _label,
                                       colorDef & _colordef);

                     /// Convert a string containing a rgb color definition.
                     /// @return True - converted. False - otherwise.
                     bool convertColor(std::string const& _color,
                                       rgbDef & _rgbdef);
                  private:
                     /// Copy constructor.
                     ncliConfiguration(ncliConfiguration const& _c);
                     /// Assignment operator.
                     ncliConfiguration& operator=(ncliConfiguration const& _c);
                  };

	       /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // NCONFIG_H

