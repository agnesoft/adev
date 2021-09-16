function print_error () {
    echo -e "\033[31m${1}\033[0m"
}

function print_ok () {
    echo -e "\033[32m${1}\033[0m"
}

function is_windows () { 
    test $OSTYPE == "msys" || test $OSTYPE == "cygwin" 
}

function is_linux () { 
    test $OSTYPE == "linux-gnu" 
}

function is_available () {
    local cmd=$(command -v $1)
    test "${cmd}"
}
