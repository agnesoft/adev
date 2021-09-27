source "sh/common.sh"

function coverage() {
    detect_llvm_cov
    detect_llvm_profdata

    result=0

    local objectArgs=""
    local profData=""

    for test in build/clang/bin/*_test${executableExtension}; do
        objectArgs="${objectArgs} -object=${test}"
        export LLVM_PROFILE_FILE="${test}.profraw"
        "${test}">/dev/null
        "${llvmProfdata}" merge "${test}.profraw" -o "${test}.profdata"
        profData="${profData} ${test}.profdata"
    done

    generate_report "${objectArgs}" "${profData}"
    generate_summary "${objectArgs}"

    rm build/clang/bin/*.profdata
    rm build/clang/bin/*.profraw

    exit $result
}

function detect_llvm_cov() {
    if is_available "llvm-cov-12"; then
        llvmCov="llvm-cov-12"
    elif is_available "llvm-cov"; then
        llvmCov="llvm-cov"
    else
        print_error "ERROR: 'llvm-cov' is not available. Try installing it with './adev.sh install llvm'"
        exit 1
    fi
}

function detect_llvm_profdata() {
    if is_available "llvm-profdata-12"; then
        llvmProfdata="llvm-profdata-12"
    elif is_available "llvm-profdata"; then
        llvmProfdata="llvm-profdata"
    else
        print_error "ERROR: 'llvm-profdata' is not available. Try installing it with './adev.sh install llvm'"
        exit 1
    fi
}

function generate_report() {
    local objectArgs=$1
    local profData=$2

    "${llvmProfdata}" merge ${profData} -o build/clang/bin/coverage.profdata
    "${llvmCov}" show ${objectArgs} -output-dir=build/clang/coverage -format=html -instr-profile=build/clang/bin/coverage.profdata
}

function generate_summary() {
    local objectArgs=$1
    local total=$($llvmCov report $objectArgs -instr-profile=build/clang/bin/coverage.profdata | grep "TOTAL.*")
    local match=$(echo $total | perl -nle'print $& while m{[\d\.]+\%}g')
    local matchArr=(${match})
    local region=${matchArr[0]}
    local function=${matchArr[1]}
    local line=${matchArr[2]}
    local branch=${matchArr[3]}

    if [[ "${function}" != "100.00%" ]] || [[ "${line}" != "100.00%" ]]; then
        result=1
    fi

    print_summary "${function}" "${line}" "${region}" "${branch}"
}

function print_summary() {
    local function="${1}"
    local line="${2}"
    local region="${3}"
    local branch="${4}"

    if (( $result == 1 )); then
        print_error "ERROR: insufficient code coverage:"
        print_error "  * Function: ${function} (must be 100.00 %)"
        print_error "  * Line: ${line} (must be 100.00 %)"
        print_error "  * Region: ${region} (can by any)"
        print_error "  * Branch: ${branch} (can by any)"
    else
        print_ok "Code coverage OK:"
        print_ok "  * Function: ${function} (must be 100.00 %)"
        print_ok "  * Line: ${line} (must be 100.00 %)"
        print_ok "  * Region: ${region} (can by any)"
        print_ok "  * Branch: ${branch} (can by any)"
    fi
}

coverage
