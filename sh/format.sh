source ./sh/common.sh

if isWindows; then
    CLANG_FORMAT="clang-format.exe"
else
    CLANG_FORMAT="clang-format-12"
fi

SOURCE_FILES=$(find ./projects -name "*.cpp" -o -name "*.hpp" -type f)
for file in $SOURCE_FILES
do
    $CLANG_FORMAT -i $file
done

MODIFIED=$(git status --short)

if [[ "$MODIFIED" != "" ]]; then
    echo "$MODIFIED"
    exit 1
else
    exit 0
fi