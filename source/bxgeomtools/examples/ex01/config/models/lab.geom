# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of the laboratory area

#@key_label   "name"
#@meta_label  "type"

##########################################################
[name="dummy.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to configure a dummy spherical 3D shape

#@description The name of a dummy 3D shape
shape_type  : string = "sphere"

#@description The implicit length unit
length_unit : string = "mm"

#@description The implicit angle unit
angle_unit : string = "degree"

#@description The outer radius
r_max      : real = 200.0 mm

#@description The inner radius
r_min      : real = 125.0 mm

#@description The starting phi angle
start_phi    : real = 30.0 degree

#@description The delta phi angle
delta_phi    : real = 60.0 degree

#@description The starting theta angle
start_theta  : real = 20.0 degree

#@description The delta phi angle
delta_theta  : real = 70.0 degree

#@description The name of the material of the source film
material.ref : string = "air"

#@description The recommended color for the display of the source film
visibility.color            : string  = "green"

#@description The visibility hidden flag for the display of the source film
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of the source film
visibility.daughters.hidden : boolean = 0



########################################################
[name="lab.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to configure the laboratory experimental area

#@description The name of the 3D shape of the lab area
shape_type  : string = "box"

#@description The implicit length unit for the experimental setup placement
length_unit : string = "mm"

#@description The X dimension of the box
x           : real = 1900.0

#@description The Y dimension of the box
y           : real = 1900.0

#@description The Z dimension of the box
z           : real = 1900.0

#@description The name of the material that fills the lab atmosphere
material.ref : string = "air"

#@description The recommended color for the display of the lab area
visibility.color            : string  = "cyan"

#@description The visibility hidden flag for the display of the lab area
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of the lab area
visibility.daughters.hidden : boolean = 0

#@description The list of daughter volumes by labels
internal_item.labels : string[4] = \
  "vacuum_chamber" \
  "shield_wall_a" \
  "shield_wall_b" \
  "dummy"

#@description The model of the "vacuum_chamber" daughter volume
internal_item.model.vacuum_chamber     : string  = "vessel.model"

#@description The placement of the "vacuum_chamber" daughter volume
internal_item.placement.vacuum_chamber : string  = "0 0 0 (mm) / x +90 (degree)"

#@description The mapping directives for the "vacuum_chamber" daughter volume
mapping.daughter_id.vacuum_chamber : string  = "[vessel.gc:chamber=3]"

#@description The model of the "shield_wall_a" daughter volume
internal_item.model.shield_wall_a     : string  = "shielding_brick_array.model"

#@description The placement of the "shield_wall_a" daughter volume
internal_item.placement.shield_wall_a : string  = "-80 0 0 (cm) / z +90 (degree) "

#@description The mapping directives for the "vacuum_chamber" daughter volume
mapping.daughter_id.shield_wall_a : string  = "[shield_wall.gc:wall=0]"

#@description The model of the "shield_wall_b" daughter volume
internal_item.model.shield_wall_b     : string  = "shielding_brick_array.model"

#@description The placement of the "shield_wall_b" daughter volume
internal_item.placement.shield_wall_b : string  = "+80 0 0 (cm) / z +90 (degree)"

#@description The mapping directives for the "shield_wall_b" daughter volume
mapping.daughter_id.shield_wall_b : string  = "[shield_wall.gc:wall=1]"

#@description The model of the "dummy" daughter volume
internal_item.model.dummy     : string  = "dummy.model"

#@description The placement of the "dummy" daughter volume
internal_item.placement.dummy : string  = "+55 0 65 (cm)" # / z +30 (degree)"


# End of list of multi-properties.
