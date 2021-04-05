@echo off

set PROJECTS_ROOT=%cd%\projects

mkdir build
cd build
mkdir bin

set ASTL_INCLUDE_PATH=%PROJECTS_ROOT%\astl\include
set CPP_FLAGS=/nologo /W4 /WX /std:c++latest /EHsc /I"%ASTL_INCLUDE_PATH%" /headerUnit "%ASTL_INCLUDE_PATH%\astl.hpp=astl.hpp.ifc"

REM astl
cl.exe %CPP_FLAGS% /c /exportHeader /Fo /TP "%ASTL_INCLUDE_PATH%\astl.hpp"

REM astl_test
cl.exe %CPP_FLAGS% /Fe"bin\astl_test.exe" "%PROJECTS_ROOT%\astl\test\main.cpp" astl.obj

REM atest
cl.exe %CPP_FLAGS% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\source_location.cpp"
cl.exe %CPP_FLAGS% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\data.cpp"
cl.exe %CPP_FLAGS% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\stringify.cpp"
cl.exe %CPP_FLAGS% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\printer.cpp"
cl.exe %CPP_FLAGS% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\reporter.cpp"
cl.exe %CPP_FLAGS% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\expect_base.cpp"
cl.exe %CPP_FLAGS% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\expect_tomatch.cpp"
cl.exe %CPP_FLAGS% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\expect_tothrow.cpp"
cl.exe %CPP_FLAGS% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\matcher.cpp"
cl.exe %CPP_FLAGS% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\expect.cpp"
cl.exe %CPP_FLAGS% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\test_runner.cpp"
cl.exe %CPP_FLAGS% /c /Fo /interface /TP "%PROJECTS_ROOT%\atest\atest.cpp"
lib.exe /NOLOGO source_location.obj data.obj stringify.obj printer.obj reporter.obj expect_base.obj expect_tomatch.obj expect_tothrow.obj matcher.obj expect.obj test_runner.obj atest.obj /OUT:atest.lib

REM atest_test
cl.exe %CPP_FLAGS% /Fe"bin\atest_test.exe" "%PROJECTS_ROOT%\atest\test\atest_test.cpp" "%PROJECTS_ROOT%\atest\test\expect_tobe_test.cpp" "%PROJECTS_ROOT%\atest\test\expect_tothrow_test.cpp" "%PROJECTS_ROOT%\atest\test\expect_tomatch_test.cpp" "%PROJECTS_ROOT%\atest\test\printer_test.cpp" "%PROJECTS_ROOT%\atest\test\bad_test_suite.cpp" atest.lib astl.obj

cd ..
