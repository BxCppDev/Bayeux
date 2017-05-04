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

[name="world" type="geomtools::simple_shaped_model"]

#@config the list of parameters that define the top-level large green world box

shape_type   : string = "box"    # must be supported by the model class
length_unit  : string = "m"      # note: we use CLHEP unit system
x            : real   = 1.5      # dimensions of the shape;
y            : real   = 1.5      #   must be coherent with the
z            : real   = 1.5      #   type above (box)
material.ref : string = "vacuum" # must be known by an external agent

#@description the names of two internal objects placed in the world volume
internal_item.labels : string[2] = "box1" "box2" # we must use here some
		                                 # distinct names

#@description the model of the first box 
internal_item.model.box1     : string  = "small_red_box" # we must use here some
                                                         # a known model name

#@description the placement of the first box in the mother reference frame
internal_item.placement.box1 : string  = "0.4 0.5 0.6 (m) @ 80.0 60.0 30.0 (degree)"
  # note here the syntax for the rotation using ZYZ Euler angle

#@description the model of the second box 
internal_item.model.box2     : string  = "small_red_box" # we must use here some
                                                         # a known model name

#@description the placement of the second box in the mother reference frame
internal_item.placement.box2 : string  = "0.2 -0.3 -0.6 (m) / z 30.0 (degree)"
  # note here the syntax for the simple rotation along the z axis

visibility.hidden           : boolean = 0       # hidden flag
visibility.color            : string  = "green" # display color
visibility.daughters.hidden : boolean = 0       # only for the daughter boxes
