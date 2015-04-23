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
z           : real as length = 5 um

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


###################################################################
[name="source_support.model" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the source support

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

#######################
# Material parameters #
#######################

#@description The name of the material of the source support ring
material.ref : string = "aluminium"

#########################
# Visibility parameters #
#########################

#@description The recommended color for the display of the ring
visibility.color            : string  = "blue"

#@description The visibility hidden flag for the display of ring
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of ring
visibility.daughters.hidden : boolean = 0


###########################################################
[name="source.model" type="geomtools::simple_shaped_model"]

############
# Geometry #
############

#@description The default implicit length unit
length_unit : string = "mm"

#@description The name of the 3D shape of the source cylindric envelope
shape_type  : string = "cylinder"

#@description The R dimension (radius) of the source cylindric envelope
r           : real = 12.0

#@description The Z dimension (thickness) of the source cylindric envelope
z           : real = 5.0

#######################
# Material parameters #
#######################

#@description The name of the material
material.ref      : string  = "lab_medium"

###########################
# Internal/daughter items #
###########################

#@description The list of daughter volumes by label
internal_item.labels 	     : string[2] = "support" "film"

#@description The model of the "support" daughter volume
internal_item.model.support     : string  = "source_support.model"

#@description The placement of the "support" daughter volume
internal_item.placement.support : string  = "0 0 0 (mm)"

#@description The model of the "film" daughter volume
internal_item.model.film     : string  = "source_film.model"

#@description The placement of the "film" daughter volume
internal_item.placement.film : string  = "0 0 0 (mm)"

#########################
# Visibility parameters #
#########################

#@description The recommended color for the display of the ring
visibility.color            : string  = "grey"

#@description The visibility hidden flag for the display of ring
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of ring
visibility.daughters.hidden : boolean = 0


##########################################
# GID mapping of internal/daughter items #
##########################################

#@description The mapping directives for the "film" daughter volume
mapping.daughter_id.film : string  = "[source_film.gc]"

#@description The mapping directives for the "support" daughter volume
mapping.daughter_id.support : string  = "[source_support.gc]"


# End of list of multi-properties.
