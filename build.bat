set PROJECTS_ROOT=%cd%\projects

mkdir build_msvc
cd build_msvc
mkdir bin

set ASTL_INCLUDE_PATH=%PROJECTS_ROOT%\astl\include
set CPP_FLAGS=/nologo /std:c++latest /EHsc /I"%ASTL_INCLUDE_PATH%" /headerUnit "%ASTL_INCLUDE_PATH%\astl.hpp=astl.hpp.ifc"

REM astl
cl.exe %CPP_FLAGS% /c /exportHeader /Fo /TP "%ASTL_INCLUDE_PATH%\astl.hpp"

REM astl_test
cl.exe %CPP_FLAGS% /Fe"bin\astl_test.exe" "%PROJECTS_ROOT%\astl\test\main.cpp" astl.obj

REM atest
cl.exe %CPP_FLAGS% /c /Fo /interface "%PROJECTS_ROOT%\atest\atest.ixx"

REM atest_test
cl.exe %CPP_FLAGS% /Fe"bin\atest_test.exe" "%PROJECTS_ROOT%\atest\test\atest_test.cpp" "%PROJECTS_ROOT%\atest\test\atest_nomain_test.cpp" atest.obj astl.obj

cd ..
