#!/bin/bash
set -e
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.."

rm -rf "out"
mkdir -pv "out/include"
cp -rfv "libhashset/include/"*.h "out/include"

for target in i686 x86_64 arm64; do
	compiler="/usr/local/musl/${target}/bin/musl-gcc"
	target="$("${compiler}" -dumpmachine)"
	echo -e "--------------------------------\n${target}\n--------------------------------"
	make CC="${compiler}" clean
	make CC="${compiler}"
	mkdir -pv "out/lib/${target}" "out/bin/${target}"
	cp -rv "libhashset/lib/"* "out/lib/${target}"
	for i in example test; do
		for j in hash-set hash-map; do
			cp -fv "${i}/${j}/bin/${i}-${j}" "out/bin/${target}"
		done
	done
done
