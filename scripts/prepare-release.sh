#!/bin/sh
set -eu

version="${NEXT_RELEASE_VERSION:?NEXT_RELEASE_VERSION is required}"

sed -i.bak "s/^project(kash VERSION .*/project(kash VERSION ${version})/" CMakeLists.txt
rm -f CMakeLists.txt.bak

for f in dist/kash-*.tar.gz; do
  [ -e "$f" ] || continue
  suffix="$(basename "$f" .tar.gz | sed 's/^kash-//')"
  mv "$f" "dist/kash-${version}-${suffix}.tar.gz"
done
