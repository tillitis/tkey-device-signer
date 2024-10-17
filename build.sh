#! /bin/sh

# Copyright (C) 2023 - Tillitis AB
# SPDX-License-Identifier: BSD-2-Clause

set -eu

tkey_libs_version="v0.1.2"

printf "Building tkey-libs with version: %s\n" "$tkey_libs_version"

if [ -d ../tkey-libs ]
then
    (cd ../tkey-libs; git checkout "$tkey_libs_version")
else
    git clone -b "$tkey_libs_version" https://github.com/tillitis/tkey-libs.git ../tkey-libs
fi

make -j -C ../tkey-libs

make -j
