#!/usr/bin/env bash

HTMLDIR=./html
test -d ${HTMLDIR} && rm -fr ${HTMLDIR}
mkdir ${HTMLDIR}
cp tutorial.html ${HTMLDIR}
mkdir ${HTMLDIR}/styles/ ${HTMLDIR}/code ${HTMLDIR}/images ${HTMLDIR}/scripts
cp code/*.cpp ${HTMLDIR}/code/
rm -f ${HTMLDIR}/code/tutorial_pba_noheader.cpp
cp styles/*.css ${HTMLDIR}/styles/
cp images/*.png ${HTMLDIR}/images/
cp scripts/jquery-1.4.min.js ${HTMLDIR}/scripts/

exit 0

# end
