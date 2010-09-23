#!/usr/bin/env bash

which materials-config > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: test_manager.sh: Package 'materials' is not setup ! Abort !" >&2
    exit 1
fi

${MATERIALS_ROOT}/$(materials-config --system)/bin/test_manager --debug \
  ${MATERIALS_ROOT}/resources/test_isotopes.def \
  ${MATERIALS_ROOT}/resources/test_elements.def \
  ${MATERIALS_ROOT}/resources/test_materials.def \
  ${MATERIALS_ROOT}/resources/test_materials_aliases.def

exit 0

# end
