// Copyright 2020 Michael Vlach
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef AFILE_FILE_STREAM_HPP
#define AFILE_FILE_STREAM_HPP

#include "FileStreamBuffer.hpp"

namespace afile
{
//! The FileStream provides file based serialization
//! of binary data.
//!
//! It is a file based specialization of acore::DataStreamBase
//! using the file based internal buffer.
//!
//! See acore::DataStreamBase class documentation
//! for serialization details.
class FileStream : public acore::DataStreamBase<FileStreamBuffer>
{
public:
    //! Constructs the stream on the \a filename.
    //! If the \a filename does not exist it will
    //! be created otherwise the file is open in
    //! read/write mode.
    explicit FileStream(const char *filename);
};
}

#endif
