#!/bin/bash
# Wrap configure/build/test steps on GitHub Actions into a script

# Set(ings):
# - Echo each command to stdout
# - Stop on first command that fails
set -ex

# Bayeux *cannot* run without the USER env var..
# It may not be set in the image, so....
export USER=`whoami`

# Find ourselves
SELFDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECTDIR="$(dirname $(dirname "${SELFDIR}"))"

echo "[log] USER=${USER}"
echo "[log] SELFDIR=${SELFDIR}"
echo "[log] PROJECTDIR=${PROJECTDIR}"

# Compiler selection
if [ `uname` == "Linux" ] ; then
  # GCC 7 is default
  export CC=gcc-7
  export CXX=g++-7
  export FC=gfortran-7

  # GCC 9 on Ubuntu 20.04
  if [ -e "/usr/bin/gcc-9" ] ; then
    export CC=gcc-9
    export CXX=g++-9
    export FC=gfortran-9
  fi
fi

# Create build directory, deleting if present
rm -Rf build && mkdir build
cd build

qt5_dir="/usr/lib/x86_64-linux-gnu/cmake"
clhep_prefix_dir=$(clhep-config --prefix | tr -d '"')

# Configure, assuming presence of Ninja tool
cmake -DBAYEUX_COMPILER_ERROR_ON_WARNING=ON \
      -DBayeux_WITH_BXDECAY0=OFF \
      -DBAYEUX_WITH_IWYU_CHECK=OFF \
      -DBAYEUX_WITH_DEVELOPER_TOOLS=ON \
      -DBAYEUX_MINIMAL_BUILD=ON \
      -DBAYEUX_ENABLE_TESTING=ON \
      -DBAYEUX_WITH_DOCS=OFF \
      -DBAYEUX_WITH_DOCS_OCD=OFF \
      -DCMAKE_INSTALL_LIBDIR=lib \
      -DBAYEUX_WITH_QT_GUI=ON \
      -DQt5Core_DIR="${qt5_dir}/Qt5Core" \
      -DQt5Gui_DIR="${qt5_dir}/Qt5Gui" \
      -DQt5Widgets_DIR="${qt5_dir}/Qt5Widgets" \
      -DQt5Svg_DIR="${qt5_dir}/Qt5Svg" \
      -DBAYEUX_WITH_CLHEP_MODULE_TRICK=ON \
      -DCLHEP_ROOT_DIR="${clhep_prefix_dir}" \
      -DCMAKE_PREFIX_PATH="${clhep_prefix_dir}" \
      -GNinja \
      $PROJECTDIR

# Build using Ninja to auto-parallelize
ninja

# Run tests - rerunning any that fail in verbose mode
ctest || ctest -VV --rerun-failed

# On Linux, check install time behaviour of programs
if [ `uname` == "Linux" ] ; then
  DESTDIR=$PWD/test-install ninja install
  # Print rpaths
  readelf -d $PWD/test-install/usr/local/lib/libBayeux.so
  # Execute bxquery help to be sure...
  $PWD/test-install/usr/local/bin/bxquery --help
fi

# end
