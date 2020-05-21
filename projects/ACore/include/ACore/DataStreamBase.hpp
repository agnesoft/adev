#pragma once

#include "Endianness.hpp"

#include <array>
#include <exception>
#include <string>
#include <vector>

namespace acore
{
//! The DataStreamBase is the template base
//! that provides serialization of binary data.
//!
//! It provides common functionality for serialization
//! classes allowing different buffers to be used
//! (e.g. memory, file or network based).
//!
//! The provided serialization is platform independent
//! which means that data on one supported platform
//! will be correctly deserialized on another.
//!
//! The relevant operators are implemented for all
//! in-built types as well as for \c std::vector,
//! \c std::pair and \c std::string types.
//!
//! When you want to support serialization of your
//! own types you should always provide the serialization
//! operator for the DataStreamBase rather than for
//! the particular subclass in order to allow serialization
//! using different DataStreamBase subclasses. It
//! is also advisable to serialize your own types
//! by using existing operators for in-built types
//! and common standard types mentioned above rather
//! than using serialization primitives like read()
//! and write() on the data itself.
//!
//! The internal buffer used as the template argument
//! must implement the following methods:
//!
//! - <tt> auto %read(size_type offset, char *buffer, size_type count) -> void </tt>
//! - <tt> auto resize(size_type size) -> void </tt>
//! - <tt> auto %size() const -> size_type </tt>
//! - <tt> auto %write(size_type offset, const char *buffer, size_type count) -> void </tt>
//!
//! See DataStreamBuffer for an example of the
//! in-memory buffer class.
template<typename Buffer>
class DataStreamBase
{
public:
    //! Returns mutable reference to internal \c Buffer.
    [[nodiscard]] constexpr auto buffer() noexcept -> Buffer &
    {
        return mBuffer;
    }

    //! Returns read-only reference to internal \c Buffer.
    [[nodiscard]] constexpr auto buffer() const noexcept -> const Buffer &
    {
        return mBuffer;
    }

    //! Returns the current position of the stream.
    [[nodiscard]] constexpr auto pos() const noexcept -> size_type
    {
        return mIndex;
    }

    //! Reads the \a count of bytes from the stream
    //! starting at current pos() into the \a data.
    //! The \a data must be pre-allocated and must
    //! be big enough to hold at least \a count bytes.
    //!
    //! If pos() is not within valid range
    //! <tt>(0 < pos() < buffer().size())</tt>
    //! an exception of type acore::Exception is thrown.
    auto read(char *data, size_type count) -> void
    {
        size_type end = mIndex + count;

        if (mIndex < 0 || end < mIndex || mBuffer.size() < end)
        {
            throw std::exception{std::string{"Cannot read outside of the stream's buffer boundaries (current position: " + std::to_string(mIndex) + ", buffer size: " + std::to_string(mBuffer.size()) + ", count to read: " + std::to_string(count) + ')'}.c_str()};
        }

        mBuffer.read(mIndex, data, count);
        mIndex = end;
    }

    //! Resets the current position of the stream
    //! to 0 (beginning).
    constexpr auto reset() noexcept -> void
    {
        mIndex = 0;
    }

    //! Sets the current position of the stream to
    //! \a pos.
    //!
    //! \note No bounds checking happens when this
    //! method is called.
    constexpr auto seek(size_type pos) noexcept -> void
    {
        mIndex = pos;
    }

    //! Writes the \a count bytes from \a data into
    //! the stream starting at the current pos().
    //! If pos() is beyond \c buffer 's \c %size() the
    //! internal buffer is reallocated to accomodate
    //! the new value.
    //!
    //! If pos() is not within valid range
    //! <tt> 0 < pos() </tt>
    //! an exception of type acore::Exception is thrown.
    auto write(const char *data, size_type count) -> void
    {
        if (count <= 0)
        {
            return;
        }

        size_type end = mIndex + count;

        if (mIndex < 0)
        {
            throw std::exception{std::string{"Cannot write outside of the stream's buffer boundaries (current position: " + std::to_string(mIndex) + ", buffer size: " + std::to_string(mBuffer.size()) + ", count to write: " + std::to_string(count) + ')'}.c_str()};
        }

        if (mBuffer.size() < end)
        {
            mBuffer.resize(end);
        }

        mBuffer.write(mIndex, data, count);
        mIndex = end;
    }

protected:
    //! Constructs the stream and passes the \a args
    //! to the constructor of the internal \c Buffer.
    template<typename... Args>
    explicit constexpr DataStreamBase(Args... args) :
        mBuffer(args...)
    {
    }

private:
    Buffer mBuffer;
    size_type mIndex = 0;
};

//! \cond IMPLEMENTAION_DETAIL
template<typename Buffer, typename T>
auto load(DataStreamBase<Buffer> &stream, T &value) -> DataStreamBase<Buffer> &
{
    std::array<char, sizeof(T)> bytes{};
    stream.read(bytes.data(), static_cast<size_type>(bytes.size()));
    memcpy(&value, bytes.data(), bytes.size());

    if constexpr (sizeof(T) > 1)
    {
        value = fromLittleEndian(value);
    }

    return stream;
}

template<typename Temp, typename Buffer, typename T>
auto loadCast(DataStreamBase<Buffer> &stream, T &value) -> DataStreamBase<Buffer> &
{
    Temp temp = 0;
    acore::load(stream, temp);
    value = static_cast<T>(temp);
    return stream;
}

template<typename Buffer, typename T>
auto save(DataStreamBase<Buffer> &stream, T value) -> DataStreamBase<Buffer> &
{
    if constexpr (sizeof(T) > 1)
    {
        value = toLittleEndian(value);
    }

    std::array<char, sizeof(T)> bytes{};
    memcpy(bytes.data(), &value, bytes.size());
    stream.write(bytes.data(), static_cast<size_type>(bytes.size()));
    return stream;
}

template<typename Temp, typename Buffer, typename T>
auto saveCast(DataStreamBase<Buffer> &stream, T value) -> DataStreamBase<Buffer> &
{
    return acore::save(stream, static_cast<Temp>(value));
}
//! \endcond

//! \relates DataStreamBase
//! Serializes the boolean \a value into the \a stream
//! as a single byte. Returns the \a stream.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, bool value) -> DataStreamBase<Buffer> &
{
    return save(stream, value);
}

//! \relates DataStreamBase
//! Deserializes boolean \a value from the \a stream.
//! Returns the \a stream.
template<typename Buffer>
auto operator>>(DataStreamBase<Buffer> &stream, bool &value) -> DataStreamBase<Buffer> &
{
    return load(stream, value);
}

//! \relates DataStreamBase
//! Serializes single byte \a value into the \a stream.
//! Returns the \a stream.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, char value) -> DataStreamBase<Buffer> &
{
    return saveCast<std::uint8_t>(stream, value);
}

//! \relates DataStreamBase
//! Deserializes single byte \a value from the \a stream.
//! Returns the \a stream.
template<typename Buffer>
auto operator>>(DataStreamBase<Buffer> &stream, char &value) -> DataStreamBase<Buffer> &
{
    return loadCast<std::uint8_t>(stream, value);
}

//! \relates DataStreamBase
//! Serializes single byte unsigned number \a value
//! into the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, std::uint8_t value) -> DataStreamBase<Buffer> &
{
    return save(stream, value);
}

//! \relates DataStreamBase
//! Deserializes single byte unsigned number \a value
//! from the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator>>(DataStreamBase<Buffer> &stream, std::uint8_t &value) -> DataStreamBase<Buffer> &
{
    return load(stream, value);
}

//! \relates DataStreamBase
//! Serializes single byte signed number \a value
//! into the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, std::int8_t value) -> DataStreamBase<Buffer> &
{
    return saveCast<std::uint8_t>(stream, value);
}

//! \relates DataStreamBase
//! Deserializes single byte signed number \a value
//! from the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator>>(DataStreamBase<Buffer> &stream, std::int8_t &value) -> DataStreamBase<Buffer> &
{
    return loadCast<std::uint8_t>(stream, value);
}

//! \relates DataStreamBase
//! Serializes single byte unsigned number \a value
//! into the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, std::uint16_t value) -> DataStreamBase<Buffer> &
{
    return save(stream, value);
}

//! \relates DataStreamBase
//! Deserializes single byte unsigned number \a value
//! from the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator>>(DataStreamBase<Buffer> &stream, std::uint16_t &value) -> DataStreamBase<Buffer> &
{
    return load(stream, value);
}

//! \relates DataStreamBase
//! Serializes single byte signed number \a value
//! into the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, std::int16_t value) -> DataStreamBase<Buffer> &
{
    return saveCast<std::uint16_t>(stream, value);
}

//! \relates DataStreamBase
//! Deserializes single byte signed number \a value
//! from the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator>>(DataStreamBase<Buffer> &stream, std::int16_t &value) -> DataStreamBase<Buffer> &
{
    return loadCast<std::uint16_t>(stream, value);
}

//! \relates DataStreamBase
//! Serializes single byte unsigned number \a value
//! into the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, std::uint32_t value) -> DataStreamBase<Buffer> &
{
    return save(stream, value);
}

//! \relates DataStreamBase
//! Deserializes single byte unsigned number \a value
//! from the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator>>(DataStreamBase<Buffer> &stream, std::uint32_t &value) -> DataStreamBase<Buffer> &
{
    return load(stream, value);
}

//! \relates DataStreamBase
//! Serializes single byte signed number \a value
//! into the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, std::int32_t value) -> DataStreamBase<Buffer> &
{
    return saveCast<std::uint32_t>(stream, value);
}

//! \relates DataStreamBase
//! Deserializes single byte signed number \a value
//! from the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator>>(DataStreamBase<Buffer> &stream, std::int32_t &value) -> DataStreamBase<Buffer> &
{
    return loadCast<std::uint32_t>(stream, value);
}

//! \relates DataStreamBase
//! Serializes single byte unsigned number \a value
//! into the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, std::uint64_t value) -> DataStreamBase<Buffer> &
{
    return save(stream, value);
}

//! \relates DataStreamBase
//! Deserializes single byte unsigned number \a value
//! from the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator>>(DataStreamBase<Buffer> &stream, std::uint64_t &value) -> DataStreamBase<Buffer> &
{
    return load(stream, value);
}

//! \relates DataStreamBase
//! Serializes single byte signed number \a value
//! into the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, std::int64_t value) -> DataStreamBase<Buffer> &
{
    return saveCast<std::uint64_t>(stream, value);
}

//! \relates DataStreamBase
//! Deserializes single byte signed number \a value
//! from the \a stream. Returns the \a stream.
template<typename Buffer>
auto operator>>(DataStreamBase<Buffer> &stream, std::int64_t &value) -> DataStreamBase<Buffer> &
{
    return loadCast<std::uint64_t>(stream, value);
}

//! \relates DataStreamBase
//! Serializes the floating point number \a value
//! into the \a stream. Returns the \a stream.
//!
//! \note No endianness conversion is done for floating
//! point numbers.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, float value) -> DataStreamBase<Buffer> &
{
    std::array<char, sizeof(value)> bytes{};
    memcpy(bytes.data(), &value, bytes.size());
    stream.write(bytes.data(), static_cast<size_type>(bytes.size()));
    return stream;
}

//! \relates DataStreamBase
//! Deserializes the floating point number \a value from
//! the \a stream. Returns the \a stream.
//!
//! \note No endianness conversion is done for floating
//! point numbers.
template<typename Buffer>
auto operator>>(DataStreamBase<Buffer> &stream, float &value) -> DataStreamBase<Buffer> &
{
    std::array<char, sizeof(value)> bytes{};
    stream.read(bytes.data(), static_cast<size_type>(bytes.size()));
    memcpy(&value, bytes.data(), bytes.size());
    return stream;
}

//! \relates DataStreamBase
//! Serializes the floating point number \a value
//! into the \a stream. Returns the \a stream.
//!
//! \note No endianness conversion is done for floating
//! point numbers.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, double value) -> DataStreamBase<Buffer> &
{
    std::array<char, sizeof(value)> bytes{};
    memcpy(bytes.data(), &value, bytes.size());
    stream.write(bytes.data(), static_cast<size_type>(bytes.size()));
    return stream;
}

//! \relates DataStreamBase
//! Deserializes the floating point number \a value
//! from the \a stream. Returns the \a stream.
//!
//! \note No endianness conversion is done for floating
//! point numbers.
template<typename Buffer>
auto operator>>(DataStreamBase<Buffer> &stream, double &value) -> DataStreamBase<Buffer> &
{
    std::array<char, sizeof(value)> bytes{};
    stream.read(bytes.data(), static_cast<size_type>(bytes.size()));
    memcpy(&value, bytes.data(), bytes.size());
    return stream;
}

//! \relates DataStreamBase
//! Serializes a null terminated c-string \a value
//! into the stream as \c std::int64_t value representing
//! the string's size obtained by the call to \c strlen()
//! followed by the string data. Returns the \a stream.
//!
//! \note There is no deseializing counterpart to
//! this function as that would be dangerous and
//! potentially leaky with the requiring allocation.
//! Use deserialization into std::string instead.
//!
//! \warning The \a value must be a null terminated
//! string.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, const char *value) -> DataStreamBase<Buffer> &
{
    size_type size = std::strlen(value);
    stream << size;
    stream.write(value, size);
    return stream;
}

//! \relates DataStreamBase
//! Serializes an std::string \a value into the stream
//! as \c std::int64_t value representing the string's
//! size obtained by the call to \c size() followed
//! by the string data. Returns the \a stream.
template<typename Buffer>
auto operator<<(DataStreamBase<Buffer> &stream, const std::string &value) -> DataStreamBase<Buffer> &
{
    const auto size = static_cast<size_type>(value.size());
    stream << size;
    stream.write(value.data(), size);
    return stream;
}

//! \relates DataStreamBase
//! Deserializes the std::string \a value from the
//! \a stream by reading the \c std::int64_t size,
//! resizing the \a value to it and reading the data
//! from the stream into the \a value 's internal
//! buffer. Returns the \a stream.
template<typename Buffer>
auto operator>>(DataStreamBase<Buffer> &stream, std::string &value) -> DataStreamBase<Buffer> &
{
    size_type size = 0;
    stream >> size;
    value.resize(static_cast<size_t>(size));
    stream.read(value.data(), size);
    return stream;
}

//! \relates DataStreamBase
//! Serializes an std::pair \a value into the \a stream
//! by writing type values of \c first and \c second
//! consecutively into the \a stream. Returns the \a stream.
//!
//! \note There needs to be the \c operator<<() for
//! both types of \c first and \c second.
template<typename Buffer, typename First, typename Second>
auto operator<<(DataStreamBase<Buffer> &stream, const std::pair<First, Second> &value) -> DataStreamBase<Buffer> &
{
    return stream << value.first << value.second;
}

//! \relates DataStreamBase
//! Deserializes an std::pair \a value from the
//! \a stream by reading the values of \c first
//! and \c second consecutively from the \a stream.
//! Returns the \a stream.
//!
//! \note There needs to be the \c operator>>() for
//! both types of \c first and \c second.
template<typename Buffer, typename First, typename Second>
auto operator>>(DataStreamBase<Buffer> &stream, std::pair<First, Second> &value) -> DataStreamBase<Buffer> &
{
    return stream >> value.first >> value.second;
}

//! \relates DataStreamBase
//! Serializes an std::vector \a value into the
//! \a stream by storing first its size as
//! \c std::int64_t followed by the elements.
//! Returns the \a stream.
//!
//! \note There needs to be the \c operator<<() for
//! the type \c T.
template<typename Buffer, typename T>
auto operator<<(DataStreamBase<Buffer> &stream, const std::vector<T> &value) -> DataStreamBase<Buffer> &
{
    stream << static_cast<size_type>(value.size());

    for (const T &v : value)
    {
        stream << v;
    }

    return stream;
}

//! \relates DataStreamBase
//! Deserializes an std::vector \a value from the
//! \a stream by reading the size as \c std::int64_t,
//! resizing the vector to that size and reading
//! the elements' values into it. Returns the \a stream.
//!
//! \note There needs to be the \c operator>>() for
//! the type \c T.
template<typename Buffer, typename T>
auto operator>>(DataStreamBase<Buffer> &stream, std::vector<T> &value) -> DataStreamBase<Buffer> &
{
    size_type size = 0;
    stream >> size;
    value.resize(static_cast<size_t>(size));

    for (size_type i = 0; i < size; i++)
    {
        stream >> value[i];
    }

    return stream;
}

//! \relates DataStreamBase
//! Helper overload to allow serializing use of
//! r-value stream.
template<typename Buffer, typename T>
auto operator<<(DataStreamBase<Buffer> &&stream, const T &value) -> DataStreamBase<Buffer> &&
{
    return std::move(stream << value);
}

//! \relates DataStreamBase
//! Helper overload to allow deseriazing use of
//! r-value stream.
template<typename Buffer, typename T>
auto operator>>(DataStreamBase<Buffer> &&stream, T &value) -> DataStreamBase<Buffer> &&
{
    return std::move(stream >> value);
}
}
