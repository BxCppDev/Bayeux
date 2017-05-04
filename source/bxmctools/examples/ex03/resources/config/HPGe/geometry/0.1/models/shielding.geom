# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry models of the shielding
#@key_label  "name"
#@meta_label "type"


#####################################################################
[name="shielding_bottom.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the bottom shield plate

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "box"

#@description The X dimension (box)
x : real as length = 80.0 cm

#@description The Y dimension (box)
y : real as length = 60.0 cm

#@description The Z dimension (box)
z : real as length = 50.0 mm

############
# Material #
############

#@description The name of the material
material.ref : string = "lead"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden  : boolean = 0

#@description The recommended color for the display of the volume
visibility.color   : string  = "magenta"

#@description The recommended opacity for the display of the volume
visibility.opacity : real    = 0.05

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1


##################################################################
[name="shielding_top.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the top shield plate

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "box"

#@description The X dimension (box)
x : real as length = 75.0 cm

#@description The Y dimension (box)
y : real as length = 55.0 cm

#@description The Z dimension (box)
z : real as length =  5.0 cm

############
# Material #
############

#@description The name of the material
material.ref : string = "lead"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden  : boolean = 0

#@description The recommended color for the display of the volume
visibility.color   : string  = "magenta"

#@description The recommended opacity for the display of the volume
visibility.opacity : real    = 0.05

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1


######################################################################
[name="shielding_side_fb.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the front and back shield plates

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "box"

#@description The X dimension (box)
x : real as length =  5.0 cm

#@description The Y dimension (box)
y : real as length = 50.0 cm

#@description The Z dimension (box)
z : real as length = 60.0 cm

############
# Material #
############

#@description The name of the material
material.ref : string = "lead"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden  : boolean = 0

#@description The recommended color for the display of the volume
visibility.color   : string  = "magenta"

#@description The recommended opacity for the display of the volume
visibility.opacity : real    = 0.05

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1


######################################################################
[name="shielding_side_lr.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for the left and right shield plates

############
# Geometry #
############

#@description The name of the 3D shape of the logical volume
shape_type : string = "box"

#@description The X dimension (box)
x : real as length = 60.0 cm

#@description The Y dimension (box)
y : real as length =  5.0 cm

#@description The Z dimension (box)
z : real as length = 60.0 cm

############
# Material #
############

#@description The name of the material
material.ref : string = "lead"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display of the volume
visibility.hidden  : boolean = 0

#@description The recommended color for the display of the volume
visibility.color   : string  = "magenta"

#@description The recommended opacity for the display of the volume
visibility.opacity : real    = 0.05

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1


# end of shielding.geom