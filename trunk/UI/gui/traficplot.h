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
 * $Id: traficplot.h,v 1.1.2.2 2007/02/27 17:02:32 wojci Exp $
 */

#ifndef TRAFICPLOT_H
#define TRAFICPLOT_H

#include <bcore/status.h>
#include "traficmodel.h"

#include <plotmm/plot.h>
#include <plotmm/curve.h>

#include <gtkmm/label.h>
#include <gtkmm/box.h>

namespace btg
{
   namespace UI
      {
         namespace gui
            {
               /**
                * \addtogroup gui
                */
               /** @{ */

               /// Implements a trafic plot.
               class traficPlot
                  {
                  public:
                     /// Constructor.
                     traficPlot();

                     /// Redraws the canvas.
                     void redraw();

                     /// Reset the contained counters.
                     void resetCounters();

                     /// Update the counters with information
                     /// contained in a status object. This does not
                     /// redraw the canvas.
                     void updateCounter(btg::core::Status const& _status);

                     /// Update the contained model. Called to create
                     /// the shown graphs.
                     void updateModel();

                     /// Get the contained widget that can be added to
                     /// other containers.
                     // PlotMM::Plot* getWidget() const;
                     Gtk::VBox* getWidget() const;

                     /// Show the contained widget.
                     void show();

                     /// Destructor.
                     ~traficPlot();
                  private:
                     /// Convert a number of bytes per second to a
                     /// human readable string.
                     std::string bytesPerSecondToString(int const _bytes, 
                                                        bool const _shrt = false);

                     /// The contained traffic model.
                     traficModel                 tm;

                     /// Download rate, counter.
                     float                       download_rate;

                     /// Upload reate, counter.
                     float                       upload_rate;

		     /// Pointer to a class used for the actual
		     /// plotting.
                     PlotMM::Plot*               plot;

		     /// Reference to the download curve.
                     Glib::RefPtr<PlotMM::Curve> dl_curve;

		     /// Reference to the upload curve.
                     Glib::RefPtr<PlotMM::Curve> ul_curve;

		     /// Label used for showing current and max
		     /// bandwidth.
                     Gtk::Label*                 label;

		     /// Vertical box used to keep the plot above the
		     /// label.
                     Gtk::VBox*                  box;
                  };

               /** @} */

            } // namespace gui
      } // namespace UI
} // namespace btg

#endif
