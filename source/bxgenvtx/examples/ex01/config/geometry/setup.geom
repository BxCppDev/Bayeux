# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of the world (mother-of-all) volume

#@key_label  "name"
#@meta_label "type"


##############################################################
[name="capacitor.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to configure the capacitor logical volume

#@description The name of the 3D shape
shape_type   : string =  "cylinder"

#@description The length unit
length_unit       : string  = "mm"

#@description The R dimension
r            : real   = 2.0

#@description The Z dimension
z            : real   = 8.0

#@description The name of the material
material.ref : string = "polystyrene"

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "orange"


#########################################################
[name="chip.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to configure the capacitor logical volume

#@description The name of the 3D shape
shape_type   : string =  "box"

#@description The length unit
length_unit       : string  = "mm"

#@description The X dimension
x            : real   = 6.0

#@description The Y dimension
y            : real   = 10.0

#@description The Z dimension
z            : real   = 1.0

#@description The name of the material
material.ref : string = "silicon_dioxide"

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "blue"


#########################################################
[name="fpga.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to configure the capacitor logical volume

#@description The name of the 3D shape
shape_type   : string =  "box"

#@description The length unit
length_unit       : string  = "mm"

#@description The X dimension
x            : real   = 20.0

#@description The Y dimension
y            : real   = 24.0

#@description The Z dimension
z            : real   = 2.0

#@description The name of the material
material.ref : string = "silicon_dioxide"

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "red"


##################################################################
[name="mezzanine_pcb.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to configure the capacitor logical volume

#@description The name of the 3D shape
shape_type   : string =  "box"

#@description The length unit
length_unit  : string  = "mm"

#@description The X dimension
x            : real   = 80.0

#@description The Y dimension
y            : real   = 50.0

#@description The Z dimension
z            : real   = 2.0

#@description The name of the material
material.ref : string = "epoxy"

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "green"


##############################################################
[name="mezzanine.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to configure the capacitor logical volume

#@description The name of the 3D shape
shape_type   : string =  "box"

#@description The length unit
length_unit  : string  = "mm"

#@description The X dimension
x            : real   = 80.0

#@description The Y dimension
y            : real   = 50.0

#@description The Z dimension
z            : real   = 10.0

#@description The name of the material
material.ref : string = "air"

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "transparent"

#@description The list of daughter volumes by labels
internal_item.labels : string[6] = \
  "pcb"  \
  "fpga" \
  "chip_a" \
  "chip_b" \
  "capacitor_a" \
  "capacitor_b"

internal_item.model.pcb             : string = "mezzanine_pcb.model"
internal_item.model.fpga            : string = "fpga.model"
internal_item.model.capacitor_a     : string = "capacitor.model"
internal_item.model.capacitor_b     : string = "capacitor.model"
internal_item.model.chip_a          : string = "chip.model"
internal_item.model.chip_b          : string = "chip.model"

internal_item.placement.pcb         : string = "  0   0  -4 (mm)"
internal_item.placement.fpga        : string = "+10   0  -2 (mm)"
internal_item.placement.capacitor_a : string = "+32  10  +1 (mm)"
internal_item.placement.capacitor_b : string = "+33 -10  +1 (mm)"
internal_item.placement.chip_a      : string = "-20  10  -2.5 (mm) / z 90 (degree)"
internal_item.placement.chip_b      : string = "-20 -10  -2.5 (mm) / z 90 (degree)"

mapping.daughter_id.pcb             : string = "[mezzanine_pcb.gc]"
mapping.daughter_id.fpga            : string = "[fpga.gc]"
mapping.daughter_id.capacitor_a     : string = "[capacitor.gc:position=0]"
mapping.daughter_id.capacitor_b     : string = "[capacitor.gc:position=1]"
mapping.daughter_id.chip_a          : string = "[chip.gc:slot=0]"
mapping.daughter_id.chip_b          : string = "[chip.gc:slot=1]"


##############################################################
[name="board_pcb.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to configure the capacitor logical volume

#@description The name of the 3D shape
shape_type   : string =  "box"

#@description The length unit
length_unit  : string  = "mm"

#@description The X dimension
x            : real   = 200.0

#@description The Y dimension
y            : real   = 240.0

#@description The Z dimension
z            : real   = 2.0

#@description The name of the material
material.ref : string = "epoxy"

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "green"


##########################################################
[name="board.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to configure the capacitor logical volume

#@description The name of the 3D shape
shape_type   : string =  "box"

#@description The length unit
length_unit  : string  = "mm"

#@description The X dimension
x            : real   = 200.0

#@description The Y dimension
y            : real   = 240.0

#@description The Z dimension
z            : real   = 40.0

#@description The name of the material
material.ref : string = "air"

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "transparent"


#@description The list of daughter volumes by labels
internal_item.labels : string[5] = \
  "pcb"  \
  "daughter_board_a" \
  "daughter_board_b" \
  "daughter_board_c" \
  "daughter_board_d"

internal_item.model.pcb              : string = "board_pcb.model"
internal_item.model.daughter_board_a : string = "mezzanine.model"
internal_item.model.daughter_board_b : string = "mezzanine.model"
internal_item.model.daughter_board_c : string = "mezzanine.model"
internal_item.model.daughter_board_d : string = "mezzanine.model"

internal_item.placement.pcb              : string = "  0   0  -3 (mm)"
internal_item.placement.daughter_board_a : string = "+30  15  +5 (mm)"
internal_item.placement.daughter_board_b : string = "+30 -60  +5 (mm)"
internal_item.placement.daughter_board_c : string = "-50 -10  +5 (mm) / z 90 (deg)"
internal_item.placement.daughter_board_d : string = "+30  70  +5 (mm)"

mapping.daughter_id.pcb              : string = "[board_pcb.gc]"
mapping.daughter_id.daughter_board_a : string = "[mezzanine.gc:daughter=0]"
mapping.daughter_id.daughter_board_b : string = "[mezzanine.gc:daughter=1]"
mapping.daughter_id.daughter_board_c : string = "[mezzanine.gc:daughter=2]"
mapping.daughter_id.daughter_board_d : string = "[mezzanine.gc:daughter=10]"


###################################################
[name="world" type="geomtools::simple_world_model"]

#@config The list of properties to configure the world logical volume

#@description The name of the material that fills the world volume
material.ref    : string = "vacuum"

#@description The name of the model that represents the experimental setup
setup.model     : string = "board.model"

#@description The angular unit for the experimental setup placement
angle_unit      : string = "degree"

#@description The setup placement phi angle
setup.phi       : real   = 0.0

#@description The setup placement theta angle
setup.theta     : real   = 0.0

#@description The length unit for the experimental setup placement
length_unit     : string = "mm"

#@description The setup placement X coordinate
setup.x         : real   = 0.0

#@description The setup placement Y coordinate
setup.y         : real   = 0.0

#@description The setup placement Z coordinate
setup.z         : real   = 0.0

#@description The world volume X dimension (box)
world.x         : real   = 300.0

#@description The world volume Y dimension (box)
world.y         : real   = 300.0

#@description The world volume Z dimension (box)
world.z         : real   = 300.0

#@description The recommended color for the display of the world volume
visibility.color            : string  = "grey"

#@description The visibility hidden flag for the display of the world volume
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display
visibility.daughters.hidden : boolean = 0

#@description The mapping directives for the "setup" daughter volume
mapping.daughter_id.setup : string  = "[board.gc:board=6]"


# End of list of multi-properties.
