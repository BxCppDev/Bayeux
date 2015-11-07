# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of the world (mother-of-all) volume

#@key_label  "name"
#@meta_label "type"

####################################################
[name="world" type="geomtools::simple_shaped_model"]

#@config The list of properties to configure the world logical volume

############
# Geometry #
############

### #@description The default length unit
### length_unit     : string = "mm"

#@description The name of the 3D shape of the world logical volume
shape_type  : string = "box"

#@description The world volume X dimension (box)
x         : real   = 800.0 mm

#@description The world volume Y dimension (box)
y         : real   = 800.0 mm

#@description The world volume Z dimension (box)
z         : real   = 800.0 mm

############
# Material #
############

#@description The name of the material that fills the lab atmosphere
material.ref : string = "vacuum"

###################
# Daughter volume #
###################

#@description The list of daughter volumes by labels
internal_item.labels : string[6] = \
  "source_A" "source_B" \
  "detector_A" "detector_B" "detector_C" "detector_D"

#@description The model of the "source_A" daughter volume
internal_item.model.source_A         : string  = "source.model"

#@description The placement of the "source_A" daughter volume
internal_item.placement.source_A     : string  = "-20 0 0 (cm) / y 90 (degree)"

#@description The model of the "source" daughter volume
internal_item.model.source_B         : string  = "source.model"

#@description The placement of the "detector0" daughter volume
internal_item.placement.source_B     : string  = "-15 +25 +7 (cm) @ -30 90 0 (degree)"

#@description The model of the "detector0" daughter volume
internal_item.model.detector_A       : string  = "detector.model"

#@description The placement of the "detector0" daughter volume
internal_item.placement.detector_A   : string  = "+30 -4 -4 (cm) / y 90 (degree)"

#@description The model of the "detector1" daughter volume
internal_item.model.detector_B       : string  = "detector.model"

#@description The placement of the "detector1" daughter volume
internal_item.placement.detector_B   : string  = "+30 -4 +4 (cm) / y 90 (degree)"

#@description The model of the "detector0" daughter volume
internal_item.model.detector_C       : string  = "detector.model"

#@description The placement of the "detector0" daughter volume
internal_item.placement.detector_C   : string  = "+30 +4 -4 (cm) / y 90 (degree)"

#@description The model of the "detector1" daughter volume
internal_item.model.detector_D       : string  = "detector.model"

#@description The placement of the "detector1" daughter volume
internal_item.placement.detector_D   : string  = "+25 +20 +15 (cm) @ 25 70 (degree)"

###########
# Mapping #
###########

#@description The mapping directives for the "source_A" daughter volume
mapping.daughter_id.source_A    : string  = "[source.gc:position=0]"

#@description The mapping directives for the "source_B" daughter volume
mapping.daughter_id.source_B    : string  = "[source.gc:position=1]"

#@description The mapping directives for the "detector_A" daughter volume
mapping.daughter_id.detector_A : string  = "[detector.gc:module=0]"

#@description The mapping directives for the "detector_B" daughter volume
mapping.daughter_id.detector_B : string  = "[detector.gc:module=1]"

#@description The mapping directives for the "detector_C" daughter volume
mapping.daughter_id.detector_C : string  = "[detector.gc:module=2]"

#@description The mapping directives for the "detector_D" daughter volume
mapping.daughter_id.detector_D : string  = "[detector.gc:module=3]"

##############
# Visibility #
##############

#@description The recommended color for the display of the world volume
visibility.color            : string  = "cyan"

#@description The visibility hidden flag for the display of the world volume
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display
visibility.daughters.hidden : boolean = 0

