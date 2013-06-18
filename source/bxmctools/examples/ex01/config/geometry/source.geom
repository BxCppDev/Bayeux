# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry models that compose the source
#@key_label   "name"
#@meta_label  "type"


################################################################
[name="source_film.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the source film

############
# Geometry #
############

#@description The default implicit length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the source film
shape_type  : string = "cylinder"

#@description The R dimension (radius) of the cylinder source film
r           : real = 9.5

#@description The Z dimension (thickness) of the cylinder source film
z           : real as length = 25 um

#######################
# Material parameters #
#######################

#@description The name of the material of the source film
material.ref : string = "mylar"

#########################
# Visibility parameters #
#########################

#@description The recommended color for the display of the source film
visibility.color            : string  = "magenta"

#@description The visibility hidden flag for the display of the source film
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of the source film
visibility.daughters.hidden : boolean = 0


###########################################################
[name="source.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the source

############
# Geometry #
############

#@description The default implicit length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the source support ring
shape_type  : string = "tube"

#@description The inner R dimension (inner radius) of the source support ring
inner_r     : real = 10.0

#@description The outer R dimension (inner radius) of the source support ring
outer_r     : real = 12.0

#@description The Z dimension (thickness) of the source support ring
z           : real = 5.0

#@description The build mode for the central of the source support tube as the daughter of the envelope
filled_mode : string = "by_envelope"

#######################
# Material parameters #
#######################

#@description The name of the material of the source support ring
material.ref : string = "aluminium"

#@description The name of the material that fills the void volume within the ring
material.filled.ref : string = "vacuum"

#########################
# Visibility parameters #
#########################

#@description The recommended color for the display of the ring
visibility.color            : string  = "blue"

#@description The visibility hidden flag for the display of ring
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of ring
visibility.daughters.hidden : boolean = 0

###########################
# Internal/daughter items #
###########################

#@description The list of daughter volumes by label
internal_item.filled.labels 	     : string[1] = "film"

#@description The model of the "film" daughter volume
internal_item.filled.model.film     : string  = "source_film.model"

#@description The placement of the "film" daughter volume
internal_item.filled.placement.film : string  = "0 0 0 (mm)"

#@description The label associated to the source support tube as the daughter of the envelope
filled_label : string = "support"

##########################################
# GID mapping of internal/daughter items #
##########################################

#@description The mapping directives for the "film" daughter volume
mapping.filled.daughter_id.film : string  = "[source_film.gc]"

#@description The mapping directives for the "support" daughter volume
mapping.filled.daughter_id.support : string  = "[source_support.gc]"


##############################################################
[name="source_chain.model" type="geomtools::replicated_model"]

#@config The configuration parameters for a set of aligned sources

############
# Geometry #
############

#@description The default implicit length unit
length_unit                : string = "cm"

#@description The replication axis
replicated.axis            : string = "x"

#@description The number of replicated volumes
replicated.number_of_items : integer = 3

#@description The model of the replicated volumes
replicated.model           : string = "source.model"

#@description The label associated to the replicated volumes
replicated.label           : string = "sources"

#@description The step between replicated volumes
replicated.step            : real as length = 10 cm

#######################
# Material parameters #
#######################

#@description The name of the material around the sources objects
material.ref               : string = "vacuum"

###########################
# Internal/daughter items #
###########################

#@description The mapping directives for the "sources" daughter volumes
mapping.daughter_id.sources : string  = "[source.gc:position+0]"


# End of list of multi-properties.
