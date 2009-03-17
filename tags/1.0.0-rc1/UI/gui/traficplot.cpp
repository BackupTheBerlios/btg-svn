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

#include "traficplot.h"

#include <pangomm/layout.h>

#include <iostream>
#include <string>

#include <bcore/t_string.h>
#include <bcore/hrr.h>

#include <gtkmm/notebook.h>

#include <plotmm/paint.h>

#include <gtkmm/label.h>
#include <gtkmm/box.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         using namespace btg::core;

         traficPlot::traficPlot()
            : tm(512 /* keep this number of samples */),
              download_rate(0),
              upload_rate(0),
              plot(0),
              dl_curve(0),
              ul_curve(0),
              label(0),
              box(0)
         {
            plot = Gtk::manage(new class PlotMM::Plot());

            plot->scale(PlotMM::AXIS_LEFT)->set_enabled(true);
            plot->scale(PlotMM::AXIS_RIGHT)->set_enabled(false);
            plot->scale(PlotMM::AXIS_BOTTOM)->set_enabled(true);
            plot->scale(PlotMM::AXIS_TOP)->set_enabled(false);
  
            plot->scale(PlotMM::AXIS_BOTTOM)->set_ticklengths(1, 50);
            plot->scale(PlotMM::AXIS_LEFT)->set_ticklengths(1, 50);

            plot->scale(PlotMM::AXIS_BOTTOM)->set_autoscale(true);
            plot->scale(PlotMM::AXIS_LEFT)->set_autoscale(true);

            //plot->title()->set_text("Bandwidth");
            //plot->title()->set_enabled(true);

            plot->label(PlotMM::AXIS_BOTTOM)->set_text("Time (samples)");
            plot->label(PlotMM::AXIS_BOTTOM)->set_enabled(true);

            plot->label(PlotMM::AXIS_LEFT)->set_text("KiB/sec");
            plot->label(PlotMM::AXIS_LEFT)->set_enabled(true);

            // plot->label(PlotMM::AXIS_RIGHT)->set_enabled(false);

            dl_curve = Glib::RefPtr<PlotMM::Curve>(new PlotMM::Curve);
            ul_curve = Glib::RefPtr<PlotMM::Curve>(new PlotMM::Curve);

            dl_curve->paint()->set_pen_color(Gdk::Color("green"));
            ul_curve->paint()->set_pen_color(Gdk::Color("red"));

            plot->add_curve(dl_curve);
            plot->add_curve(ul_curve);

            plot->replot();

            label = Gtk::manage(new class Gtk::Label("Current bandwidth:\nMax bandwidth:"));

            box = Gtk::manage(new class Gtk::VBox(false, 0));
            box->set_homogeneous(false);
            box->set_spacing(0);
            box->pack_start(*plot);
            box->pack_start(*label, Gtk::PACK_SHRINK, 0);

            label->show();
            plot->show();
            box->show();
         }

         void traficPlot::updateCounter(btg::core::Status const& _status)
         {
            download_rate += _status.downloadRate();
            upload_rate   += _status.uploadRate();
         }

         void traficPlot::updateModel()
         {
            // Update the model.
            tm.add(static_cast<int>(download_rate), static_cast<int>(upload_rate));

            // Update the plots.
            std::vector<double> x;
            std::vector<double> y;

            tm.getDownload(x,y);
            dl_curve->set_data(x, y);

            x.clear();
            y.clear();

            tm.getUpload(x,y);
            ul_curve->set_data(x, y);
         }

         void traficPlot::redraw()
         {
            // Update the title:
            std::string title("Current bandwidth: ");
            title += bytesPerSecondToString(tm.getCurrentDownload(), true);
            title += " dl / ";
            title += bytesPerSecondToString(tm.getCurrentUpload(), true);
            title += " ul";
            title += "\n";

            title += "Max bandwidth: ";
            title += bytesPerSecondToString(tm.getMaxDownload(), true);
            title += " dl / ";
            title += bytesPerSecondToString(tm.getMaxUpload(), true);
            title += " ul";

            label->set_text(title);

            plot->replot();
         }

         std::string traficPlot::bytesPerSecondToString(int const _bytes, bool const _shrt)
         {
            humanReadableRate hrr = humanReadableRate::convert(_bytes);

            return hrr.toString(_shrt);
         }

         /*
         PlotMM::Plot* traficPlot::getWidget() const
         {
            return plot;
         }
         */

         Gtk::VBox* traficPlot::getWidget() const
         {
            return box;
         }

         void traficPlot::show()
         {
            plot->show();
         }

         void traficPlot::resetCounters()
         {
            download_rate = 0;
            upload_rate   = 0;            
         }

         traficPlot::~traficPlot()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg
