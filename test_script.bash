#!/bin/bash

t="examples/tests"

cat $t/mov_instruction_testing.txt   >  test_program.txt
cat $t/add_instruction_testing.txt   >> test_program.txt
cat $t/alias_instruction_testing.txt >> test_program.txt

./test test_program.txt

