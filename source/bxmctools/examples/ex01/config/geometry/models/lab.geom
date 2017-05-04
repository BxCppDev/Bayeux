# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of the laboratory area
#@key_label   "name"
#@meta_label  "type"

########################################################
[name="lab.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the laboratory experimental area

############
# Geometry #
############

#@description The default implicit length unit for the setup placement
length_unit : string = "mm"

#@description The name of the 3D shape of the lab area
shape_type  : string = "box"

#@description The X dimension of the box
x           : real = 1900.0

#@description The Y dimension of the box
y           : real = 1900.0

#@description The Z dimension of the box
z           : real = 1900.0

#######################
# Material parameters #
#######################

#@description The name of the material that fills the lab atmosphere
material.ref : string = "air"

#########################
# Visibility parameters #
#########################

#@description The recommended color for the display of the lab area
visibility.color            : string  = "cyan"

#@description The visibility hidden flag for the display of the lab area
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of the lab area
visibility.daughters.hidden : boolean = 0

###########################
# Internal/daughter items #
###########################

#@description The list of daughter volumes by labels
internal_item.labels : string[3] = \
  "vacuum_chamber" \
  "shield_wall_a" \
  "shield_wall_b"

#@description The model of the "vacuum_chamber" daughter volume
internal_item.model.vacuum_chamber     : string  = "vessel.model"

#@description The placement of the "vacuum_chamber" daughter volume
internal_item.placement.vacuum_chamber : string  = "0 0 0 (mm) / x +90 (degree)"

#@description The model of the "shield_wall_a" daughter volume
internal_item.model.shield_wall_a     : string  = "shielding_brick_array.model"

#@description The placement of the "vacuum_chamber" daughter volume
internal_item.placement.shield_wall_a : string  = "-80 0 0 (cm) / z +90 (degree) "

#@description The model of the "shield_wall_b" daughter volume
internal_item.model.shield_wall_b     : string  = "shielding_brick_array.model"

#@description The placement of the "vacuum_chamber" daughter volume
internal_item.placement.shield_wall_b : string  = "+80 0 0 (cm) / z +90 (degree)"

##########################################
# GID mapping of internal/daughter items #
##########################################

#@description The mapping directives for the "vacuum_chamber" daughter volume
mapping.daughter_id.vacuum_chamber : string  = "[vessel.gc:chamber=3]"

#@description The mapping directives for the "vacuum_chamber" daughter volume
mapping.daughter_id.shield_wall_a : string  = "[shield_wall.gc:wall=0]"

#@description The mapping directives for the "vacuum_chamber" daughter volume
mapping.daughter_id.shield_wall_b : string  = "[shield_wall.gc:wall=1]"


# End of list of multi-properties.
