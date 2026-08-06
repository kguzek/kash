#!/bin/sh
set -eu

version="${1:?version is required}"
echo "$version" > .kash-released-version

sed -i.bak "s/^project(kash VERSION .*/project(kash VERSION ${version})/" CMakeLists.txt
rm -f CMakeLists.txt.bak
