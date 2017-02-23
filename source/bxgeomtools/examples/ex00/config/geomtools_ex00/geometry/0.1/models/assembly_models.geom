# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description Some geometry models that implements an smart assembly of volumes

#@key_label   "name"
#@meta_label  "type"


#############################################################
[name="detector_stack.model" type="geomtools::stacked_model"]

#@config Configuration parameters to describe a detector unit as a stack of detection elements

############
# Geometry #
############

#@description The mother volume
envelope_shape  : string = "cylinder"

#@description The default length unit
length_unit : string = "mm"

#@description The stacking axis
stacked.axis    : string = "z"

#@description The number of stacked volumes
stacked.number_of_items : integer = 4

#@description The name of the geometry model at stacking slot 0
stacked.model_0 : string = "long_blue_cylinder0.model"

#@description The name of the geometry model at stacking slot 1
stacked.model_1 : string = "orange_medium_cylinder0.model"

#@description The name of the geometry model at stacking slot 2
stacked.model_2 : string = "green_box0.model"

#@description The name of the geometry model at stacking slot 3
stacked.model_3 : string = "red_medium_cylinder0.model"

#@description The name of the volume at stacking slot 0
stacked.label_0 : string = "Collector"

#@description The name of the volume at stacking slot 1
stacked.label_1 : string = "BackSensor"

#@description The name of the volume at stacking slot 2
stacked.label_2 : string = "Attenuator"

#@description The name of the volume at stacking slot 3
stacked.label_3 : string = "FrontSensor"

############
# Material #
############

#@description The name of the material that fills the box
material.ref    : string = "vacuum"

##############
# Visibility #
##############

#@description The recommended color for the display of the mother volume
visibility.color  : string  = "grey"

###########
# Mapping #
###########

# #@description The mapping directives for the "Attenuator" daughter volume
# mapping.daughter_id.Attenuator : string = "[attenuator.gc]"

# #@description The mapping directives for the "Collector" daughter volume
# mapping.daughter_id.Collector  : string = "[collector.gc]"

# #@description The mapping directives for the "BackSensor" daughter volume
# mapping.daughter_id.BackSensor  : string = "[sensor.gc:position=0]"

# #@description The mapping directives for the "FrontSensor" daughter volume
# mapping.daughter_id.FrontSensor  : string = "[sensor.gc:position=1]"


###################################################################
[name="stack_with_insertion.model" type="geomtools::stacked_model"]

#@config Configuration parameters to describe a stack of volumes with special positioning

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The stacking axis
stacked.axis    : string = "z"

#@description The number of stacked volumes
stacked.number_of_items : integer = 4

#@description The name of the geometry model at stacking slot 0
stacked.model_0 : string = "blue_large_tube0.model"

#@description The position of the top contact point of the geometry model at stacking slot 0
stacked.limit_max_0 : real as length = 20.0 mm

#@description The name of the geometry model at stacking slot 1
stacked.model_1 : string = "orange_medium_cylinder0.model"

#@description The name of the geometry model at stacking slot 2
stacked.model_2 : string = "green_box0.model"

#@description The position of the bottom contact point of the geometry model at stacking slot 3
stacked.limit_min_3 : real as length = -130.0 mm

#@description The name of the geometry model at stacking slot 3
stacked.model_3 : string = "red_huge_tube0.model"

############
# Material #
############

#@description The name of the material that fills the box
material.ref    : string = "vacuum"

##############
# Visibility #
##############

#@description The recommended color for the display of the mother volume
visibility.color  : string  = "grey"


##########################################################
[name="exbox1.model" type="geomtools::extruded_box_model"]

#@config Configuration parameters to describe a closed extruded box

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The X dimension
x : real as length = 10 cm

#@description The Y dimension
y : real as length = 10 cm

#@description The Z dimension
z : real as length = 20 cm

#@description The thickness of the walls
thickness : real as length = 2 cm

############
# Material #
############

#@description The name of the material that fills the box
material.ref : string = "copper"

##############
# Visibility #
##############

#@description The recommended color for the display of the box
visibility.color  : string  = "red"


##########################################################
[name="exbox2.model" type="geomtools::extruded_box_model"]

#@config Configuration parameters to describe a semi closed extruded box

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The X dimension
x : real as length = 16 cm

#@description The Y dimension
y : real as length = 16 cm

#@description The Z dimension
z : real as length = 10 cm

#@description The thickness of the walls
thickness : real as length = 1 cm

#@description Bottom flag
has_bottom : boolean = false


############
# Material #
############

#@description The name of the material that fills the box
material.ref : string = "copper"

##############
# Visibility #
##############

#@description The recommended color for the display of the box
visibility.color  : string  = "green"


############################################################
[name="another_stack.model" type="geomtools::stacked_model"]

#@config Configuration parameters to describe a stack of volumes with special positioning

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The stacking axis
stacked.axis : string = "z"

#@description The number of stacked volumes
stacked.number_of_items : integer = 5

#@description The name of the geometry model at stacking slot 0
stacked.model_0 : string = "blue_box0.model"

#@description The position of the top contact point of the geometry model at stacking slot 0
stacked.limit_max_0 : real as length = 20.0 mm

#@description The name of the geometry model at stacking slot 1
stacked.model_1 : string = "blue_box0.model"

#@description The name of the geometry model at stacking slot 2
stacked.model_2 : string = "exbox2.model"

#@description The name of the geometry model at stacking slot 3
stacked.model_3 : string = "exbox1.model"

#@description The position of the top/right contact point of the geometry model at stacking slot 3
stacked.limit_max_3 : real as length = 12 cm

#@description The name of the geometry model at stacking slot 4
stacked.model_4 : string = "exbox2.model"

############
# Material #
############

#@description The name of the material that fills the box
material.ref    : string = "vacuum"

##############
# Visibility #
##############

#@description The recommended color for the display of the mother volume
visibility.color  : string  = "grey"
