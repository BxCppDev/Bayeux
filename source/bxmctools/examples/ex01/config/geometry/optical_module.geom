# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description List of geometry models
#
# This geometry sample setup describes an optical module
# based on the coupling of a PMT and a scintillator block
# through a light guide.
#
#@key_label   "name"
#@meta_label  "type"

#################################################################
[name="front_window.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the film in front of the scintillator block

############
# Geometry #
############

#@description The default implicit length unit
length_unit       : string  = "mm"

#@description The name of the 3D shape
shape_type :   string =  "box"

#@description The X dimension
x   : real   = 300.0

#@description The Y dimension
y   : real   = 300.0

#@description The Z dimension
z   : real   = 12.0e-3

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref      : string  = "mylar"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "green"


#######################################################################
[name="scintillator_block.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the scintillator block

#@description The name of the 3D shape
shape_type :   string =  "box"

############
# Geometry #
############

#@description The default implicit length unit
length_unit       : string  = "mm"

#@description The X dimension
x   : real   = 300.0

#@description The Y dimension
y   : real   = 300.0

#@description The Z dimension
z   : real   =  10.0

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
visibility.color  : string  = "cyan"

#####################
# Sensitive volumes #
#####################

#@description The 'sensitive' category attached to this detector volume
sensitive.category  : string  = "scin.sd"

# Note:
# We recommend that you name any sensitive category with the
# ".sd" suffix. This is to ease the reading of other configuration
# files at the simulation level (step hit processors). Also note
# that if you prefix the sensitive category name with two underscores
# (example: "__test.sd"), then the sensitive category is considered as
# "private" (non official) and may be processed in some special way by
# the simulation engine.

################################################################
[name="light_guide.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the light guide

############
# Geometry #
############

#@description The default implicit length unit
length_unit : string    = "mm"

#@description The name of the 3D shape
shape_type : string = "polyhedra"

#@description The polyhedra build mode
build_mode : string = "points"

#@description The polyhedra number of sides
sides        : integer = 4

#@description The list of Z coordinates for the shape
list_of_z    : real [3] = -80.     +60.     +80.

#@description The list of inner radius coordinates for the shape
list_of_rmin : real [3] =    0.      0.       0.

#@description The list of outer radius coordinates for the shape
list_of_rmax : real [3] =   60.    150.     150.

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string  = "plexiglass"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden          : boolean = 0

#@description The recommended color for the display
visibility.color           : string  = "cyan"


########################################################################
[name="rotated_light_guide.model" type="geomtools::rotated_boxed_model"]

#@config The configuration parameters for the rotated light guide

############
# Geometry #
############

#@description The default implicit length unit
length_unit   : string = "mm"

#@description The default implicit angle unit
angle_unit    : string = "degree"

#@description The rotation axis
rotated.axis  : string = "z"

#@description The rotation angle
rotated.angle : real   = 45.0

#@description The X dimension
x             : real   = 300.0

#@description The Y dimension
y             : real   = 300.0

#@description The name of the model to be rotated
rotated.model : string = "light_guide.model"

#@description The label associated to the rotated daughter model
rotated.label : string = "light_guide"

#######################
# Material parameters #
#######################

#@description The name of the material around the light guide to be rotated
material.ref      : string  = "vacuum"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden          : boolean = 0

#@description The recommended color for the display
visibility.color           : string  = "grey"

#@description The visibility hidden flag for the envelope
visibility.hidden_envelope  : boolean = 0


#######################################################################
[name="focusing_electrode.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the PMT's focusing electrode

############
# Geometry #
############

#@description The shape
shape_type   : string =  "tube"

#@description The length unit
length_unit   : string = "mm"

#@description The Z dimension
z             : real   = 1.0

#@description The inner radius dimension
inner_r      : real   = 30.0

#@description The outer radius dimension
outer_r      : real   = 40.0

#######################
# Material parameters #
#######################

#@description The material name
material.ref      : string  = "copper"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "red"


###########################################################
[name="dynode.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the PMT's dynode

############
# Geometry #
############

#@description The default implicit length unit
length_unit  : string = "mm"

#@description The shape
shape_type   : string =  "box"

#@description The X dimension
x            : real   = 35.0

#@description The Y dimension
y            : real   = 20.0

#@description The Z dimension
z            : real   =  0.5

#######################
# Material parameters #
#######################

#@description The material name
material.ref      : string  = "copper"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "red"


########################################################
[name="PMT.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the PMT's bulb and its contents

############
# Geometry #
############

#@description The default implicit length unit
length_unit : string = "mm"

#@description The shape of the PMT's bulb
shape_type  : string = "polycone"

#@description The polycone build mode
build_mode  : string = "datafile"

#@description The polycone coordinates filename
datafile    : string = "config/geometry/pmt_hamamatsu_R5912MOD_polycone.data"

#@description The 'filled' mode to build the model
filled_mode : string = "by_envelope"

#@description The label of the PMT's bulb volume as daughter volume of the model's envelope
filled_label : string = "bulb"

#######################
# Material parameters #
#######################

#@description The material name
material.ref        : string = "glass"

#@description The inner material name
material.filled.ref : string = "vacuum"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden           : boolean = 0

#@description The visibility hidden flag for the envelope
visibility.hidden_envelop   : boolean = 0

#@description The recommended color for the display
visibility.color            : string  = "magenta"

#@description The visibility hidden flag for the daughters volumes
#visibility.daughters.hidden : boolean = 0

#@description The visibility hidden flag for the display
visibility.filled.hidden           : boolean = 0

#@description The visibility hidden flag for the envelope
visibility.filled.hidden_envelop   : boolean = 1

#@description The recommended color for the display
visibility.filled.color            : string  = "grey"

#@description The visibility hidden flag for the daughters volumes
visibility.filled.daughters.hidden : boolean = 0

###########################
# Internal/daughter items #
###########################

#@description The list of daughter volumes by labels
internal_item.filled.labels : string[12] = \
				"focusing_electrode" \
				"dynode_0" \
				"dynode_1" \
				"dynode_2" \
				"dynode_3" \
				"dynode_4" \
				"dynode_5" \
				"dynode_6" \
				"dynode_7" \
				"dynode_8" \
				"dynode_9" \
				"anode"

#@description The placement of the "focusing_electrode" daughter volume
internal_item.filled.placement.focusing_electrode : string  = "0 0 +20 (mm)"

#@description The model of the "focusing_electrode" daughter volume
internal_item.filled.model.focusing_electrode     : string  = "focusing_electrode.model"

internal_item.filled.model.dynode_0     : string  = "dynode.model"
internal_item.filled.placement.dynode_0 : string  = "0 -15 +0 (mm) / x +45 (degree)"

internal_item.filled.model.dynode_1     : string  = "dynode.model"
internal_item.filled.placement.dynode_1 : string  = "0 +15 +0 (mm) / x +45 (degree)"

internal_item.filled.model.dynode_2     : string  = "dynode.model"
internal_item.filled.placement.dynode_2 : string  = "0 -15 -20 (mm) / x +45 (degree)"

internal_item.filled.model.dynode_3     : string  = "dynode.model"
internal_item.filled.placement.dynode_3 : string  = "0 +15 -20 (mm) / x +45 (degree)"

internal_item.filled.model.dynode_4     : string  = "dynode.model"
internal_item.filled.placement.dynode_4 : string  = "0 -15 -40 (mm) / x +45 (degree)"

internal_item.filled.model.dynode_5     : string  = "dynode.model"
internal_item.filled.placement.dynode_5 : string  = "0 +15 -40 (mm) / x +45 (degree)"

internal_item.filled.model.dynode_6     : string  = "dynode.model"
internal_item.filled.placement.dynode_6 : string  = "0 -15 -60 (mm) / x +45 (degree)"

internal_item.filled.model.dynode_7     : string  = "dynode.model"
internal_item.filled.placement.dynode_7 : string  = "0 +15 -60 (mm) / x +45 (degree)"

internal_item.filled.model.dynode_8     : string  = "dynode.model"
internal_item.filled.placement.dynode_8 : string  = "0 -15 -80 (mm) / x +45 (degree)"

internal_item.filled.model.dynode_9     : string  = "dynode.model"
internal_item.filled.placement.dynode_9 : string  = "0 +15 -80 (mm) / x +45 (degree)"

internal_item.filled.placement.anode    : string  = "0 0 -95 (mm) / x +20 (degree)"
internal_item.filled.model.anode        : string  = "dynode.model"

##########################################
# GID mapping of internal/daughter items #
##########################################

mapping.filled.daughter_id.dynode_0 : string  = "[pmt_dynode.gc:dynode=0]"
mapping.filled.daughter_id.dynode_1 : string  = "[pmt_dynode.gc:dynode=1]"
mapping.filled.daughter_id.dynode_2 : string  = "[pmt_dynode.gc:dynode=2]"
mapping.filled.daughter_id.dynode_3 : string  = "[pmt_dynode.gc:dynode=3]"
mapping.filled.daughter_id.dynode_4 : string  = "[pmt_dynode.gc:dynode=4]"
mapping.filled.daughter_id.dynode_5 : string  = "[pmt_dynode.gc:dynode=5]"
mapping.filled.daughter_id.dynode_6 : string  = "[pmt_dynode.gc:dynode=6]"
mapping.filled.daughter_id.dynode_7 : string  = "[pmt_dynode.gc:dynode=7]"
mapping.filled.daughter_id.dynode_8 : string  = "[pmt_dynode.gc:dynode=8]"
mapping.filled.daughter_id.dynode_9 : string  = "[pmt_dynode.gc:dynode=9]"
mapping.filled.daughter_id.anode    : string  = "[pmt_dynode.gc:dynode=10]"
mapping.filled.daughter_id.focusing_electrode : string = "[pmt_focus_electrode.gc]"
mapping.filled.daughter_id.bulb     : string  = "[pmt_bulb.gc]"


#############################################################
[name="PMT_base.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the PMT's base

############
# Geometry #
############

#@description The name of the 3D shape
shape_type   : string =  "cylinder"

#@description The length unit
length_unit       : string  = "mm"

#@description The R dimension
r            : real   = 30.0

#@description The Z dimension
z            : real   = 40.0

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string = "polystyrene"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "orange"


##########################################################################
[name="diaphragm.model" type="geomtools::cylindric_extrusion_boxed_model"]

#@config The configuration parameters for the diaphragm

############
# Geometry #
############

#@description The length unit
length_unit       : string  = "mm"

#@description The extrusion radius dimension
extrusion_radius : real   =  40.0

#@description The X dimension
x                : real   = 320.0

#@description The Y dimension
y                : real   = 320.0

#@description The Z dimension
z                : real   =   1.0

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string = "aluminium"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "red"


#############################################################
[name="optical_module.model" type="geomtools::stacked_model"]

#@config The configuration parameters for the optical module

############
# Geometry #
############

#@description The stacking axis
stacked.axis            : string = "z"

#@description The number of stacked volumes
stacked.number_of_items : integer = 6

#@description The model of the stacked volume #5
stacked.model_5   : string  = "diaphragm.model"

#@description The label of the stacked volume #5
stacked.label_5   : string  = "diaphragm"

stacked.model_4   : string  = "front_window.model"
stacked.label_4   : string  = "front_window"

stacked.model_3   : string  = "scintillator_block.model"
stacked.label_3   : string  = "scintillator_block"

stacked.model_2   : string  = "rotated_light_guide.model"
stacked.label_2   : string  = "light_guide"

stacked.model_1   : string  = "PMT.model"
stacked.label_1   : string  = "PMT"

stacked.model_0   : string  = "PMT_base.model"
stacked.label_0   : string  = "PMT_base"

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref      : string    = "vacuum"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden : boolean  = 0

#@description The recommended color for the display
visibility.color  : string   = "grey"

##########################################
# GID mapping of internal/daughter items #
##########################################

#@description The mapping directive for the "scintillator_block" daughter volume
mapping.daughter_id.scintillator_block : string  = "[scin_block.gc]"

#@description The mapping directive for the "diaphragm" daughter volume
mapping.daughter_id.diaphragm          : string  = "[diaphragm.gc]"

#@description The mapping directive for the "light_guide" daughter volume
mapping.daughter_id.light_guide        : string  = "[light_guide.gc]"


#################################################################
[name="optical_module_column.model" type="geomtools::replicated_model"]

#@config The configuration parameters for the column of optical modules

############
# Geometry #
############

#@description The default implicit length unit
length_unit                : string = "cm"

#@description The replication axis
replicated.axis            : string = "y"

#@description The number of replicated volumes
replicated.number_of_items : integer = 2

#@description The model of the replicated volumes
replicated.model           : string = "optical_module.model"

#@description The label associated to the replicated volumes
replicated.label           : string = "optical_modules"

#@description The step between replicated volumes
replicated.step            : real = 30

#######################
# Material parameters #
#######################

#@description The name of the material around the optical modules
material.ref      : string    = "vacuum"

###########################
# Internal/daughter items #
###########################

#@description The mapping directives for the "optical_modules" daughter volumes
mapping.daughter_id.optical_modules : string  = "[optical_module.gc:row+0]"


###################################################################
[name="optical_module_array.model" type="geomtools::replicated_model"]

#@config The configuration parameters for the array of optical modules

############
# Geometry #
############

#@description The default implicit length unit
length_unit                : string = "cm"

#@description The replication axis
replicated.axis            : string = "x"

#@description The number of replicated volumes
replicated.number_of_items : integer = 2

#@description The model of the replicated volumes
replicated.model           : string = "optical_module_column.model"

#@description The label associated to the replicated volumes
replicated.label           : string = "optical_modules"

#@description The step between replicated volumes
replicated.step            : real = 30

#######################
# Material parameters #
#######################

#@description The name of the material around the replicated volumes
material.ref      : string    = "vacuum"

##########################################
# GID mapping of internal/daughter items #
##########################################

#@description The mapping directives for the "optical_modules" daughter volumes
mapping.daughter_id.optical_modules : string  = "[optical_module_column.gc:column+0]"


# End of list of multi-properties.
