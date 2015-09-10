# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

# This is the geometry definition file for the detector

#@description List of geometry models
#@key_label   "name"
#@meta_label  "type"

###################################################################
[name="detector_block.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the detector block

############
# Geometry #
############

#@description The name of the 3D shape
shape_type :   string =  "box"

# #@description The default implicit length unit
# length_unit       : string  = "mm"

#@description The X dimension
x   : real   = 200.0 mm

#@description The Y dimension
y   : real   = 100.0 mm

#@description The Z dimension
z   : real   = 100.0 mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref      : string = "polystyrene"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "blue"

####################
# Sensitive volume #
####################

#@description The 'sensitive' category attached to this detection volume
sensitive.category : string  = "detector_SD"

# End of list of multi-properties.
