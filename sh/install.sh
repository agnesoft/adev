[ -n "$INSTALL_SH" ] && return || readonly INSTALL_SH=1

source "sh/common.sh"

function install() {
    if [[ "${1}" == "list" ]]; then
        list_packages
    else
        install_package "${1}"
    fi
}

function install_package() {
    local -r package="${1}"
    local -r installScript="sh/install/${package}.sh"

    if [[ -f "${installScript}" ]]; then
        echo "Installing package '${package}'..."
        run_script "${installScript}"
    else
        print_error "ERROR: Package '${package}' does not exist."
        exit 1;
    fi
}

function list_packages() {
    echo "Available packages:"

    for script in sh/install/*.sh; do
        if [[ "${script}" =~ sh/install/(.*)\.sh ]]; then
            echo "  ${BASH_REMATCH[1]}"
        fi
    done
}
