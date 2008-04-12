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

      sBuffer::sBuffer(const t_byteP _buffer, const t_uint _size)
         : size_(_size),
           buffer_(0)
      {
         if (size_ > 0)
            {
               buffer_ = new t_byte[size_];
               memcpy(buffer_, _buffer, _size);
            }
      }

      sBuffer::sBuffer(dBuffer & _dbuffer)
         : size_(_dbuffer.size()),
           buffer_(new t_byte[size_])
      {
         _dbuffer.getBytes(buffer_, size_);
      }

      sBuffer::sBuffer(sBuffer const& _sbuffer)
         : Serializable(),
           Printable(),
           size_(_sbuffer.size_),
           buffer_(new t_byte[size_])
      {
         memcpy(buffer_, _sbuffer.buffer_, size_);
      }

      sBuffer::sBuffer(std::vector<sBuffer> const& _source)
         : size_(0),
           buffer_(0)
      {
         t_uint totalSize = 0; 
         std::vector<sBuffer>::const_iterator iter;
         for (iter = _source.begin();
              iter != _source.end();
              iter++)
            {
               totalSize += iter->size();
            }

         if (totalSize > 0)
            {
               buffer_    = new t_byte[totalSize];
               t_uint pos = 0;

               for (iter = _source.begin();
                    iter != _source.end();
                    iter++)
                  {
                     memcpy(&buffer_[pos], iter->buffer_, iter->size());
                     pos += iter->size();
                  }
               size_ = totalSize;
            }
      }

      bool sBuffer::getByte(t_uint _offset, t_byte & _byte) const
      {
         bool status = false;

         if (_offset > size_)
            {
               return status;
            }

         _byte = buffer_[_offset];

         status = true;
         return status;
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
               //BTG_NOTICE("Failed to read file " << _filename << ".");
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
                     t_uint temp = size_;
                     reset();
                     size_ = temp;

                     buffer_ = new t_byte[size_];

                     file.read(reinterpret_cast<char*>(buffer_), size_);
                  }

               file.close();
            }
         else
            {
               size_   = 0;
               delete [] buffer_;
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
         
         if (file.is_open())
            {
               file.write(reinterpret_cast<char*>(buffer_), size_);
            }
         else
            {
               status = false;
            }

         return status;
      }

      void sBuffer::reset()
      {
         delete [] buffer_;
         buffer_ = 0;
         size_   = 0;
      }

      bool sBuffer::serialize(btg::core::externalization::Externalization* _e) const
      {
         if(!_e->addBytes(buffer_, size_))
            {
               return false;
            }
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
            {
               return false;
            }

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

               memcpy(buffer_, _sbuffer.buffer_, size_);
            }

         return *this;
      }

      bool sBuffer::operator== (sBuffer const & _sbuffer) const
      {
         bool status = true;

         if (_sbuffer.size_ != size_)
            {
               return false;
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
                                 break;
                              }
                        }
                  }
            }

         return status;
      }

      bool sBuffer::split(t_uint const _pieceSize, 
                          std::vector<sBuffer> & _destination)
      {
         if (size_ == 0)
            {
               return false;
            }

         if (size_ == _pieceSize)
            {
               _destination.push_back(sBuffer(*this));
               return true;
            }

         t_uint pos = 0;
         while (pos < size_)
            {
               if ((pos + _pieceSize) < size_)
                  {
                     _destination.push_back(sBuffer(&buffer_[pos], _pieceSize));
                     pos += _pieceSize;
                  }
               else
                  { 
                     if ((size_ - pos) > 0)
                        {
                           t_uint remainder = size_ - pos;
                           _destination.push_back(sBuffer(&buffer_[pos], remainder));
                           pos += remainder;
                        }
                  }
            }
         
         return true;
      }

      sBuffer::~sBuffer()
      {
         reset();
      }

   } // namespace core
} // namespace btg
