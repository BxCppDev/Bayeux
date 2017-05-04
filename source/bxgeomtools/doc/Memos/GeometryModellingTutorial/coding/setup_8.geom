@description List of geometry models and their relationships in a simple world
#@key_label  "name"
#@meta_label "type"



##################
# CYLINDER MODEL #
##################

[name="blue_cylinder" type="geomtools::simple_shaped_model"]
shape_type        : string  = "cylinder"   
length_unit       : string  = "cm"     
r                 : real    = 10.0 
z                 : real    = 40.0    
material.ref      : string  = "__default__"
visibility.hidden : boolean = 0       
visibility.color  : string  = "blue"   


#############
# BOX MODEL #
#############

[name="huge_red_box" type="geomtools::simple_shaped_model"]
shape_type        : string  = "box"   
length_unit       : string  = "cm"     
x                 : real    = 50.0     
y                 : real    = 50.0    
z                 : real    = 40.0    
material.ref      : string  = "__default__"
visibility.hidden : boolean = 0       
visibility.color  : string  = "red"   


##############
# TUBE MODEL #
##############

[name="magenta_tube" type="geomtools::simple_shaped_model"]
shape_type        : string  = "tube"   
length_unit       : string  = "cm"     
inner_r           : real    = 10.0 
outer_r           : real    = 15.0 
z                 : real    = 40.0    
material.ref      : string  = "__default__"
visibility.hidden : boolean = 0       
visibility.color  : string  = "magenta"   


##################
# CYLINDER MODEL #
##################

[name="orange_cylinder" type="geomtools::simple_shaped_model"]
shape_type        : string  = "cylinder"   
length_unit       : string  = "cm"     
r                 : real    = 3.0 
z                 : real    = 50.0    
material.ref      : string  = "__default__"
visibility.hidden : boolean = 0       
visibility.color  : string  = "orange"   

####################
# SURROUNDED MODEL #
####################


[name="stacked_box" type="geomtools::stacked_model"]
material.ref : string = "__default__"

stacked.axis            : string = "z"
stacked.number_of_items : integer = 4
stacked.model_0     : string  = "blue_cylinder"
stacked.label_0     : string  = "stacked_0"
stacked.model_1     : string  = "huge_red_box"
stacked.label_1     : string  = "stacked_1"
stacked.model_2     : string  = "magenta_tube"
stacked.label_2     : string  = "stacked_2"
stacked.model_3     : string  = "orange_cylinder"
stacked.label_3     : string  = "stacked_3"

visibility.hidden : boolean   = 0
visibility.color  : string    = "grey"

length_unit  : string = "cm"
x            : real   = 55.
y            : real   = 55.
z            : real   = 200.
#stacked.limit_min_3 : real    = 40.
#stacked.limit_min_2 : real    = 40.

###############
# WORLD MODEL #
###############

[name="world" type="geomtools::simple_world_model"]
material.ref    : string = "vacuum" 
setup.model     : string = "stacked_box"
length_unit     : string = "m"  
setup.x         : real   = 0.0  
setup.y         : real   = 0.0   
setup.z         : real   = 0.0
angle_unit      : string = "degree" 
setup.phi       : real   = 0.0  
setup.theta     : real   = 0.0 
setup.delta     : real   = 0.0  
world.x         : real   = 1.5 
world.y         : real   = 1.5 
world.z         : real   = 2.5
visibility.hidden           : boolean = 0        
visibility.color            : string  = "green"   
visibility.daughters.hidden : boolean = 0 
