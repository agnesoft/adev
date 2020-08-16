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

#ifndef AFILE_FILE_HPP
#define AFILE_FILE_HPP

#include "FileRecords.hpp"

namespace afile
{

//! The File provides persistent file-based storage.
//!
//! The File associates stored data with an integral
//! \c index. This index is used to access and modify
//! the data. It is possible to store arbitrary data
//! as long as the operators acore::DataStreamBase
//! \c operator<< and acore::DataStreamBase \c operator>>
//! are provided to handle serialization and deserialization.
//!
//! <b> Writing and Reading Values </b>
//!
//! For example to store a vector of integers and
//! modify one of its values:
//!
//! \snippet AFile/test/FileTest.cpp [vector]
//!
//! \note You need to calculate the offset to particular
//! value and therefore know the binary representation
//! of your data.
//!
//! It is also possible to extend the stored value
//! and input new data to it without the need to
//! deserialize and then serialize it that might be
//! expensive for large values. Continuing from
//! previous example:
//!
//! \snippet AFile/test/FileTest.cpp [extend_vector]
//!
//! The File automatically resizes your data to
//! accomodate the new value and inserts zeroes if
//! the offset is beyond the original record's size.
//!
//! When an error is made the File will report it
//! by throwing an acore::Exception. For example when
//! writing to an invalid index or with negative offset.
//!
//! When reading the values you can either read the whole
//! value or use the offset to read only part of the value.
//! Either use loadValue() that throws an acore::Exception
//! when an error occurs or value() that returns
//! default-constructed value in case of an error.
//!
//! <b> File Data Management </b>
//!
//! The File works similarly to a memory manager.
//! Therefore any modification to a value's size,
//! unless it was at the end of the file, will leave
//! the old value in the file along with the changed
//! one that will be appended. This may be relied
//! upon when appending values for example. There
//! is however a small cost because while appending
//! values will automatically resize your value its
//! index will need to be updated upon every append.
//! If you know the final size of your value consider
//! using resize() to save on the needless updates
//! to the index and related I/O operations.
//!
//! The unused sections of the file can be purged
//! using optimize() method. Depending on the size
//! of the file this might take some time so take
//! good care when you can afford to call it.
//!
//! \warning Only one instance of the File can
//! operate over a single file. If a second instance
//! is created with the same filename that is still
//! in use by another File object an acore::Exception
//! will be thrown.
//!
//! <b> Write Ahead Log </b>
//!
//! The File also features Write Ahead Logging (WAL)
//! mechanism to ensure data integrity and durability.
//! Each File when created will spawn another file
//! called ".<filename>" that serves as the WAL.
//!
//! The WAL is used automatically for every write
//! operation on the File and contains information
//! how to revert the main file into valid state
//! in case of unexpected failure. All pending logs
//! are automatically reverted during the destruction
//! of the File. That can occur for example if a write
//! operation threw an exception or in case of a manual
//! WAL control. If the destructor did not run (e.g.
//! a hardware failure, call to std::terminate etc.) the
//! WAL is used during construction of File to get
//! it into a valid state.
//!
//! You can take manual control over WAL with beginWAL()
//! and endWAL() to make multiple write operations
//! seem atomic and be all reverted in case of failure.
//! The calls to beginWAL() and endWAL() can be
//! nested but it is only when the outer most endWAL()
//! is called the transaction is actually committed
//! into the main file. If the calls do not match
//! the entire transaction that begun with the first
//! call to beginWAL() will be reverted regardless
//! of any intermediate calls to endWAL().
//!
//! Reconstruction a valid state using WAL is an expensive
//! operation and as such is not intended to be used
//! during normal execution of a program. It is strictly
//! tied to the construction and destruction of File.
//! In both cases the File will use the WAL file to
//! get itself into a valid state. The only way to
//! trigger the WAL transaction rollback is therefore
//! destroying the File and creating it anew.
//!
//! \warning Such operation will render all dependent
//! classes and containers invalid. These often rely
//! on the information they loaded from File which
//! is why it is not possible to revert it using WAL
//! during its object lifetime but only at its beginning
//! or end.
class File
{
public:
    //! Default constructs the File with \a filename.
    //! If \a filename exists it is loaded otherwise
    //! it is created.
    explicit File(const char *filename);

    //! Disabled copy constructor.
    File(const File &other) = delete;

    //! Move constructor.
    File(File &&other) = default;

    //! Destroys the File and processes any uncommitted
    //! WAL operations.
    ~File();

    //! Takes manual control over write ahead logging
    //! transaction mechanism. By default every operation
    //! is logged and committed. Calling this function
    //! makes it possible to sequence multiple write
    //! operations as if they were a single one ensuring
    //! their practical atomicity.
    //!
    //! The calls to beginWAL() must be paired with
    //! the exact same number of calls to endWAL().
    //! Only when the last (outer most) endWAL() is
    //! called the transaction is actually committed
    //! as a whole.
    //!
    //! If the calls do not match the WAL transaction
    //! is considered incomplete and will be reverted
    //! during destruction of File.
    auto beginWAL() noexcept -> void;

    //! Removes all values from the file, resizes it
    //! to the minimum size and resets its internals
    //! including the values' index back to 0.
    auto clear() -> void;

    //! Returns \c true if the File contains the \a index
    //! or \c false otherwise.
    [[nodiscard]] auto contains(acore::size_type index) const noexcept -> bool;

    //! Copies the \a size bytes inside a value identified
    //! by the \a index from the \a offset to the
    //! \a newOffset. The \a offset and the \c offset
    //! + \c size must be within the current value's
    //! size. The \a newOffset or the \c newOffset
    //! + \c size however can be outside of the current
    //! value's size or even within the \c offset
    //! + \c size range. If the \a newOffset is outside
    //! of the current value's size the bytes between
    //! the old size and the \a newOffset will be
    //! zeroed out. On the other hand the original
    //! range \c {offset + size} will \c NOT be zeroed
    //! out.
    auto copy(acore::size_type index, acore::size_type offset, acore::size_type newOffset, acore::size_type size) -> void;

    //! Returns the <tt> current number </tt> of
    //! valid indexes stored in the File.
    [[nodiscard]] auto count() const noexcept -> acore::size_type;

    //! Ends manual control over the write ahead logging
    //! transaction mechanism. There must be the same
    //! number of calls to beginWAL() as to endWAL()
    //! to ensure that the transaction being done
    //! is committed. Only when the last (outer most)
    //! endWAL() is called the transaction is actually
    //! committed as a whole.
    //!
    //! If the calls do not match the WAL transaction is
    //! considered incomplete and will be reverted during
    //! destruction of File.
    auto endWAL() -> void;

    //! Returns the \c name of the file the File operates on.
    [[nodiscard]] auto filename() const noexcept -> const char *;

    //! Returns the \c list of currently valid indexes.
    [[nodiscard]] auto indexes() const -> std::vector<acore::size_type>;

    //! Appends the \a value to the file and returns
    //! its new index that can be used as the value's
    //! handle for further operations such as reading,
    //! updating or removing. The \a value is serialized
    //! using acore::DataStreamBase.
    //!
    //! \note Indexes are auto-generated and can be
    //! reused when their corresponding value is
    //! removed from the File and new value is inserted.
    template<typename T>
    constexpr auto insert(const T &value) -> acore::size_type
    {
        beginWAL();
        const acore::size_type index = mRecords.newIndex();
        insert(index, 0, value);
        endWAL();
        return index;
    }

    //! Updates the existing value identified with
    //! the \a index by writing the \a value at
    //! the \a offset. The \a value is seriaized
    //! using acore::DataStreamBase.
    //!
    //! When the \a index is invalid or the \a offset is
    //! negative an acore::Exception is thrown.
    //!
    //! When the \a offset is at value's size or
    //! beyond it the value will first be moved to
    //! the end of the file and extended appropriately
    //! inserting zeroes inbetween the old size and
    //! the \a offset.
    //!
    //! Try to avoid repeatedly extending the value
    //! this way however. While the cost of the
    //! operation is relatively minor once the value
    //! is at the end of the file it still requires
    //! updating the index with the new size every
    //! time. Consider using resize() to get the
    //! desired final size of the value before
    //! inserting multiple times into it.
    template<typename T>
    constexpr auto insert(acore::size_type index, acore::size_type offset, const T &value) -> void
    {
        validateWriteInput(index, offset);
        beginWAL();
        write(index, offset, (acore::DataStream{} << value).buffer().data());
        endWAL();
    }

    //! Returns \c true if there are no valid indexes
    //! stored in the File or \c false otherwise.
    [[nodiscard]] auto isEmpty() const noexcept -> bool;

    //! Reclaims the unused parts of the file moving
    //! all the values next to each other. Call this
    //! function to avoid bloating of the file.
    //!
    //! Depending on its size this operation may take
    //! significant amount of time for large files
    //! with lots of unused sections.
    auto optimize() -> void;

    //! Removes the \a index from the File if it
    //! is present. The data will remain in the file
    //! unless they were stored at the end of the
    //! ile (the file is simply truncated in that
    //! case).
    //!
    //! The \a index might be reused upon the next
    //! insertion of a new value. In order to reclaim
    //! the removed parts of the file call optimize().
    auto remove(acore::size_type index) -> void;

    //! Moves the value indentified by the \a index
    //! to the end of the file (unless it was already
    //! at the end) and extends it or shrinks it to
    //! the \a size bytes.
    //!
    //! Use this method to shrink values that no
    //! longer need as much space or conversely to
    //! enlarge values that will be updated with
    //! more data and will require more space.
    auto resize(acore::size_type index, acore::size_type size) -> void;

    //! Returns the \c size of the underlying file.
    [[nodiscard]] auto size() const noexcept -> acore::size_type;

    //! Returns the \c size of the value associated
    //! with the \a index or the acore::INVALID_INDEX
    //! when the \a index is not valid.
    [[nodiscard]] auto size(acore::size_type index) const noexcept -> acore::size_type;

    //! Returns a value of type \c T deserialized
    //! from the data associated with the \a index.
    //! If the \a index is not valid or if the
    //! deserialization overruns the value's size
    //! an exception is thrown.
    template<typename T>
    [[nodiscard]] constexpr auto value(acore::size_type index) const -> T
    {
        return value<T>(index, 0);
    }

    //! Returns a value of type \c T deserialized
    //! from the data associated with the \a index
    //! at the \a offset. If the \a index or the
    //! \a offset are not valid or if the deserialization
    //! overruns the value's size an exception is thrown.
    template<typename T>
    [[nodiscard]] auto value(acore::size_type index, acore::size_type offset) const -> T
    {
        T val{};
        loadValue(index, offset, val);
        return val;
    }

    //! Disabled copy assignment constructor.
    auto operator=(const File &other) -> File & = delete;

    //! Move assignment operator.
    auto operator=(File &&other) -> File & = default;

private:
    [[nodiscard]] auto extendsValue(acore::size_type index, acore::size_type offset, acore::size_type valueSize) const noexcept -> bool;
    [[nodiscard]] static auto filesInUse() -> std::vector<std::filesystem::path> *;

    template<typename T>
    constexpr auto loadValue(acore::size_type index, acore::size_type offset, T &value) const -> void
    {
        validateReadInput(index, offset);
        mData.read(mRecords.pos(index) + offset) >> value;
        validatePos(index, mData.pos());
    }

    auto moveRecord(acore::size_type index, acore::size_type to, acore::size_type sizeToMove) -> void;
    auto moveRecordToEnd(acore::size_type index, acore::size_type size) -> void;
    [[nodiscard]] auto optimizeRecord(acore::size_type index, acore::size_type pos) -> acore::size_type;
    auto removeData(acore::size_type index) -> void;
    auto resizeAt(acore::size_type recordIndex, acore::size_type newSize) -> void;
    auto resizeAtEnd(acore::size_type index, acore::size_type newSize) -> void;
    auto validateCopyInput(acore::size_type index, acore::size_type offset, acore::size_type newOffset, acore::size_type size) const -> void;
    [[nodiscard]] static auto validateFileNotInUse(const char *filename) -> const char *;
    auto validateIndex(acore::size_type recordIndex) const -> void;
    auto validateOffset(acore::size_type index, acore::size_type offset) const -> void;
    static auto validateOffset(acore::size_type offset) -> void;
    auto validatePos(acore::size_type index, acore::size_type pos) const -> void;
    auto validateReadInput(acore::size_type index, acore::size_type offset) const -> void;
    static auto validateSize(acore::size_type size) -> void;
    auto validateWriteInput(acore::size_type index, acore::size_type offset) const -> void;
    auto write(acore::size_type index, acore::size_type offset, const std::vector<char> &data) -> void;

    FileData mData;
    FileRecords mRecords;
};
}

#endif
