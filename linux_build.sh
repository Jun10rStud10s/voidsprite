#!/bin/sh

unset prefix

no_setup=
run=
_prefix="${PWD}/target/debug"
portable=false
buildtype=debug
while [[ $# -gt 0 ]]; do
	case $1 in
		"--no-setup") no_setup=1; shift;;
		"--run") run=1; shift;;
		"--global") prefix="/usr/local"; shift;;
		"--portable") portable=true; shift;;
		"--prefix") prefix="$2"; shift; shift;;
		"--release") buildtype=release; _prefix="${PWD}/target/release"; shift;;
	esac
done

prefix="${prefix:-${_prefix}}"

if [ ! -e "freesprite" ]; then echo "Not in source directory"; exit 1; fi
if [ -e "$prefix" ]; then rm -r "$prefix"; fi

set -e
if [ "$no_setup" != "1" ] || [ ! -e 'build' ]; then
	if [ -e 'build' ]; then rm -r 'build'; fi
	meson setup --prefix="$prefix" -Dportable="$portable" --buildtype="$buildtype" build
fi
meson compile -C build
meson install --skip-subprojects -C build
set +e

if [ "$run" = "1" ]; then
	"$prefix"/bin/voidsprite
	exit $?
else
	echo ""
	echo "Run the compiled binary with: $prefix/bin/voidsprite"
fi
