EN7528 DDR calibration source
=============================

The reconstructed source comes from the XC220 G3 GPL release and executes from
FE SRAM at ``0x9fa30000``. The target is MIPS32r2 little-endian.

Use ``tools/build-econet-ddr.sh en7528`` with a GNU MIPS little-endian
toolchain. ``reference/`` is not used by the build.
