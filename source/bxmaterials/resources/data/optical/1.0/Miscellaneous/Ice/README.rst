==========================================
Refractive index for water ice
==========================================

From ``http://refractiveindex.info/database/main/H2O/Warren.yml``:

.. raw:: sh

   $ cat Warren.yml | grep ^[[:space:]]*[[:digit:]]\. > Ice_nk.data
   $ cat Ice_nk.data | tr -s ' ' | sed -e 's/^ //g' | cut -d' ' -f1,2 > Ice_n.data
   $ cat Ice_nk.data | tr -s ' ' | sed -e 's/^ //g' | cut -d' ' -f1,3 > Ice_k.data
..
