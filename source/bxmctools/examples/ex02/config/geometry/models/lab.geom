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

# #@description The default implicit length unit for the setup placement
# length_unit : string = "mm"

#@description The name of the 3D shape of the lab area
shape_type  : string = "box"

#@description The X dimension of the box
x           : real = 900.0 mm

#@description The Y dimension of the box
y           : real = 900.0 mm

#@description The Z dimension of the box
z           : real = 900.0 mm

#######################
# Material parameters #
#######################

#@description The name of the material that fills the lab atmosphere
material.ref : string = "lab_medium"

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
internal_item.labels : string[2] = "beam_pipe" "detector"

#@description The model of the "beam_pipe" daughter volume
internal_item.model.beam_pipe       : string  = "beam_pipe.model"

#@description The placement of the "beam_pipe" daughter volume
internal_item.placement.beam_pipe   : string  = "-40 0 0 (cm) / y +90 (degree)"

#@description The model of the "detector" daughter volume
internal_item.model.detector        : string  = "detector_block.model"

#@description The placement of the "detector" daughter volume
internal_item.placement.detector    : string  = "+30 0 0 (cm)"

##########################################
# GID mapping of internal/daughter items #
##########################################

#@description The mapping directives for the "detector" daughter volume
mapping.daughter_id.detector : string  = "[detector.gc:block=0]"


# End of list of multi-properties.
