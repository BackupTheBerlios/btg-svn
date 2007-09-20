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
 * $Id: limitwindow.h,v 1.1.2.9 2007/01/16 20:26:36 wojci Exp $
 */

#ifndef LIMITWINDOW_H
#define LIMITWINDOW_H

#include <string>
#include <vector>

#include <bcore/project.h>
#include <bcore/type_btg.h>

#include <bcore/status.h>

extern "C"
{
#include <ncurses.h>
}

#include "window.h"

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

               class mainWindow;

               /// Limit dialog, used for per torrent limit and for
               /// global limits. Both types of limit use 4 counters.
               class limitWindow: public baseWindow, public dialog
                  {
                  public:
                     /// Constructor.
                     /// @param [in] _kmap Keyboard mapping.
                     /// @param [in] _ws   The size of this window.
                     /// @param [in] _counter1_value The value of the first counter.
                     /// @param [in] _counter1_min   The min value of the first counter.
                     /// @param [in] _counter1_max   The max value of the first counter.
                     /// @param [in] _counter1label  The label to use for the first counter.
                     /// 
                     /// @param [in] _counter2_value The value of the second counter.
                     /// @param [in] _counter2_min   The min value of the second counter.
                     /// @param [in] _counter2_max   The max value of the second counter.
                     /// @param [in] _counter2label  The label to use for the second counter.
                     /// 
                     /// @param [in] _counter3_value The value of the third counter.
                     /// @param [in] _counter3_min   The min value of the third counter.
                     /// @param [in] _counter3_max   The max value of the third counter.
                     /// @param [in] _counter3label  The label to use for the third counter.
                     /// 
                     /// @param [in] _counter4_value The value of the fourth counter.
                     /// @param [in] _counter4_min   The min value of the fourth counter.
                     /// @param [in] _counter4_max   The max value of the fourth counter.
                     /// @param [in] _counter4label  The label to use for the fourth counter.

                     limitWindow(keyMapping const& _kmap,
                                 windowSize const& _ws,
                                 t_int _counter1_value,
                                 t_int _counter1_min,
                                 t_int _counter1_max,
                                 std::string const& _counter1label,
                                 t_int _counter2_value,
                                 t_int _counter2_min,
                                 t_int _counter2_max,
                                 std::string const& _counter2label,
                                 t_int _counter3_value,
                                 t_int _counter3_min,
                                 t_int _counter3_max,
                                 std::string const& _counter3label,
                                 t_int _counter4_value,
                                 t_int _counter4_min,
                                 t_int _counter4_max,
                                 std::string const& _counter4label);

                     void resize(windowSize const& _ws);

                     dialog::RESULT run();

                     /// Get the selected limits.
                     /// @return True - limits were set. False otherwise.
                     bool getLimits(t_int & _upload,
                                    t_int & _download,
                                    t_int & _seedTimeout,
                                    t_int & _seedPercent) const;

                     /// Destructor.
                     virtual ~limitWindow();
                  private:
                     /// Counters.
                     enum LIMIT
                        {
                           COUNTER1 = 0, //!< Counter 1.
                           COUNTER2,     //!< Counter 2.
                           COUNTER3,     //!< Counter 3.
                           COUNTER4      //!< Counter 4.
                        };

                     enum 
                        {
                           /// The size of the array used for keeping the
                           /// data for the counters.
                           COUNTERSIZE = COUNTER4 + 1
                        };

                     /// Size of this window.
                     windowSize size_;

                     /// The currently selected limit.
                     LIMIT currentLimit_;

                     /// limit counter.
                     t_int counter_;

                     /// Data for each counter.
                     struct counterData
                     {
                        /// Initial value.
                        t_int       initial; 
                        /// Current value.
                        t_int       value;
                        /// Max value.
                        t_int       minimum;
                        /// Min value.
                        t_int       maximum;

                        /// Label for this counter shown to the user.
                        std::string label;
                     };

                     /// Array of counters.
                     counterData counters_[COUNTERSIZE];

                     /// The currently selected counter.
                     LIMIT choice_;

                     /// Indicates that a limit was set.
                     bool  limitSet_;

                     void refresh();

                     /// Draw the contents of this window.
                     void draw();

                     /// Add a counter topic to the window.
                     void addTopic(std::string const& _text);

                     /// Add a counter value to the window.
                     void addValue(LIMIT const _type, 
                                   t_int const _value, 
                                   bool const _changed);

                     /// Move to the next counter.
                     void nextChoice();

                     /// Move to the previous counter.
                     void prevChoice();

                     /// Increase the current counter's value.
                     void incValue(t_int const _increment = 1);

                     /// Decrease the current counter's value.
                     void decValue(t_int const _increment = 1);

                     /// Increase a value by some amount.
                     void genInc(t_int const _increment, t_int & _value, t_int const _max) const;

                     /// Decrease a value by some amount.
                     void genDec(t_int const _increment, t_int & _value, t_int const _min) const;

                     windowSize calculateDimenstions(windowSize const& _ws) const;

                     /// Show help.
                     /// @return True - the help window was resized, false otherwise.
                     bool showHelp();
                  };

	       /** @} */

            } // namespace cli
      } // namespace UI
} // namespace btg

#endif // LIMITWINDOW_H

