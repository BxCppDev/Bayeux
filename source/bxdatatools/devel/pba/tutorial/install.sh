#!/usr/bin/env bash

HTMLDIR=./html
test -d ${HTMLDIR} && rm -fr ${HTMLDIR}
mkdir ${HTMLDIR}
cp tutorial.html ${HTMLDIR}
mkdir ${HTMLDIR}/styles/ ${HTMLDIR}/code ${HTMLDIR}/images
cp code/*.cpp ${HTMLDIR}/code/
rm -f ${HTMLDIR}/code/tutorial_pba_noheader.cpp
cp styles/*.css ${HTMLDIR}/styles/
cp images/*.png ${HTMLDIR}/images/

exit 0

# end
