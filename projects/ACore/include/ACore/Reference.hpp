#ifndef ACORE_REFERENCE_HPP
#define ACORE_REFERENCE_HPP

#include "ACoreModule.hpp"

#include <utility>

namespace acore
{
//! The Reference<T, Data> is a template class that
//! provides custom reference wrapper.
//!
//! It facilitates the reference-like operations for
//! values that cannot rely on standard references
//! (e.g. values stored in a file). It requires the
//! type \c Data to implement the following methods:
//!
//! - <tt> auto %setValue(acore::size_type index, T) -> void </tt>
//! - <tt> auto %value(acore::size_type index) const -> T </tt>
//!
//! Example of creating the \c Data type and instantiating
//! the Reference pointing to the data stored in a
//! file:
//!
//! \snippet ReferenceTest.cpp [usage]
//!
//! In the example we create a struct that knows how
//! to write and retrieve data from/to file and then
//! instantiating the Reference object with the
//! desired index and the data struct's object address.
//! When the operator=() is called the value passed
//! is written to the file. When assigned to the \c T
//! the Reference is implicitely converted to \c T by
//! reading the value from the file.
//!
//! \note operator& is declared deleted. You cannot
//! convert the Reference to a pointer as such operation
//! on non-memory reference does not make sense.
template<typename T, typename Data>
class Reference
{
public:
    //! Constructs a Reference with \a index and \a data.
    constexpr Reference(size_type index, Data *data) noexcept :
        mIndex(index),
        mData(data)
    {
    }

    //! Default copy constructor.
    constexpr Reference(const Reference &other) noexcept = default;

    //! Default move constructor.
    constexpr Reference(Reference &&other) noexcept = default;

    //~ Default destructor.
    ~Reference() = default;

    //! Implicitly converts the Reference to the
    //! \c T by returning the \c Data::value().
    [[nodiscard]] constexpr operator T() const //NOLINT(google-explicit-constructor)
    {
        return mData->value(mIndex);
    }

    //! Same as operator T().
    [[nodiscard]] constexpr auto operator*() const -> T
    {
        return static_cast<T>(*this);
    }

    //! Assigns the \a value to the underlying \c Data
    //! by calling \c Data::setValue().
    constexpr auto operator=(T value) -> Reference &
    {
        mData->setValue(mIndex, std::move(value));
        return *this;
    }

    //! Copy assignment operator. It assigns to the
    //! underlying value, not the Reference itself.
    constexpr auto operator=(const Reference &other) -> Reference &
    {
        *this = *other;
        return *this;
    }

    //! Move assignment operator. It assigns both
    //! the underlying value and the Reference itself.
    constexpr auto operator=(Reference &&other) noexcept -> Reference &
    {
        if (*this != other)
        {
            *this = other;
            other.mIndex = acore::INVALID_INDEX;
            other.mData = nullptr;
        }

        return *this;
    }

    //! The operator is deleted.
    auto operator&() const -> Reference * = delete; //NOLINT(google-runtime-operator)

private:
    size_type mIndex = INVALID_INDEX;
    Data *mData = nullptr;
};

//! \relates acore::Reference
//! Returns \c true if the \a reference's value
//! is equal to the \a value or \c false otherwise.
template<typename T, typename Data>
[[nodiscard]] constexpr auto operator==(Reference<T, Data> reference, const T &value) -> bool
{
    return *reference == value;
}

//! \relates acore::Reference
//! Returns \c true if the \a reference's value
//! is not equal to the \a value or \c false otherwise.
template<typename T, typename Data>
[[nodiscard]] constexpr auto operator!=(Reference<T, Data> reference, const T &value) -> bool
{
    return !(*reference == value);
}

//! \relates acore::Reference
//! Returns \c true if the \a left references has
//! the same value as the \a right.
template<typename T, typename Data>
[[nodiscard]] constexpr auto operator==(Reference<T, Data> left, Reference<T, Data> right) -> bool
{
    return *left == *right;
}

//! \relates acore::Reference
//! Returns \c true if the \a left references has
//! a different value than the \a right.
template<typename T, typename Data>
[[nodiscard]] constexpr auto operator!=(Reference<T, Data> left, Reference<T, Data> right) -> bool
{
    return !(*left == *right);
}
}

#endif
