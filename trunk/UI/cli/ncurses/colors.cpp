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
 * $Id: colors.cpp,v 1.1.2.4 2006/12/31 00:34:00 wojci Exp $
 */

#include "colors.h"

#include <ncurses.h>

#include <bcore/project.h>
#include <bcore/type_btg.h>

#include "nconfig.h"

namespace btg
{
   namespace UI
   {
      namespace cli
      {
         Colors::Colors()
	   : c_normalSet(false),
	     c_borderSet(false),
	     c_fileSet(false),
	     c_directorySet(false),
	     c_markSet(false),
	     c_warningSet(false),
	     c_errorSet(false)
         {
         }

         bool Colors::init()
         {
            if (!::has_colors())
               {
                  return false;
               }

            ::start_color();

	    if (::can_change_color() == 0)
	      {
		// No color changing, but the default ones work.
		setDefaults();
		return true;
	      }

	    t_int colorCounter = 1;

	    // Set the user defined colors.
	    std::vector<colorDef>::const_iterator colorIter;
	    for (colorIter = userColors.begin();
		 colorIter != userColors.end();
		 colorIter++)
	      {
		switch (colorIter->label)
		  {
		  case C_NORMAL:
		    {
		      c_normalSet = true;
		      break;
		    }
		  case C_BORDER:
		    {
		      c_borderSet = true;
		      break;
		    }
		  case C_FILE:
		    {
		      c_fileSet = true;
		      break;
		    }
		  case C_DIRECTORY:
		    {
		      c_directorySet = true;
		      break;
		    }
		  case C_MARK:
		    {
		      c_markSet = true;
		      break;
		    }
		  case C_WARNING:
		    {
		      c_warningSet = true;
		      break;
		    }
		  case C_ERROR:
		    {
		      c_errorSet = true;
		      break;
		    }
		  default:
		    {
		      continue;
		      break;
		    }
		  }

		COLORLABEL label = static_cast<COLORLABEL>(colorIter->label);

		short color0 = colorCounter++;
		short r = colorIter->foreground.r;
		short g = colorIter->foreground.g;
		short b = colorIter->foreground.b;

		t_int res = ::init_color(color0, 
					 r, 
					 g, 
					 b);
		if (res == ERR)
		  {
		    std::cout << "BLAH" << std::endl;
		    // Unable to create this color..
		  }

		short color1 = colorCounter++;
		r = colorIter->background.r;
		g = colorIter->background.g;
		b = colorIter->background.b;

		res = ::init_color(color1, 
				   r, 
				   g, 
				   b);

		if (res == ERR)
		  {
		    std::cout << "BLAH" << std::endl;
		    // Unable to create this color..
		  }
		
		res = ::init_pair(label, color0, color1);

		if (res == ERR)
		  {
		    std::cout << "BLAH" << std::endl;
		    // Unable to create this color..
		  }

	      }

	    // Set the rest of the colors.
	    setDefaults();

            /*
              #define COLOR_BLACK     0
              #define COLOR_RED       1
              #define COLOR_GREEN     2
              #define COLOR_YELLOW    3
              #define COLOR_BLUE      4
              #define COLOR_MAGENTA   5
              #define COLOR_CYAN      6
              #define COLOR_WHITE     7
            */

            return true;
         }

	void Colors::set(std::vector<colorDef> const& _colors)
	{
	  userColors = _colors;
	}

	void Colors::setDefaults()
	{
	  if (!c_normalSet)
	    {
	      ::init_pair(C_NORMAL,    COLOR_WHITE,   COLOR_BLACK);
	      c_normalSet = true;
	    }

	  if (!c_borderSet)
	    {
	      ::init_pair(C_BORDER,    COLOR_CYAN,    COLOR_BLACK);
	      c_borderSet = true;
	    }

	  if (!c_fileSet)
	    {
	      ::init_pair(C_FILE,      COLOR_YELLOW,  COLOR_BLACK);
	      c_fileSet = true;
	    }

	  if (!c_directorySet)
	    {
	      ::init_pair(C_DIRECTORY, COLOR_GREEN,   COLOR_BLACK);
	      c_fileSet = true;
	    }

	  if (!c_markSet)
	    {
	      ::init_pair(C_MARK,      COLOR_GREEN,   COLOR_BLACK);
	      c_markSet = true;
	    }

	  if (!c_warningSet)
	    {
	      ::init_pair(C_WARNING,   COLOR_RED,     COLOR_BLACK);
	      c_warningSet = true;
	    }

	  if (!c_errorSet)
	    {
	      ::init_pair(C_ERROR,     COLOR_RED,     COLOR_BLACK);
	      c_errorSet = true;
	    }
	}

         Colors::~Colors()
         {

         }

      } // namespace cli
   } // namespace UI
} // namespace btg

