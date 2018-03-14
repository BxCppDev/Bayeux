#!/usr/bin/env bash

boostincdir="$(brew --prefix)/include"

# This build is ok:
g++ -I${boostincdir} diag-bx-issue-13.cxx

# This build fails:
g++ -Werror=shadow -I${boostincdir} diag-bx-issue-13.cxx

# This build is ok
g++ -Werror=shadow -I${boostincdir} diag-bx-issue-13-fix.cxx

# echo >&2 "Running..."
# ./a.out
rm -f a.out

exit 0

# end
