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
 * $Id: file_info.h,v 1.8.4.13 2007/06/20 19:53:43 wojci Exp $
 */

#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <bitset>
#include <vector>
#include <string>

#include <bcore/serializable.h>
#include <bcore/printable.h>
#include <bcore/project.h>

namespace btg
{
   namespace core
      {
	/**
          * \addtogroup core
          */
         /** @{ */

         /// Holds information about which pieces of a file has been downloaded.
         class fileInformation: public Serializable, public Printable
            {
            public:
               /// Copy constructor.
               /// @param [in] _fileinfo Reference to the object to copy.
               fileInformation(fileInformation const& _fileinfo);

               /// Modifier, which indicates two special cases, when
               /// all bits in the contained vector of bits are set
               /// and when none of the bits are set.
               enum bitsetModifier
                  {
                     DONE_ALL   = 1, //!< All pieces are done.
                     DONE_NONE  = 2, //!< None of the pieces are done.
                     DONE_UNDEF = 0  //!< Not set.
                  };

               /// Constructor.
               /// @param _filename      The filename.
               /// @param _pieces        The pieces of this file.
               /// @param _bytesPerPiece How many bytes is represented by each piece.
               /// @param _file_size     Filesize in bytes.
               fileInformation(std::string const& _filename, 
                               t_bitList const& _pieces, 
                               t_int const _bytesPerPiece, 
                               t_ulong const _file_size);

               /// Constructor. Creates an instance with parts downloaded.
               /// @param _filename      The filename.
               /// @param _bytesPerPiece How many bytes is represented by each piece.
               /// @param _file_size     Filesize in bytes.
               fileInformation(std::string const& _filename,
                                       t_int const _bytesPerPiece,
                                       t_ulong const _file_size);

               /// Default constructor.
               fileInformation();

               /// The equality operator.
               bool operator== (fileInformation const& _fileinfo) const;

               /// Return the number of pieces this class holds.
               t_uint size() const;

               /// Return the exact file size;
               t_ulong getFileSize() const;

               /// Return the filename.
               std::string getFilename() const;

               /// Return the bits.
               t_bitList getBits() const;

               /// Used to convert this object to a byte array. This does not
               /// create a valid representation, since the session information
               /// and command type is not included, only the data from this
               /// object.
               bool serialize(btg::core::externalization::Externalization* _e) const;

               /// Setup this object with data contained in the byte array.
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Returns true if all bits are set.
               bool isFull() const;

               /// Returns true if none of the contained bits are set.
               bool isEmpty() const;

               /// Convert an instance of this class to a std::string.
               /// Used for debugging purposes.
               std::string toString() const;

               /// Destructor.
               virtual ~fileInformation();
            private:
               enum
                  {
                     /// Max passes, uses to limit the number of pieces.
                     MAX_PASS_COUNT = 5
                  };

               /// The filename this class holds piece-info about.
               std::string    filename;
               /// The number of the pieces.
               t_uint         pieces_size;
               /// The pieces of this file.
               t_bitList      pieces;
               /// The number of bytes each piece represents.
               t_int          bytesPerPiece;
               /// The exact file size.
               t_ulong        fileSize;
               /// Modifier, used for not having to keep bits when all or
               /// none of the pieces are set.
               bitsetModifier status;
            };

         /// A file which can be selected or deselected.
         class selectedFileEntry: public Serializable
            {
            public:
               /// Default constructor.
               /// Creates a selected file named "none".
               selectedFileEntry();

               /// Constructor.
               /// Creates a selected file.
               selectedFileEntry(std::string const& _filename);

               /// Constructor.
               /// Creates a file, either selected or deselected.
               selectedFileEntry(std::string const& _filename,
                                 bool const _selected);

               /// Copy constructor.
               selectedFileEntry(selectedFileEntry const& _entry);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Select this file.
               void select();

               /// Deselect this file.
               void deSelect();

               /// Get the filename.
               std::string filename() const;

               /// Get an indication if the file is selected.
               bool selected() const;

               /// The equality operator.
               bool operator== (selectedFileEntry const& _entry) const;

               /// The assignment operator.
               selectedFileEntry& operator= (selectedFileEntry const& _entry);

               /// Destructor.
               virtual ~selectedFileEntry();
            protected:
               /// The filename.
               std::string filename_;

               /// Indicates if the file is selected.
               bool        selected_;
            };

         /// List of files, each is selectable.
         class selectedFileEntryList: public Serializable
            {
            public:
               /// Default constructor.
               selectedFileEntryList();

               /// Constructor.
               selectedFileEntryList(std::vector<selectedFileEntry> const& _files);

               /// Copy constructor.
               selectedFileEntryList(selectedFileEntryList const& _list);

               /// The equality operator.
               bool operator== (selectedFileEntryList const& _list) const;

               /// The assignment operator.
               selectedFileEntryList& operator= (selectedFileEntryList const& _list);

               /// Add an entry.
               void add(std::string const& _filename, 
                        bool _selected = true);

               bool serialize(btg::core::externalization::Externalization* _e) const;
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Select a filename.
               void select(std::string const& _filename);

               /// Unselect a filename.
               void deSelect(std::string const& _filename);

               /// Is a file selected.
               bool selected(std::string const& _filename) const;

               /// Get the contained files.
               std::vector<selectedFileEntry> files() const;

               /// Set the contained files.
               void setFiles(std::vector<selectedFileEntry> const & _files);

               /// Return the size of the contained list.
               t_int size() const;

               /// Destructor.
               virtual ~selectedFileEntryList();
            protected:
               /// Set the flag belonging to a certain filename.
               void setFlag(std::string const& _filename, 
                            bool const _flagValue);

               /// List of files.
               std::vector<selectedFileEntry> files_;
            };

	 /** @} */

      } // namespace core
} // namespace btg

#endif
