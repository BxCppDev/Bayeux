Primary event generators definitions (version 1.0)
==================================================

The  'genbb::manager'  class  can  be  initialized  through  an  ASCII
configuration   file.   One   must    use   the   ASCII   format   for
'datatools::properties'.  See also: ::

 $ bxocd_manual --action show genbb::manager

Other  ASCII  configuration  files  that contain  the  definitions  of
various  event generators  must be  provided. One  must use  the ASCII
format for 'datatools::multi_properties'.

 - 'manager.conf':  the main configuration file for the manager,
 - 'backgrounds.conf' : background decay generators,
 - 'calibrations.conf' : calibration source decay generators,
 - 'dbd.conf' : various double beta decay generators,
 - 'misc.conf' : other decay generators.
