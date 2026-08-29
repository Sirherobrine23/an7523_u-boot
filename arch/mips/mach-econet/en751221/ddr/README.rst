EN751221 DDR calibration source
===============================

The files under ``reconstructed/`` preserve the code, symbols and relocations
from the GPL DDR calibration objects as assembly source. No prebuilt DDR binary
or object is required in the source tree.

The original stage executes from FE SRAM at ``0x9fa32800``. Use
``tools/build-econet-ddr.sh en751221`` with a big-endian GNU MIPS toolchain to
produce the temporary build artifact consumed by Binman.

``reference/`` contains disassembly listings for code review only.
