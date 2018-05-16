#!/usr/bin/env bash

# A Bash script to build and install devel Bayeux on Ubuntu (16/18.04).

opwd=$(pwd)

function my_exit()
{
    local error_code=$1
    if [ "x${error_code}" = "x" ]; then
	error_code=0
    fi
    shift 1
    local error_msg="$@"
    if [ ${error_code} -ne 0 -a -n "${error_msg}" ]; then
	echo >&2 "[error] ${error_msg}"
    fi
    exit ${error_code}
}

function my_usage()
{
    cat<<EOF

Options:

   --help               : print help
   --debug              : activate debug mode
   --only-configure     : perform configuration stage only

EOF
    return
}

rebuild=0
only_configure=0
debug=0
bayeux_source_dir="${opwd}"
bayeux_version="develop"
install_base_dir=$(pwd)/_install.d
build_base_dir=$(pwd)/_build.d

function cl_parse()
{
    while [ -n "$1" ]; do
	local arg="$1"
	if [ "${arg}" = "--help" ]; then
	    my_usage
	    my_exit 0
	elif [ "${arg}" = "--debug" ]; then
	    debug=1
	elif [ "${arg}" = "--rebuild" ]; then
	    rebuild=1
	elif [ "${arg}" = "--only-configure" ]; then
	    only_configure=1
	fi
	shift 1
    done
    return 0
}


echo >&2 "[info] Bayeux source dir : '${bayeux_source_dir}'"

cl_parse $@
if [ $? -ne 0 ]; then
    echo >&2 "[error] Command line parsing failed!"
    my_exit 1
fi

# Check distribution:
distrib_id=$(cat /etc/lsb-release | grep DISTRIB_ID | cut -d= -f2)
distrib_release=$(cat /etc/lsb-release | grep DISTRIB_RELEASE | cut -d= -f2)

if [ "${distrib_id}" != "Ubuntu" ]; then
    echo >&2 "[error] Not an Ubuntu Linux!"
    my_exit 1
fi

if [ "${distrib_release}" != "16.04" -a "${distrib_release}" != "18.04" ]; then
    echo >&2 "[error] Not an Ubuntu Linux version 16.04 or 18.04! Abort!"
    my_exit 1
else
    echo >&2 "[info] Found Ubuntu Linux ${distrib_release}"
fi

# Check Git source repository:
echo >&2 "[info] Bayeux source directory: '${bayeux_source_dir}'"
ls -l ${bayeux_source_dir}/.git
if [ ! -d ${bayeux_source_dir}/.git ]; then
    echo >&2 "[error] Not the Bayeux source directory."
    app_exit 1
fi
bx_branch=$(git --git-dir=${bayeux_source_dir}/.git --work-tree=${bayeux_source_dir} branch | grep \* | tr -d [[:space:]\*] )
echo >&2 "[info] Bayeux Git branch: '${bx_branch}'"
bayeux_version=${bx_branch}

install_dir=${install_base_dir}/${bayeux_version}
build_dir=${build_base_dir}/${bayeux_version}

# Check Linuxbrew:
which brew > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] Linuxbrew is not setup! Please run linuxbrew_setup! Abort!"
    my_exit 1
else
    echo >&2 "[info] Found Linuxbrew : $(which brew)"
fi

if [ ! -d ${bayeux_source_dir} ]; then
    echo >&2 "[error] Bayeux source directory '${bayeux_source_dir}' does not exist! Abort!"
    my_exit 1
fi

if [ -d ${install_dir} ]; then
    rm -fr ${install_dir}
fi

if [ ! -d ${build_dir} ]; then
    mkdir -p ${build_dir}
else
    if [ ${rebuild} -eq 1 ]; then
	echo >&2 "[info] Rebuilding Bayeux..."
	rm -fr ${build_dir}
	mkdir -p ${build_dir}
    fi
fi

linuxbrew_prefix=$(brew --prefix)
echo >&2 "[info] Linuxbrew prefix : '${linuxbrew_prefix}'"

qt5dir="$(brew --prefix qt5-base)/lib/cmake"
qt_option1="-DQt5Core_DIR=${qt5dir}/Qt5Core"
qt_option2="-DQt5Gui_DIR=${qt5dir}/Qt5Gui"
qt_option3="-DQt5Widgets_DIR=${qt5dir}/Qt5Widgets"
qt_option4="-DQt5Svg_DIR=${qt5dir}/Qt5Svg"

cd ${build_dir}
echo >&2 ""
echo >&2 "[info] Configuring..."
cmake \
    -DCMAKE_BUILD_TYPE:STRING="Release" \
    -DCMAKE_INSTALL_PREFIX:PATH="${install_dir}" \
    -DCMAKE_PREFIX_PATH:PATH="${linuxbrew_prefix}" \
    -DBAYEUX_COMPILER_ERROR_ON_WARNING=ON \
    -DBAYEUX_WITH_IWYU_CHECK=OFF \
    -DBAYEUX_WITH_DEVELOPER_TOOLS=ON \
    -DBAYEUX_WITH_BRIO=ON \
    -DBAYEUX_WITH_CUTS=ON \
    -DBAYEUX_WITH_MYGSL=ON \
    -DBAYEUX_WITH_MATERIALS=ON \
    -DBAYEUX_WITH_DPP=ON \
    -DBAYEUX_WITH_GEOMTOOLS=ON \
    -DBAYEUX_WITH_EMFIELD=ON \
    -DBAYEUX_WITH_GENBB=ON \
    -DBAYEUX_WITH_GENVTX=ON \
    -DBAYEUX_WITH_MCTOOLS=ON \
    -DBAYEUX_WITH_LAHAGUE=OFF \
    -DBAYEUX_WITH_GEANT4_MODULE=ON \
    -DBAYEUX_WITH_MCNP_MODULE=OFF \
    -DBAYEUX_WITH_QT_GUI=ON \
    ${qt_option1} \
    ${qt_option2} \
    ${qt_option3} \
    ${qt_option4} \
    -DBAYEUX_ENABLE_TESTING=ON \
    -DBAYEUX_WITH_DOCS=OFF\
    -DBAYEUX_WITH_DOCS_OCD=OFF \
    -GNinja \
    ${bayeux_source_dir}
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux ${bayeux_version} configuration failed!"
    my_exit 1
fi

if [ ${only_configure} -eq 0 ]; then

    echo >&2 ""
    echo >&2 "[info] Building..."
    ninja -j4
    if [ $? -ne 0 ]; then
	echo >&2 "[error] Bayeux ${bayeux_version} build failed!"
	my_exit 1
    fi

    echo >&2 ""
    echo >&2 "[info] Testing..."
    ninja test
    if [ $? -ne 0 ]; then
	echo >&2 "[error] Bayeux ${bayeux_version} testing failed!"
	my_exit 1
    fi

    echo >&2 ""
    echo >&2 "[info] Installing..."
    ninja install
    if [ $? -ne 0 ]; then
	echo >&2 "[error] Bayeux ${bayeux_version} installation failed!"
	my_exit 1
    fi
fi

my_exit 0

# end
