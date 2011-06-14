#!/usr/bin/env bash

export LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH}

echo "" >&2
echo "Running Aprg0..." >&2
./Aprg0
echo "Done." >&2
echo "" >&2

echo "Running Aprg1..." >&2
./Aprg1
echo "Done." >&2
echo "" >&2

echo "Running Aprg2..." >&2
./Aprg2
echo "Done." >&2
echo "" >&2

echo "Running Bprg1..." >&2
./Bprg1
echo "Done." >&2
echo "" >&2

echo "Running Bprg1bis..." >&2
./Bprg1bis
echo "Done." >&2
echo "" >&2

echo "Running Bprg2..." >&2
./Bprg2
echo "Done." >&2
echo "" >&2

echo "Running Bprg3..." >&2
./Bprg3
echo "Done." >&2
echo "" >&2

echo "Running Bprg0..." >&2
./Bprg0
echo "Done." >&2
echo "" >&2

exit 0

# end
