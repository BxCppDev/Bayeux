#!/usr/bin/env bash


# Usage:
#  ./process.sh --cadfael-dir ${SW_WORK_DIR}/Cadfael/Binary/Cadfael-trunk/Install-Linux-x86_64 --bayeux-dir ${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk/Install-Linux-x86_64
#  ./process.sh --cadfael-dir /data/nemo/group/sw/Cadfael/Binary/Cadfael-trunk/Install-SL5.3-x86_64 --bayeux-dir /data/nemo/group/sw/Bayeux/Binary/Bayeux-trunk/Install-SL5.3-x86_64

while [ -n "$1" ]; do
    token="$1"
    if [ "x${token:0:1}" = "x-" ]; then
	option="${token}"
	if [ "${token}" = "--cadfael-dir" ]; then
	    shift 1
	    export CADFAEL_INSTALL_DIR="$1"
	elif [ "${token}" = "--bayeux-dir" ]; then
	    shift 1
	    export BAYEUX_INSTALL_DIR="$1"
	else
	    logger --stderr --priority warning "Ignoring option '${option}'!"
	fi
    else
	argument="${token}"
	logger --stderr --priority warning "Ignoring argument '${argument}'!"
    fi
    shift 1
done

base_dir=$(pwd)
install_dir=${base_dir}/__install
# Build:
mkdir __build
cd __build
cmake \
    -DCMAKE_INSTALL_PREFIX:PATH=${install_dir} \
    -DCMAKE_FIND_ROOT_PATH:PATH="${BAYEUX_INSTALL_DIR};${CADFAEL_INSTALL_DIR}" \
    ..
if [ $? -ne 0 ]; then
    echo "error: CMake configuration failed !" 1>&2
    exit 1
fi
make
if [ $? -ne 0 ]; then
    echo "error: Build failed !" 1>&2
    exit 1
fi

# Run the freshly built executable:
echo "" 1>&2
echo "======= RUN THE BUILT VERSION =======" 1>&2
DATATOOLS_LIBINFO_TRACE=1 ./bxexample01
if [ $? -ne 0 ]; then
    echo "error: Run failed !" 1>&2
    exit 1
fi
make install
if [ $? -ne 0 ]; then
    echo "error: Installation failed !" 1>&2
    exit 1
fi
cd ..

# Run the installed executable:
echo "" 1>&2
echo "======= RUN THE INSTALLED VERSION =======" 1>&2
export PATH=${install_dir}/bin:${PATH}
bxexample01 --interactive
if [ $? -ne 0 ]; then
    echo "error: Run failed !" 1>&2
    exit 1
fi

# Cleanup
rm -fr __install/
rm -fr __build/
rm -f bxexample01_brio.brio
rm -f bxexample01_datatools.conf
rm -f *~

exit 0
