# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of the detector volume

#@key_label   "name"
#@meta_label  "type"

# We define here several geometry models with simple shaped volume (box, cylinder...)
# with arbitrary dimensions and materials.
# Some special *visibility* directives are used to set the colors used for the
# graphics rendering of the volumes.


##############################################################
[name="blue_box0.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a geometry model with a simple (blue) box of arbitrary dimensions and material

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the detector volume
shape_type  : string = "box"

#@description The X dimension of the box
x           : real as length = 100.0 mm

#@description The Y dimension of the box
y           : real as length =  50.0 mm

#@description The Z dimension of the box
z           : real as length =  20.0 mm

############
# Material #
############

#@description The name of the material that fills the box
material.ref : string = "copper"

##############
# Visibility #
##############

#@description The recommended color for the display of the box
visibility.color  : string  = "blue"

#@description The visibility hidden flag for the display of the box
visibility.hidden : boolean = 0


##############################################################
[name="red_box0.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a geometry model with a simple (red) box of arbitrary dimensions and material

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the volume
shape_type  : string = "box"

#@description The X dimension of the box
x           : real as length = 100.0 mm

#@description The Y dimension of the box
y           : real as length =  50.0 mm

#@description The Z dimension of the box
z           : real as length =  20.0 mm

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

#@description The visibility hidden flag for the display of the box
visibility.hidden : boolean = 0


##############################################################
[name="magenta_box0.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a geometry model with a simple (magenta) box of arbitrary dimensions and material

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the volume
shape_type  : string = "box"

#@description The X dimension of the box
x           : real as length = 100.0 mm

#@description The Y dimension of the box
y           : real as length =  50.0 mm

#@description The Z dimension of the box
z           : real as length =  20.0 mm

############
# Material #
############

#@description The name of the material that fills the box
material.ref : string = "copper"

##############
# Visibility #
##############

#@description The recommended color for the display of the box
visibility.color  : string  = "magenta"

#@description The visibility hidden flag for the display of the box
visibility.hidden : boolean = 0


##############################################################
[name="green_box0.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a geometry model with a simple (green) box of arbitrary dimensions and material

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the volume
shape_type  : string = "box"

#@description The X dimension of the box
x           : real as length = 100.0 mm

#@description The Y dimension of the box
y           : real as length =  50.0 mm

#@description The Z dimension of the box
z           : real as length =  20.0 mm

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

#@description The visibility hidden flag for the display of the box
visibility.hidden : boolean = 0


###################################################################
[name="blue_cylinder0.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a geometry model with a simple (blue) cylinder of arbitrary dimensions and material

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the volume
shape_type  : string = "cylinder"

#@description The radius of the cylinder
r           : real as length =  40.0 mm

#@description The height of the cylinder
z           : real as length =  50.0 mm

############
# Material #
############

#@description The name of the material that fills the box
material.ref : string = "lead"

##############
# Visibility #
##############

#@description The recommended color for the display of the box
visibility.color  : string  = "blue"

#@description The visibility hidden flag for the display of the box
visibility.hidden : boolean = 0


#####################################################################
[name="orange_cylinder0.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a geometry model with a simple (orange) cylinder of arbitrary dimensions and material

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the volume
shape_type  : string = "cylinder"

#@description The radius of the cylinder
r           : real as length =  40.0 mm

#@description The height of the cylinder
z           : real as length =  50.0 mm

############
# Material #
############

#@description The name of the material that fills the box
material.ref : string = "lead"

##############
# Visibility #
##############

#@description The recommended color for the display of the box
visibility.color  : string  = "orange"

#@description The visibility hidden flag for the display of the box
visibility.hidden : boolean = 0


####################################################################
[name="green_cylinder0.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a geometry model with a simple (green) cylinder of arbitrary dimensions and material

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the volume
shape_type  : string = "cylinder"

#@description The radius of the cylinder
r           : real as length =  40.0 mm

#@description The height of the cylinder
z           : real as length =  50.0 mm

############
# Material #
############

#@description The name of the material that fills the box
material.ref : string = "lead"

##############
# Visibility #
##############

#@description The recommended color for the display of the box
visibility.color  : string  = "green"

#@description The visibility hidden flag for the display of the box
visibility.hidden : boolean = 0


##################################################################
[name="red_cylinder0.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a geometry model with a simple (red) cylinder of arbitrary dimensions and material

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the volume
shape_type  : string = "cylinder"

#@description The radius of the cylinder
r           : real as length =  40.0 mm

#@description The height of the cylinder
z           : real as length =  50.0 mm

############
# Material #
############

#@description The name of the material that fills the box
material.ref : string = "lead"

##############
# Visibility #
##############

#@description The recommended color for the display of the box
visibility.color  : string  = "red"

#@description The visibility hidden flag for the display of the box
visibility.hidden : boolean = 0
