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

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the world logical volume
shape_type : string = "box"

#@description The world volume X dimension (box)
x          : real as length = 1000.0 mm

#@description The world volume Y dimension (box)
y          : real as length = 1000.0 mm

#@description The world volume Z dimension (box)
z          : real as length = 1000.0 mm

############
# Material #
############

#@description The name of the material that fills the lab atmosphere
material.ref : string = "vacuum"

###################
# Daughter volume #
###################

#@description The list of daughter volumes by labels
internal_item.labels : string[18] = \
  "BoxA" "BoxB" "BoxC" "BoxD" "BoxE" \
  "ManyBoxes" "CircleBoxes" \
  "WallBricks" \
  "CylA" "CylB" "CylC" "CylD" "CylE" "CylF" \
  "CylCol0" "CylCol1a" "CylCol2a" "CylSubGrid"

#@description The model of the "BoxA" daughter volume
internal_item.model.BoxA       : string  = "blue_box0.model"

#@description The placement of the "BoxA" daughter volume
internal_item.placement.BoxA   : string  = "-40 40 -40 (cm)"

#@description The model of the "BoxB" daughter volume
internal_item.model.BoxB       : string  = "blue_box0.model"

#@description The placement of the "BoxB" daughter volume
internal_item.placement.BoxB   : string  = "-40 30 -40 (cm)/ z 30 (degree) "

#@description The model of the "BoxC" daughter volume
internal_item.model.BoxC       : string  = "blue_box0.model"

#@description The placement of the "BoxC" daughter volume
internal_item.placement.BoxC   : string  = "-40 20 -40 (cm) / y 60 (degree) "

#@description The model of the "BoxD" daughter volume
internal_item.model.BoxD       : string  = "blue_box0.model"

#@description The placement of the "BoxD" daughter volume
internal_item.placement.BoxD   : string  = "-40 10 -40 (cm) / x 20 (degree) "

#@description The model of the "BoxE" daughter volume
internal_item.model.BoxE       : string  = "blue_box0.model"

#@description The placement of the "BoxE" daughter volume
internal_item.placement.BoxE   : string  = "-40 0 -40 (cm) @ -30 25 0 (degree) "

#@description The model of the "ManyBoxes" daughter volume
internal_item.model.ManyBoxes       : string  = "box0_linear_chain.model"

#@description The placement of the "ManyBoxes" daughter volume
internal_item.placement.ManyBoxes   : string  = "-40 -25 -40 (cm) "

#@description The model of the "CircleBoxes" daughter volume
internal_item.model.CircleBoxes     : string  = "box0_circular_chain.model"

#@description The placement of the "CircleBoxes" daughter volume
internal_item.placement.CircleBoxes : string  = "-5 -20 -40 (cm) "

#@description The model of the "WallBricks" daughter volume
internal_item.model.WallBricks      : string  = "box0_yz_grid.model"

#@description The placement of the "WallBricks" daughter volume
internal_item.placement.WallBricks  : string  = "-40 0 0 (cm) "

#@description The model of the "CylA" daughter volume
internal_item.model.CylA         : string  = "blue_cylinder0.model"

#@description The placement of the "CylA" daughter volume
internal_item.placement.CylA     : string  = "-20 40 -40 (cm)"

#@description The model of the "CylB" daughter volume
internal_item.model.CylB         : string  = "blue_cylinder0.model"

#@description The placement of the "CylB" daughter volume
internal_item.placement.CylB     : string  = "0 40 -40 (cm) / x 30 (degree)"

#@description The model of the "CylC" daughter volume
internal_item.model.CylC         : string  = "blue_cylinder0.model"

#@description The placement of the "CylC" daughter volume
internal_item.placement.CylC     : string  = "20 40 -40 (cm) / x 60 (degree)"

#@description The model of the "CylD" daughter volume
internal_item.model.CylD         : string  = "blue_cylinder0.model"

#@description The placement of the "CylD" daughter volume
internal_item.placement.CylD     : string  = "0 40 -30 (cm) / x 60 (degree)"

#@description The model of the "CylE" daughter volume
internal_item.model.CylE         : string  = "blue_cylinder0.model"

#@description The placement of the "CylE" daughter volume
internal_item.placement.CylE     : string  = "20 40 -30 (cm) / x 90 (degree)"

#@description The model of the "CylF" daughter volume
internal_item.model.CylF         : string  = "blue_cylinder0.model"

#@description The placement of the "CylF" daughter volume
internal_item.placement.CylF     : string  = "20 40 -20 (cm) / x 120 (degree)"

#@description The model of the "CylCol0" daughter volume
internal_item.model.CylCol0      : string  = "cylinder0_big_column.model"

#@description The placement of the "CylCol0" daughter volume
internal_item.placement.CylCol0  : string  = "-20 40 0 (cm)"

#@description The model of the "CylCol1a" daughter volume
internal_item.model.CylCol1a     : string  = "cylinder0_medium_column.model"

#@description The placement of the "CylCol1a" daughter volume
internal_item.placement.CylCol1a : string  = "0 40 -10 (cm) / x 75 (degree)"

#@description The model of the "CylCol2a" daughter volume
internal_item.model.CylCol2a     : string  = "cylinder0_small_column.model"

#@description The placement of the "CylCol2a" daughter volume
internal_item.placement.CylCol2a : string  = "20 40 -5 (cm) / x 90 (degree)"

#@description The model of the "CylSubGrid" daughter volume
internal_item.model.CylSubGrid      : string  = "box0_small_grid.model"

#@description The placement of the "CylSubGrid" daughter volume
internal_item.placement.CylSubGrid  : string  = "10 45 20 (cm)"

###########
# Mapping #
###########

# Here all the boxes of model "box0.model" that have been set in the world
# volume (directly or indireclty through dedicated *replicated* models)
# are considered to be classified (and labelled) in the same geometry
# category: "box.gc". The numbering start arbitrarily from 0 (for
# daughter "BoxA") and runs up to 17 (to last box in the "CircleBoxes"
# replica model.

#@description The mapping directives for the "BoxA" daughter volume
mapping.daughter_id.BoxA : string  = "[box.gc:position=0]"

#@description The mapping directives for the "BoxB" daughter volume
mapping.daughter_id.BoxB : string  = "[box.gc:position=1]"

#@description The mapping directives for the "BoxC" daughter volume
mapping.daughter_id.BoxC : string  = "[box.gc:position=2]"

#@description The mapping directives for the "BoxD" daughter volume
mapping.daughter_id.BoxD : string  = "[box.gc:position=3]"

#@description The mapping directives for the "BoxE" daughter volume
mapping.daughter_id.BoxE : string  = "[box.gc:position=4]"

# #@description The mapping directives for the "ManyBoxes" daughter volume
# mapping.daughter_id.ManyBoxes : string  = "[box.gc:position+5]"

# #@description The mapping directives for the "CircleBoxes" daughter volume
# mapping.daughter_id.CircleBoxes : string  = "[box.gc:position+9]"

#@description The mapping directives for the "CylA" daughter volume
mapping.daughter_id.CylA : string  = "[any_node.gc:column=0,row=0]"

#@description The mapping directives for the "CylB" daughter volume
mapping.daughter_id.CylB : string  = "[any_node.gc:column=1,row=0]"

#@description The mapping directives for the "CylC" daughter volume
mapping.daughter_id.CylC : string  = "[any_node.gc:column=2,row=0]"

#@description The mapping directives for the "CylD" daughter volume
mapping.daughter_id.CylD : string  = "[any_node.gc:column=1,row=1]"

#@description The mapping directives for the "CylE" daughter volume
mapping.daughter_id.CylE : string  = "[any_node.gc:column=2,row=1]"

#@description The mapping directives for the "CylF" daughter volume
mapping.daughter_id.CylF : string  = "[any_node.gc:column=2,row=2]"

#@description The mapping directives for the "CylCol0" daughter volume
mapping.daughter_id.CylCol0  : string = "[any_column.gc:column=0]"

#@description The mapping directives for the "CylCol1a" daughter volume
mapping.daughter_id.CylCol1a : string = "[any_column.gc:column=1]"

#@description The mapping directives for the "CylCol2a" daughter volume
mapping.daughter_id.CylCol2a : string = "[any_column.gc:column=2]"


##############
# Visibility #
##############

#@description The recommended color for the display of the world volume
visibility.color            : string  = "cyan"

#@description The visibility hidden flag for the display of the world volume
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display
visibility.daughters.hidden : boolean = 0
