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

#ifndef AFILE_TEST_PCH_HPP
#define AFILE_TEST_PCH_HPP

#ifdef _WIN32
#    pragma warning(push)
#    pragma warning(disable : 4702) //Disabled because of File::insert instantiated with BadValue that throws in the middle of the function
#endif
#include <AFile.hpp>
#ifdef _WIN32
#    pragma warning(pop)
#endif

#endif
