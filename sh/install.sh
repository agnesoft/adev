source ./sh/common.sh

package=$1

function list_packages() {
    echo "Available packages:"

    for script in sh/install_*.sh; do
        if [[ "${script}" =~ sh/install_(.*)\.sh ]]; then
            echo "  ${BASH_REMATCH[1]}"
        fi
    done
}

function install_package() {
    local installScript="./sh/install_${package}.sh"

    if [[ -f "$installScript" ]]; then
        echo "Installing package '$package'..."
        run_install_script $installScript
    else
        print_error "ERROR: Package '$package' does not exist."
        echo ""
        list_packages
        exit 1;
    fi
}

function run_install_script() {
    local installScript=$1
    "${installScript[@]}"
    local status=$?

    if (( $? != 0 )); then
        print_error "ERROR: Installing package '$package' ($1) failed: $STATUS"
        exit 1
    fi
}

if [[ "$package" == "list" ]]; then
    list_packages
else
    install_package
fi
