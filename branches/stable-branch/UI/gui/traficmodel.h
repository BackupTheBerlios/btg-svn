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

#ifndef TRAFICMODEL_H
#define TRAFICMODEL_H

#include <bcore/type.h>
#include <vector>

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

         /// Implements a model of the data used for graphing
         /// upload/download traffic.
         class traficModel
         {
         public:
            /// Constructor.
            /// @param [in] _resolutionX X-resolution.
            traficModel(t_int const _resolutionX);

            /// Add a sample.
            void add(t_int const _downloadBytesPerSecond, 
                     t_int const _uploadBytesPerSecond);
                     
            /// Get vectors of the contained download rate
            /// data.
            void getDownload(std::vector<double> & _x,
                             std::vector<double> & _y) const;

            /// Get vectors of the contained upload rate
            /// data.
            void getUpload(std::vector<double> & _x,
                           std::vector<double> & _y) const;

            /// Get the current upload.
            t_int getCurrentUpload() const;

            /// Get the current download.
            t_int getCurrentDownload() const;

            /// Get the max download in B/sec.
            t_int getMaxDownload() const;

            /// Get the max upload in B/sec.
            t_int getMaxUpload() const;

            /// Destructor.
            ~traficModel();
         private:
            /// The resolution.
            t_int                     resolutionX;

            /// Current position.
            t_int                     position;

            /// Upload samples.
            std::vector<double>       upload;

            /// Download samples.
            std::vector<double>       download;

            /// The current download rate.
            t_int                     current_dl;
            /// The current upload rate.
            t_int                     current_ul;
         };

         /** @} */

      } // namespace gui
   } // namespace UI
} // namespace btg

#endif
