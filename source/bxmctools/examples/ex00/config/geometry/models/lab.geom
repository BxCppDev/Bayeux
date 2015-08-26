# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of the laboratory area
#@key_label   "name"
#@meta_label  "type"

##################################################################
[name="thin_absorber.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the thin_absorber front of the detector

############
# Geometry #
############

#@description The default implicit length unit
length_unit : string  = "mm"

#@description The name of the 3D shape
shape_type :   string =  "box"

#@description The X dimension
x   : real   = 100.0

#@description The Y dimension
y   : real   = 100.0

#@description The Z dimension
z   : real as length  = 50 um

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref      : string  = "copper"

####################
# Sensitive volume #
####################

#@description The 'sensitive' category attached to this detector volume
sensitive.category : string = "absorber_SD"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "red"


##############################################################
[name="black_box.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the box around the detector

############
# Geometry #
############

#@description The default implicit length unit
length_unit : string = "mm"

#@description The name of the 3D shape
shape_type : string = "polyhedra"

#@description The polyhedra build mode
build_mode : string = "points"

#@description The polyhedra number of sides
sides        : integer = 4

#@description Build by apothem
build_by_apothem : boolean = true

#@description The list of Z coordinates for the shape
list_of_z    : real [2] =   -125.     +125.

#@description The list of inner radius coordinates for the shape
list_of_rmin : real [2] =    51.      51.
# list_of_rmin : real [2] =    72.      72.

#@description The list of outer radius coordinates for the shape
list_of_rmax : real [2] =    53.      53.
# list_of_rmax : real [2] =    75.      75.

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string  = "inox"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden          : boolean = 0

#@description The recommended color for the display
visibility.color           : string  = "blue"


#################################################################
[name="shield_brick.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the shield brick

############
# Geometry #
############

#@description The default implicit length unit
length_unit : string = "mm"

#@description The name of the 3D shape
shape_type : string = "box"

#@description The list of X coordinates
x  : real as length =  200. mm

#@description The list of Y coordinates
y  : real as length =   50. mm

#@description The list of Y coordinates
z  : real as length =  100. mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string  = "lead"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden          : boolean = 0

#@description The recommended color for the display
visibility.color           : string  = "red"

####################
# Sensitive volume #
####################

#@description The 'sensitive' category attached to this detector volume
sensitive.category : string = "shield_SD"


###########################################################
[name="screen.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the screen

############
# Geometry #
############

#@description Shape build mode
shape_build_mode : string = "factory"

#@description The shape reference
shape_ref : string = "Screen"

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string  = "aluminium"

####################
# Sensitive volume #
####################

# #@description The 'sensitive' category attached to this detector volume
# sensitive.category : string = "screen_SD"


#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden          : boolean = 0

#@description The recommended color for the display
visibility.color           : string  = "magenta"



################################################################
[name="screen_roof.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the screen roof

############
# Geometry #
############

#@description The default implicit length unit
length_unit : string = "mm"

#@description The name of the 3D shape
shape_type : string = "box"

#@description The list of X coordinates
x  : real as length =  820. mm

#@description The list of Y coordinates
y  : real as length =  420. mm

#@description The list of Y coordinates
z  : real as length =  4. mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string  = "aluminium"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden          : boolean = 0

#@description The recommended color for the display
visibility.color           : string  = "magenta"


#########################################################
[name="slab.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the slab

############
# Geometry #
############

#@description The default implicit length unit
length_unit : string = "mm"

#@description The name of the 3D shape
shape_type : string = "box"

#@description The list of X coordinates
x  : real as length =  850. mm

#@description The list of Y coordinates
y  : real as length =  420. mm

#@description The list of Y coordinates
z  : real as length =  50. mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string  = "glass"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden          : boolean = 0

#@description The recommended color for the display
visibility.color           : string  = "green"


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
x           : real = 900.0

#@description The Y dimension of the box
y           : real = 450.0

#@description The Z dimension of the box
z           : real = 450.0

#######################
# Material parameters #
#######################

#@description The name of the material that fills the lab atmosphere
material.ref : string = "lab_medium"

####################
# Sensitive volume #
####################

#@description The 'sensitive' category attached to this detector volume
sensitive.category : string = "lab_SD"

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
internal_item.labels : string[9] = \
  "source" "detector" "box" "absorber0" "brick0" "brick1" \
  "screen" "screen_roof" \
  "slab"

#@description The model of the "source" daughter volume
internal_item.model.source        : string  = "source.model"

#@description The placement of the "source" daughter volume
internal_item.placement.source    : string  = "20 0 0 (cm) / y +90 (degree)"

#@description The model of the "detector" daughter volume
internal_item.model.detector      : string  = "optical_module.model"

#@description The placement of the "detector" daughter volume
internal_item.placement.detector  : string  = "-20 0 0.5 (cm) / y +90 (degree) "

#@description The model of the "box" daughter volume
internal_item.model.box           : string  = "black_box.model"

#@description The placement of the "box" daughter volume
internal_item.placement.box       : string  = "-24 0 0.5 (cm) @  0 90 45 (degree) "

#@description The model of the "absorber0" daughter volume
internal_item.model.absorber0     : string  = "thin_absorber.model"

#@description The placement of the "absorber0" daughter volume
internal_item.placement.absorber0 : string  = "0 0 0 (cm) @  0 90 0 (degree) "

#@description The model of the "brick0" daughter volume
internal_item.model.brick0     : string  = "shield_brick.model"

#@description The placement of the "brick0" daughter volume
internal_item.placement.brick0 : string  = "-15 -10 0 (cm) "

#@description The model of the "brick1" daughter volume
internal_item.model.brick1     : string  = "shield_brick.model"

#@description The placement of the "brick1" daughter volume
internal_item.placement.brick1 : string  = "-15 +10 0 (cm) "

#@description The model of the "screen" daughter volume
internal_item.model.screen     : string  = "screen.model"

#@description The placement of the "screen" daughter volume
internal_item.placement.screen : string  = "0 0 5 (cm) "

#@description The model of the "screen_roof" daughter volume
internal_item.model.screen_roof     : string  = "screen_roof.model"

#@description The placement of the "screen_roof" daughter volume
internal_item.placement.screen_roof : string  = "0 0 15.2 (cm) "

#@description The model of the "slab" daughter volume
internal_item.model.slab     : string  = "slab.model"

#@description The placement of the "slab" daughter volume
internal_item.placement.slab : string  = "0 0 -7.5 (cm) "


##########################################
# GID mapping of internal/daughter items #
##########################################

#@description The mapping directives for the "source" daughter volume
mapping.daughter_id.source   : string  = "[source.gc:position=0]"

#@description The mapping directives for the "detector" daughter volume
mapping.daughter_id.detector : string  = "[optical_module.gc:detector=0]"


# End of list of multi-properties.
