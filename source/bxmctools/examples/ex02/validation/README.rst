======================================================================
Validation of the  tracking of arbitrarily charged ions  in a constant
electric    field    in     the    Bayeux/mctools    Geant4    module.
======================================================================

This directory contains a Gnuplot  macro that displays the matching of
results obtained  by the Bayeux/mctools ex02  example with theoretical
expectations. This allows to check  that the modelling of the equation
of  motion of  such  ions  in an  electric  field produces  acceptable
results.

 * ``validation.gp`` : The Gnuplot macro
 * ``g4_impacts.data``  : This  files  contains the  positions of  the
   C12(N+) ion  trajectory when  they traverse  the "lab"  and "world"
   volumes boundaries  in the final  stage of the Geant4  tracking. We
   used N=1..6 effective charge for the ions. The electric field
 * ``mctools_g4_ex02_C12_ions_1keV_traj.pdf``   :   The  figure   that
   superimposes the predicted trajectories  with result from the Geant
   tracking.
 * ``mctools_g4_ex02_C12_ion_10keV_traj.pdf``   :   The  figure   that
   superimposes the predicted  trajectory of a C12 2+  ion with result
   from the  Geant tracking.  We check  here the impact of  the ion on
   the detector predicted by Geant4 with the exact solution.

The simulation  have been done using  the ``bxg4_production`` program,
here with the ``C12_ion_1plus_monokinetic_1keV`` event generator: ::

  $ bxg4_production \
            --logging-priority "warning" \
            --number-of-events-modulo 1 \
            --interactive \
            --g4-visu \
            --config "${CONFIG_DIR}/simulation/manager.conf" \
            --vertex-generator-name "gun_mouth_point.vg" \
            --vertex-generator-seed 0 \
            --event-generator-name "C12_ion_1plus_monokinetic_1keV" \
            --event-generator-seed 0 \
            --shpf-seed 0 \
            --g4-manager-seed 0 \
            --output-prng-seeds-file "prng_seeds.save" \
            --output-prng-states-file "prng_states.save" \
            --output-profiles "all_details" \
            --output-data-file "mctools_ex02_output.xml" \
            --g4-macro "${CONFIG_DIR}/simulation/geant4_visualization.mac"

Then the following event generators have been used too:

  * ``C12_ion_1plus_monokinetic_1keV``
  * ``C12_ion_2plus_monokinetic_1keV``
  * ``C12_ion_3plus_monokinetic_1keV``
  * ``C12_ion_4plus_monokinetic_1keV``
  * ``C12_ion_5plus_monokinetic_1keV``
  * ``C12_nucleus_monokinetic_1keV``

The  ions are  emitted along  the x  axis from  a virtual  gun in  the
direction of a  detection block (scintillator). The  electric field is
2500 V/m on the y axis.

Another  test has  been preformed  with a  C12(2+) ion  @ 10  keV with
impact on the entrance face of the detector.

See the ``README.rst`` file in  the base directory of example ``ex02``
for details.
