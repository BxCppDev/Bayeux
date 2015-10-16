# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of the world (mother-of-all) volume
#@key_label  "name"
#@meta_label "type"


####################################################
[name="world" type="geomtools::simple_shaped_model"]

#@config configuration parameters for the world logical volume

############
# Geometry #
############

#@description The default length unit
length_unit : string = "mm"

#@description The default implicit angular unit
angle_unit : string = "degree"

#@description The name of the 3D shape of the world logical volume
shape_type : string = "box"

#@
#@description The world volume X dimension (box)
x : real as length = 2.0 m

#@description The world volume Y dimension (box)
y : real as length = 2.0 m

#@description The world volume Z dimension (box)
z : real as length = 2.0 m

############
# Material #
############

#@description The name of the material that fills the world volume
material.ref : string = "vacuum"

#########################
# Visibility parameters #
#########################

#@description The recommended color for the display of the volume
visibility.color            : string  = "grey"

#@description The recommended opacity for the display of the volume
visibility.opacity          : real  = 0.0

#@description The visible edge flag for the display of the volume
visibility.visible_edges    : boolean  = 1

#@description The visibility hidden flag for the display of the volume
visibility.hidden           : boolean = 0

#@description The visibility hidden flag for the display of daughter volumes
visibility.daughters.hidden : boolean = 0

###################
# Daughter volume #
###################

#@description The list of daughter volumes by label
internal_item.labels : string[1] = "Lab"

#@description The placement of the 'Lab' daughter volume
internal_item.placement.Lab : string = "0 0 0 (cm)"

#@description The geometry model of the 'Lab' daughter volume
internal_item.model.Lab     : string = "lab.model"

############################################
# GID mapping of the daughter setup volume #
############################################

#@description The mapping directives for the 'Lab' daughter volume
mapping.daughter_id.Lab : string  = "[lab.gc:area=0]"


# End of world.geom
