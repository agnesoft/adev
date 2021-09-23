source "sh/common.sh"

action="${1}"

if [[ "${action}" == "" ]] || [[ "${action}" == "help" ]] || [[ "${action}" == "?" ]]; then
    sh/help.sh
elif [[ "${action}" == "analyse" ]]; then
    sh/analyse.sh $2 $3
elif [[ "${action}" == "build" ]]; then
    sh/build.sh $2 $3
elif [[ "${action}" == "format" ]]; then
    sh/format.sh $2 $3
elif [[ "${action}" == "install" ]]; then
    sh/install.sh $2 $3
elif [[ "${action}" == "test" ]]; then
    sh/test.sh $2 $3
else
    print_error "ERROR: unknown action '${action}'. Run './adev.sh' to display available actions."
fi
