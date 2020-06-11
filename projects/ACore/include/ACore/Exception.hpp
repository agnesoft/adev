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

#ifndef ACORE_EXCEPTION_HPP
#define ACORE_EXCEPTION_HPP

#include "ACoreModule.hpp"

#include <sstream>
#include <string>
#include <utility>

namespace acore
{
//! The Exception is a standard layout streamable
//! exception.
//!
//! Unlike the <a href="http://en.cppreference.com/w/cpp/error/exception">std::exception</a>
//! the Exception is a standard layout class and
//! can be safely used (and thrown) across library
//! boundaries. Similar to <a href="http://en.cppreference.com/w/cpp/error/exception">std::exception</a>
//! the what() method is provided to access the
//! message associated with the exception.
//!
//! The exception text is composed using operator<<()
//! in order to provide safe and convenient way of
//! composing error messages. The exception text can
//! be retrieved using what() method.
//!
//! To provide different error states inherit from the
//! Exception. In order to keep the <a href="https://en.cppreference.com/w/cpp/types/is_standard_layout">standard layout</a>
//! property you must not add any new data members
//! however.
//!
//! Example:
//! \snippet ExceptionTest.cpp [Usage]
class Exception
{
public:
    //! Returns the null terminated message.
    [[nodiscard]] auto what() const noexcept -> const char *
    {
        return mMessage.c_str();
    }

    //! Appends the arbitrary \a text to the exception's
    //! message. The type \c T must support the
    //! \c std::stringstream::operator<<(). Returns
    //! the r-value of this exception.
    template<typename T>
    auto operator<<(const T &text) -> Exception &&
    {
        std::stringstream stream;
        stream << text;
        mMessage += stream.str();
        return std::move(*this);
    }

private:
    std::string mMessage;
};
}

#endif
