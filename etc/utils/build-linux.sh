#!/bin/bash
set -e
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.."

readonly mode="${1,,}"
if [[ "${mode}" != "gnu" && "${mode}" != "musl" ]]; then
	echo "Error: Parameter must be either \"gnu\" or \"musl\" !!!"
	exit 1
fi

rm -rf "out"
mkdir -pv "out/include"
cp -rfv "libhashset/include/"*.h "out/include"

case "${mode}" in
	gnu)
		for cpu in i686 x86_64 aarch64; do
			compiler="${cpu}-linux-gnu-gcc"
			target="$("${compiler}" -dumpmachine)"
			echo -e "--------------------------------\n${target}\n--------------------------------"
			make CC="${compiler}" STRIP=1
			mkdir -pv "out/lib/${target}" "out/bin/${target}"
			cp -rv "libhashset/lib/"* "out/lib/${target}"
			for i in example test; do
				for j in hash-set hash-map; do
					cp -fv "${i}/${j}/bin/${i}-${j}" "out/bin/${target}"
				done
			done
		done
		;;
	musl)
		for cpu in i686 x86_64 arm64; do
			compiler="/usr/local/musl/${cpu}/bin/musl-gcc"
			target="$("${compiler}" -dumpmachine | sed 's/-gnu/-musl/i')"
			echo -e "--------------------------------\n${target}\n--------------------------------"
			make CC="${compiler}" STATIC=1 STRIP=1
			mkdir -pv "out/lib/${target}" "out/bin/${target}"
			cp -rv "libhashset/lib/"* "out/lib/${target}"
			for i in example test; do
				for j in hash-set hash-map; do
					cp -fv "${i}/${j}/bin/${i}-${j}" "out/bin/${target}"
				done
			done
		done
		;;
esac
