# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry models that compose the shielding
#@key_label   "name"
#@meta_label  "type"


##########################################################
[name="brick.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the shielding brick

############
# Geometry #
############

#@description The default implicit length unit
length_unit : string = "cm"

#@description The name of the 3D shape of the source film
shape_type  : string = "box"

#@description The X dimension of the brick
x           : real = 20.0

#@description The Y dimension of the brick
y           : real =  5.0

#@description The Z dimension of the brick
z           : real = 10.0

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref : string = "lead"

#########################
# Visibility parameters #
#########################

#@description The recommended color for the display
visibility.color            : string  = "magenta"

#@description The visibility hidden flag for the display of the source film
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of the source film
visibility.daughters.hidden : boolean = 0


#################################################################
[name="shielding_brick_array.model" type="geomtools::grid_model"]

#@config The configuration parameters for the shielding

############
# Geometry #
############

#@description The default lenght unit
#length_unit : string = "cm"

#@description The direction of the grid
grid.plane : string = "xz"

#@description The number of columns (X axis)
grid.x.number_of_items : integer = 7

#@description The number of rows (Z axis)
grid.z.number_of_items : integer = 12

#@description The number of columns (X axis)
grid.x.step : real as length = 21 cm

#@description The number of columns (X axis)
grid.z.step : real as length = 10.5 cm

#@description Force the X dimension of the grid enclosing box volume (X axis)
x : real as length = 165 cm

#@description Force the Y dimension of the grid enclosing box volume (Y axis)
y : real as length = 7 cm

#@description Force the Z dimension of the grid enclosing box volume (Z axis)
z : real as length = 130 cm

#@description The name of the geometry model
grid.model : string = "brick.model"

#@description The name of the geometry model
grid.daughter_label : string = "bricks"

#######################
# Material parameters #
#######################

#@description The name of the material around/between the bricks (same as "lab.model")
material.ref : string = "air"

##########################################
# GID mapping of internal/daughter items #
##########################################

#@description The mapping directives for the "endcap0" daughter volume
mapping.daughter_id.bricks : string  = "[shield_brick.gc:column+0,row+0]"


# End of list of multi-properties.
