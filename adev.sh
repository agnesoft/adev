action=$1
arg1=$2
arg2=$3

if [[ "${action}" == "analyse" ]]; then
    sh/analyse.sh $arg1
elif [[ "${action}" == "build" ]]; then
    sh/build.sh $arg1 $arg2
elif [[ "${action}" == "format" ]]; then
    sh/format.sh
elif [[ "${action}" == "install" ]]; then
    sh/install.sh $arg1
elif [[ "${action}" == "test" ]]; then
    sh/test.sh $arg1 $arg2
else
    sh/help.sh
fi
