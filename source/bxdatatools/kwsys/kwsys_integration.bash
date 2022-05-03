#!/usr/bin/env bash

opwd=$(pwd)
build_dir="$(pwd)/_build.d"
install_dir="$(pwd)/_install.d"
mkdir ${build_dir}
cd ${build_dir}

cmake ..

cd ${opwd}

work_dir="_work.d"
mkdir ${work_dir}


cp ${build_dir}/datatools/Configure.h ${work_dir}/
cp ${build_dir}/datatools/Configure.hxx ${work_dir}/
cp ${build_dir}/datatools/DynamicLoader.hxx ${work_dir}/
cp kwsys-master/DynamicLoader.cxx ${work_dir}/

tree ${work_dir}/

exit 0

# end
