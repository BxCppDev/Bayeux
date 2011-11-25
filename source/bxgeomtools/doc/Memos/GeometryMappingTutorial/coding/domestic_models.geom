# -*- mode: conf-unix; -*-
# domestic_models.geom

#@description List of domestic geometry models 
#@key_label   "name"
#@meta_label  "type"

[name="small_drawer.model" type="geomtools::simple_shaped_model"]
shape_type   : string =  "box"
x            : real   = 50.0
y            : real   = 30.0
z            : real   = 10.0
length_unit  : string = "cm"
material.ref : string  = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "magenta"


[name="table_leg.model" type="geomtools::simple_shaped_model"]
shape_type   : string =  "box"
x            : real   =  5.0
y            : real   =  5.0
z            : real   = 80.0
length_unit  : string = "cm"
material.ref : string  = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "red"


[name="table_plateau.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "box"
x            : real   = 100.0
y            : real   = 100.0
z            : real   = 5.0
length_unit  : string = "cm"
material.ref : string  = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "red"


[name="table.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "box"
x            : real   = 100.0
y            : real   = 100.0
z            : real   = 100.0
length_unit  : string = "cm"
material.ref : string  = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "grey"

internal_item.labels : string[6] = \
		         "leg_0" "leg_1" "leg_2" "leg_3" \
                         "plateau" \
                         "drawer"

internal_item.model.leg_0       : string  = "table_leg.model"
internal_item.placement.leg_0   : string  = "+40 +40 -10 (cm)"

internal_item.model.leg_1       : string  = "table_leg.model"
internal_item.placement.leg_1   : string  = "+40 -40 -10 (cm)"

internal_item.model.leg_2       : string  = "table_leg.model"
internal_item.placement.leg_2   : string  = "-40 -40 -10 (cm)"

internal_item.model.leg_3       : string  = "table_leg.model"
internal_item.placement.leg_3   : string  = "-40 +40 -10 (cm)"

internal_item.model.plateau     : string  = "table_plateau.model"
internal_item.placement.plateau : string  = "0 0 +32.5 (cm)"

internal_item.model.drawer      : string  = "small_drawer.model"
internal_item.placement.drawer  : string  = "0 +30 +25 (cm)"

[name="room_0.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 4.5
y                 : real   = 3.0
z                 : real   = 3.5
length_unit       : string = "m"
material.ref      : string = "air"

visibility.hidden : boolean = 0
visibility.color  : string  = "cyan"

[name="room_0_with_tables.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 4.5
y                 : real   = 3.0
z                 : real   = 3.5
length_unit       : string = "m"
material.ref      : string = "air"

visibility.hidden : boolean = 0
visibility.color  : string  = "cyan"

internal_item.labels : string[2] = "table_U" "table_V"

internal_item.model.table_U       : string  = "table.model"
internal_item.placement.table_U   : string  = "-0.5 0 -1.25 (m)"

internal_item.model.table_V      : string  = "table.model"
internal_item.placement.table_V   : string  = "1.0 0 -1.25 (m)"

[name="room_1.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 2.0
y                 : real   = 3.0
z                 : real   = 2.5
length_unit       : string = "m"
material.ref      : string = "air"

visibility.hidden : boolean = 0
visibility.color  : string  = "cyan"

[name="floor_0.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 10.0
y                 : real   = 4.0
z                 : real   = 4.5
length_unit       : string = "m"
material.ref      : string = "concrete"

visibility.hidden : boolean = 0
visibility.color  : string  = "blue"

internal_item.labels : string[2] = "room_Y" "room_Z"

internal_item.model.room_Y       : string  = "room_0.model"
internal_item.placement.room_Y   : string  = "-2.5 0 -0.5 (m)"

internal_item.model.room_Z       : string  = "room_0_with_tables.model"
internal_item.placement.room_Z   : string  = "+2.5 0 -0.5 (m)"

[name="floor_1.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 10.0
y                 : real   = 4.0
z                 : real   = 3.0
length_unit       : string = "m"
material.ref      : string = "concrete"

visibility.hidden : boolean = 0
visibility.color  : string  = "blue"

internal_item.labels : string[4] = "room_A" "room_B" "room_C" "room_D"

internal_item.model.room_A       : string  = "room_1.model"
internal_item.placement.room_A   : string  = "-3.75 0 -0.25 (m)"

internal_item.model.room_B       : string  = "room_1.model"
internal_item.placement.room_B   : string  = "-1.25 0 -0.25 (m)"

internal_item.model.room_C       : string  = "room_1.model"
internal_item.placement.room_C   : string  = "+1.25 0 -0.25 (m)"

internal_item.model.room_D       : string  = "room_1.model"
internal_item.placement.room_D   : string  = "+3.75 0 -0.25 (m)"

[name="house.model" type="geomtools::stacked_model"]
length_unit       : string = "m"
material.ref      : string = "concrete"

visibility.hidden : boolean = 0
visibility.color  : string  = "green"

stacked.axis            : string  = "z"
stacked.number_of_items : integer = 2
stacked.label_1         : string  = "second_floor"
stacked.model_1         : string  = "floor_1.model"
stacked.label_0         : string  = "ground_floor"
stacked.model_0         : string  = "floor_0.model"
stacked.limit_max_0     : real    = +2.5

[name="world" type="geomtools::simple_world_model"]
material.ref    : string = "vacuum"
setup.model     : string = "house.model" 

angle_unit      : string = "degree"
setup.phi       : real   = 0.0
setup.theta     : real   = 0.0

length_unit     : string = "m"
setup.x         : real   = 0.0
setup.y         : real   = 0.0
setup.z         : real   = 0.0
world.x         : real   = 14.0
world.y         : real   = 10.0
world.z         : real   = 14.0

visibility.hidden           : boolean = 0
visibility.daughters.hidden : boolean = 0

# end of domestic_models.geom