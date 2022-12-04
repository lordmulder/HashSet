#!/bin/bash
set -e
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.."

mkdir -pv "out/_next/include"
cp -rfv "libhashset/include/"*.h "out/_next/include"

target="$(gcc -dumpmachine)"
echo -e "--------------------------------\n${target}\n--------------------------------"
make CC="gcc" STATIC=1 STRIP=1
rm -rf "out/_next/lib/${target}" "out/_next/bin/${target}"
mkdir -pv "out/_next/lib/${target}" "out/_next/bin/${target}"
cp -rv "libhashset/lib/"* "out/_next/lib/${target}"
for i in example test; do
	for j in hash-set hash-map; do
		cp -fv "${i}/${j}/bin/${i}-${j}.exe" "out/_next/bin/${target}"
	done
done

[ "$(uname -o | tr 'A-Z' 'a-z')" == "cygwin" ] && \
	cp -fv "$(which cygwin1.dll)" "out/_next/bin/${target}" || true
