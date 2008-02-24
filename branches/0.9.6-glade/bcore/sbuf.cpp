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

#include "sbuf.h"

#include "t_string.h"

#include <bcore/os/fileop.h>

#include <fstream>

namespace btg
{
   namespace core
   {

      sBuffer::sBuffer()
         : size_(0),
           buffer_(0)
      {
      }

      sBuffer::sBuffer(dBuffer & _dbuffer)
         : size_(_dbuffer.size()),
           buffer_(new t_byte[size_])
      {
         t_byte b;
         for (t_uint i = 0; i < size_; i++)
            {
               _dbuffer.getByte(&b);
               buffer_[i] = b;
            }

      }

      sBuffer::sBuffer(sBuffer const& _sbuffer)
         : Serializable(),
           Printable(),
           size_(_sbuffer.size_),
           buffer_(new t_byte[size_])
      {
         for (t_uint i = 0; i < size_; i++)
            {
               buffer_[i] = _sbuffer.buffer_[i];
            }
      }

      bool sBuffer::read(std::string const& _filename)
      {
         bool status = true;

         // Open file for reading.
         std::ifstream file;

         std::string errorString;
         std::string fullFilename(_filename);
         if(!btg::core::os::fileOperation::check(fullFilename, errorString, false))
            {
               BTG_NOTICE("Failed to read file " << _filename << ".");
               return false;
            }

#if HAVE_IOS_BASE
         file.open(fullFilename.c_str(), std::ios_base::in | std::ios_base::binary);
#else
         file.open(fullFilename.c_str(), std::ios::in | std::ios::binary);
#endif

         if (file.is_open())
            {
               // Find out the size of the file.
#if HAVE_IOS_BASE
               file.seekg (0, std::ios_base::end);
               size_ = file.tellg();
               file.seekg (0, std::ios_base::beg);
#else
               file.seekg (0, std::ios::end);
               size_ = file.tellg();
               file.seekg (0, ios::beg);
#endif

               if (size_ > 0)
                  {
                     reset();

                     buffer_ = new t_byte[size_];

                     file.read(reinterpret_cast<char*>(buffer_), size_);
                  }

               file.close();
            }
         else
            {
               size_   = 0;
               delete buffer_;
               buffer_ = 0;
               status  = false;
            }

         return status;
      }

      bool sBuffer::write(std::string const& _filename) const
      {
         bool status = true;

         std::ofstream file;

#if HAVE_IOS_BASE
         file.open(_filename.c_str(), std::ios_base::out | std::ios_base::binary);
#else
         file.open(_filename.c_str(), std::ios::out | std::ios::binary);
#endif

         if (size_ <= 0)
            {
               BTG_NOTICE("Size of buffer is zero.");
            }

         if (file.is_open())
            {
               file.write(reinterpret_cast<char*>(buffer_), size_);
            }
         else
            {
               BTG_NOTICE("Opening file '" << _filename << "' for reading failed.");
               status = false;
            }

         return status;
      }

      void sBuffer::reset()
      {
         delete [] buffer_;
         buffer_ = 0;
      }

      bool sBuffer::serialize(btg::core::externalization::Externalization* _e) const
      {
         if(!_e->addBytes(buffer_, size_))
            return false;

         return true;
      }

      bool sBuffer::deserialize(btg::core::externalization::Externalization* _e)
      {
         if (buffer_)
            {
               delete [] buffer_;
               buffer_ = 0;
            }

         if(!_e->getBytes(&buffer_, size_))
            return false;

         return true;
      }

      std::string sBuffer::toString() const
      {
         std::string output;

         output = "Sbuffer, size = ";
         output += btg::core::convertToString<t_uint>(size_);

         return output;
      }

      t_uint sBuffer::size() const
      {
         return size_;
      }

      sBuffer & sBuffer::operator= (sBuffer const & _sbuffer)
      {
         bool eq = (_sbuffer == *this);

         if (!eq)
            {
               size_ = 0;
               delete [] buffer_;
               buffer_ = 0;

               size_   = _sbuffer.size_;
               buffer_ = new t_byte[size_];

               for (t_uint i = 0; i < size_; i++)
                  {
                     buffer_[i] = _sbuffer.buffer_[i];
                  }
            }

         return *this;
      }

      bool sBuffer::operator== (sBuffer const & _sbuffer) const
      {
         bool status = true;

         if (_sbuffer.size_ != size_)
            {
               status = false;
            }

         if (status)
            {
               // Compare the contents.
               if (size_ > 0)
                  {
                     for (t_uint i = 0; i < size_; i++)
                        {
                           if (_sbuffer.buffer_[i] != buffer_[i])
                              {
                                 status = false;
                              }
                        }
                  }
            }

         return false;
      }

      sBuffer::~sBuffer()
      {
         reset();
      }

   } // namespace core
} // namespace btg
