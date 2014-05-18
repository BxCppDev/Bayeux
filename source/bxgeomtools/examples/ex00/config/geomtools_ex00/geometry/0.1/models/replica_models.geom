# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description Some geometry models that implements a replication algorithm

#@key_label   "name"
#@meta_label  "type"


###################################################################
[name="box0_linear_chain.model" type="geomtools::replicated_model"]

#@config The list of properties to configure the replication of some volumes along an arbitrary axis

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The label used to name the replicated volumes
replicated.label : string = "aligned_boxes"

#@description The replication axis
replicated.axis : string = "y"

#@description The name of the geometry model to be replicated
replicated.model : string = "red_box0.model"

#@description The number of replicated objects
replicated.number_of_items : integer = 4

#@description The step length along the replication axis
replicated.step : real as length = 10 cm

############
# Material #
############

#@description The name of the material that fills the mother volume that hosts replicated volumes
material.ref : string = "vacuum"

###########
# Mapping #
###########

#@description The mapping directives for the "aligned_boxes" daughter volume
mapping.daughter_id.aligned_boxes : string  = "[box.gc:position+5]"

##############
# Visibility #
##############

#@description The recommended color for the display of the mother volume
visibility.color  : string  = "grey"

#@description The visibility hidden flag for the display of the mother volume
visibility.hidden : boolean = 0


##############################################################################
[name="box0_circular_chain.model" type="geomtools::replicated_circular_model"]

#@config The list of properties to configure the replication of some volumes around an arbitrary rotation axis

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The default length unit
angle_unit : string = "degree"

#@description The label used to name the replicated volumes
replicated.label : string = "go_round_boxes"

#@description The replication axis (only Z is supported)
replicated.rotation_axis : string = "z"

#@description The name of the geometry model to be replicated
replicated.model : string = "magenta_box0.model"

#@description The number of replicated objects
replicated.number_of_items : integer = 9

#@description The radius of the replication rotation
replicated.radius : real as length = 20 cm

#@description The step angle around the replication rotation axis
replicated.step_angle : real as angle = 30 degree

#@description The start angle around the replication rotation axis
replicated.start_angle : real as angle = 45 degree

#@description The flag to rotate the model together with the rotation around the replication rotation axis
replicated.follow_rotation : boolean = true

#@description The maximum radius of the mother tube
r_max : real as length = 26 cm

#@description The minimum radius of the mother tube
r_min : real as length = 14 cm

#@description The height of the mother tube
z : real as length = 4 cm

############
# Material #
############

#@description The name of the material that fills the mother volume that hosts replicated boxes
material.ref : string = "vacuum"

###################
# Daughter volume #
###################

#@description The list of daughter volumes by labels
internal_item.labels : string[2] = "SpecialBox0" "SpecialBox1"

#@description The model of the "SpecialBox0" daughter volume
internal_item.model.SpecialBox0     : string  = "blue_box0.model"

#@description The placement of the "SpecialBox0" daughter volume
internal_item.placement.SpecialBox0 : string  = "+140 -140 0 (mm)"

#@description The model of the "SpecialBox1" daughter volume
internal_item.model.SpecialBox1     : string  = "green_box0.model"

#@description The placement of the "SpecialBox1" daughter volume
internal_item.placement.SpecialBox1 : string  = "+200 0 0 (mm) / z 90 (degree)"


###########
# Mapping #
###########

#@description The mapping directives for the "go_round_boxes" daughter volumes
mapping.daughter_id.go_round_boxes : string  = "[box.gc:position+9]"

#@description The mapping directives for the "SpecialBox0" daughter volume
mapping.daughter_id.SpecialBox0 : string  = "[box.gc:position=18]"

#@description The mapping directives for the "SpecialBox1" daughter volume
mapping.daughter_id.SpecialBox1 : string  = "[box.gc:position=19]"

##############
# Visibility #
##############

#@description The recommended color for the display of the mother volume
visibility.color  : string  = "grey"

#@description The visibility hidden flag for the display of the mother volume
visibility.hidden : boolean = 0

########################################################
[name="box0_yz_grid.model" type="geomtools::grid_model"]

#@config Configuration parameters for the placement of some volumes on a 2D grid

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The label used to name the volumes
grid.daughter_label : string = "grid"

#@description The plane of the grid
grid.plane : string = "yz"

#@description The number of colums along the Y axis
grid.y.number_of_items : integer = 9

#@description The number of rows along the Z axis
grid.z.number_of_items : integer = 3

#@description The step length along the Y axis
grid.y.step : real as length = 10 cm

#@description The step length along the Z axis
grid.z.step : real as length = 20 cm

#@description The name of the geometry model to be placed ont he grid
grid.model : string = "green_box0.model"

############
# Material #
############

#@description The name of the material that fills the mother volume that hosts replicated boxes
material.ref : string = "vacuum"

##############
# Visibility #
##############

#@description The recommended color for the display of the mother volume
visibility.color  : string  = "grey"

#@description The visibility hidden flag for the display of the mother volume
visibility.hidden : boolean = 0

###################
# Daughter volume #
###################

#@description The list of daughter volumes by labels
internal_item.labels : string[2] = "AdditionalBox0" "AdditionalBox1"

#@description The model of the "AdditionalBox0" daughter volume
internal_item.model.AdditionalBox0   : string  = "blue_box0.model"

#@description The placement of the "AdditionalBox0" daughter volume
internal_item.placement.AdditionalBox0 : string  = "0 -250 -100 (mm)"

#@description The model of the "AdditionalBox1" daughter volume
internal_item.model.AdditionalBox1   : string  = "blue_box0.model"

#@description The placement of the "AdditionalBox1" daughter volume
internal_item.placement.AdditionalBox1 : string  = "0 +150 +80 (mm) / z 90 (degree)"

###########
# Mapping #
###########

#@description The mapping directives for the "grid" daughter volume
mapping.daughter_id.grid : string  = "[brick.gc:column+0,row+0]"

#@description The mapping directives for the "AdditionalBox0" daughter volume
mapping.daughter_id.AdditionalBox0 : string  = "[box.gc:position=20]"

#@description The mapping directives for the "AdditionalBox1" daughter volume
mapping.daughter_id.AdditionalBox1 : string  = "[box.gc:position=21]"


######################################################################
[name="cylinder0_big_column.model" type="geomtools::replicated_model"]

#@config Configuration properties for a column of cylinder volumes

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The label used to name the replicated volumes
replicated.label : string = "aligned_cylinders"

#@description The replication axis
replicated.axis : string = "z"

#@description The name of the geometry model to be replicated
replicated.model : string = "orange_cylinder0.model"

#@description The number of replicated objects
replicated.number_of_items : integer = 7

#@description The step length along the replication axis
replicated.step : real as length = 10 cm

############
# Material #
############

#@description The name of the material that fills the mother volume that hosts replicated volumes
material.ref : string = "vacuum"

###########
# Mapping #
###########

#@description The mapping directives for the "aligned_cylinders" daughter volume
mapping.daughter_id.aligned_cylinders : string = "[any_node.gc:row+1]"

##############
# Visibility #
##############

#@description The recommended color for the display of the mother volume
visibility.color  : string  = "cyan"

#@description The visibility hidden flag for the display of the mother volume
visibility.hidden : boolean = 0


#########################################################################
[name="cylinder0_medium_column.model" type="geomtools::replicated_model"]

#@config Configuration properties for a column of cylinder volumes

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The label used to name the replicated volumes
replicated.label : string = "aligned_cylinders"

#@description The replication axis
replicated.axis : string = "y"

#@description The name of the geometry model to be replicated
replicated.model : string = "green_cylinder0.model"

#@description The number of replicated objects
replicated.number_of_items : integer = 3

#@description The step length along the replication axis
replicated.step : real as length = 10 cm

############
# Material #
############

#@description The name of the material that fills the mother volume that hosts replicated volumes
material.ref : string = "vacuum"

###########
# Mapping #
###########

#@description The mapping directives for the "aligned_cylinders" daughter volume
mapping.daughter_id.aligned_cylinders : string = "[any_node.gc:row+2]"

##############
# Visibility #
##############

#@description The recommended color for the display of the mother volume
visibility.color  : string  = "cyan"

#@description The visibility hidden flag for the display of the mother volume
visibility.hidden : boolean = 0


########################################################################
[name="cylinder0_small_column.model" type="geomtools::replicated_model"]

#@config Configuration properties for a column of cylinder volumes

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The label used to name the replicated volumes
replicated.label : string = "aligned_cylinders"

#@description The replication axis
replicated.axis : string = "y"

#@description The name of the geometry model to be replicated
replicated.model : string = "red_cylinder0.model"

#@description The number of replicated objects
replicated.number_of_items : integer = 2

#@description The step length along the replication axis
replicated.step : real as length = 10 cm

############
# Material #
############

#@description The name of the material that fills the mother volume that hosts replicated volumes
material.ref : string = "vacuum"

###########
# Mapping #
###########

#@description The mapping directives for the "aligned_cylinders" daughter volume
mapping.daughter_id.aligned_cylinders : string = "[any_node.gc:row+3]"

##############
# Visibility #
##############

#@description The recommended color for the display of the mother volume
visibility.color  : string  = "cyan"

#@description The visibility hidden flag for the display of the mother volume
visibility.hidden : boolean = 0


###########################################################
[name="box0_small_grid.model" type="geomtools::grid_model"]

#@config Configuration parameters for the placement of some volumes on a small 2D grid

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The label used to name the volumes
grid.daughter_label : string = "grid"

#@description The plane of the grid
grid.plane : string = "xz"

#@description The number of colums along the X axis
grid.x.number_of_items : integer = 2

#@description The number of rows along the Z axis
grid.z.number_of_items : integer = 3

#@description The step length along the X axis
grid.x.step : real as length = 20 cm

#@description The step length along the Z axis
grid.z.step : real as length = 10 cm

#@description The name of the geometry model to be placed ont he grid
grid.model : string = "blue_box0.model"

############
# Material #
############

#@description The name of the material that fills the mother volume that hosts replicated boxes
material.ref : string = "vacuum"

##############
# Visibility #
##############

#@description The recommended color for the display of the mother volume
visibility.color  : string  = "cyan"

#@description The visibility hidden flag for the display of the mother volume
visibility.hidden : boolean = 0

###########
# Mapping #
###########

#@description The mapping directives for the "grid" daughter volume
mapping.daughter_id.grid  : string = "[any_node.gc:column+1,row+5]"
