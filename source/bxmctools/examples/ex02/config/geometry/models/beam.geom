# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of the laboratory area
#@key_label   "name"
#@meta_label  "type"


##############################################################
[name="beam_pipe.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the PMT's base

############
# Geometry #
############

#@description The name of the 3D shape
shape_type   : string =  "tube"

#@description The length unit
length_unit       : string  = "mm"

#@description The Z dimension
z            : real   = 10 cm

#@description The inner radius dimension
inner_r      : real   = 20.0 mm

#@description The outer radius dimension
outer_r      : real   = 25.0 mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string = "inox"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "orange"


# End of list of multi-properties.
