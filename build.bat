@echo off

set PROJECTS_ROOT=%cd%\projects

mkdir build
cd build
mkdir bin

set BUILD_ROOT=%cd%
set ASTL_INCLUDE_PATH=%PROJECTS_ROOT%\astl\include
set RAPIDJSON_INCLUDE_PATH=%PROJECTS_ROOT%\rapidjson\include
set CPP_FLAGS=/nologo ^
              /W4 ^
              /WX ^
              /std:c++20 ^
              /EHsc ^
              /I"%ASTL_INCLUDE_PATH%" ^
              /I"%RAPIDJSON_INCLUDE_PATH%" ^
              /headerUnit "C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\um\Windows.h=%BUILD_ROOT%\Windows.h.ifc" ^
              /headerUnit "%ASTL_INCLUDE_PATH%\astl.hpp=%BUILD_ROOT%\astl\astl.hpp.ifc" ^
              /headerUnit "%RAPIDJSON_INCLUDE_PATH%\rapidjson.hpp=%BUILD_ROOT%\rapidjson\rapidjson.hpp.ifc" ^
              /ifcSearchDir "%BUILD_ROOT%\atest" ^
              /ifcSearchDir "%BUILD_ROOT%\acore" ^
              /ifcSearchDir "%BUILD_ROOT%\abuild" ^
              /ifcSearchDir "%BUILD_ROOT%\abuild_test"
set CPP_FLAGS_OPTIMIZED=/O2 %CPP_FLAGS%

REM astl
mkdir astl
cd astl
cl.exe /O1 %CPP_FLAGS% /c /exportHeader /Fo /TP "%ASTL_INCLUDE_PATH%\astl.hpp"
cd ..

REM rapidjson
mkdir rapidjson
cd rapidjson
cl.exe %CPP_FLAGS_OPTIMIZED% /c /exportHeader /Fo /TP "%RAPIDJSON_INCLUDE_PATH%\rapidjson.hpp"
cd ..

REM acore
mkdir acore
cd acore
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\acore\acore_common.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\acore\commandline_option.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\acore\commandline.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\acore\process_windows.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\acore\process.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /interface /TP "%PROJECTS_ROOT%\acore\acore.cpp"
lib.exe /NOLOGO ^
        /OUT:acore.lib ^
        acore.obj ^
        acore_common.obj ^
        commandline.obj ^
        commandline_option.obj ^
        process.obj ^
        process_windows.obj
cd ..

REM abuild
mkdir abuild
cd abuild
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\settings.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\project.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\dependency.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\file.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\header.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\source.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\module.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\error.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\warning.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\build_cache.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\project_scanner.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\token.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\tokenizer.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /interface /TP "%PROJECTS_ROOT%\abuild\abuild.cpp"
lib.exe /NOLOGO ^
        /OUT:abuild.lib ^
        token.obj ^
        tokenizer.obj ^
        dependency.obj ^
        settings.obj ^
        project.obj ^
        file.obj ^
        header.obj ^
        source.obj ^
        module.obj ^
        error.obj ^
        warning.obj ^
        build_cache.obj ^
        project_scanner.obj ^
        abuild.obj
cl.exe %CPP_FLAGS_OPTIMIZED% ^
       /Fe"%BUILD_ROOT%\bin\abuild.exe" ^
       "%PROJECTS_ROOT%\abuild\main.cpp" ^
       "%BUILD_ROOT%\acore\acore.lib" ^
       "%BUILD_ROOT%\abuild\abuild.lib" ^
       "%BUILD_ROOT%\astl\astl.obj" ^
       "%BUILD_ROOT%\rapidjson\rapidjson.obj"
cd ..

REM atest
mkdir atest
cd atest
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\data.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\stringify.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\printer.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\reporter.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\expect_base.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\expect_tomatch.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\expect_tothrow.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\matcher.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\expect.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\atest\test_runner.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /interface /TP "%PROJECTS_ROOT%\atest\atest.cpp"
lib.exe /NOLOGO ^
        /OUT:atest.lib ^
        data.obj ^
        stringify.obj ^
        printer.obj ^
        reporter.obj ^
        expect_base.obj ^
        expect_tomatch.obj ^
        expect_tothrow.obj ^
        matcher.obj ^
        expect.obj ^
        test_runner.obj ^
        atest.obj
cd ..

REM atest_test
mkdir atest_test
cd atest_test
cl.exe %CPP_FLAGS_OPTIMIZED% ^
       /Fe"%BUILD_ROOT%\bin\atest_test.exe" ^
       /ifcSearchDir "%BUILD_ROOT%\atest" ^
       "%PROJECTS_ROOT%\atest\test\main.cpp" ^
       "%PROJECTS_ROOT%\atest\test\expect_tobe_test.cpp" ^
       "%PROJECTS_ROOT%\atest\test\expect_tothrow_test.cpp" ^
       "%PROJECTS_ROOT%\atest\test\expect_tomatch_test.cpp" ^
       "%PROJECTS_ROOT%\atest\test\printer_test.cpp" ^
       "%PROJECTS_ROOT%\atest\test\bad_test_suite.cpp" ^
       "%PROJECTS_ROOT%\atest\test\assert_test.cpp" ^
       "%BUILD_ROOT%\atest\atest.lib" ^
       "%BUILD_ROOT%\astl\astl.obj"
cd ..

REM astl_test
mkdir astl_test
cd astl_test
cl.exe %CPP_FLAGS_OPTIMIZED% ^
       /Fe"%BUILD_ROOT%\bin\astl_test.exe" ^
       "%PROJECTS_ROOT%\astl\test\main.cpp" ^
       "%BUILD_ROOT%\astl\astl.obj"
cd ..

REM acore_test
mkdir acore_test
cd acore
cl.exe %CPP_FLAGS_OPTIMIZED% ^
       /Fe"%BUILD_ROOT%\bin\acore_test.exe" ^
       "%PROJECTS_ROOT%\acore\test\main.cpp" ^
       "%PROJECTS_ROOT%\acore\test\commandline_option_test.cpp" ^
       "%PROJECTS_ROOT%\acore\test\commandline_test.cpp" ^
       "%PROJECTS_ROOT%\acore\test\process_test.cpp" ^
       "%BUILD_ROOT%\astl\astl.obj" ^
       "%BUILD_ROOT%\atest\atest.lib" ^
       "%BUILD_ROOT%\acore\acore.lib"
cd ..

REM abuild_test
mkdir abuild_test
cd abuild_test
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\test\test_cache.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\test\test_project.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /internalPartition "%PROJECTS_ROOT%\abuild\test\test_project_with_content.cpp"
cl.exe %CPP_FLAGS_OPTIMIZED% /c /Fo /interface /TP "%PROJECTS_ROOT%\abuild\test\abuild_test_utilities.cpp"
lib.exe /NOLOGO ^
        /OUT:abuild_test_utilities.lib ^
        abuild_test_utilities.obj ^
        test_cache.obj ^
        test_project.obj ^
        test_project_with_content.obj
cl.exe %CPP_FLAGS_OPTIMIZED% ^
       /Fe"%BUILD_ROOT%\bin\abuild_test.exe" ^
       "%PROJECTS_ROOT%\abuild\test\main.cpp" ^
       "%PROJECTS_ROOT%\abuild\test\file_test.cpp" ^
       "%PROJECTS_ROOT%\abuild\test\dependency_test.cpp" ^
       "%PROJECTS_ROOT%\abuild\test\header_test.cpp" ^
       "%PROJECTS_ROOT%\abuild\test\source_test.cpp" ^
       "%PROJECTS_ROOT%\abuild\test\module_test.cpp" ^
       "%PROJECTS_ROOT%\abuild\test\project_scanner_headers_test.cpp" ^
       "%PROJECTS_ROOT%\abuild\test\project_scanner_projects_test.cpp" ^
       "%PROJECTS_ROOT%\abuild\test\project_scanner_sources_test.cpp" ^
       "%PROJECTS_ROOT%\abuild\test\settings_test.cpp" ^
       "%PROJECTS_ROOT%\abuild\test\project_test.cpp" ^
       "%PROJECTS_ROOT%\abuild\test\tokenizer_test.cpp" ^
       "%PROJECTS_ROOT%\abuild\test\build_cache_test.cpp" ^
       "%BUILD_ROOT%\astl\astl.obj" ^
       "%BUILD_ROOT%\atest\atest.lib" ^
       "%BUILD_ROOT%\acore\acore.lib" ^
       "%BUILD_ROOT%\abuild\abuild.lib" ^
       "%BUILD_ROOT%\rapidjson\rapidjson.obj" ^
       "%BUILD_ROOT%\abuild_test\abuild_test_utilities.lib"
cd ..

cd ..
