# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry models of the detector
#@key_label  "name"
#@meta_label "type"

#################################################################
[name="crystal_well.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the HPGe crystal well

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "cylinder"

#@description The R dimension (cylinder)
r : real as length =  5.0 mm

#@description The Z dimension (cylinder)
z : real as length = 40.0 mm

############
# Material #
############

#@description The name of the material
material.ref : string = "vacuum"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden           : boolean = 0

#@description The recommended color for the display of the volume
visibility.color            : string  = "cyan"

#@description The recommended opacity for the display of the volume
visibility.opacity          : real    = 0.20

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1


############################################################
[name="crystal.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the HPGe crystal

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "cylinder"

#@description The R dimension (cylinder)
r : real as length = 30.0 mm

#@description The Z dimension (cylinder)
z : real as length = 60.0 mm

############
# Material #
############

#@description The name of the material
material.ref : string = "germanium"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden           : boolean = 0

#@description The recommended color for the display of the volume
visibility.color            : string  = "cyan"

#@description The recommended opacity for the display of the volume
visibility.opacity          : real    = 0.20

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1

#@description The daughters' visibility hidden flag
visibility.daughters.hidden : boolean = 0

######################
# Sensitive detector #
######################

#@description The name of the sensitive category
sensitive.category : string = "detector_SD"

##################
# Internal items #
##################

#@description List of daughter volumes by label
internal_item.labels : string[1] = "Well"

#@description The placement of the "Well" volume
internal_item.placement.Well : string = "0.0 0.0 -10.0 (mm)"

#@description The model of the "Well" volume
internal_item.model.Well     : string = "crystal_well.model"


###############################################################
[name="cover_side.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the HPGe cover side

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "tube"

#@description The inner R dimension (tube)
inner_r : real as length = 34.0 mm

#@description The outer R dimension (tube)
outer_r : real as length = 35.0 mm

#@description The Z dimension (tube)
z       : real as length = 118.0 mm

############
# Material #
############

#@description The name of the material
material.ref : string = "aluminium"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden           : boolean = 0

#@description The recommended color for the display of the volume
visibility.color            : string  = "blue"

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1

#@description The recommended opacity for the display of the volume
visibility.opacity          : real  = 0.10


##############################################################
[name="cover_top.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the HPGe cover top

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "cylinder"

#@description The R dimension (cylinder)
r : real as length = 35.0 mm

#@description The Z dimension (cylinder)
z : real as length =  1.0 mm

############
# Material #
############

#@description The name of the material
material.ref : string = "aluminium"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden           : boolean = 0

#@description The recommended color for the display of the volume
visibility.color            : string  = "blue"

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1

#@description The recommended opacity for the display of the volume
visibility.opacity          : real  = 0.05


#################################################################
[name="cover_bottom.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the HPGe cover bottom

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "cylinder"

#@description The R dimension (cylinder)
r : real as length = 35.0 mm

#@description The Z dimension (cylinder)
z : real as length =  1.0 mm

############
# Material #
############

#@description The name of the material
material.ref : string = "aluminium"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden           : boolean = 0

#@description The recommended color for the display of the volume
visibility.color            : string  = "blue"

#@description The recommended opacity for the display of the volume
visibility.opacity          : real  = 0.10

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1


#############################################################
[name="detector.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the full HPGe detector envelope

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "cylinder"

#@description The R dimension (cylinder)
r : real as length = 35.0 mm

#@description The Z dimension (cylinder)
z : real as length = 120.0 mm

############
# Material #
############

#@description The name of the material
material.ref : string = "vacuum"

##############
# Visibility #
##############

#@description The visibility hidden flag for the display of the volume
visibility.hidden : boolean = 0

#@description The recommended color for the display of the volume
visibility.color  : string  = "grey"

#@description The recommended opacity for the display of the volume
visibility.opacity          : real  = 0.05

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1

##################
# Internal items #
##################

#@description List of daughter volumes by label
internal_item.labels : string[4] = \
  "Crystal" \
  "CoverBottom" \
  "CoverTop" \
  "CoverSide"

#@description The placement of the "Crystal" volume
internal_item.placement.Crystal     : string = "0.0 0.0 25.0 (mm)"

#@description The model of the "Crystal" volume
internal_item.model.Crystal         : string = "crystal.model"

#@description The placement of the "CoverSide" volume
internal_item.placement.CoverSide   : string = "0.0 0.0 0.0 (mm)"

#@description The model of the "CoverSide" volume
internal_item.model.CoverSide       : string = "cover_side.model"

#@description The placement of the "CoverBottom" volume
internal_item.placement.CoverBottom : string = "0.0 0.0 -59.5 (mm)"

#@description The model of the "CoverBottom" volume
internal_item.model.CoverBottom     : string = "cover_bottom.model"

#@description The placement of the "CoverTop" volume
internal_item.placement.CoverTop    : string = "0.0 0.0 +59.5 (mm)"

#@description The model of the "CoverTop" volume
internal_item.model.CoverTop        : string = "cover_top.model"

#######################################
# GID mapping of the daughter volumes #
#######################################

#@description The mapping directives for the "Crystal" daughter volume
mapping.daughter_id.Crystal     : string = "[crystal.gc]"


# end of detector.geom