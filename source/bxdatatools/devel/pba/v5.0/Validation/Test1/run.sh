#!/usr/bin/env bash


./prog1
./prog2

od -t u1 a.data > a.log
od -t u1 b.data > b.log

diff a.log b.log

./prog1_alt
./prog2_alt

od -t u1 a_alt.data > a_alt.log
od -t u1 b_alt.data > b_alt.log

diff a_alt.log b_alt.log

exit 0
