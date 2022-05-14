#!/usr/bin/env bash

#source "sh/analyse.sh"
source "sh/build.sh"
#source "sh/coverage.sh"
source "sh/format.sh"
source "sh/help.sh"
#source "sh/install.sh"
#source "sh/test.sh"

readonly action="${1}"

if [[ "${action}" == "analyse" ]]; then
    analyse "${2}" "${3}"
elif [[ "${action}" == "help" ]]; then
    help_
elif [[ "${action}" == "format" ]]; then
    format "${2}" "${3}"
elif [[ "${action}" == "install" ]]; then
    install "${2}"
elif [[ "${action}" == "test" ]]; then
    test_ "${2}" "${3}" "${4}" "${5}"
else 
    build_ "${1}" "${2}" "${3}"
fi
