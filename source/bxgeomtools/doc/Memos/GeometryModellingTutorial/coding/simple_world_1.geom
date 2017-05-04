@description List of geometry models and their relationships in a simple world
#@key_label   "name"
#@meta_label  "type"


#############
# BOX MODEL #
#############

[name="small_red_box" type="geomtools::simple_shaped_model"]

#@config the list of parameters that define a small red box

#@description the shape of the volume 
shape_type        : string =  "box"   # must be supported by the model class

#@description the unit for all length dimensions (see above)
length_unit       : string = "cm"     # note: we use CLHEP unit system
x                 : real   = 20.0     # dimensions of the shape;
y                 : real   = 25.0     #  must be coherent with the
z                 : real   = 15.0     #  type above (box)

#@description the name of the material the box is made of
material.ref      : string  = "__default__" # must be known by an external agent

#@description optional visibility flag
visibility.hidden : boolean = 0        #  hidden flag

#@description optional visibility information
visibility.color  : string  = "red"    #  display color


###############
# WORLD MODEL #
###############

[name="world" type="geomtools::simple_world_model"]

#@config the list of parameters that define the top-level large green world box

#@description the name of the material the box is made of
material.ref    : string = "vacuum"       # must be known by an external agent

#@description the name of the model that is contained in the world volume
setup.model     : string = "small_red_box" # we use here the name of the 
                                           # only available model

#@description the unit for all angular dimensions (see below)
angle_unit      : string = "degree" # CLHEP system

#@description  ZYZ Euler first angle
setup.phi       : real   = 80.0  

#@description  ZYZ Euler second angle
setup.theta     : real   = 60.0 

#@description  ZYZ Euler third angle
setup.delta     : real   = 30.0  

#@description the unit for all length dimensions (see below)
length_unit     : string = "m"  # CLHEP system

#@description x position of the setup in the world coordinate system
setup.x         : real   = 0.4  

#@description y position of the setup in the world coordinate system
setup.y         : real   = 0.5   

#@description z position of the setup in the world coordinate system
setup.z         : real   = 0.6

#@description dimension of the world box on the x axis
world.x         : real   = 1.5 # must be larger enough to fully enclose the setup

#@description dimension of the world box on the y axis
world.y         : real   = 1.5 # must be larger enough to fully enclose the setup

#@description dimension of the world box on the z axis
world.z         : real   = 1.5 # must be larger enough to fully enclose the setup

#@description optional visibility flag
visibility.hidden : boolean = 0        #  hidden flag

#@description optional visibility information
visibility.color  : string  = "green"    #  display color

#@description optional visibility information for daughters 
visibility.daughters.hidden : boolean = 0 # only the 'setup' daughter is affected
