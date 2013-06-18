# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry models that compose the probe volume
#@key_label   "name"
#@meta_label  "type"

##########################################################
[name="probe.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the sensitive probe

############
# Geometry #
############

#@description The default implicit length unit
length_unit : string    = "mm"

#@description The name of the 3D shape
shape_type  :   string =  "tube"

#@description The X dimension
inner_r : real as length  = 5 cm

#@description The Y dimension
outer_r : real as length  = 15 cm

#@description The Z dimension
z       : real as length  = 4 cm

#@description The tube filled mode
filled_mode  : string = "by_envelope"

#######################
# Material parameters #
#######################

#@description The name of the material the tube is made of
material.ref      : string = "copper"

#@description The name of the material inside the tube cavity
material.filled.ref : string = "plexiglass"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "green"

#@description The recommended color for the display of the cavity (envelope)
visibility.filled.hidden : boolean = 0

#@description The recommended color for the display of the cavity (envelope)
visibility.filled.color  : string  = "cyan"

###########################
# Internal/daughter items #
###########################

#@description The label of the ring
filled_label : string = "ring"

##########################################
# GID mapping of internal/daughter items #
##########################################

#@description The mapping directives for the "ring" daughter volume
mapping.filled.daughter_id.ring : string  = "[probe_ring.gc]"

#####################
# Sensitive volumes #
#####################

#@description The 'sensitive' category attached to this detector volume
sensitive.category : string  = "probe.sd"

# Note:
# We recommend that you name any sensitive category with the
# ".sd" suffix. This is to ease the reading of other configuration
# files at the simulation level (step hit processors). Also note
# that if you prefix the sensitive category name with two underscores
# (example: "__test.sd"), then the sensitive category is considered as
# "private" (non official) and may be processed in some special way by
# the simulation engine.
