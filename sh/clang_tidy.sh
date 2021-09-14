source ./sh/common.sh

CLANG_TIDY="clang-tidy-12"

function analyse_project () {
    local PROJECT=$1
    local SOURCES=$(find projects/$PROJECT/test -name *.cpp)

    $CLANG_TIDY -header-filter=.* "projects/$PROJECT/$PROJECT.cpp"

    for SOURCE in $SOURCES
    do
        $CLANG_TIDY $SOURCE
    done
}

analyse_project acommandline
analyse_project astl
analyse_project atest
