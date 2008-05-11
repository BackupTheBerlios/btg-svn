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

#include <bcore/file_info.h>
#include <bcore/t_string.h>
#include <bcore/helpermacro.h>

namespace btg
{
   namespace core
   {
      fileInformation::fileInformation()
         : Serializable(),
           Printable(),
           filename("undefined file"),
           pieces_size(0),
           pieces(0),
           bytesPerPiece(0),
           fileSize(0),
           status(fileInformation::DONE_UNDEF)
      {
      }


      fileInformation::fileInformation(fileInformation const& _fileinfo)
         : Serializable(),
           Printable(),
           filename(_fileinfo.filename),
           pieces_size(_fileinfo.pieces_size),
           pieces(_fileinfo.pieces),
           bytesPerPiece(_fileinfo.bytesPerPiece),
           fileSize(_fileinfo.fileSize),
           status(_fileinfo.status)
      {
      }

      fileInformation::fileInformation(std::string const& _filename,
                                       t_int const _bytesPerPiece,
                                       t_ulong const _file_size)
         : Serializable(),
           Printable(),
           filename(_filename),
           pieces_size(0),
           pieces(0),
           bytesPerPiece(_bytesPerPiece),
           fileSize(_file_size),
           status(fileInformation::DONE_ALL)
      {
         
      }

      fileInformation::fileInformation(std::string const& _filename, 
                                       t_bitVector const& _pieces, 
                                       t_int const _bytesPerPiece, 
                                       t_ulong const _file_size)
         : Serializable(),
           Printable(),
           filename(_filename),
           pieces_size(0),
           pieces(0),
           bytesPerPiece(_bytesPerPiece),
           fileSize(_file_size),
           status(fileInformation::DONE_UNDEF)
      {
         if (_pieces.size() > 0)
            {
               t_bitVectorCI iter;
               t_uint b_set = 0;

               for (iter = _pieces.begin(); 
                    iter != _pieces.end();
                    iter++)
                  {
                     if (*iter)
                        {
                           b_set++;
                        }
                  }

               if (b_set == _pieces.size())
                  {
                     status      = fileInformation::DONE_ALL;
                     pieces_size = _pieces.size();
                  }
               else if (b_set == 0)
                  {
                     status      = fileInformation::DONE_NONE;
                     pieces_size = _pieces.size();
                  }
               else
                  {
                     status      = fileInformation::DONE_UNDEF;
                     pieces      = _pieces;
                     pieces_size = _pieces.size();
                  }
            } // pieces size is not 0.
         else
            {
               status      = fileInformation::DONE_UNDEF;
               pieces_size = 0;
            }
      }

      bool fileInformation::operator== (fileInformation const& _fileinfo) const
      {
         if (this->filename != _fileinfo.filename)
            {
               return false;
            }

         if (this->pieces_size != _fileinfo.pieces_size)
            {
               return false;
            }

         if (this->status != _fileinfo.status)
            {
               return false;
            }

         if (this->pieces != _fileinfo.pieces)
            {
               return false;
            }

         if (this->bytesPerPiece != _fileinfo.bytesPerPiece)
            {
               return false;
            }

         if (this->fileSize != _fileinfo.fileSize)
            {
               return false;
            }

         return true;
      }

      bool fileInformation::serialize(btg::core::externalization::Externalization* _e) const
      {
         // string, filename;
         _e->stringToBytes(&this->filename);
         BTG_RCHECK(_e->status());

         t_int temp_status = this->status;
         _e->intToBytes(&temp_status);
         BTG_RCHECK(_e->status());

         _e->intToBytes(&this->bytesPerPiece);
         BTG_RCHECK(_e->status());

         _e->uLongToBytes(&this->fileSize);
         BTG_RCHECK(_e->status());

         // Length, in bits.
         _e->intToBytes(&this->pieces_size);
         BTG_RCHECK(_e->status());

         if (status == fileInformation::DONE_UNDEF)
         {
            pieces.serialize(_e);
            BTG_RCHECK(_e->status());
         }

         return true;
      }

      bool fileInformation::deserialize(btg::core::externalization::Externalization* _e)
      {
         // string, filename;
         _e->bytesToString(&this->filename);
         BTG_RCHECK(_e->status());

         t_int temp_status = 0;
         _e->bytesToInt(&temp_status);
         BTG_RCHECK(_e->status());
         this->status = (bitsetModifier)temp_status;

         _e->bytesToInt(&this->bytesPerPiece);
         BTG_RCHECK(_e->status());

         _e->bytesToULong(&this->fileSize);
         BTG_RCHECK(_e->status());

         // Length, in bits.
         _e->bytesToInt(&this->pieces_size);
         BTG_RCHECK(_e->status());

         if (status == fileInformation::DONE_UNDEF)
         {
            pieces.deserialize(_e);
            BTG_RCHECK(_e->status());
         }

         return true;
      }

      t_uint fileInformation::size() const
      {
         return pieces_size;
      }

      std::string fileInformation::getFilename() const
      {
         return filename;
      }

      t_bitVector fileInformation::getBits() const
      {
         if ((this->status != fileInformation::DONE_ALL) && 
             (this->status != fileInformation::DONE_NONE))
            {
               return pieces;
            }

         t_bitVector list_to_return;

         if (this->status == fileInformation::DONE_ALL)
            {
               for (t_int i=0; i<this->pieces_size; i++)
                  {
                     list_to_return.push_back(1);
                  }
            }
         else if (this->status == fileInformation::DONE_NONE)
            {
               for (t_int i=0; i<this->pieces_size; i++)
                  {
                     list_to_return.push_back(0);
                  }
            }

         return list_to_return;
      }

      std::string fileInformation::toString() const
      {
         t_int const lineWidth = 79;

         std::string output;

         output += "Filename: ";
         output += this->filename;
         output += GPD->sNEWLINE();

         output += "Size: ";
         output += convertToString<t_uint>(this->pieces_size);
         output += GPD->sNEWLINE();

         output += "Bytes per piece: ";
         output += convertToString<t_uint>(this->bytesPerPiece);
         output += GPD->sNEWLINE();



         switch (this->status)
            {
            case fileInformation::DONE_ALL:
               {
                  output += "All bits set.";
                  output += GPD->sNEWLINE();
                  break;
               }
            case fileInformation::DONE_NONE:
               {
                  output += "All bits unset.";
                  output += GPD->sNEWLINE();
                  break;
               }
            case fileInformation::DONE_UNDEF:
               {
                  if (this->pieces_size <= 0)
                     {
                        output += "Undefined.";
                        output += GPD->sNEWLINE();
                     }
                  else
                     {
                        t_uint const max_char = lineWidth;
                        t_uint charcount      = 0;
                        t_bitVectorCI iter;

                        for (iter = pieces.begin(); iter != pieces.end(); iter++)
                           {
                              if (*iter)
                                 {
                                    output += "X";
                                 }
                              else
                                 {
                                    output += "_";
                                 }

                              charcount++;
                              if (charcount >= max_char)
                                 {
                                    charcount  = 0;
                                    output    += GPD->sNEWLINE();
                                 }
                           } // for iter

                     } // else
                  break;
               }
            } // switch

         return output;
      }

      bool fileInformation::isFull() const
      {
         bool op_status = false;
         if (this->status == fileInformation::DONE_ALL)
            {
               op_status = true;
            }
         return op_status;
      }

      bool fileInformation::isEmpty() const
      {
         bool op_status = false;
         if (this->status == fileInformation::DONE_NONE)
            {
               op_status = true;
            }
         return op_status;
      }

      t_ulong fileInformation::getFileSize() const
      {
         return fileSize;
      }

      fileInformation::~fileInformation()
      {
      }

      /* */

      selectedFileEntry::selectedFileEntry()
         : Serializable(),
           filename_("none"),
           selected_(true)
      {
      }

      selectedFileEntry::selectedFileEntry(std::string const& _filename)
         : Serializable(),
           filename_(_filename),
           selected_(true)
      {
      }

      selectedFileEntry::selectedFileEntry(std::string const& _filename,
                                           bool const _selected)
         : Serializable(),
           filename_(_filename),
           selected_(_selected)
      {

      }

      bool selectedFileEntry::serialize(btg::core::externalization::Externalization* _e) const
      {
         BTG_RCHECK( _e->stringToBytes(&this->filename_) );
         BTG_RCHECK( _e->boolToBytes(this->selected_) );
         return true;
      }

      bool selectedFileEntry::deserialize(btg::core::externalization::Externalization* _e)
      {
         BTG_RCHECK( _e->bytesToString(&this->filename_) );
         BTG_RCHECK( _e->bytesToBool(this->selected_) );

         return true;
      }

      void selectedFileEntry::select()
      {
         selected_ = true;
      }

      void selectedFileEntry::deSelect()
      {
         selected_ = false;
      }

      std::string selectedFileEntry::filename() const
      {
         return filename_;
      }

      bool selectedFileEntry::selected() const
      {
         return selected_;
      }

      selectedFileEntry::selectedFileEntry(selectedFileEntry const& _entry)
         : Serializable(),
           filename_(_entry.filename_),
           selected_(_entry.selected_)
      {
      }

      bool selectedFileEntry::operator== (selectedFileEntry const& _entry) const
      {
         if (
             (_entry.filename_ == filename_) &&
             (_entry.selected_ == selected_)
             )
            {
               return true;
            }

         return false;
      }

      selectedFileEntry& selectedFileEntry::operator= (selectedFileEntry const& _entry)
      {
         bool eq = (_entry == *this);
         if (!eq)
            {
               filename_ = _entry.filename_;
               selected_ = _entry.selected_;
            }

         return *this;
      }

      selectedFileEntry::~selectedFileEntry()
      {
         
      }

      /* */

      selectedFileEntryList::selectedFileEntryList()
         : Serializable(),
           files_()
      {
      }

      selectedFileEntryList::selectedFileEntryList(std::vector<selectedFileEntry> const& _files)
         : Serializable(),
           files_(_files)
      {

      }

      selectedFileEntryList::selectedFileEntryList(selectedFileEntryList const& _list)
         : Serializable(),
           files_(_list.files_)
      {
      }

      void selectedFileEntryList::add(std::string const& _filename, 
                                      bool _selected)
      {
         selectedFileEntry sfe(_filename, _selected);
         files_.push_back(sfe);
      }

      bool selectedFileEntryList::operator== (selectedFileEntryList const& _list) const
      {
         if (_list.files_ == files_)
            {
               return true;
            }

         return false;
      }

      selectedFileEntryList& selectedFileEntryList::operator= (selectedFileEntryList const& _list)
      {
         bool eq = (_list == *this);
         if (!eq)
            {
               files_ = _list.files_;
            }

         return *this;
      }

      bool selectedFileEntryList::serialize(btg::core::externalization::Externalization* _e) const
      {
         t_int const list_size = files_.size();

         // Write the length of the list:
         BTG_RCHECK( _e->intToBytes(&list_size) );

         // Write each entry.
         std::vector<selectedFileEntry>::const_iterator iter;
         for (iter = files_.begin();
              iter != files_.end();
              iter++)
            {
               BTG_RCHECK( iter->serialize(_e) );
            }
         
         return true;
      }

      bool selectedFileEntryList::deserialize(btg::core::externalization::Externalization* _e)
      {
         t_int list_size = 0;

         // First, get the size of the list:
         BTG_RCHECK( _e->bytesToInt(&list_size) );

         selectedFileEntry sfe;
         for (t_int i=0; i<list_size; i++)
            {
               BTG_RCHECK( sfe.deserialize(_e) );
               files_.push_back(sfe);
            }

         return true;
      }

      void selectedFileEntryList::select(std::string const& _filename)
      {
         setFlag(_filename, true);
      }

      void selectedFileEntryList::deSelect(std::string const& _filename)
      {
         setFlag(_filename, false);
      }
      
      std::vector<selectedFileEntry> selectedFileEntryList::files() const
      {
         return files_;
      }

      void selectedFileEntryList::setFiles(std::vector<selectedFileEntry> const& _files)
      {
         files_ = _files;
      }

      void selectedFileEntryList::setFlag(std::string const& _filename, 
                                          bool const _flagValue)
      {
         std::vector<selectedFileEntry>::iterator iter;
         for (iter = files_.begin();
              iter != files_.end();
              iter++)
            {
               if (iter->filename() == _filename)
                  {
                     break;
                  }
            }

         if (iter != files_.end())
            {
               if (_flagValue)
                  {
                     iter->select();
                  }
               else
                  {
                     iter->deSelect();
                  }
            }
      }

      t_int selectedFileEntryList::size() const
      {
         return files_.size();
      }

      bool selectedFileEntryList::selected(std::string const& _filename) const
      {
         std::vector<selectedFileEntry>::const_iterator iter;
         for (iter = files_.begin();
              iter != files_.end();
              iter++)
            {
               if (iter->filename() == _filename)
                  {
                     break;
                  }
            }

         if (iter != files_.end())
            {
               if (iter->selected())
                  {
                     return true;
                  }
            }

         return false;
      }

      selectedFileEntryList::~selectedFileEntryList()
      {

      }

      /* */

   } // namespace core
} // namespace btg
