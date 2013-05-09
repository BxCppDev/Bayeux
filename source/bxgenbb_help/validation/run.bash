#!/usr/bin/env bash

pgs=$(grep -n "^\[name=\"" ${GENBB_HELP_ROOT_DIR}/share/genbb_help/resources/manager/config/pro-1.0/*.conf | cut -d= -f2 | cut -d' ' -f1 | tr -d "\"")

pgs="\
K40 \
Bi214_Po214 \
Tl208 \
Co60 \
Cs137 \
Bi212_Po212 \
Pa234m \
Na22 \
Mn54 \
Sr90 \
Bi207 \
Am241 \
bb0nu_Ca48 \
bb2nu_Ca48 \
bb2nu_Ca48_HE \
bb0nu_Se82 \
bb2nu_Se82 \
bb2nu_Se82_HE \
bb0nu_Nd150 \
bb2nu_Nd150 \
bb2nu_Nd150_HE \
"

only="$1"

pgs_fortran=$(grep -n "^\[name=\"" ${GENBB_HELP_ROOT_DIR}/share/genbb_help/resources/manager/config/pro-1.0-fortran/*.conf | cut -d= -f2 | cut -d' ' -f1 | tr -d "\"")

echo "Generators: ${pgs}"
###pgs="Bi214_Po214"

nevents=50000
seed=314159
ofext=.xml

work_dir="/transcend/work/tmp"
if [ ! -d ${work_dir} ]; then
    mkdir -p ${work_dir}
    if [ $? -ne 0 ]; then
	echo "ERROR: Cannot create temp dir '${work_dir}' !" 1>&2
	exit 1
    fi
fi

for pg in ${pgs}; do
    if [ "x${only}" != "x" ]; then
	if [  "x${only}" != "x${pg}" ]; then
	    continue
	fi
    fi
    if [ "x${pg}" = "xAm241" ]; then
	nevents=50000
    fi

    echo "NOTICE: Run '${pg}' with C++ library..." 1>&2
    ofcpp="${work_dir}/histos_${pg}-cpp${ofext}"
    offortran="${work_dir}/histos_${pg}-fortran${ofext}"
    delayed_opt=" "
    delayed_histos_opt=" "
    delayed_opt="--delayed"
    delayed_histos_opt=" --histo-def ./config/inspector_histos_delayed.conf "
    if [ "x${pg:0:5}" = "xBi214" -o "x${pg:0:5}" = "xBi212" ]; then
	delayed_opt="--delayed"
	delayed_histos_opt=" --histo-def ./config/inspector_histos_delayed.conf "
    fi
    name_suffix_cpp=".cpp"
    name_suffix_fortran=".fortran"
    title_prefix_cpp="${pg}"
    title_prefix_fortran="${pg}"
    #if [ "x${ofext}" = "x.root" ]; then
    title_prefix_cpp="${pg} (decay0 C++ port)"
    title_prefix_fortran="${pg} (decay0 fortran version)"
    #fi
    genbb_inspector \
	--configuration "${GENBB_HELP_ROOT_DIR}/share/genbb_help/resources/manager/config/pro-1.0/manager.conf" \
	--generator "${pg}" \
	--prng-seed ${seed} \
	--prng-tracker "${work_dir}/prng-cpp.trk" \
	--number-of-events ${nevents} \
	--histo-def "./config/inspector_histos_prompt.conf" \
	${delayed_histos_opt} \
	--prompt-time-limit 1 \
	--prompt \
	${delayed_opt} \
	--name-suffix "${name_suffix_cpp}" \
	--title-prefix "${title_prefix_cpp}" \
	--output-file ${ofcpp} \
	--output-file ${work_dir}/histos_${pg}-cpp.root \
	> ${work_dir}/${pg}-cpp.log

    echo "NOTICE: Run '${pg}' with Fortran lib..." 1>&2

    dummy=$(echo $pgs_fortran| tr ' ' '\n' | grep ${pg})
    if [ "x${dummy}" = "x" ]; then
	echo "ERROR: Cannot find particle generator '${pg}' in the Fortran configuration..." 1>&2
	break
    fi
    genbb_inspector \
	--configuration "${GENBB_HELP_ROOT_DIR}/share/genbb_help/resources/manager/config/pro-1.0-fortran/manager.conf" \
	--generator "${pg}" \
	--prng-seed ${seed} \
	--prng-tracker "${work_dir}/prng-fortran.trk" \
	--number-of-events ${nevents} \
	--histo-def "./config/inspector_histos_prompt.conf" \
	${delayed_histos_opt} \
	--prompt-time-limit 1 \
	--prompt \
	${delayed_opt} \
	--name-suffix "${name_suffix_fortran}" \
	--title-prefix "${title_prefix_fortran}" \
	--output-file ${offortran} \
	--output-file ${work_dir}/histos_${pg}-fortran.root \
	> ${work_dir}/${pg}-fortran.log

    diff ${ofcpp} ${offortran}
    if [ $? -ne 0 ]; then
	echo "WARNING: C++ and Fortran outputs are different for ${pg} !"
    fi

    #break
done

exit 0

# end
