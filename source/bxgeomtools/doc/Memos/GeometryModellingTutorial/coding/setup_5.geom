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
y                 : real   = 45.0    
z                 : real   = 15.0    
material.ref      : string  = "__default__"
visibility.hidden : boolean = 0       
visibility.color  : string  = "red"   


#################
# ROTATED MODEL #
#################

[name="rotated_small_red_box" type="geomtools::rotated_boxed_model"]
#@config the list of parameters that define a rotated box
rotated.axis  : string = "z"       # possible axis : "x" "y" "z"
angle_unit    : string = "degree"
rotated.angle : real   = 35.0
length_unit   : string = "cm"
x             : real   = 50.0
y             : real   = 50.0
rotated.model : string = "small_red_box"
rotated.label : string = "twisty"
visibility.hidden_envelope  : boolean = 0
visibility.daughters.hidden : boolean = 0
visibility.color            : string  = "magenta"   


#########################
# SPECIAL ROTATED MODEL #
#########################

[name="special_rotated_small_red_box" type="geomtools::rotated_boxed_model"]
#@config the list of parameters that define a rotated box
rotated.axis  : string = "x"
angle_unit    : string = "degree"
rotated.special_angle : string = "90" # possible values : "0" "90" "180" "270"
length_unit   : string = "mm"
rotated.model : string = "small_red_box"
rotated.label : string = "twisty"
visibility.hidden_envelope  : boolean = 0
visibility.daughters.hidden : boolean = 0


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
internal_item.labels         : string[3] =  "box1" "box2" "box3" 
internal_item.model.box1     : string    = "small_red_box" 
internal_item.placement.box1 : string    = " 0.5 0.0 0.0 (m)"
internal_item.model.box2     : string    = "rotated_small_red_box"
internal_item.placement.box2 : string    = " 0.0 0.0 0.0 (m)"
internal_item.model.box3     : string    = "special_rotated_small_red_box"
internal_item.placement.box3 : string    = "-0.5 0.0 0.0 (m)"
visibility.hidden            : boolean   = 0       
visibility.color             : string    = "green"
visibility.daughters.hidden  : boolean   = 0     
