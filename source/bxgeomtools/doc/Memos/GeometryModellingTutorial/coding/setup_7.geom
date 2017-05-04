@description List of geometry models and their relationships in a simple world
#@key_label  "name"
#@meta_label "type"


#############
# BOX MODEL #
#############

[name="small_orange_box" type="geomtools::simple_shaped_model"]
shape_type        : string  = "box"   
length_unit       : string  = "cm"     
x                 : real    = 30.0     
y                 : real    = 20.0    
z                 : real    = 10.0    
material.ref      : string  = "__default__"
visibility.hidden : boolean = 0       
visibility.color  : string  = "orange"   


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


####################
# SURROUNDED MODEL #
####################

[name="surrounded_box" type="geomtools::surrounded_boxed_model"]
material.ref             : string  = "__default__"
surrounded.model         : string  = "huge_red_box"
surrounded.label         : string  = "the_central_box"
surrounded.centered_x    : boolean = 0
surrounded.centered_y    : boolean = 0
surrounded.centered_z    : boolean = 1
surrounded.bottom_model  : string  = "blue_cylinder"
surrounded.top_model     : string  = "magenta_tube"
surrounded.front_model   : string  = "small_orange_box"
visibility.hidden           : boolean   = 0
visibility.daughters.hidden : boolean = 0 

#surrounded.right_model  : string  = ""
#surrounded.top_model    : string  = ""
#surrounded.bottom_model : string  = ""
#length_unit       : string    = "mm"
#x                 : real   = 200.0
#y                 : real   = 200.0
#z                 : real   = 200.0


###############
# WORLD MODEL #
###############

[name="world" type="geomtools::simple_world_model"]
material.ref    : string = "vacuum" 
setup.model     : string = "surrounded_box"
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
world.z         : real   = 1.5
visibility.hidden           : boolean = 0        
visibility.color            : string  = "green"   
visibility.daughters.hidden : boolean = 0 
