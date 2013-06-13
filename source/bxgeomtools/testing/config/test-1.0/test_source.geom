# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry models that compose the source

#@key_label   "name"
#@meta_label  "type"


################################################################
[name="source_film.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to describe the source film

#@description The name of the 3D shape of the source film
shape_type  : string = "cylinder"

#@description The length unit
length_unit : string = "mm"

#@description The R dimension (radius) of the cylinder source film
r           : real = 9.5

#@description The Z dimension (thickness) of the cylinder source film
z           : real = 0.05

#@description The name of the material of the source film
material.ref : string = "mylar"

#@description The recommended color for the display of the source film
visibility.color            : string  = "magenta"

#@description The visibility hidden flag for the display of the source film
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of the source film
visibility.daughters.hidden : boolean = 0


###################################################################
[name="source_support.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to describe the source support ring

#@description The name of the 3D shape of the source support ring
shape_type  : string = "tube"

#@description The build mode for the central of the source support ring
filled_mode : string = "by_envelope"

#@description The length unit
length_unit : string = "mm"

#@description The inner R dimension (inner radius) of the source support ring
inner_r     : real = 10.0

#@description The outer R dimension (inner radius) of the source support ring
outer_r     : real = 12.0

#@description The Z dimension (thickness) of the source support ring
z           : real = 5.0

#@description The name of the material of the source support ring
material.ref : string = "aluminium"

#@description The name of the material that fills the void volume within the ring
material.filled.ref : string = "vacuum"

#@description The recommended color for the display of the ring
visibility.color            : string  = "blue"

#@description The visibility hidden flag for the display of ring
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of ring
visibility.daughters.hidden : boolean = 0

#@description The list of daughter volumes by label
internal_item.labels 	    : string[1] = "film"

#@description The model of the "film" daughter volume
internal_item.model.film     : string  = "source_film.model"

#@description The placement of the "film" daughter volume
internal_item.placement.film : string  = "0 0 0 (mm)"

#@description The mapping directives for the "film" daughter volumes
mapping.daughter_id.film : string  = "[source_film.gc]"


###################################################################
[name="source_chain.model" type="geomtools::replicated_model"]

#@description The name of the material
material.ref : string = "vacuum"

#@description The replication axis
replicated.axis            : string = "x"

#@description The number of replicated volumes
replicated.number_of_items : integer = 3

#@description The model of the replicated volumes
replicated.model           : string = "source_support.model"

#@description The label associated to the replicated volumes
replicated.label           : string = "sources"

#@description The length unit
length_unit                : string = "cm"

#@description The step between replicated volumes
replicated.step            : real = 10

#@description The mapping directives for the "sources" daughter volumes
mapping.daughter_id.sources : string  = "[source.gc:position+0]"

# End of list of multi-properties.
