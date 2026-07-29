#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0+
"""Merge EN7512 manufacturing information into a U-Boot image."""

import argparse
from pathlib import Path

MINFO_OFFSET = 0xFF00
MINFO_SIZE = 0x100


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "copy the 256-byte TCBoot manufacturing block at 0xff00 "
            "from a stock bootloader into u-boot-en7512.bin"
        )
    )
    parser.add_argument("--image", required=True, type=Path,
                        help="u-boot-en7512.bin input")
    parser.add_argument("--stock", required=True, type=Path,
                        help="stock TCBoot image containing manufacturing data")
    parser.add_argument("--output", required=True, type=Path,
                        help="output image")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    image = bytearray(args.image.read_bytes())
    stock = args.stock.read_bytes()
    end = MINFO_OFFSET + MINFO_SIZE

    if len(image) < end:
        raise SystemExit(f"input image is too small: {len(image):#x} < {end:#x}")
    if len(stock) < end:
        raise SystemExit(f"stock image is too small: {len(stock):#x} < {end:#x}")

    image[MINFO_OFFSET:end] = stock[MINFO_OFFSET:end]
    args.output.write_bytes(image)


if __name__ == "__main__":
    main()
