# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of the lab volume
#@key_label  "name"
#@meta_label "type"


###########################################################
[name="ground.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the floor

#@description The name of the 3D shape of the logical volume
shape_type : string = "box"

#@description The X dimension (box)
x : real as length = 1.6 m

#@description The Y dimension (box)
y : real as length = 1.6 m

#@description The Z dimension (box)
z : real as length = 0.2 m

############
# Material #
############

#@description The name of the material of the ground
material.ref : string = "concrete"

#########################
# Visibility parameters #
#########################

#@description The recommended color for the display of the volume
visibility.color            : string  = "brown"

#@description The recommended opacity for the display of the volume
visibility.opacity          : real  = 0.10

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1

#@description The visibility hidden flag for the display of the volume
visibility.hidden           : boolean = 0


###############################################################
[name="atmosphere.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the lab atmosphere volume

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "box"

#@description The X dimension (box)
x : real as length = 1.6 m

#@description The Y dimension (box)
y : real as length = 1.6 m

#@description The Z dimension (box)
z : real as length = 1.4 m

############
# Material #
############

#@description The name of the material that fills the lab atmosphere
material.ref : string = "air"

#########################
# Visibility parameters #
#########################

#@description The recommended color for the display of the volume
visibility.color            : string  = "cyan"

#@description The recommended opacity for the display of the volume
visibility.opacity          : real  = 0.05

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1

#@description The visibility hidden flag for the display of the volume
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display
visibility.daughters.hidden : boolean = 0

##################
# Internal items #
##################

#@description List of daughter volumes by label
internal_item.labels : string[8] = \
  "ShieldingBottom"     \
  "ShieldingTop"        \
  "ShieldingSideFront"  \
  "ShieldingSideBack"   \
  "ShieldingSideLeft"   \
  "ShieldingSideRight"  \
  "Detector"  \
  "Source"

#@description The placement of the "ShieldingBottom" volume
internal_item.placement.ShieldingBottom    : string = "0.0 0.0 -67.5 (cm)"

#@description The model of the "ShieldingBottom" volume
internal_item.model.ShieldingBottom        : string = "shielding_bottom.model"

#@description The placement of the "ShieldingSideFront" volume
internal_item.placement.ShieldingSideFront : string = "+32.5 0.0 -35.0 (cm)"

#@description The model of the "ShieldingSideFront" volume
internal_item.model.ShieldingSideFront     : string = "shielding_side_fb.model"

#@description The placement of the "ShieldingSideBack" volume
internal_item.placement.ShieldingSideBack  : string = "-32.5 0.0 -35.0 (cm)"

#@description The model of the "ShieldingSideBack" volume
internal_item.model.ShieldingSideBack      : string = "shielding_side_fb.model"

#@description The placement of the "ShieldingSideLeft" volume
internal_item.placement.ShieldingSideLeft  : string = "0.0 -22.5 -35.0 (cm)"

#@description The model of the "ShieldingSideLeft" volume
internal_item.model.ShieldingSideLeft      : string = "shielding_side_lr.model"

#@description The placement of the "ShieldingSideRight" volume
internal_item.placement.ShieldingSideRight : string = "0.0 +22.5 -35.0 (cm)"

#@description The model of the "ShieldingSideRight" volume
internal_item.model.ShieldingSideRight     : string = "shielding_side_lr.model"

#@description The placement of the "ShieldingTop" volume
internal_item.placement.ShieldingTop    : string = "0.0 0.0 -2.5 (cm)"

#@description The model of the "ShieldingTop" volume
internal_item.model.ShieldingTop        : string = "shielding_top.model"

#@description The placement of the "Detector" volume
internal_item.placement.Detector    : string = "0.0 0.0 -59.0 (cm)"

#@description The model of the "Detector" volume
internal_item.model.Detector        : string = "detector.model"

#@description The placement of the "Source" volume
internal_item.placement.Source    : string = "0.0 0.0 -47.75 (cm)"

#@description The model of the "Source" volume
internal_item.model.Source        : string = "bottle.model"

#############################################
# GID mapping of the daughter setup volumes #
#############################################

#@description The mapping directives for the "Detector" daughter volume
mapping.daughter_id.Detector     : string = "[detector.gc:module=0]"


##################################################
[name="lab.model" type="geomtools::stacked_model"]

#@config Configuration parameters for the lab volume

############
# Geometry #
############

#@description The stacking axis
stacked.axis    : string = "z"

#@description The number of stacked volumes
stacked.number_of_items : integer = 2

#@description The geometry model of the volume at stacking slot 0
stacked.model_0 : string = "ground.model"

#@description The label of the volume at stacking slot 0
stacked.label_0 : string = "Ground"

#@description The geometry model of the volume at stacking slot 1
stacked.model_1 : string = "atmosphere.model"

#@description The label of the volume at stacking slot 1
stacked.label_1 : string = "Atmosphere"

############
# Material #
############

#@description The name of the material
material.ref : string = "vacuum"

##############
# Visibility #
##############

#@description The recommended color for the display of the mother volume
visibility.color  : string  = "grey"

#@description The recommended opacity for the display of the volume
visibility.opacity          : real  = 0.05

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1

#@description The visibility hidden flag for the display of the mother volume
visibility.hidden : boolean = 0

#############################################
# GID mapping of the daughter setup volumes #
#############################################

#@description The mapping directives for the "Ground" daughter volume
mapping.daughter_id.Ground     : string = "[ground.gc]"

#@description The mapping directives for the "Atmosphere" daughter volume
mapping.daughter_id.Atmosphere : string = "[atmosphere.gc]"


# end of lab.geom