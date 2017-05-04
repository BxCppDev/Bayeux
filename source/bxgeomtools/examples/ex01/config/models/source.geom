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


########################################################################
[name="source_support_hole.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to describe the small hole in the source support ring


#@description The name of the 3D shape of the source support ring
shape_type  : string = "cylinder"

#@description The default implicit length unit
length_unit : string = "mm"

#@description The inner R dimension of the source support ring hole
r           : real = 0.5

#@description The Z dimension (thickness) of the source support ring
z           : real = 5.0

#@description The name of the material of the hole
material.ref : string = "vacuum"

#@description The recommended color for the display of the source film
visibility.color            : string  = "blue"

#@description The visibility hidden flag for the display of the source film
visibility.hidden           : boolean = 0


###########################################################
[name="source.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to describe the source

######### Shape description #########

#@description The name of the 3D shape of the source support ring
shape_type  : string = "tube"

#@description The length unit
length_unit : string = "mm"

#@description The inner R dimension (inner radius) of the source support ring
inner_r     : real = 10.0

#@description The outer R dimension (inner radius) of the source support ring
outer_r     : real = 12.0

#@description The Z dimension (thickness) of the source support ring
z           : real = 5.0

#@description The build mode for the cavity volume of the tube
filled_mode : string = "by_envelope"

#@description The name of the source support tube for mapping
filled_label : string = "support"

######### Special properties attached to materials #########

#@description The name of the material of the source support ring
material.ref : string = "aluminium"

#@description Some property attached to the material of the source support ring
material.radioactivity.Bi214.activity : real as mass_activity = 35.4 uBq/kg

#@description Some property attached to the material of the source support ring
material.radioactivity.Tl208.activity : real as mass_activity = 12.5 uBq/kg

#@description Some property attached to the material of the tube cavity
material.group : string = "absorption.roi"

#@description The name of the material that fills the cavity volume of the tube
material.filled.ref : string = "vacuum"

#@description Some property attached to the material of the tube cavity
material.filled.group : string = "transport.roi"


######### Special properties attached to sensitive volumes #########

#@description The sensitive category (tube volume)
sensitive.category : string = "__source_support.sd"

#@description Some sensitive property attached to the model (tube volume)
sensitive.hit.category : string = "__in_source_support.hc"

#@description Some sensitive property attached to the model (tube volume)
sensitive.hit.color : string = "red"

#@description Some sensitive property attached to the model (tube volume)
sensitive.debug    : boolean = 1

#@description The sensitive category attached to the model (tube cavity)
sensitive.filled.category : string = "__source_vacuum.sd"

#@description Some sensitive property attached to the model (tube cavity)
sensitive.filled.hit.category : string = "__in_source_vacuum.hc"

#@description Some sensitive property attached to the model (tube cavity)
sensitive.filled.hit.color : string = "green"


######### Visibility parameters attached to volumes #########

#@description The recommended color for the display of the volume of interest
visibility.color           : string  = "blue"

#@description The visibility hidden flag for the display of the envelope volume
visibility.hidden_envelope  : boolean = 0

#@description The visibility hidden flag for the display of ring
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of ring
visibility.daughters.hidden : boolean = 0


######### Items placed within the tube cavity #########

#@description The list of daughter volumes by label
internal_item.labels 	    : string[2] = "hole0" "hole1"

#@description The model of the "hole0" daughter volume
internal_item.model.hole0     : string  = "source_support_hole.model"

#@description The placement of the "hole0" daughter volume
internal_item.placement.hole0 : string  = "11 0 0 (mm)"

#@description The model of the "hole1" daughter volume
internal_item.model.hole1     : string  = "source_support_hole.model"

#@description The placement of the "hole1" daughter volume
internal_item.placement.hole1 : string  = "0 11 0 (mm)"

#@description The list of daughter volumes by label
internal_item.filled.labels 	    : string[1] = "film"

#@description The model of the "film" daughter volume
internal_item.filled.model.film     : string  = "source_film.model"

#@description The placement of the "film" daughter volume
internal_item.filled.placement.film : string  = "0 0 0 (mm)"


######### Rules for GID computing #########

#@description The mapping directives for the "hole0" daughter volumes
mapping.daughter_id.hole0 : string  = "[source_support_hole.gc:hole=0]"

#@description The mapping directives for the "hole1" daughter volumes
mapping.daughter_id.hole1 : string  = "[source_support_hole.gc:hole=1]"

#@description The mapping directives for the "ring" daughter volumes
mapping.filled.daughter_id.support : string  = "[source_support.gc]"

#@description The mapping directives for the "film" daughter volumes
mapping.filled.daughter_id.film : string  = "[source_film.gc]"


###################################################################
[name="source_chain.model" type="geomtools::replicated_model"]

#@description The name of the material
material.ref : string = "vacuum"

#@description The replication axis
replicated.axis            : string = "x"

#@description The number of replicated volumes
replicated.number_of_items : integer = 3

#@description The model of the replicated volumes
replicated.model           : string = "source.model"

#@description The label associated to the replicated volumes
replicated.label           : string = "sources"

#@description The length unit
length_unit                : string = "cm"

#@description The step between replicated volumes
replicated.step            : real = 10

#@description The mapping directives for the "sources" daughter volumes
mapping.daughter_id.sources : string  = "[source.gc:position+0]"

#@description The recommended color for the display of the ring
#visibility.color            : string  = "transparent"
visibility.color            : string  = "grey"

# End of list of multi-properties.
