# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of the world (mother-of-all) volume
#@key_label   "name"
#@meta_label  "type"


###################################################
[name="world" type="geomtools::simple_world_model"]

#@config configuration parameters for the world logical volume

############
# Geometry #
############

#@description The default implicit length unit
length_unit     : string = "mm"

#@description The default implicit angular unit for the experimental setup placement
angle_unit      : string = "degree"

#@description The world volume X dimension (box)
world.x         : real   = 1000.0

#@description The world volume Y dimension (box)
world.y         : real   = 500.0

#@description The world volume Z dimension (box)
world.z         : real   = 500.0

#@description The name of the model that represents the experimental setup daughter volume
setup.model     : string = "lab.model"

#@description The setup placement phi angle
setup.phi       : real   = 0.0

#@description The setup placement theta angle
setup.theta     : real   = 0.0

#@description The setup placement X coordinate
setup.x         : real   = 0.0

#@description The setup placement Y coordinate
setup.y         : real   = 0.0

#@description The setup placement Z coordinate
setup.z         : real   = 0.0

#######################
# Material parameters #
#######################

#@description The name of the material that fills the world volume
material.ref    : string = "vacuum"

#########################
# Visibility parameters #
#########################

#@description The recommended color for the display of the world volume
visibility.color            : string  = "grey"

#@description The visibility hidden flag for the display of the world volume
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display
visibility.daughters.hidden : boolean = 0

############################################
# GID mapping of the daughter setup volume #
############################################

#@description The mapping directives for the 'setup' daughter volume
mapping.daughter_id.setup : string  = "[lab.gc:area=0]"


# End of list of multi-properties.
