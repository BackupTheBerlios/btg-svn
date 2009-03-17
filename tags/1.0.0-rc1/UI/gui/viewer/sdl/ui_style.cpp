#include "ui.h"

#include "agar-if.h"

//#include <agar/gui/primitive.h>
//#include <agar/gui/widget.h>

//#include <agar/core.h>
//#include <agar/gui.h>
//#include <agar/dev.h>

#include <agar/core.h>
#include <agar/gui.h>
#include <agar/gui/primitive.h>
#include <agar/dev.h>

#define WIDTH(wd)  AGWIDGET(wd)->w
#define HEIGHT(wd) AGWIDGET(wd)->h

namespace btg
{
   namespace UI
   {
      namespace gui
      {
         namespace viewer
         {

            static void customInit(AG_Style* _style);
            static void customWindowBackground(AG_Window* _window);
            static void customTitlebarBackground(void* _tbar, 
                                                 int _isPressed, 
                                                 int _windowIsFocused);
            
            void setColors()
            {
               agColors[TITLEBAR_FOCUSED_COLOR]   = SDL_MapRGB(agVideoFmt, 123, 54, 24);
               agColors[TITLEBAR_UNFOCUSED_COLOR] = SDL_MapRGB(agVideoFmt, 127, 127, 127);
               agColors[TABLE_COLOR] = SDL_MapRGB(agVideoFmt, 79, 145, 169);
               // agColors[TABLE_LINE_COLOR] = SDL_MapRGB(agVideoFmt, 173, 209, 222);
               agColors[WINDOW_BG_COLOR] = SDL_MapRGB(agVideoFmt, 15, 73, 77);
               agColors[TEXT_COLOR] = SDL_MapRGB(agVideoFmt, 200, 200, 200);
            }

            void setUIStyle(AG_Style & _style)
            {
               _style             = agStyleDefault;
               _style.name        = "btg";
               _style.version.maj = 1;
               _style.version.min = 0;

               //_style.WindowBackground   = customWindowBackground;
               //_style.TitlebarBackground = customTitlebarBackground;

               /*
                 ButtonBackground,
                 ButtonTextOffset,
                 BoxFrame,
                 CheckboxButton,
                 ConsoleBackground,
                 FixedPlotterBackground,
                 MenuRootBackground,
                 MenuRootSelectedItemBackground,
                 MenuBackground,
                 MenuItemBackground,
                 MenuItemSeparator,
                 NotebookBackground,
                 NotebookTabBackground,
                 PaneHorizDivider,
                 PaneVertDivider,
                 RadioGroupBackground,
                 RadioButton,
                 ProgressBarBackground,
                 ScrollbarBackground,
                 ScrollbarVertButtons,
                 ScrollbarHorizButtons,
                 SeparatorHoriz,
                 SeparatorVert,
                 SocketBackground,
                 SocketOverlay,
                 TableBackground,
                 TableColumnHeaderBackground,
                 TableRowBackground,
                 TableCellBackground,
                 TextboxBackground,
                 ListBackground,
                 ListItemBackground,
                 TreeSubnodeIndicator
               */
            }

            static void customInit(AG_Style* _style)
            {

            }

            static void customWindowBackground(AG_Window* _window)
            {
               AG_SizeAlloc alloc;
               AG_WidgetSizeAlloc(_window, &alloc);
               AG_DrawRectFilled(_window,
                                 AG_RECT(0, 0, alloc.w, alloc.h),
                                 AG_COLOR(WINDOW_BG_COLOR));
            }

            /* Background of default Titlebar widgets */
            static void customTitlebarBackground(void* _tbar, 
                                                 int _isPressed, 
                                                 int _windowIsFocused)
            {
               AG_SizeAlloc alloc;
               AG_WidgetSizeAlloc(_tbar, &alloc);
               AG_DrawBox(_tbar,
                          AG_RECT(0, 0, alloc.w, alloc.h),
                          _isPressed ? -1 : 1,
                          _windowIsFocused ? AG_COLOR(TITLEBAR_FOCUSED_COLOR) :
                          AG_COLOR(TITLEBAR_UNFOCUSED_COLOR));
            }
         } // namespace viewer
      } // namespace gui
   } // namespace UI
} // namespace btg

