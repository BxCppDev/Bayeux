#!/bin/bash
# -*- mode: shell-script; -*-

cat /usr/include/gsl/gsl_const_mksa.h | grep "#define GSL_CONST_"  |  sed 's/#define //g' | cut -d ' '  -f1  | sed 's/MKSA/MKS/g' > mksa.lis

cat /usr/include/gsl/gsl_const_mks.h | grep "#define GSL_CONST_"  |  sed 's/#define //g' | cut -d ' '  -f1 > mks.lis

diff mksa.lis mks.lis
if [ $? -eq 0 ]; then
	echo "No difference between 'gsl_const_mksa.h' and 'gsl_const_mks.h'." 1>&2
fi

rm -f mksa.lis mks.lis

cat /usr/include/gsl/gsl_const_mksa.h | grep "#define GSL_CONST_"  |  sed 's/#define //g' | cut -d ' '  -f1 |  sort > mksa_consts.lis

cat /usr/include/gsl/gsl_const_num.h | grep "#define GSL_CONST_"  |  sed 's/#define //g' | cut -d ' '  -f1 |  sort > num_consts.lis

cat /usr/include/gsl/gsl_const_cgs.h | grep "#define GSL_CONST_"  |  sed 's/#define //g' | cut -d ' '  -f1 |  sort > cgs_consts.lis

cat /usr/include/gsl/gsl_const_cgsm.h | grep "#define GSL_CONST_"  |  sed 's/#define //g' | cut -d ' '  -f1 |  sort > cgsm_consts.lis

### header:
header=__constants.h
source=__constants.cc

test -f ${header} && rm -f  ${header} 
test -f ${source} && rm -f  ${source} 

touch ${header} 
touch ${source} 

cat>>${header}<<EOF
// mygsl::constants.h

#ifndef __mygsl__constants_h 
#define __mygsl__constants_h 1

#include <gsl/gsl_const_mksa.h>
#include <gsl/gsl_const_cgsm.h>
#include <gsl/gsl_const_num.h>

namespace mygsl {

  namespace constants {

EOF

cat>>${source}<<EOF
// mygsl::constants.cc

#include <mygsl/constants.h>

namespace mygsl {

  namespace constants {

EOF


### MKSA 
python tools/make_consts.py mksa_consts.lis GSL_CONST_MKSA_ include >>${header}
python tools/make_consts.py mksa_consts.lis GSL_CONST_MKSA_ source >>${source}

### NUM 
python tools/make_consts.py num_consts.lis  GSL_CONST_NUM_ include >>${header}
python tools/make_consts.py num_consts.lis  GSL_CONST_NUM_ source >>${source}

### CGSM 
python tools/make_consts.py cgsm_consts.lis GSL_CONST_CGSM_ include >>${header}
python tools/make_consts.py cgsm_consts.lis GSL_CONST_CGSM_ source >>${source}

#### header:
cat>>${header}<<EOF

  } // end of namespace constants

} // end of namespace mygsl

#endif // __mygsl__constants_h

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of mygsl::constants.h
EOF

#### source:
cat>>${source}<<EOF

  } // end of namespace constants

} // end of namespace mygsl

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of mygsl::constants.cc
EOF

test -f mksa_consts.lis && rm -f mksa_consts.lis
test -f num_consts.lis  && rm -f num_consts.lis
test -f cgsm_consts.lis  && rm -f cgsm_consts.lis

basedir=${MYGSL_ROOT}

if [ "x${basedir}" = "x" ]; then
    basedir=.
fi

echo "Install files '${header}' and '${source}' in 'include/mygsl/' and 'src/' directories [Y|n] ? "  1>&2
read ANS
if [ "x${ANS}" = "x" -o "x${ANS}" = "xy" ]; then
    
    mv -f ${header}  ${basedir}/include/mygsl/constants.h
    
    mv -f ${source}  ${basedir}/src/constants.cc

fi

exit 0

# end
