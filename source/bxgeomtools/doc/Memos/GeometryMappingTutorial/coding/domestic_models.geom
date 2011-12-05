# -*- mode: conf-unix; -*-
# domestic_models.geom

#@description List of domestic geometry models 
#@key_label   "name"
#@meta_label  "type"

###################################################################

[name="chair_leg.model" type="geomtools::simple_shaped_model"]
shape_type   : string =  "box"
x            : real   =  5.0
y            : real   =  5.0
z            : real   = 50.0
length_unit  : string = "cm"
material.ref : string  = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "red"

[name="chair_back.model" type="geomtools::simple_shaped_model"]
shape_type   : string =  "box"
x            : real   =  5.0
y            : real   = 50.0
z            : real   = 50.0
length_unit  : string = "cm"
material.ref : string  = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "red"

[name="chair_seat.model" type="geomtools::simple_shaped_model"]
shape_type   : string =  "box"
x            : real   = 50.0
y            : real   = 50.0
z            : real   = 10.0
length_unit  : string = "cm"
material.ref : string  = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "red"

[name="chair.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "box"
x            : real   = 50.0
y            : real   = 50.0
z            : real   = 110.0
length_unit  : string = "cm"
material.ref : string  = "air"

visibility.hidden : boolean = 0
visibility.color  : string  = "grey"

internal_item.labels : string[6] = \
		         "leg_0" "leg_1" "leg_2" "leg_3" \
                         "seat" \
                         "back"

internal_item.model.leg_0       : string  = "chair_leg.model"
internal_item.placement.leg_0   : string  = "+22.5 +22.5 -30 (cm)"

internal_item.model.leg_1       : string  = "chair_leg.model"
internal_item.placement.leg_1   : string  = "+22.5 -22.5 -30 (cm)"

internal_item.model.leg_2       : string  = "chair_leg.model"
internal_item.placement.leg_2   : string  = "-22.5 -22.5 -30 (cm)"

internal_item.model.leg_3       : string  = "chair_leg.model"
internal_item.placement.leg_3   : string  = "-22.5 +22.5 -30 (cm)"

internal_item.model.seat        : string  = "chair_seat.model"
internal_item.placement.seat    : string  = "0 0 0 (cm)"

internal_item.model.back        : string  = "chair_back.model"
internal_item.placement.back    : string  = "-22.5 0 +30 (cm)"


###################################################################

[name="drawer_handle.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "cylinder"
r            : real   = 2.0
z            : real   = 5.0
length_unit  : string = "cm"
material.ref : string = "aluminium"

visibility.hidden : boolean = 0
visibility.color  : string  = "magenta"

[name="small_drawer_box.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "box"
x            : real   = 40.0
y            : real   = 50.0
z            : real   = 10.0
length_unit  : string = "cm"
material.ref : string = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "magenta"

[name="small_drawer.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "box"
x            : real   = 45.0
y            : real   = 50.0
z            : real   = 10.0
length_unit  : string = "cm"
material.ref : string = "air"

visibility.hidden : boolean = 0
visibility.color  : string  = "grey"

internal_item.labels           : string[2] = "box" "handle"
internal_item.model.box        : string    = "small_drawer_box.model"
internal_item.placement.box    : string    = "-2.5 0 0 (cm)"
internal_item.model.handle     : string    = "drawer_handle.model"
internal_item.placement.handle : string    = "20 0 0 (cm) @ 0 90 (degree)"

###################################################################

[name="bed_blanket.model" type="geomtools::simple_shaped_model"]
shape_type   : string =  "box"
x            : real   = 170.0
y            : real   = 85.0
z            : real   = 10.0
length_unit  : string = "cm"
material.ref : string  = "whool"

visibility.hidden : boolean = 0
visibility.color  : string  = "green"

###################################################################

[name="bed_leg.model" type="geomtools::simple_shaped_model"]
shape_type   : string =  "box"
x            : real   = 10.0
y            : real   = 10.0
z            : real   = 100.0
length_unit  : string = "cm"
material.ref : string  = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "red"

[name="bed_body.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "box"
x            : real   = 200.0
y            : real   = 90.0
z            : real   = 30.0
length_unit  : string = "cm"
material.ref : string = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "red"

[name="coussin.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "cylinder"
r            : real   = 10.0
z            : real   = 85.0
length_unit  : string = "cm"
material.ref : string = "cotton"

visibility.hidden : boolean = 0
visibility.color  : string  = "green"

[name="bed.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "box"
x            : real   = 210.0
y            : real   = 110.0
z            : real   = 100.0
length_unit  : string = "cm"
material.ref : string  = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "grey"

internal_item.labels : string[7] = \
		         "leg_0" "leg_1" "leg_2" "leg_3" \
                         "body" "coussin" "blanket"

internal_item.model.leg_0       : string  = "bed_leg.model"
internal_item.placement.leg_0   : string  = "+100 +50 0 (cm)"

internal_item.model.leg_1       : string  = "bed_leg.model"
internal_item.placement.leg_1   : string  = "+100 -50 0 (cm)"

internal_item.model.leg_2       : string  = "bed_leg.model"
internal_item.placement.leg_2   : string  = "-100 -50 0 (cm)"

internal_item.model.leg_3       : string  = "bed_leg.model"
internal_item.placement.leg_3   : string  = "-100 +50 0 (cm)"

internal_item.model.body        : string  = "bed_body.model"
internal_item.placement.body    : string  = "0 0 -10 (cm)"

internal_item.model.coussin        : string  = "coussin.model"
internal_item.placement.coussin    : string  = "+85 0 +15 (cm) @ 90 90 (degree)"

internal_item.model.blanket        : string  = "bed_blanket.model"
internal_item.placement.blanket    : string  = "-15 0 +10 (cm)"

[name="twin_bed.model" type="geomtools::replicated_boxed_model"]
replicated.axis            : string  = "z"
replicated.number_of_items : integer = 2
replicated.model           : string  = "bed.model"
replicated.label           : string  = "stacked_bed"

visibility.hidden : boolean = 0
visibility.color  : string  = "grey"

mapping.daughter_id.stacked_bed : string = "[bed:bed_number+0]"


###################################################################

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
                         "drawer" \
		         "leg_0" "leg_1" "leg_2" "leg_3" \
                         "plateau" 

internal_item.model.drawer      : string  = "small_drawer.model"
internal_item.placement.drawer  : string  = "+27.5 0 +25 (cm)"

mapping.daughter_id.drawer : string = "[small_drawer]"

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



###################################################################

[name="large_drawer_box.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "box"
x            : real   = 45.0
y            : real   = 60.0
z            : real   = 20.0
length_unit  : string = "cm"
material.ref : string = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "magenta"

[name="large_drawer.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "box"
x            : real   = 50.0
y            : real   = 60.0
z            : real   = 20.0
length_unit  : string = "cm"
material.ref : string = "air"

visibility.hidden : boolean = 0
visibility.color  : string  = "grey"

internal_item.labels           : string[2] = "box" "handle"
internal_item.model.box        : string    = "large_drawer_box.model"
internal_item.placement.box    : string    = "-2.5 0 0 (cm)"
internal_item.model.handle     : string    = "drawer_handle.model"
internal_item.placement.handle : string    = "22.5 0 0 (cm) @ 0 90 (degree)"

###################################################################

[name="cupboard_body.model" type="geomtools::simple_shaped_model"]
shape_type   : string =  "box"
x            : real   =  50.0
y            : real   =  80.0
z            : real   = 100.0
length_unit  : string = "cm"
material.ref : string = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "red"


[name="cupboard_plateau.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "box"
x            : real   = 60.0
y            : real   = 90.0
z            : real   = 5.0
length_unit  : string = "cm"
material.ref : string = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "red"


[name="cupboard.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "box"
x            : real   = 60.0
y            : real   = 90.0
z            : real   = 100.0
length_unit  : string = "cm"
material.ref : string  = "wood"

visibility.hidden : boolean = 0
visibility.color  : string  = "grey"

internal_item.labels : string[6] =  \
		         "body"     \
                         "plateau"  \
                         "drawer_0" \
                         "drawer_1" \
                         "drawer_2" \
                         "drawer_3"

internal_item.model.body        : string  = "cupboard_body.model"
internal_item.placement.body    : string  = "0 0 0 (cm)"

internal_item.model.plateau     : string  = "cupboard_plateau.model"
internal_item.placement.plateau : string  = "0 0 +52.5 (cm)"

internal_item.model.drawer_0      : string  = "large_drawer.model"
internal_item.placement.drawer_0  : string  = "5 0 -37.5 (cm)"

internal_item.model.drawer_1      : string  = "large_drawer.model"
internal_item.placement.drawer_1  : string  = "5 0 -12.5 (cm)"

internal_item.model.drawer_2      : string  = "large_drawer.model"
internal_item.placement.drawer_2  : string  = "5 0 +12.5 (cm)"

internal_item.model.drawer_3      : string  = "large_drawer.model"
internal_item.placement.drawer_3  : string  = "5 0 +37.5 (cm)"

mapping.daughter_id.drawer_0 : string = "[large_drawer:drawer_number=0]"
mapping.daughter_id.drawer_1 : string = "[large_drawer:drawer_number=1]"
mapping.daughter_id.drawer_2 : string = "[large_drawer:drawer_number=2]"
mapping.daughter_id.drawer_3 : string = "[large_drawer:drawer_number=3]"

###################################################################

[name="mail_box.model" type="geomtools::simple_shaped_model"]
shape_type   : string = "box"
x            : real   = 25.0
y            : real   = 35.0
z            : real   =  5.0
length_unit  : string = "cm"
material.ref : string = "aluminium"

visibility.hidden : boolean = 0
visibility.color  : string  = "blue"

[name="large_mail_column.model" type="geomtools::replicated_boxed_model"]
replicated.axis            : string  = "z"
replicated.number_of_items : integer = 13
replicated.model           : string  = "mail_box.model"
replicated.label           : string  = "mail_box"

visibility.hidden : boolean = 0
visibility.color  : string  = "grey"

mapping.daughter_id.mail_box : string = "[mailrow:row+0]"

[name="small_mailbox_column.model" type="geomtools::replicated_boxed_model"]
replicated.axis            : string  = "z"
replicated.number_of_items : integer = 5
replicated.model           : string  = "mail_box.model"
replicated.label           : string  = "mail_box"

visibility.hidden : boolean = 0
visibility.color  : string  = "grey"

mapping.daughter_id.mail_box : string = "[mailrow:row+0]"

[name="large_mailbox.model" type="geomtools::replicated_boxed_model"]
replicated.axis            : string  = "y"
replicated.number_of_items : integer = 4
replicated.model           : string  = "large_mail_column.model"
replicated.label           : string  = "mail_column"

visibility.hidden : boolean = 0
visibility.color  : string  = "grey"

mapping.daughter_id.mail_column : string = "[mailcolumn:column+0]"

[name="small_mailbox.model" type="geomtools::replicated_boxed_model"]
replicated.axis            : string  = "y"
replicated.number_of_items : integer = 2
replicated.model           : string  = "small_mailbox_column.model"
replicated.label           : string  = "mail_column"

visibility.hidden : boolean = 0
visibility.color  : string  = "grey"

mapping.daughter_id.mail_column : string = "[mailcolumn:column+0]"

###################################################################

[name="room_0.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 4.5
y                 : real   = 3.0
z                 : real   = 3.5
length_unit       : string = "m"
material.ref      : string = "air"

visibility.hidden : boolean = 0
visibility.color  : string  = "cyan"

internal_item.labels : string[4] = \
  "beds_a" "cupboard_a" "table_a" "chair_a"

internal_item.model.beds_a      : string  = "twin_bed.model"
internal_item.placement.beds_a  : string  = "-1.70 -0.45 -0.75 (m) / z 270 (degree)"

internal_item.model.cupboard_a     : string  = "cupboard.model"
internal_item.placement.cupboard_a : string  = "+1.95 +0.75 -1.2 (m) / z 180 (degree)"

internal_item.model.table_a     : string  = "table.model"
internal_item.placement.table_a  : string  = "0.5 -1.0 -1.25 (m) / z 90 (degree)"

internal_item.model.chair_a      : string  = "chair.model"
internal_item.placement.chair_a  : string  = "0.5 -0.25 -1.2 (m) / z 270 (degree)"

#mapping.daughter_id.cupboard_X : string = "[cupboard:cupboard_number=0]"
mapping.daughter_id.cupboard_a : string = "[cupboard:cupboard_number=0]"

[name="room_0_with_tables.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 4.5
y                 : real   = 3.0
z                 : real   = 3.5
length_unit       : string = "m"
material.ref      : string = "air"

visibility.hidden : boolean = 0
visibility.color  : string  = "cyan"

internal_item.labels : string[9] = \
  "table_U" \
  "table_V" \
  "chair_A" \
  "chair_B" \
  "chair_C" \
  "chair_D" \
  "cupboard_X" \
  "mailbox_Y" \
  "mailbox_Z"

internal_item.model.table_U      : string  = "table.model"
internal_item.placement.table_U  : string  = "-0.5 -0.75 -1.25 (m) / z 180 (degree)"

internal_item.model.table_V      : string  = "table.model"
internal_item.placement.table_V  : string  = "0.75 +0.75 -1.25 (m)"

internal_item.model.chair_A      : string  = "chair.model"
internal_item.placement.chair_A  : string  = "-1.5 -0.75 -1.2 (m)"

internal_item.model.chair_B      : string  = "chair.model"
internal_item.placement.chair_B  : string  = "+1.75 +0.75 -1.2 (m) / z 180 (degree)"

internal_item.model.chair_C      : string  = "chair.model"
internal_item.placement.chair_C  : string  = "-0.5 +1.0 -1.2 (m)"

internal_item.model.chair_D      : string  = "chair.model"
internal_item.placement.chair_D  : string  = "-0.5 +0.25 -1.2 (m)"

internal_item.model.cupboard_X     : string  = "cupboard.model"
internal_item.placement.cupboard_X : string  = "-1.95 +0.9 -1.2 (m)"

internal_item.model.mailbox_Y      : string  = "large_mailbox.model"
internal_item.placement.mailbox_Y  : string  = "-2.125 -0.75 +0.5 (m)"

internal_item.model.mailbox_Z      : string  = "small_mailbox.model"
internal_item.placement.mailbox_Z  : string  = "1. 1.375 +0.35 (m)/ z 90 (degree)"

mapping.daughter_id.table_U : string = "[table:table_number=0]"
mapping.daughter_id.table_V : string = "[table:table_number=1]"
mapping.daughter_id.chair_A : string = "[chair:chair_number=0]"
mapping.daughter_id.chair_B : string = "[chair:chair_number=1]"
mapping.daughter_id.chair_C : string = "[chair:chair_number=2]"
mapping.daughter_id.chair_D : string = "[chair:chair_number=3]"
mapping.daughter_id.cupboard_X : string = "[cupboard:cupboard_number=0]"
mapping.daughter_id.mailbox_Y  : string = "[mailbox:mailbox_number=9]"
mapping.daughter_id.mailbox_Z  : string = "[mailbox:mailbox_number=7]"

[name="room_1.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 2.0
y                 : real   = 3.0
z                 : real   = 2.5
length_unit       : string = "m"
material.ref      : string = "air"

visibility.hidden : boolean = 0
visibility.color  : string  = "cyan"

internal_item.labels : string[2] = \
  "cupboard_a" \
  "bed_a" 

internal_item.model.cupboard_a    : string  = "cupboard.model"
internal_item.placement.cupboard_a : string  = "-0.70 -0.8 -0.75 (m)"

internal_item.model.bed_a      : string  = "bed.model"
internal_item.placement.bed_a  : string  = "+0.45 0.45 -0.750 (m) / z 90 (degree)"

mapping.daughter_id.bed_a   : string = "[bed:bed_number=0]"
mapping.daughter_id.cupboard_a : string = "[cupboard:cupboard_number=0]"

[name="room_1_with_bed.model" type="geomtools::simple_shaped_model"]
shape_type        : string = "box"
x                 : real   = 2.0
y                 : real   = 3.0
z                 : real   = 2.5
length_unit       : string = "m"
material.ref      : string = "air"

visibility.hidden : boolean = 0
visibility.color  : string  = "cyan"

internal_item.labels : string[2] = "bed_a" "chair_a" 

internal_item.model.bed_a      : string  = "bed.model"
internal_item.placement.bed_a  : string  = "-0.45 0.45 -0.750 (m) / z 90 (degree)"

internal_item.model.chair_a      : string  = "chair.model"
internal_item.placement.chair_a  : string  = "+0.6 1.25 -0.70 (m) / z 270 (degree)"

mapping.daughter_id.bed_a   : string = "[bed:bed_number=0]"
mapping.daughter_id.chair_a : string = "[chair:chair_number=0]"

#######################################################################

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

mapping.daughter_id.room_Y : string = "[room:room_number=0]"
mapping.daughter_id.room_Z : string = "[room:room_number=1]"

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

internal_item.model.room_B       : string  = "room_1_with_bed.model"
internal_item.placement.room_B   : string  = "-1.25 0 -0.25 (m)"

internal_item.model.room_C       : string  = "room_1.model"
internal_item.placement.room_C   : string  = "+1.25 0 -0.25 (m)"

internal_item.model.room_D       : string  = "room_1_with_bed.model"
internal_item.placement.room_D   : string  = "+3.75 0 -0.25 (m) / z 180 (degree)"

mapping.daughter_id.room_A : string = "[room:room_number=0]"
mapping.daughter_id.room_B : string = "[room:room_number=1]"
mapping.daughter_id.room_C : string = "[room:room_number=2]"
mapping.daughter_id.room_D : string = "[room:room_number=3]"

[name="house.model" type="geomtools::stacked_model"]
length_unit       : string = "m"
material.ref      : string = "concrete"

visibility.hidden : boolean = 0
visibility.color  : string  = "green"

stacked.axis            : string  = "z"
stacked.number_of_items : integer = 2
stacked.label_1         : string  = "first_floor"
stacked.model_1         : string  = "floor_1.model"
stacked.label_0         : string  = "ground_floor"
stacked.model_0         : string  = "floor_0.model"

mapping.daughter_id.ground_floor : string = "[floor:floor_number=0]"
mapping.daughter_id.first_floor  : string = "[floor:floor_number=1]"

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

mapping.daughter_id.setup : string = "[house:house_number=666]"

# end of domestic_models.geom