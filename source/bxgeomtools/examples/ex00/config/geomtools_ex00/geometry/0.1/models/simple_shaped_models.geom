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


########################################################################
[name="long_blue_cylinder0.model" type="geomtools::simple_shaped_model"]

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
z           : real as length =  120.0 mm

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


##########################################################################
[name="blue_medium_cylinder0.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a geometry model with a simple (blue) cylinder of arbitrary dimensions and material

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the volume
shape_type  : string = "cylinder"

#@description The radius of the cylinder
r           : real as length =  60.0 mm

#@description The height of the cylinder
z           : real as length =  40.0 mm

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


############################################################################
[name="orange_medium_cylinder0.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a geometry model with a simple (orange) cylinder of arbitrary dimensions and material

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the volume
shape_type  : string = "cylinder"

#@description The radius of the cylinder
r           : real as length =  65.0 mm

#@description The height of the cylinder
z           : real as length =  45.0 mm

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


###########################################################################
[name="green_medium_cylinder0.model" type="geomtools::simple_shaped_model"]

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


#########################################################################
[name="red_medium_cylinder0.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a geometry model with a simple (red) cylinder of arbitrary dimensions and material

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the volume
shape_type  : string = "cylinder"

#@description The radius of the cylinder
r           : real as length =  70.0 mm

#@description The height of the cylinder
z           : real as length =  55.0 mm

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


#####################################################################
[name="blue_large_tube0.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a geometry model with a simple (blue) tube of arbitrary dimensions and material

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the volume
shape_type  : string = "tube"

#@description The inner radius of the cylinder
inner_r     : real as length =  80.0 mm

#@description The outer radius of the cylinder
outer_r     : real as length =  100.0 mm

#@description The height of the cylinder
z           : real as length =  100.0 mm

############
# Material #
############

#@description The name of the material the tube is made of
material.ref : string = "iron"

##############
# Visibility #
##############

#@description The recommended color for the display of the tube
visibility.color  : string  = "blue"

#@description The visibility hidden flag for the display of the tube
visibility.hidden : boolean = 0


###################################################################
[name="red_huge_tube0.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a geometry model with a simple (red) tube of arbitrary dimensions and material

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the volume
shape_type  : string = "tube"

#@description The inner radius of the cylinder
inner_r     : real as length =  65.0 mm

#@description The outer radius of the cylinder
outer_r     : real as length =  90 mm

#@description The height of the cylinder
z           : real as length =  200.0 mm

############
# Material #
############

#@description The name of the material the tube is made of
material.ref : string = "iron"

##############
# Visibility #
##############

#@description The recommended color for the display of the tube
visibility.color  : string  = "red"

#@description The visibility hidden flag for the display of the tube
visibility.hidden : boolean = 0


########################################################
[name="foo.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a foo geometry model

#@description Shape build mode
shape_build_mode : string = "factory"

############
# Geometry #
############

# On the fly creation of 3D shapes and registration in the shape factory
shapes.names : string[2] = "foo_A" "foo_B"

shapes.shape_type.foo_A : string = "geomtools::box"
shapes.params.foo_A.x   : real as length = 3.2 cm
shapes.params.foo_A.y   : real as length = 1.6 cm
shapes.params.foo_A.z   : real as length = 0.8 cm

shapes.shape_type.foo_B : string = "geomtools::cylinder"
shapes.params.foo_B.r   : real as length = 0.75 cm
shapes.params.foo_B.z   : real as length = 0.5 cm

#@description The shape type
shape_type : string = "geomtools::union_3d"

shape.first_shape.name       : string = "foo_A"
shape.second_shape.name      : string = "foo_B"
shape.second_shape.placement : string = "0.0 0.0 0.5 (cm)"

############
# Material #
############

#@description The name of the material the tube is made of
material.ref : string = "iron"

##############
# Visibility #
##############

#@description The recommended color for the display of the tube
visibility.color  : string  = "red"

#@description The visibility hidden flag for the display of the tube
visibility.hidden : boolean = 0


########################################################
[name="bar.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a bar geometry model

#@description Shape build mode
shape_build_mode : string = "factory"

############
# Geometry #
############

#@description The shape reference from the factory
shape_ref : string = "foo_A"

############
# Material #
############

#@description The name of the material the tube is made of
material.ref : string = "iron"

##############
# Visibility #
##############

#@description The recommended color for the display of the tube
visibility.color  : string  = "red"

#@description The visibility hidden flag for the display of the tube
visibility.hidden : boolean = 0

##########################################################
[name="dummy.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a dummy geometry model

#@description Shape build mode
shape_build_mode : string = "factory"

############
# Geometry #
############

#@description The shape type
shape_type : string = "geomtools::union_3d"

# shape.first_shape.name       : string = "Cube1"
shape.first_shape.name       : string = "El1"
shape.second_shape.name      : string = "ThinBox1"
shape.second_shape.placement : string = "0.0 0.0 0.5 (mm) @ 35.0 90.0 (degree)"

############
# Material #
############

#@description The name of the material the tube is made of
material.ref : string = "iron"

##############
# Visibility #
##############

#@description The recommended color for the display of the tube
visibility.color  : string  = "green"

#@description The visibility hidden flag for the display of the tube
visibility.hidden : boolean = 0


#############################################################
[name="tessella.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a tessellated geometry model

#@description Shape build mode
shape_build_mode : string = "factory"

############
# Geometry #
############

#@description The shape type
shape_ref : string = "Ts1"

############
# Material #
############

#@description The name of the material the tube is made of
material.ref : string = "iron"

##############
# Visibility #
##############

#@description The recommended color for the display of the tube
visibility.color  : string  = "blue"

#@description The visibility hidden flag for the display of the tube
visibility.hidden : boolean = 0


#############################################################
[name="tessella2.model" type="geomtools::simple_shaped_model"]

#@config Configuration parameters for a tessellated geometry model

#@description Shape build mode
shape_build_mode : string = "factory"

############
# Geometry #
############

#@description The shape type
shape_ref : string = "Ts2"
# shape_ref : string = "Elt1"

############
# Material #
############

#@description The name of the material the tube is made of
material.ref : string = "iron"

##############
# Visibility #
##############

#@description The recommended color for the display of the tube
visibility.color  : string  = "magenta"

#@description The visibility hidden flag for the display of the tube
visibility.hidden : boolean = 0
