set PROJECTS_ROOT=%cd%\projects

mkdir build_msvc
cd build_msvc
mkdir bin

set ASTL_INCLUDE_PATH=%PROJECTS_ROOT%\astl\include
set CPP_FLAGS=/nologo /std:c++latest /EHsc /I"%ASTL_INCLUDE_PATH%" /headerUnit "%ASTL_INCLUDE_PATH%\astl.hpp=astl.hpp.ifc"

REM astl
cl.exe /c %CPP_FLAGS% /exportHeader /Fo /TP "%ASTL_INCLUDE_PATH%\astl.hpp"

REM astl_test
cl.exe %CPP_FLAGS% /Fe"bin\astl_test" "%PROJECTS_ROOT%\astl\test\main.cpp" astl.obj

cd ..
