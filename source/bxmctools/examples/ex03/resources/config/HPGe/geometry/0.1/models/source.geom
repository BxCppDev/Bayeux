# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry models of the source
#@key_label  "name"
#@meta_label "type"

####################################################################
[name="bottle_contents.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the contents of the bottle (liquid)

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "cylinder"

#@description The R dimension (cylinder)
r : real as length = 46.75 mm

#@description The Z dimension (cylinder)
z : real as length = 60.0 mm

############
# Material #
############

material.ref : string = "water"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden  : boolean = 0

#@description The recommended color for the display of the volume
visibility.color   : string  = "red"

#@description The recommended opacity for the display of the volume
visibility.opacity : real    = 0.20

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 0


########################################################################
[name="bottle_inner_volume.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the inner volume of the bottle

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "cylinder"

#@description The R dimension (cylinder)
r : real as length = 46.75 mm

#@description The Z dimension (cylinder)
z : real as length = 80.0 mm

############
# Material #
############

material.ref : string = "air"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden : boolean = 0

#@description The recommended color for the display of the volume
visibility.color  : string = "cyan"

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 0

#@description The recommended opacity for the display of the volume
visibility.opacity : real    = 0.10

#@description The daughters' visibility hidden flag for the display
visibility.daughters.hidden : boolean = 0

##################
# Internal items #
##################

#@description List of daughter volumes by label
internal_item.labels : string[1] = "Sample"

#@description The placement of the "Sample" volume
internal_item.placement.Sample  : string = "0.0 0.0 -10.0 (mm)"

#@description The model of the "Sample" volume
internal_item.model.Sample      : string = "bottle_contents.model"

#############################################
# GID mapping of the daughter setup volumes #
#############################################

#@description The mapping directives for the "Sample" daughter volume
mapping.daughter_id.Sample     : string = "[sample.gc:sample=0]"


###############################################################
[name="bottle_top.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the top of the bottle

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "tube"

#@description The inner R dimension (tube)
inner_r : real as length = 27.5 mm

#@description The outer R dimension (tube)
outer_r : real as length = 48.25 mm

#@description The Z dimension (tube)
z       : real as length =  1.5 mm

############
# Material #
############

material.ref : string = "PVC"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden  : boolean = 0

#@description The recommended color for the display of the volume
visibility.color   : string  = "green"

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1

#@description The recommended opacity for the display of the volume
visibility.opacity : real    = 0.10


##################################################################
[name="bottle_bottom.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the bottom of the bottle

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "cylinder"

#@description The R dimension (cylinder)
r : real as length = 48.25 mm

#@description The Z dimension (cylinder)
z : real as length =  1.5 mm

############
# Material #
############

material.ref : string = "PVC"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden  : boolean = 0

#@description The recommended color for the display of the volume
visibility.color   : string  = "green"

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1

#@description The recommended opacity for the display of the volume
visibility.opacity : real    = 0.10


################################################################
[name="bottle_side.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the side of the bottle

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "tube"

#@description The inner R dimension (cylinder)
inner_r : real as length = 46.75 mm

#@description The outer R dimension (cylinder)
outer_r : real as length = 48.25 mm

#@description The Z dimension (cylinder)
z       : real as length = 80.0 mm

############
# Material #
############

material.ref : string = "PVC"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden  : boolean = 0

#@description The recommended color for the display of the volume
visibility.color   : string  = "green"

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1

#@description The recommended opacity for the display of the volume
visibility.opacity : real    = 0.10


####################################################################
[name="bottleneck.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the bottleneck of the bottle

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "tube"

#@description The inner R dimension (cylinder)
inner_r : real as length = 24.0 mm

#@description The outer R dimension (cylinder)
outer_r : real as length = 28.0 mm

#@description The Z dimension (cylinder)
z       : real as length =  22.5 mm

############
# Material #
############

material.ref : string = "PVC"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden  : boolean = 0

#@description The recommended color for the display of the volume
visibility.color   : string  = "green"

#@description The recommended opacity for the display of the volume
visibility.opacity : real    = 0.10

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 0


#####################################################
[name="bottle.model" type="geomtools::stacked_model"]

#@config Configuration parameters for the bottle logical volume

############
# Geometry #
############

#@description The stacking axis
stacked.axis    : string = "z"

#@description The number of stacked volumes
stacked.number_of_items : integer = 5

#@description The geometry model of the volume at stacking slot 0
stacked.model_0 : string = "bottle_bottom.model"

#@description The label of the volume at stacking slot 0
stacked.label_0 : string = "Bottom"

#@description The geometry model of the volume at stacking slot 1
stacked.model_1 : string = "bottle_side.model"

#@description The label of the volume at stacking slot 1
stacked.label_1 : string = "Side"

#@description The max limit
stacked.limit_max_1 : real = -40 mm

#@description The geometry model of the volume at stacking slot 2
stacked.model_2 : string = "bottle_inner_volume.model"

#@description The label of the volume at stacking slot 2
stacked.label_2 : string = "InnerVolume"

#@description The geometry model of the volume at stacking slot 3
stacked.model_3 : string = "bottle_top.model"

#@description The label of the volume at stacking slot 3
stacked.label_3 : string = "Top"

#@description The geometry model of the volume at stacking slot 3
stacked.model_4 : string = "bottleneck.model"

#@description The label of the volume at stacking slot 3
stacked.label_4 : string = "Bottleneck"

############
# Material #
############

material.ref : string = "air"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden  : boolean = 0

#@description The recommended color for the display of the volume
visibility.color   : string  = "gray"

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 0

#@description The recommended opacity for the display of the volume
visibility.opacity : real    = 0.0

#@description The daughters' visibility hidden flag for the display
visibility.daughters.hidden : boolean = 0


# end of source.geom