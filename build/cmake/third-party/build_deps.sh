#!/usr/bin/env bash

[ ! $# -ge 4 ] &&\
echo "Usage: $0 PLATFORM OCEAN_THIRD_PARTY_SOURCE_DIR BUILD_DIRECTORY_BASE EXTRA_BUILD_FLAGS SUBDIVIDE_INSTALL [OTHER_CMAKE_CONFIG_ARGS...]" &&\
exit 1

PLATFORM=$1
OCEAN_THIRD_PARTY_SOURCE_DIR=$2
BUILD_DIRECTORY_BASE=$3
EXTRA_BUILD_FLAGS=$4
SUBDIVIDE_INSTALL=$5  # "ON" or "OFF"

shift
shift
shift
shift
shift  # One more shift for the new parameter

alldeps=$(grep -v '^#' "${OCEAN_THIRD_PARTY_SOURCE_DIR}"/dependencies_allplatforms.txt)

platfile="${OCEAN_THIRD_PARTY_SOURCE_DIR}"/dependencies_"${PLATFORM}".txt
if [ -f $platfile ] ; then
    alldeps="$alldeps $(grep -v '^#' $platfile)"
fi

echo "Building the following dependencies for the platform \"${PLATFORM}\":"
current_dependency_index=1
for dep in $alldeps; do
    echo "  ${current_dependency_index}. ${dep}"
    current_dependency_index=$((current_dependency_index + 1))
done

# Extract base install prefix from arguments
BASE_INSTALL_PREFIX=""
for arg in "$@"; do
	if [[ $arg == -DCMAKE_INSTALL_PREFIX=* ]]; then
		BASE_INSTALL_PREFIX="${arg#-DCMAKE_INSTALL_PREFIX=}"
		break
	fi
done

if [[ "${SUBDIVIDE_INSTALL}" == "ON" ]]; then
	echo ""
	echo "Per-library subdivision: ENABLED"
	echo "Libraries will be installed to: ${BASE_INSTALL_PREFIX}/[library_name]/"
	echo ""
else
	echo ""
	echo "Per-library subdivision: DISABLED (flat structure)"
	echo "Libraries will be installed to: ${BASE_INSTALL_PREFIX}/"
	echo ""
fi

current_dependency_index=1
for dep in $alldeps; do
	echo ""
	echo ""
	echo ""
	echo ""
	echo ""
	echo ""
	echo "Building: ${current_dependency_index}. ${dep}"
	echo ""
	echo ""
	echo ""
	echo ""
	echo ""
	echo ""

	if [[ "${SUBDIVIDE_INSTALL}" == "ON" ]]; then
		# --- PER-LIBRARY SUBDIVISION MODE ---

		# Build list of previously-installed library subdirectories
		PREVIOUS_LIBS_PATH="${BASE_INSTALL_PREFIX}"
		for prev_dep in $alldeps; do
			if [ "$prev_dep" == "$dep" ]; then
				break
			fi
			if [ -d "${BASE_INSTALL_PREFIX}/${prev_dep}" ]; then
				PREVIOUS_LIBS_PATH="${PREVIOUS_LIBS_PATH};${BASE_INSTALL_PREFIX}/${prev_dep}"
			fi
		done

		# Build new arguments list with per-library install prefix
		NEW_ARGS=()
		for arg in "$@"; do
			if [[ $arg == -DCMAKE_INSTALL_PREFIX=* ]]; then
				NEW_ARGS+=("-DCMAKE_INSTALL_PREFIX=${BASE_INSTALL_PREFIX}/${dep}")
			elif [[ $arg == -DCMAKE_FIND_ROOT_PATH=* ]]; then
				NEW_ARGS+=("-DCMAKE_FIND_ROOT_PATH=${PREVIOUS_LIBS_PATH}")
			else
				NEW_ARGS+=("$arg")
			fi
		done

		echo "Installing to: ${BASE_INSTALL_PREFIX}/${dep}"
		echo "CMAKE_FIND_ROOT_PATH includes $((current_dependency_index - 1)) previously built libraries"

		cmake -S "${OCEAN_THIRD_PARTY_SOURCE_DIR}" -B "${BUILD_DIRECTORY_BASE}"/$dep -DINCLUDED_DEP_NAME=$dep "${NEW_ARGS[@]}" \
		|| exit 1

		eval cmake --build "${BUILD_DIRECTORY_BASE}"/$dep --target install $EXTRA_BUILD_FLAGS \
		|| exit 2
	else
		# --- FLAT STRUCTURE MODE (DEFAULT) ---

		cmake -S "${OCEAN_THIRD_PARTY_SOURCE_DIR}" -B "${BUILD_DIRECTORY_BASE}"/$dep -DINCLUDED_DEP_NAME=$dep "$@" \
		|| exit 1

		eval cmake --build "${BUILD_DIRECTORY_BASE}"/$dep --target install $EXTRA_BUILD_FLAGS \
		|| exit 2
	fi

	current_dependency_index=$((current_dependency_index + 1))
done
