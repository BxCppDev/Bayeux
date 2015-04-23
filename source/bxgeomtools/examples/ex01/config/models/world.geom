# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of the world (mother-of-all) volume 

#@key_label   "name"
#@meta_label  "type"


###################################################
[name="world" type="geomtools::simple_world_model"]

#@config The list of properties to configure the world logical volume

#@description The name of the material that fills the world volume
material.ref    : string = "vacuum"

#@description The name of the model that represents the experimental setup
setup.model     : string = "lab.model" 

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
world.x         : real   = 2000.0

#@description The world volume Y dimension (box)
world.y         : real   = 2000.0

#@description The world volume Z dimension (box)
world.z         : real   = 2000.0

#@description The recommended color for the display of the world volume
visibility.color            : string  = "grey"

#@description The visibility hidden flag for the display of the world volume
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display
visibility.daughters.hidden : boolean = 0

#@description The mapping directives for the "setup" daughter volume
mapping.daughter_id.setup : string  = "[lab.gc:area=0]"


# End of list of multi-properties.
