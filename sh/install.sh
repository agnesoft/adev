source "sh/common.sh"

function install_package() {
    local package="${1}"
    local installScript="sh/install/${package}.sh"

    if [[ -f "${installScript}" ]]; then
        echo "Installing package '${package}'..."
        run_script "${installScript}"
    else
        print_error "ERROR: Package '${package}' does not exist."
        echo ""
        list_packages
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

if [[ "${1}" == "list" ]]; then
    list_packages
else
    install_package "${1}"
fi
