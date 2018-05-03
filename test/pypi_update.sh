#!/bin/sh
# update mcush to pypi
# account name: pengshulin
# twine must be installed before
# Peng Shulin <trees_peng@163.com>
#


python setup.py sdist

twine upload sdist/*


