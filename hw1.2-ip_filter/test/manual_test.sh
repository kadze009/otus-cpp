#!/bin/bash

EXE=${1:-../build/ip_filter}

EXP_HASH="24e7a7b2270daee89c64d3ca5fb3da1a"
ACT_HASH="$(cat ./ip_filter.tsv | $EXE | md5sum | cut -d' ' -f 1)"

if [[ $EXP_HASH == $ACT_HASH ]] ; then
	echo -e "\e[92mTEST PASSED\e[0m"
	exit 0
else
	echo -e "\e[91mTEST FAILED\e[0m\n    exp = ${EXP_HASH}\n    act = ${ACT_HASH}"
	exit 1
fi

