set PROJECTS_ROOT=%cd%\projects

mkdir build_msvc
cd build_msvc
mkdir bin

set CPP_FLAGS=/nologo /std:c++latest /EHsc

REM astl
set ASTL_INCLUDE_PATH=%PROJECTS_ROOT%\astl\include
cl.exe /c %CPP_FLAGS% /exportHeader /Fo /TP "%ASTL_INCLUDE_PATH%\astl.hpp"

REM astl_test
cl.exe %CPP_FLAGS% /I"%ASTL_INCLUDE_PATH%" /headerUnit "%ASTL_INCLUDE_PATH%\astl.hpp=astl.hpp.ifc" /Fe"bin\astl_test" "%PROJECTS_ROOT%\astl\test\main.cpp" astl.obj

cd ..