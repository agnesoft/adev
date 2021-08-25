source ./sh/common.sh

STATUS=0
TOOLCHAIN=$1
BUILD_ROOT="build/$TOOLCHAIN"
BIN_DIR="$BUILD_ROOT/bin"
IFC_DIR="$BUILD_ROOT/ifc"
PCM_DIR="$BUILD_ROOT/pcm"
EXECUTABLE_SUFFIX=
CLANG=

if isWindows; then
    EXECUTABLE_SUFFIX=".exe"
    CLANG="clang++"
else
    CLANG="clang++-12"
fi

MSVC_COMPILER_FLAGS="/nologo ^
                     /std:c++latest ^
                     /EHsc ^
                     /O2 ^
                     /W4 ^
                     /WX ^
                     /ifcSearchDir \"$IFC_DIR\""
CLANG_COMPILER_FLAGS="-std=c++20 \
                      -Wall \
                      -Wextra \
                      -pedantic \
                      -Wno-missing-field-initializers \
                      -Werror \
                      -fmodules \
                      -stdlib=libc++ \
                      -fimplicit-module-maps"

function build () {
    echo "*** $1 ***"

    if test "$TOOLCHAIN" == "msvc"; then
        buildMSVC "$MSVC"
    elif test "$TOOLCHAIN" == "clang"; then
        buildClang "$CLANG"
    else
        printError "ERROR: Unknown toolchain '$TOOLCHAIN'"
        $STATUS=1
    fi

    exit $STATUS
}

function buildClang () {
    local BUILD_SCRIPT="mkdir -p \"$BIN_DIR\"
$1"
    eval "$BUILD_SCRIPT"
    STATUS=$?
}

function buildMSVC () {
    detectMSVCEnvScript
    local BUILD_SCRIPT="@echo off
call \"${MSVC_ENV_SCRIPT}\" >nul
if not exist \"$BIN_DIR\" mkdir \"$BIN_DIR\" >nul
$1"
    echo "$BUILD_SCRIPT" > build.bat
    cmd //c build.bat
    STATUS=$?
    rm build.bat
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

        printError "ERROR: Visual Studio environment script not found."
        exit 1
    fi
}
