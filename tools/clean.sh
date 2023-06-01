#!/usr/bin/env bash

# A Bash script to clean default devel Bayeux build/installation dirs.
#
#
opwd=$(pwd)
install_dir=$(pwd)/_install.d
build_dir=$(pwd)/_build.d
if [ -d /opt/SW/Bayeux3 ]; then
    build_base_dir="/opt/SW/Bayeux3/_build.d"
    install_base_dir="/opt/SW/Bayeux3/_install.d"
fi
if [ -d ${install_dir} ]; then
    rm -fr ${install_dir}
fi
if [ -d ${build_dir} ]; then
    rm -fr ${build_dir}
fi

cd ${opwd}
exit 0

# end
