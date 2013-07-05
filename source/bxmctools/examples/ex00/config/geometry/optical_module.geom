# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#
# This geometry sample setup describes an optical module based on the coupling
# of a wrapped scintillator block, a light guide and a photomultiplier tube (PMT).
#
#@description List of geometry models
#@key_label   "name"
#@meta_label  "type"

##################################################################
[name="zpos_wrapping.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the wrapping in front of the scintillator block

############
# Geometry #
############

#@description The default implicit length unit
length_unit : string  = "mm"

#@description The name of the 3D shape
shape_type  :   string =  "box"

#@description The X dimension
x   : real   = 100.0

#@description The Y dimension
y   : real   = 100.0

#@description The Z dimension
z   : real   = 12 um

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


#####################################################################
[name="x_sides_wrapping.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the wrapping on X sides of the scintillator block

############
# Geometry #
############

#@description The default implicit length unit
length_unit       : string  = "mm"

#@description The name of the 3D shape
shape_type :   string =  "box"

#@description The X dimension
x   : real   = 40.0

#@description The Y dimension
y   : real   = 100.0

#@description The Z dimension
z   : real   = 12 um

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


#####################################################################
[name="y_sides_wrapping.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the wrapping on Y sides of the scintillator block

############
# Geometry #
############

#@description The default implicit length unit
length_unit       : string  = "mm"

#@description The name of the 3D shape
shape_type :   string =  "box"

#@description The X dimension
x   : real   = 100.0

#@description The Y dimension
y   : real   = 40.0

#@description The Z dimension
z   : real   = 12 um

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


##################################################################
[name="xpos_wrapping.model" type="geomtools::rotated_boxed_model"]

#@config The configuration parameters for the rotated wrapping film

############
# Geometry #
############

#@description Rotation geometry model
rotated.model : string = "x_sides_wrapping.model"

#@description Rotated geometry model label
rotated.label : string = "side_wrapping"

#@description Rotation axis
rotated.axis  : string = "y"

#@description Rotation special angle
rotated.special_angle : string  = "90"

#@description Rotation angle
#rotated.angle : real as angle = 45 degree

#@description X dimension of the envelope box
#x : real as length = 40 mm

#@description Z dimension of the envelope box
#z : real as length = 40 mm

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref      : string  = "lab_medium"


##################################################################
[name="xneg_wrapping.model" type="geomtools::rotated_boxed_model"]

#@config The configuration parameters for the rotated wrapping film

############
# Geometry #
############

#@description Rotation axis
rotated.axis  : string = "y"

#@description Rotation special angle
rotated.special_angle : string  = "270"

#@description Rotation geometry model
rotated.model : string = "x_sides_wrapping.model"

#@description Rotated geometry model label
rotated.label : string = "side_wrapping"

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref      : string  = "lab_medium"


##################################################################
[name="ypos_wrapping.model" type="geomtools::rotated_boxed_model"]

#@config The configuration parameters for the rotated wrapping film

############
# Geometry #
############

#@description Rotation axis
rotated.axis  : string = "x"

#@description Rotation special angle
rotated.special_angle : string  = "90"

#@description Rotation geometry model
rotated.model : string = "y_sides_wrapping.model"

#@description Rotated geometry model label
rotated.label : string = "side_wrapping"

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref      : string  = "lab_medium"


##################################################################
[name="yneg_wrapping.model" type="geomtools::rotated_boxed_model"]

#@config The configuration parameters for the rotated wrapping film

############
# Geometry #
############

#@description Rotation axis
rotated.axis  : string = "x"

#@description Rotation special angle
rotated.special_angle : string  = "270"

#@description Rotation geometry model
rotated.model : string = "y_sides_wrapping.model"

#@description Rotated geometry model label
rotated.label : string = "side_wrapping"

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref      : string  = "lab_medium"


#######################################################################
[name="scintillator_block.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the scintillator block

############
# Geometry #
############

#@description The name of the 3D shape
shape_type :   string =  "box"

#@description The default implicit length unit
length_unit       : string  = "mm"

#@description The X dimension
x   : real   = 100.0

#@description The Y dimension
y   : real   = 100.0

#@description The Z dimension
z   : real   =  40.0

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


##################################################################################
[name="wrapped_scintillator_block.model" type="geomtools::surrounded_boxed_model"]

#@config The configuration parameters for the wrapped scintillatorblock

############
# Geometry #
############

#@description The name of the surrounded geometry model
surrounded.model         : string = "scintillator_block.model"

#@description The label associated to the surrounded geometry model
surrounded.label         : string = "scintillator_block"

#@description The name of the (z>0) side surrounding geometry model
surrounded.top_model     : string = "zpos_wrapping.model"

#@description The name of a  (x>0) side surrounding geometry model
surrounded.front_model   : string = "xpos_wrapping.model"

#@description The name of a  (x<0) side surrounding geometry model
surrounded.back_model    : string = "xneg_wrapping.model"

#@description The name of a  (y>0) side surrounding geometry model
surrounded.right_model   : string = "ypos_wrapping.model"

#@description The name of a  (y<0) side surrounding geometry model
surrounded.left_model    : string = "yneg_wrapping.model"

#@description The label of the (z>0) side surrounding geometry model
surrounded.top_label     : string = "top_wrapping"

#@description The label of a  (x>0) side surrounding geometry model
surrounded.front_label   : string = "front_wrapping"

#@description The label of a  (x<0) side surrounding geometry model
surrounded.back_label    : string = "back_wrapping"

#@description The label of a  (y>0) side surrounding geometry model
surrounded.right_label   : string = "right_wrapping"

#@description The label of a  (y<0) side surrounding geometry model
surrounded.left_label    : string = "left_wrapping"

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref      : string  = "lab_medium"


##########################################
# GID mapping of internal/daughter items #
##########################################

#@description The mapping directive for the "scintillator_block" daughter volume
mapping.daughter_id.scintillator_block : string  = "[scin_block.gc]"

#@description The mapping directive for the "left_wrapping" daughter volume
mapping.daughter_id.front_wrapping     : string  = "[scin_block_wrapping.gc:side=0]"

#@description The mapping directive for the "right_wrapping" daughter volume
mapping.daughter_id.back_wrapping      : string  = "[scin_block_wrapping.gc:side=1]"

#@description The mapping directive for the "left_wrapping" daughter volume
mapping.daughter_id.left_wrapping      : string  = "[scin_block_wrapping.gc:side=2]"

#@description The mapping directive for the "right_wrapping" daughter volume
mapping.daughter_id.right_wrapping     : string  = "[scin_block_wrapping.gc:side=3]"

#@description The mapping directive for the "top_wrapping" daughter volume
mapping.daughter_id.top_wrapping       : string  = "[scin_block_wrapping.gc:side=5]"


################################################################
[name="light_guide.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the light guide

############
# Geometry #
############

#@description The default implicit length unit
length_unit  : string = "mm"

#@description The name of the 3D shape
shape_type   : string = "polyhedra"

#@description The polyhedra build mode
build_mode   : string = "points"

#@description The polyhedra number of sides
sides        : integer = 4

#@description The list of Z coordinates for the shape
list_of_z    : real [3] =  -40.     +30.     +40.

#@description The list of inner radius coordinates for the shape
list_of_rmin : real [3] =    0.      0.       0.

#@description The list of outer radius coordinates for the shape
list_of_rmax : real [3] =    20.    50.     50.

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
x             : real   = 100.0

#@description The Y dimension
y             : real   = 100.0

#@description The name of the model to be rotated
rotated.model : string = "light_guide.model"

#@description The label associated to the rotated daughter model
rotated.label : string = "light_guide"

#######################
# Material parameters #
#######################

#@description The name of the material around the light guide to be rotated
material.ref  : string  = "lab_medium"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden          : boolean = 0

#@description The recommended color for the display
visibility.color           : string  = "grey"

#@description The visibility hidden flag for the envelope
visibility.hidden_envelope : boolean = 0


########################################################
[name="pmt.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the photomultiplier tube

############
# Geometry #
############

#@description The default implicit length unit
length_unit  : string   = "mm"

#@description The name of the 3D shape
shape_type   : string   = "polycone"

#@description The polycone build mode
build_mode   : string   = "points"

#@description The list of Z coordinates for the shape
list_of_z    : real [6] =  -50.0   -49.0  -48.99  +48.99  +49.0 +50.0

#@description The list of inner radius coordinates for the shape
list_of_rmin : real [6] =    0.      0.    19.     19.      0.    0.

#@description The list of outer radius coordinates for the shape
list_of_rmax : real [6] =    20.    20.    20.     20.     20.   20.

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
visibility.color           : string  = "magenta"



#############################################################
[name="optical_module.model" type="geomtools::stacked_model"]

#@config The configuration parameters for the optical module

############
# Geometry #
############

#@description The stacking axis
stacked.axis            : string = "z"

#@description The number of stacked volumes
stacked.number_of_items : integer = 3

#@description The model of the stacked volume #2
stacked.model_2   : string  = "wrapped_scintillator_block.model"

#@description The label of the stacked volume #2
stacked.label_2   : string  = "scintillator_block"

#@description The model of the stacked volume #1
stacked.model_1   : string   = "rotated_light_guide.model"

#@description The label of the stacked volume #1
stacked.label_1   : string  = "light_guide"

#@description The model of the stacked volume #0
stacked.model_0   : string   = "pmt.model"

#@description The label of the stacked volume #0
stacked.label_0   : string  = "pmt"

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref      : string    = "lab_medium"

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

#@description The mapping directive for the "light_guide" daughter volume
mapping.daughter_id.light_guide        : string  = "[light_guide.gc]"

# End of list of multi-properties.
