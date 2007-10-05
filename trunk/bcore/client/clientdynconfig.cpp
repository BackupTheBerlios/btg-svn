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

#include <iostream>
#include <fstream>

#include "clientdynconfig.h"

#include <bcore/externalization/externalization_factory.h>
#include <bcore/os/fileop.h>

const t_uint cfg_format_version = 0xFF88FF22; // some magic number, mean nothing

namespace btg
{
   namespace core
   {
      namespace client
      {

         using namespace btg::core::externalization;
         using namespace btg::core;
         using namespace std;
         
         clientDynConfig::clientDynConfig(string const& _file_name)
            : m_file_name(_file_name),
              m_data_modified(false),
              m_gui_window_width(600),
              m_gui_window_height(450),
              m_gui_window_xpos(20),
              m_gui_window_ypos(20),
              m_gui_window_maximized(false),
              lastFiles_(0)
         {
            if (btg::core::os::fileOperation::check(m_file_name))
            {
               if (!load())
               {
                  BTG_NOTICE("Can't load dynconfig.");
               }
            }
            else
            {
               BTG_NOTICE("Cauldn't find file " << m_file_name);
            }
         }
         
         clientDynConfig::~clientDynConfig()
         {
            if (modified())
               save();
         }
         
         bool clientDynConfig::load()
         {
            BTG_NOTICE("load(): Loading dynconfig from file " << m_file_name);

            ifstream file;
#if HAVE_IOS_BASE
            file.open(m_file_name.c_str(), ios_base::in | ios_base::binary);
#else
            file.open(m_file_name.c_str(), ios::in | ios::binary);
#endif
            if (!file.is_open())
            {
               BTG_ERROR_LOG("load(), Failed to open " << m_file_name);
               return false;
            }

            // find out file size
            t_int size = 0;
#if HAVE_IOS_BASE
            file.seekg (0, ios_base::end);
            size = file.tellg();
            file.seekg (0, ios_base::beg);
#else
            file.seekg (0, ios::end);
            size = file.tellg();
            file.seekg (0, ios::beg);
#endif

            if (size <= 0)
            {
               BTG_ERROR_LOG("load(), " << m_file_name << " is empty or absent.");
               return false;
            }
            
            t_byteP buffer = new t_byte[size];
            memset(buffer, size, 0);
            file.read(reinterpret_cast<char*>(buffer), size);
            
            auto_ptr<Externalization> ext(Factory::createExternalization());
            
            dBuffer dbuf(buffer, size);
            ext->setBuffer(dbuf);
            
            delete [] buffer;
            buffer = 0;

            // check signatures
            t_uint sig0=0;
            t_uint sig1=0;
            if ((!ext->bytesToUint(&sig0)) || (!ext->bytesToUint(&sig1)))
            {
               BTG_ERROR_LOG("load(), failed to parse " << m_file_name
                  << ", unable to read signature byte(s).");
               return false;
            }
            if ((sig0 != ~((t_uint)sig1)))
            {
               BTG_ERROR_LOG("load(), bad signature bytes");
               // Unknown signature.
               return false;
            }
            
            if (
               !ext->bytesToInt(&m_gui_window_xpos) ||
               !ext->bytesToInt(&m_gui_window_ypos) ||
               !ext->bytesToInt(&m_gui_window_width) ||
               !ext->bytesToInt(&m_gui_window_height) ||
               !ext->bytesToBool(m_gui_window_maximized) ||
               !ext->bytesToStringList(&lastFiles_)
               )
            {
               BTG_ERROR_LOG("load(), parsing error.");
               return false;
            }

            return true;
         }
         
         bool clientDynConfig::save()
         {
            BTG_NOTICE("save(), Writing dynconfig file " << m_file_name);
            
            // Create serializator
            auto_ptr<Externalization> ext(Factory::createExternalization());

            // store signature
            t_uint sig = cfg_format_version;
            ext->uintToBytes(&sig);
            sig = ~sig;
            ext->uintToBytes(&sig);

            // store config data
            if (
               !ext->intToBytes(&m_gui_window_xpos) ||
               !ext->intToBytes(&m_gui_window_ypos) ||
               !ext->intToBytes(&m_gui_window_width) ||
               !ext->intToBytes(&m_gui_window_height) ||
               !ext->boolToBytes(m_gui_window_maximized) ||
               !ext->stringListToBytes(&lastFiles_)
               )
            {
               BTG_ERROR_LOG("save(), parsing error.");
               return false;
            }
            
            dBuffer db;
            ext->getBuffer(db);
            
            // Write file
            ofstream file;
#if HAVE_IOS_BASE
            file.open(m_file_name.c_str(), ios_base::out | ios_base::binary);
#else
            file.open(m_file_name.c_str(), ios::out | ios::binary);
#endif
            if (!file.is_open())
            {
               BTG_ERROR_LOG("save(), Failed to open " << m_file_name << " to write dynamic configuration.");
               return false;
            }

            t_int size     = db.size();
            t_byteP buffer = new t_byte[size];
            db.getBytes(buffer, size);

            file.write(reinterpret_cast<char*>(buffer), size);

            delete [] buffer;
            buffer = 0;

            // file closes when goes out of scope
            
            return true;
         }
         
         void clientDynConfig::set_gui_window_dimensions(int width, int height)
         {
            m_gui_window_width = width;
            m_gui_window_height = height;
            m_data_modified = true;
         }
         
         void clientDynConfig::set_gui_window_position(int x, int y)
         {
            m_gui_window_xpos = x;
            m_gui_window_ypos = y;
            m_data_modified = true;
         }
         
         void clientDynConfig::set_gui_window_maximized(bool maxmin)
         {
            m_gui_window_maximized = maxmin;
            m_data_modified = true;
         }

      } // namespace client
   } // namespace core
} // namespace btg
