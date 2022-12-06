#!/bin/zsh
set -e
cd -- "${0:a:h}/../../"

mkdir -pv "out/include"
cp -rfv "libhashset/include/"*.h "out/include"

for cpu in x86_64 aarch64; do
	target="${cpu}-apple-darwin"
	echo -e "--------------------------------\n${target}\n--------------------------------"
	make CC="cc -target ${target}"
	mkdir -pv "out/lib/${target}" "out/bin/${target}"
	cp -rv "libhashset/lib/"* "out/lib/${target}"
	for i in example test; do
		for j in hash-set hash-map; do
			cp -fv "${i}/${j}/bin/${i}-${j}" "out/bin/${target}"
		done
	done
done
