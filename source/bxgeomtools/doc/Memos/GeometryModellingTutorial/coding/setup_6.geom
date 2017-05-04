@description List of geometry models and their relationships in a simple world
#@key_label   "name"
#@meta_label  "type"


#############
# BOX MODEL #
#############

[name="small_red_box" type="geomtools::simple_shaped_model"]
#@config the list of parameters that define a small red box
shape_type        : string = "box"   
length_unit       : string = "cm"     
x                 : real   = 30.0     
y                 : real   = 20.0    
z                 : real   = 10.0    
material.ref      : string  = "__default__"
visibility.hidden : boolean = 0       
visibility.color  : string  = "red"   


####################
# REPLICATED MODEL #
####################

[name="row" type="geomtools::replicated_boxed_model"]
replicated.axis             : string = "x"
replicated.number_of_items  : integer = 4
replicated.model            : string = "small_red_box"
replicated.label            : string = "box"
visibility.hidden_envelope  : boolean = 1
visibility.daughters.hidden : boolean = 0


####################
# REPLICATED MODEL #
####################

[name="grid" type="geomtools::replicated_boxed_model"]
replicated.axis             : string = "y"
replicated.number_of_items  : integer = 3
replicated.model            : string = "row"
replicated.label            : string = "column"
visibility.hidden_envelope  : boolean = 1
visibility.daughters.hidden : boolean = 0


###############
# WORLD MODEL #
###############

[name="world" type="geomtools::simple_world_model"]
#@config the list of parameters that define the top-level large green world box
material.ref    : string = "vacuum" 
setup.model     : string = "grid"
length_unit     : string = "m"  
setup.x         : real   = 0.0  
setup.y         : real   = 0.0   
setup.z         : real   = 0.0
angle_unit      : string = "degree" 
setup.phi       : real   = 30.0  
setup.theta     : real   = 25.0 
setup.delta     : real   = 20.0  
world.x         : real   = 1.5 
world.y         : real   = 1.5 
world.z         : real   = 1.5
visibility.hidden           : boolean = 0        
visibility.color            : string  = "green"   
visibility.daughters.hidden : boolean = 0 
