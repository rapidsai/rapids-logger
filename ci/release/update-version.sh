#!/bin/bash
# Copyright (c) 2025, NVIDIA CORPORATION.
########################################
# rapids-logger RAPIDS Version Updater #
########################################

## Usage
# bash update-version.sh <new_version>

# Format is YY.MM.PP - no leading 'v' or trailing 'a'
NEXT_FULL_TAG=$1

# Get <major>.<minor> for next version
NEXT_MAJOR=$(echo "$NEXT_FULL_TAG" | awk '{split($0, a, "."); print a[1]}')
NEXT_MINOR=$(echo "$NEXT_FULL_TAG" | awk '{split($0, a, "."); print a[2]}')
NEXT_PATCH=$(echo "$NEXT_FULL_TAG" | awk '{split($0, a, "."); print a[3]}')
NEXT_SHORT_TAG=${NEXT_MAJOR}.${NEXT_MINOR}

# Need to distutils-normalize the versions for some use cases
NEXT_SHORT_TAG_PEP440=$(python -c "from packaging.version import Version; print(Version('${NEXT_SHORT_TAG}'))")
PATCH_PEP440=$(python -c "from packaging.version import Version; print(Version('${NEXT_PATCH}'))")

echo "Updating RAPIDS versions to $NEXT_FULL_TAG"

# Inplace sed replace; workaround for Linux and Mac
function sed_runner() {
    sed -i.bak ''"$1"'' "$2" && rm -f "${2}".bak
}

# CI files
for FILE in .github/workflows/*.yaml; do
  sed_runner "/shared-workflows/ s/@.*/@branch-${NEXT_SHORT_TAG}/g" "${FILE}"
done
sed_runner "/^RAPIDS_VERSION_MAJOR_MINOR=/ s/=.*/=\"${NEXT_SHORT_TAG}\"/" ci/check_style.sh
sed_runner "s/set(_rapids_version [0-9\.]*)/set\(_rapids_version ${NEXT_FULL_TAG}\)/" rapids_config.cmake
