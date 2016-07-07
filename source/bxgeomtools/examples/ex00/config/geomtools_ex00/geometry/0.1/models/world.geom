# -*- mode: conf-unix;  -*-
# The line  above is a  directive to  activate syntax coloring  in the
# emacs editor.  It  is not mandatory but really useful  for those who
# use this editing tool.

# This    file   use    the   ASCII    text   format    of   the    of
# "datatools::multi_properties" class.  It is  organized as a sequence
# of  sections,  each related  to  a  specific geometry  model.   Each
# section starts with a header line that identifies the model:
#
# [name="NAME OF THE GEOMETRY MODEL" type="CLASS IDENTIFIER OF THE GEOMETRY MODEL"]
#
# The  name of  the  geometry  model is  unique  with  respect to  the
# geometry manager,  even when  given in  different files.   The class
# identifier  corresponds to  the  registration name  of the  geometry
# model class in the Bayeux/geomtools library.  You can print the list
# of available  geometry models  registered in  Bayeux/geomtools using
# the OCD online manual utility:
#  shell$ bxocd_manual --action list | grep geomtools::* | grep _model
#
# At least,  this prints the  list of geometry model  classes equipped
# with the  OCD (Object Configuration Description)  feature.  The body
# of  a  geometry   model's  section  consists  in   a  collection  of
# configuration   properties  following   the  format   used  by   the
# "datatools::properties" class.

#@description The geometry model of the unique top-level world (mother-of-all) logical volume

# Mandatory meta comments that describe the expected syntax of each section header:
#@key_label  "name"
#@meta_label "type"

####################################################
[name="world" type="geomtools::simple_shaped_model"]

#@config The list of properties to configure the world logical volume

# This  geometry  model  implements  the  mandatory  top-level  volume
# (mother-of-all) Its name  is conventionnaly set to  "world". Also it
# should be  associated to the  "world" geometry category with  type 0
# (see the "lab_categories.lis" file in the "gids/" folder.

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"
# This is only used if no explicit unit is given with the x, y and z dimensions
# below.

#@description The name of the 3D shape of the world logical volume
shape_type : string = "box"

# It is  typical to  use a  3D box as  the top-level  volume. However,
# other shapes are possible (cylinder, sphere...) depending of what is
# needed.

# We must set the dimensions of the chosen shape above. In the case of
# a 3D box, 3 mandatory  parameters (dimensions) are expected.  Please
# use the  online Bayeux  OCD manual  to generate  documentation about
# supported configuration  properties for the  "geomtools::box" class:
#
#   shell$ bxocd_manual --action show geomtools::box

#@description The world volume X dimension (box)
x          : real as length = 2000.0 mm
# Here the type of physical quantity as well as the unit are explicitely set.

#@description The world volume Y dimension (box)
y          : real as length = 1500.0 mm

#@description The world volume Z dimension (box)
z          : real as length = 1500.0 mm

############
# Material #
############

#@description The name of the material that fills the lab atmosphere
material.ref : string = "vacuum"

# The identifier of the material must  match a material defined by the
# material plugin. It may be an alias defined by the user. For example
# here "vacuum" is  an alias for the  "std::vacuum" material published
# by  the "Bayeux/materials"  library  and addressed  by the  material
# plugin (see the "plugins/material_aliases.def" file).

###################
# Daughter volume #
###################

#@description Debugging the support for internal items
internal_item.logging : string = "debug"

#@description The list of daughter volumes by labels
internal_item.labels : string[31] = \
  "BoxA" "BoxB" "BoxC" "BoxD" "BoxE" \
  "ManyBoxes" "CircleBoxes" \
  "WallBricks" \
  "CylA" "CylB" "CylC" "CylD" "CylE" "CylF" \
  "CylCol0" "CylCol1a" "CylCol2a" "CylSubGrid" \
  "DetStack0" "DetStack1" "SpecialStack" "AnotherStack" \
  "Foo1" "Foo2" "Bar1" "Dummy1" "Tessella1" "Tessella2" \
  "MeshedBox0" \
  "Wall1" \
  "SIB"

# Each daughter physical  volume has an unique name  within its mother
# volume.  Also each  of them must be given: a)  a geometry model that
# specifies  the logical  volume  that represent  it,  b) a  placement
# directive (position/translation  and eventually rotation).   This is
# what is  written through the collection  of configuration properties
# below.

# The properties below, prefixed  with the "internal_item." label, are
# used as rules to position  some daughter physical volumes inside the
# logical volume (the  mother volume) handled by  the current geometry
# model. We  use the list of  identifiers given above to  identify the
# configuration properties for each daughter volume:

#@description The model of the "BoxA" daughter volume
internal_item.model.BoxA       : string  = "blue_box0.model"

# We give here the identifier/name of the geometry model associated to
# the daughter  physical volume  named "BoxA".   This model  must have
# been defined before. Thus the ordering  of ".geom" files used by the
# geometry manager is relevant.

#@description The placement of the "BoxA" daughter volume
internal_item.placement.BoxA   : string  = "-40 40 -40 (cm)"

# We give here the placement  for the "BoxA" daughter physical volume.
# There is no rotation in this case, only the translation with respect
# to the mother volume reference frame. The unit is explicitely set.

#@description The model of the "BoxB" daughter volume
internal_item.model.BoxB       : string  = "blue_box0.model"

# We give here the identifier/name of the geometry model associated to
# the daughter physical volume named "BoxB".

#@description The placement of the "BoxB" daughter volume
internal_item.placement.BoxB   : string  = "-40 30 -40 (cm) / z 30 (degree) "

# We give here the placement  for the "BoxB" daughter physical volume.
# There is not only the translation  with respect to the mother volume
# reference  frame but  also  the  rotation of  the  volume. Here  the
# rotation  operated  with respect  to  the  Z  axis.  The  units  are
# explicitely set.

#@description The model of the "BoxC" daughter volume
internal_item.model.BoxC       : string  = "blue_box0.model"

#@description The placement of the "BoxC" daughter volume
internal_item.placement.BoxC   : string  = "-40 20 -40 (cm) / y 60 (degree) "

#@description The model of the "BoxD" daughter volume
internal_item.model.BoxD       : string  = "blue_box0.model"

#@description The placement of the "BoxD" daughter volume
internal_item.placement.BoxD   : string  = "-40 10 -40 (cm) / x 20 (degree) "

#@description The model of the "BoxE" daughter volume
internal_item.model.BoxE       : string  = "blue_box0.model"

#@description The placement of the "BoxE" daughter volume
internal_item.placement.BoxE   : string  = "-40 0 -40 (cm) @ -30 25 0 (degree) "

#@description The model of the "ManyBoxes" daughter volume
internal_item.model.ManyBoxes       : string  = "box0_linear_chain.model"

#@description The placement of the "ManyBoxes" daughter volume
internal_item.placement.ManyBoxes   : string  = "-40 -25 -40 (cm) "

#@description The model of the "CircleBoxes" daughter volume
internal_item.model.CircleBoxes     : string  = "box0_circular_chain.model"

# #@description The placement of the "CircleBoxes" daughter volume
# internal_item.placement.CircleBoxes : string  = "-5 -20 -40 (cm) "
internal_item.placement.CircleBoxes.x     : real as length = 35 cm
# internal_item.placement.CircleBoxes.y     : real as length = 0 cm
internal_item.placement.CircleBoxes.z     : real as length = -40 cm
internal_item.placement.CircleBoxes.y.gap_mode     : string = "min_to_min"
internal_item.placement.CircleBoxes.y.gap_distance : real as length = 0 mm
internal_item.placement.CircleBoxes.z.gap_mode     : string = "max_to_max"
internal_item.placement.CircleBoxes.z.gap_distance : real as length = 0 mm


#@description The model of the "WallBricks" daughter volume
internal_item.model.WallBricks      : string  = "box0_yz_grid.model"

#@description The placement of the "WallBricks" daughter volume
internal_item.placement.WallBricks  : string  = "-40 0 0 (cm) "

#@description The model of the "CylA" daughter volume
internal_item.model.CylA         : string  = "blue_cylinder0.model"

#@description The placement of the "CylA" daughter volume
internal_item.placement.CylA     : string  = "-20 40 -40 (cm)"

#@description The model of the "CylB" daughter volume
internal_item.model.CylB         : string  = "blue_cylinder0.model"

#@description The placement of the "CylB" daughter volume
internal_item.placement.CylB     : string  = "0 40 -40 (cm) / x 30 (degree)"

#@description The model of the "CylC" daughter volume
internal_item.model.CylC         : string  = "blue_cylinder0.model"

#@description The placement of the "CylC" daughter volume
internal_item.placement.CylC     : string  = "20 40 -40 (cm) / x 60 (degree)"

#@description The model of the "CylD" daughter volume
internal_item.model.CylD         : string  = "blue_cylinder0.model"

#@description The placement of the "CylD" daughter volume
internal_item.placement.CylD     : string  = "0 40 -30 (cm) / x 60 (degree)"

#@description The model of the "CylE" daughter volume
internal_item.model.CylE         : string  = "blue_cylinder0.model"

#@description The placement of the "CylE" daughter volume
internal_item.placement.CylE     : string  = "20 40 -30 (cm) / x 90 (degree)"

#@description The model of the "CylF" daughter volume
internal_item.model.CylF         : string  = "blue_cylinder0.model"

#@description The placement of the "CylF" daughter volume
internal_item.placement.CylF     : string  = "20 40 -20 (cm) / x 120 (degree)"

#@description The model of the "CylCol0" daughter volume
internal_item.model.CylCol0      : string  = "cylinder0_big_column.model"

#@description The placement of the "CylCol0" daughter volume
internal_item.placement.CylCol0  : string  = "-20 40 0 (cm)"

#@description The model of the "CylCol1a" daughter volume
internal_item.model.CylCol1a     : string  = "cylinder0_medium_column.model"

#@description The placement of the "CylCol1a" daughter volume
internal_item.placement.CylCol1a : string  = "0 40 -10 (cm) / x 75 (degree)"

#@description The model of the "CylCol2a" daughter volume
internal_item.model.CylCol2a     : string  = "cylinder0_small_column.model"

#@description The placement of the "CylCol2a" daughter volume
internal_item.placement.CylCol2a : string  = "20 40 -5 (cm) / x 90 (degree)"

#@description The model of the "CylSubGrid" daughter volume
internal_item.model.CylSubGrid      : string  = "box0_small_grid.model"

#@description The placement of the "CylSubGrid" daughter volume
internal_item.placement.CylSubGrid  : string  = "10 45 20 (cm)"

#@description The model of the "DetStack0" daughter volume
internal_item.model.DetStack0      : string  = "detector_stack.model"

# #@description The placement of the "DetStack0" daughter volume
# internal_item.placement.DetStack0  : string  = "35 -20 10 (cm) / x -90 (degree)"
internal_item.placement.DetStack0.x     : real as length = 35 cm
internal_item.placement.DetStack0.y.gap_mode     : string = "min_to_min"
internal_item.placement.DetStack0.y.gap_distance : real as length = 0 mm
internal_item.placement.DetStack0.z.gap_mode     : string = "min_to_min"
internal_item.placement.DetStack0.z.gap_distance : real as length = 0 mm

#@description The model of the "DetStack1" daughter volume
internal_item.model.DetStack1      : string  = "detector_stack.model"

#@description The placement of the "DetStack1" daughter volume
internal_item.placement.DetStack1  : string  = "35 +20 10 (cm) / x 90 (degree)"

#@description The model of the "SpecialStack" daughter volume
internal_item.model.SpecialStack      : string  = "stack_with_insertion.model"

#@description The placement of the "SpecialStack" daughter volume
internal_item.placement.SpecialStack  : string  = "35 +10 +35 (cm) / x 90 (degree)"

#@description The model of the "AnotherStack" daughter volume
internal_item.model.AnotherStack      : string  = "another_stack.model"

#@description The placement of the "SpecialStack" daughter volume
internal_item.placement.AnotherStack  : string  = "35 +10 -15 (cm) / x 90 (degree)"

#@description The model of the "Foo1" daughter volume
internal_item.model.Foo1      : string  = "foo.model"

#@description The placement of the "Foo1" daughter volume
internal_item.placement.Foo1  : string  = "0 0 0 (cm)"

#@description The model of the "Foo2" daughter volume
internal_item.model.Foo2      : string  = "foo.model"

#@description The placement of the "Foo2" daughter volume
internal_item.placement.Foo2  : string  = "0 0 10 (cm)"

#@description The model of the "Bar1" daughter volume
internal_item.model.Bar1      : string  = "bar.model"

#@description The placement of the "Foo1" daughter volume
internal_item.placement.Bar1  : string  = "0 0 -10 (cm)"

#@description The model of the "Dummy1" daughter volume
internal_item.model.Dummy1      : string  = "dummy.model"

# #@description The placement of the "Dummy1" daughter volume
# internal_item.placement.Dummy1  : string  = "0 10 -100 (cm)"
internal_item.placement.Dummy1.x.gap_mode : string  = "min_to_min"
internal_item.placement.Dummy1.x.gap_distance : real as length = 0.0 mm
internal_item.placement.Dummy1.y : real as length = 10.0 cm
internal_item.placement.Dummy1.z.gap_mode : string  = "min_to_min"
internal_item.placement.Dummy1.z.gap_distance : real as length = 0.0 mm

#@description The model of the "Tessella1" daughter volume
internal_item.model.Tessella1      : string  = "tessella.model"

#@description The placement of the "Tessella1" daughter volume
internal_item.placement.Tessella1  : string  = "0 -10 -10 (cm)"

#@description The model of the "Tessella2" daughter volume
internal_item.model.Tessella2      : string  = "tessella2.model"

#@description The placement of the "Tessella2" daughter volume
internal_item.placement.Tessella2  : string  = "-10 0 -10 (cm)"

#@description The model of the "MeshedBox0" daughter volume
internal_item.model.MeshedBox0      : string  = "large_green_box.model"

# #@description The placement of the "MeshedBox0" daughter volume
# internal_item.placement.MeshedBox0  : string  = "+35 0 -40 (cm)"
internal_item.placement.MeshedBox0.x     : real as length = 35 cm
internal_item.placement.MeshedBox0.y     : real as length = 0 cm
internal_item.placement.MeshedBox0.z.gap_mode     : string = "min_to_min"
internal_item.placement.MeshedBox0.z.gap_distance : real as length = 0 mm

#####################################################################

#@description Debugging the "Wall1" daughter volume
internal_item.logging.Wall1    : string  = "debug"

#@description The model of the "Wall1" daughter volume
internal_item.model.Wall1      : string  = "wall1.model"

# #@description The placement of the "Wall1" daughter volume
# internal_item.placement.Wall1  : string  = "0 20 -40 (cm) / z 90 (degree)"

#internal_item.placement.Wall1.x : real as length = 0 mm
internal_item.placement.Wall1.x.gap_mode     : string = "min_to_min"
internal_item.placement.Wall1.x.gap_distance : real as length = 20 mm

internal_item.placement.Wall1.y.gap_mode     : string = "center_to_min"
internal_item.placement.Wall1.y.gap_distance : real as length = 30 cm

# internal_item.placement.Wall1.z.gap_mode     : string = "min_to_min"
internal_item.placement.Wall1.z.gap_mode     : string = "max_to_max"
internal_item.placement.Wall1.z.gap_distance : real as length = 0 mm

#####################################################################

#@description The model of the "SIB" daughter volume
internal_item.model.SIB      : string  = "SIB1.model"

#@description Debugging the "SIB" daughter volume
internal_item.logging.SIB    : string  = "debug"

# #@description The placement of the "SIB" daughter volume
# internal_item.placement.SIB  : string  = "600 600 -600 (mm)"

# internal_item.placement.SIB.x : real as length = 600 mm
internal_item.placement.SIB.x.gap_mode    : string = "max_to_max"
internal_item.placement.SIB.x.gap_distance: real as length = 50 mm

internal_item.placement.SIB.y : real as length =    0 mm

# internal_item.placement.SIB.z : real as length = -600 mm
# internal_item.placement.SIB.z.gap_mode     : string = "min_to_min"
internal_item.placement.SIB.z.gap_mode     : string = "max_to_max"
internal_item.placement.SIB.z.gap_distance : real as length = 0 mm

# internal_item.placement.SIB.rotation_axis  : string        = "z"
# internal_item.placement.SIB.rotation_angle : real as angle = 45 degree

###########
# Mapping #
###########

# The so-called mapping directives, prefixed with the "mapping." label
# are used  as rules to  automatically build the  geometry identifiers
# (GIDs)  associated to  physical volumes  in the  geometry hierarchy.
# Each daughter volume  an unique GID should be associated  to must be
# passed  some  mapping directive  that  will  be interpreted  by  the
# mapping manager.   Basically, we must  specify what is  the geometry
# category the daughter volume belongs to  and how its address must be
# constructed  as an  unique sequence  of integers  in the  considered
# category. Note also  that it is not mandatory to  associate a GID to
# each  daughter volume.   The user  is  invited to  publish only  the
# mapping  rules  for the  daughter  volume  he/she is  interested  in
# (examples: source  of particles  in a  setup used  by a  Monte Carlo
# application, detection units...).

# There  is  two ways  to  specify  the  mapping rules  associated  to
# daughter volumes within a given volume (model):
#  a) we  may directly embed the  "mapping.daughter_id.X" properties in
#   the configuration section of the  geometry model in the ".geom"
#   file, where "X" is the name of the daughter volume.
#  b) we  may used  some external  files to  record the  mapping rules
#     (using    the    same    syntax    as    in    a).    See    the
#     "external_mapping_rules" property in the main configuration file
#     of the geometry manager.

# If  you plan  to use  technique a)  mentionned above,  the following
# lines typically  correspond to  the mapping directives  for daughter
# objects defined in the "world" volume.

# Here all the daughter volumes  using the "box0.model" geometry model
# that have been set in the  "world" volume -- directly as unique item
# or  indirectly   through  dedicated   *replicated*  models   --  are
# considered  to be  classified (and  labelled) in  the same  geometry
# category:  "box.gc".   Other  daughter  volumes  are  classified  in
# other geometry categories ("any_node.gc", "object.gc"...).

# #@description The mapping directives for the "BoxA" daughter volume
# mapping.daughter_id.BoxA : string  = "[box.gc:position=0]"

# #@description The mapping directives for the "BoxB" daughter volume
# mapping.daughter_id.BoxB : string  = "[box.gc:position=1]"

# #@description The mapping directives for the "BoxC" daughter volume
# mapping.daughter_id.BoxC : string  = "[box.gc:position=2]"

# #@description The mapping directives for the "BoxD" daughter volume
# mapping.daughter_id.BoxD : string  = "[box.gc:position=3]"

# #@description The mapping directives for the "BoxE" daughter volume
# mapping.daughter_id.BoxE : string  = "[box.gc:position=4]"

# #@description The mapping directives for the "ManyBoxes" daughter volume
# mapping.daughter_id.ManyBoxes : string  = "[box.gc:position+5]"

# #@description The mapping directives for the "CircleBoxes" daughter volume
# mapping.daughter_id.CircleBoxes : string  = "[box.gc:position+9]"

# #@description The mapping directives for the "CylA" daughter volume
# mapping.daughter_id.CylA : string  = "[any_node.gc:column=0,row=0]"

# #@description The mapping directives for the "CylB" daughter volume
# mapping.daughter_id.CylB : string  = "[any_node.gc:column=1,row=0]"

# #@description The mapping directives for the "CylC" daughter volume
# mapping.daughter_id.CylC : string  = "[any_node.gc:column=2,row=0]"

# #@description The mapping directives for the "CylD" daughter volume
# mapping.daughter_id.CylD : string  = "[any_node.gc:column=1,row=1]"

# #@description The mapping directives for the "CylE" daughter volume
# mapping.daughter_id.CylE : string  = "[any_node.gc:column=2,row=1]"

# #@description The mapping directives for the "CylF" daughter volume
# mapping.daughter_id.CylF : string  = "[any_node.gc:column=2,row=2]"

# #@description The mapping directives for the "CylCol0" daughter volume
# mapping.daughter_id.CylCol0  : string = "[any_column.gc:column=0]"

# #@description The mapping directives for the "CylCol1a" daughter volume
# mapping.daughter_id.CylCol1a : string = "[any_column.gc:column=1]"

# #@description The mapping directives for the "CylCol2a" daughter volume
# mapping.daughter_id.CylCol2a : string = "[any_column.gc:column=2]"

# #@description The mapping directives for the "DetStack0" daughter volume
# mapping.daughter_id.DetStack0 : string = "[detector.gc:unit=0]"

# #@description The mapping directives for the "DetStack1" daughter volume
# mapping.daughter_id.DetStack1 : string = "[detector.gc:unit=1]"

# #@description The mapping directives for the "Tessella1" daughter volume
# mapping.daughter_id.Tessella1 : string = "[object.gc:item=0]"

# #@description The mapping directives for the "Tessella2" daughter volume
# mapping.daughter_id.Tessella2 : string = "[object.gc:item=1]"

##############
# Visibility #
##############

#@description The recommended color for the display of the world volume
visibility.color            : string  = "cyan"

#@description The daughters' visibility hidden flag for the display
visibility.daughters.hidden : boolean = false
