source ./sh/common.sh

STATUS=0
BUILD_ROOT=build
BIN_DIR="$BUILD_ROOT/bin"
MSVC_COMPILER_FLAGS="/nologo /c /std:c++20 /TP /EHsc /O2 /W4 /WX"

function buildWindows () {
    detectMSVCEnvScript
    local BUILD_SCRIPT="@echo off
call \"${MSVC_ENV_SCRIPT}\" >nul
if not exist \"$BIN_DIR\" mkdir \"$BIN_DIR\" >nul
$1"
    echo "$BUILD_SCRIPT" > build.bat
    cmd //c build.bat
    STATUS=$?
    rm build.bat
    return $STATUS
}

function detectMSVCEnvScript () {
    if ! test "$MSVC_ENV_SCRIPT"; then
        MSVC_ENV_SCRIPT="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Auxiliary/Build/vcvars64.bat"
        if test -f "$MSVC_ENV_SCRIPT"; then
            return
        fi
        
        MSVC_ENV_SCRIPT="C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Auxiliary/Build/vcvars64.bat"
        if test -f "$MSVC_ENV_SCRIPT"; then
            return
        fi

        MSVC_ENV_SCRIPT="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvars64.bat"
        if test -f "$MSVC_ENV_SCRIPT"; then
            return
        fi

        printError "ERROR: Visual Studio environemnt script not found."
        exit 1
    fi
}
