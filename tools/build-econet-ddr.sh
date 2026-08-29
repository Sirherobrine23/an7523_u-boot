#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
set -eu

soc="${1:-}"
srctree="${srctree:-$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)}"
objtree="${objtree:-$srctree}"

case "$soc" in
 en751221)
  dir="$srctree/arch/mips/mach-econet/en751221/ddr"
  cc_default=mips-linux-gnu-
  out="$objtree/en751221_ddr.bin"
  ;;
 en7528)
  dir="$srctree/arch/mips/mach-econet/en7528/ddr"
  cc_default=mipsel-linux-gnu-
  out="$objtree/en7528_ddr.bin"
  ;;
 en7580)
  dir="$srctree/arch/mips/mach-econet/en7580/ddr"
  cc_default=mipsel-linux-gnu-
  out="$objtree/en7580_ddr.bin"
  ;;
 *)
  echo "usage: $0 {en751221|en7528|en7580}" >&2
  exit 2
  ;;
esac

cross="${CROSS_COMPILE:-$cc_default}"
cc="${cross}gcc"
ld="${cross}ld"
objcopy="${cross}objcopy"
build="$objtree/.econet-ddr-$soc"
rm -rf "$build"
mkdir -p "$build"

cflags='-mips32r2 -mno-abicalls -fno-pic -ffreestanding -fno-builtin -Os -G0'
objs=''
for src in "$dir"/reconstructed/*.S; do
 obj="$build/$(basename "${src%.S}").o"
 "$cc" $cflags -x assembler-with-cpp -c "$src" -o "$obj"
 objs="$objs $obj"
done

if [ -f "$dir/glue.c" ]; then
 "$cc" $cflags -fno-stack-protector -c "$dir/glue.c" -o "$build/glue.o"
 objs="$objs $build/glue.o"
fi

"$ld" -T "$dir/ddr.lds" -Map "$build/ddr.map" -o "$build/ddr.elf" $objs
"$objcopy" -O binary "$build/ddr.elf" "$out"

printf '%s\n' "$out"
