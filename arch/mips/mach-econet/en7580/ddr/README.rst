EN7580 DDR calibration source
=============================

This directory replaces the previously checked-in relocatable vendor objects.
``reconstructed/`` contains assembly source preserving the original section
bytes, symbols and relocations. Several DRAMC translation units use MIPS16;
those instructions remain encoded as source bytes while calls and data
relocations are represented with ``.reloc`` directives.

The resulting code is therefore source-controlled and reproducible without
shipping ``.o`` or DDR ``.bin`` files. It executes at ``0x9fa30000``.

Use ``tools/build-econet-ddr.sh en7580`` with a GNU MIPS little-endian toolchain.
The ARM Cortex-A7 OpenWrt toolchain is not suitable for this target.
