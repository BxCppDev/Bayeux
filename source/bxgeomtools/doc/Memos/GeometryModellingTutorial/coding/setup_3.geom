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
x                 : real   = 20.0     
y                 : real   = 25.0    
z                 : real   = 15.0    
material.ref      : string  = "__default__"
visibility.hidden : boolean = 0       
visibility.color  : string  = "red"   


##################
# CYLINDER MODEL #
##################

[name="long_blue_cylinder" type="geomtools::simple_shaped_model"]
#@config the list of parameters that define a long blue cylinder
shape_type        : string = "cylinder"  
length_unit       : string = "cm" 
r                 : real   = 10.0  
z                 : real   = 85.0  
material.ref      : string  = "__default__" # must be known by an external agent
visibility.hidden : boolean = 0        #  hidden flag
visibility.color  : string  = "blue"    #  display color


###############
# WORLD MODEL #
###############

[name="world" type="geomtools::simple_shaped_model"]
#@config the list of parameters that define the top-level large green world box
shape_type   : string = "box"    # must be supported by the model class
length_unit  : string = "m"      # note: we use CLHEP unit system
x            : real   = 1.5      # dimensions of the shape;
y            : real   = 1.5      #   must be coherent with the
z            : real   = 1.5      #   type above (box)
material.ref : string = "vacuum" # must be known by an external agent

internal_item.labels : string[3] = "box1" "box2" "cyl1" 
internal_item.model.box1     : string  = "small_red_box"
internal_item.placement.box1 : string  = "0.4 0.5 0.6 (m) @ 80.0 60.0 30.0 (degree)"
internal_item.model.box2     : string  = "small_red_box" 
internal_item.placement.box2 : string  = "0.2 -0.3 -0.6 (m) / z 30.0 (degree)"
internal_item.model.cyl1     : string  = "long_blue_cylinder" 
internal_item.placement.cyl1 : string  = "-0.4 -0.3 0.3 (m) @ 30.0 25.0 (degree)"

visibility.hidden           : boolean = 0       # hidden flag
visibility.color            : string  = "green" # display color
visibility.daughters.hidden : boolean = 0       # only for the daughter boxes
