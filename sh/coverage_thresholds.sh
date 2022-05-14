[ -n "$COVERAGE_THRESHOLDS_SH" ] && return || readonly COVERAGE_THRESHOLDS_SH=1

#function | line | region | branch
#e.g. thresholds["abuild.cache.test.exe"]="1 0 1 0"
declare -A thresholds
thresholds["astl.test.exe"]="2 2 2 0"
thresholds["abuild.cache.test.exe"]="3 4 4 0"
thresholds["abuild.scanners.test.exe"]="0 0 0 1"
thresholds["acommandline.test.exe"]="0 1 12 0"
thresholds["aprocess.test.exe"]="1 9 5 5"
thresholds["atest.test.exe"]="2 31 21 2"
thresholds["awinapi.test.exe"]="0 3 1 1"