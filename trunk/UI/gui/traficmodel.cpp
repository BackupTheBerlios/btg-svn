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
 * $Id: traficmodel.cpp,v 1.7.4.3 2007/02/24 13:15:08 wojci Exp $
 */

#include "traficmodel.h"

#include <bcore/logmacro.h>

#include <bcore/command/limit_base.h>

namespace btg
{
   namespace UI
   {
      namespace gui
      {

         using namespace std;


         traficModel::traficModel(t_int const _resolutionX)
            : resolutionX(_resolutionX),
              position(0),
              upload(),
              download(),
              current_dl(0),
              current_ul(0)
         {
            /// Fill in resolutionX samples, all set to zero.
            t_int counter;
            for (counter=0; counter<resolutionX; counter++)
               {
                  download.push_back(0);
                  upload.push_back(0);
               }
            /// Position the sample counter on the last zero sample.
            position = counter;
         }

         void traficModel::add(t_int const _downloadBytesPerSecond, 
                               t_int const _uploadBytesPerSecond)
         {
            if (position >= resolutionX)
               {
                  upload.erase(upload.begin());
                  download.erase(download.begin());
                  position--;
               }

            upload.push_back  (_uploadBytesPerSecond );
            download.push_back(_downloadBytesPerSecond);

            current_dl = _downloadBytesPerSecond;
            current_ul = _uploadBytesPerSecond;

            position++;
         }

         t_int traficModel::getMaxDownload() const
         {
            std::vector<double>::const_iterator dl_max = std::max_element(download.begin(), download.end());

            double m = *dl_max;
            return static_cast<t_int>(m);
         }

         t_int traficModel::getMaxUpload() const
         {
            std::vector<double>::const_iterator ul_max = std::max_element(upload.begin(), upload.end());

            double m = *ul_max;
            return static_cast<t_int>(m);
         }

         void traficModel::getDownload(std::vector<double> & _x,
                                       std::vector<double> & _y) const
         {
            double counter = -resolutionX;
            std::vector<double>::const_iterator downloadIter;
            for (downloadIter = download.begin();
                 downloadIter != download.end();
                 downloadIter++)
               {
                  _x.push_back(counter);
                  _y.push_back(*downloadIter / btg::core::limitBase::KiB_to_B);

                  counter += 1;
               }
         }

         void traficModel::getUpload(std::vector<double> & _x,
                                     std::vector<double> & _y) const
         {
            double counter = -resolutionX;
            std::vector<double>::const_iterator uploadIter;
            for (uploadIter = upload.begin();
                 uploadIter != upload.end();
                 uploadIter++)
               {
                  _x.push_back(counter);
                  _y.push_back(*uploadIter / btg::core::limitBase::KiB_to_B);

                  counter += 1;
               }
         }

         t_int traficModel::getCurrentUpload() const
         {
            return current_ul;
         }
         
         t_int traficModel::getCurrentDownload() const
         {
            return current_dl;
         }

         traficModel::~traficModel()
         {
         }

      } // namespace gui
   } // namespace UI
} // namespace btg
