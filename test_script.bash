#!/bin/bash

# pretty text awaits
RED=`tput setaf 1`
GRN=`tput setaf 2`
YEL=`tput setaf 3`
MAG=`tput setaf 5`
CYN=`tput setaf 6`
RST=`tput sgr0`

testloc="./examples/tests"
ErrNone=0     # no errors
ErrExcept=1   # std::exception error
ErrParse=2    # parser error
ErrToken=3    # tokenizer error

echo ""

function return_type_to_string {
    local n=$1

    if [ $n == 0 ]; then echo ErrNone
    elif [ $n == 1 ]; then echo ErrExcept
    elif [ $n == 2 ]; then  echo ErrParse
    elif [ $n == 3 ]; then echo ErrToken
    fi
}

function run_test {
    local -n hmap=$1
    local src=${hmap[src]}
    local res=${hmap[res]}

    #echo "./test ${testloc}/$src $res"

    tmp=`./test ${testloc}/$src $res`

    exp=`return_type_to_string $res`
    retd=`return_type_to_string $tmp`

    if [ "$exp" = "$retd" ]; then
        echo "  ${GRN}PASS${YEL} (expected ${exp}, returned ${retd}) ${RST}${testloc}/$src"
    else
        echo "  ${RED}FAIL${YEL} (expected ${exp}, returned ${retd}) ${RST}${testloc}/$src"
    fi

}

declare -A t0=( [src]="mov_fail_rd_not_reg_or_br.txt" [res]="$ErrParse" ) && run_test t0
declare -A t1=( [src]="mov_fail_rd_opbr_not_reg.txt" [res]="$ErrParse" ) && run_test t1
declare -A t1=( [src]="mov_fail_rd_reg_not_comma.txt" [res]="$ErrParse" ) && run_test t1

echo ""