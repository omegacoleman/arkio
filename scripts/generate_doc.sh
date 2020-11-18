#! /bin/bash

export PYTHONDONTWRITEBYTECODE=1
export CLANG_RESOURCE_DIR=$(clang -print-resource-dir)
./vendor/m.css/documentation/doxygen.py ./conf.py
rm -rf ./doc
mv ./doxy/html ./doc
rm -rf ./doxy

