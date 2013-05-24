# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry models that compose the vessel

#@key_label   "name"
#@meta_label  "type"


##################################################################
[name="vessel_endcap.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to describe the vessel endcap

#@description The name of the 3D shape of the endcap cylinder
shape_type  : string = "cylinder"

#@description The length unit for the experimental setup placement
length_unit : string = "mm"

#@description The R dimension (radius) of the endcap cylinder
r           : real = 570.0

#@description The Z dimension (thickness) of the endcap cylinder
z           : real = 10.0

#@description The name of the material that fills the lab atmosphere
material.ref : string = "iron"

#@description The recommended color for the display of the vessel area
visibility.color            : string  = "blue"

#@description The visibility hidden flag for the display of the vessel
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of the vessel
visibility.daughters.hidden : boolean = 0


##################################################################
[name="vessel_inner_volume.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to describe the vessel internal volume

#@description The name of the 3D shape of the vessel internal volume
shape_type  : string = "cylinder"

#@description The length unit
length_unit : string = "cm"

#@description The R dimension (radius)
r     : real = 55.0

#@description The Z dimension (thickness) of the vessel's body
z     : real = 120.0

#@description The name of the material that fills the vessel internal volume
material.ref : string = "vacuum"

#@description The recommended color for the display of the vessel area
visibility.color            : string  = "cyan"

#@description The visibility hidden flag for the display of the vessel
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of the vessel
visibility.daughters.hidden : boolean = 0

#@description The list of daughter volumes by labels
internal_item.labels : string[2] = "sources" "detector_array"

#@description The model of the "sources" daughter volume
internal_item.model.sources       : string  = "source_chain.model"

#@description The placement of the "sources" daughter volume
internal_item.placement.sources   : string  = "15 30 -50 (cm)"

#@description The model of the "detector_array" daughter volume
internal_item.model.detector_array     : string  = "detector_array.model"

#@description The placement of the "detector_array" daughter volume
internal_item.placement.detector_array : string  = "0 0 25 (cm) / x 180 (degree)"


################################################################
[name="vessel_body.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to describe the vessel body

#@description The name of the 3D shape of the vessel body
shape_type  : string = "tube"

#@description The build mode for the inner volume of the vessel body
filled_mode : string = "none"

#@description The length unit
length_unit : string = "cm"

#@description The inner R dimension (inner radius) of the vessel's body
inner_r     : real = 55.0

#@description The outer R dimension (inner radius) of the vessel's body
outer_r     : real = 56.0

#@description The Z dimension (thickness) of the vessel's body
z           : real = 120.0

#@description The name of the material of the vessel
material.ref : string = "iron"

#@description The recommended color for the display of the vessel
visibility.color            : string  = "blue"

#@description The visibility hidden flag for the display of the vessel
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of the vessel
visibility.daughters.hidden : boolean = 0


#####################################################
[name="vessel.model" type="geomtools::stacked_model"]

#@config The list of properties to configure the vacuum chamber

#@description The label of the stacking axis
stacked.axis : string = "z"

#@description The length unit
length_unit : string = "cm"

#@description The number of stacked volume
stacked.number_of_items : integer = 4

#@description The model of the first stacked volume
stacked.model_0   : string  = "vessel_endcap.model"

#@description The label of the first stacked volume
stacked.label_0   : string  = "endcap0"

#@description The model of the second stacked volume
stacked.model_1  : string  = "vessel_body.model"

#@description The label of the second stacked volume
stacked.label_1   : string  = "body"

#@description The upper bound of the second stacked volume
stacked.limit_max_1 : real  = -60.

#@description The model of the third stacked volume
stacked.model_2  : string  = "vessel_inner_volume.model"

#@description The label of the third stacked volume
stacked.label_2   : string  = "inner_volume"

#@description The model of the fourth stacked volume
stacked.model_3  : string  = "vessel_endcap.model"

#@description The label of the fourth stacked volume
stacked.label_3   : string  = "endcap1"

#@description The name of the material that fills the lab atmosphere
material.ref : string = "air"

#@description The recommended color for the display of the vessel area
visibility.color            : string  = "transparent"

#@description The visibility hidden flag for the display of the vessel
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of the vessel
visibility.daughters.hidden : boolean = 0

#@description The visibility hidden flag for the envelope of the stacked volumes
visibility.hidden_envelope  : boolean = 1

#@description The mapping directives for the "endcap0" daughter volume
mapping.daughter_id.endcap0 : string  = "[vessel_part.gc:part=0]"

#@description The mapping directives for the "body" daughter volume
mapping.daughter_id.body : string  = "[vessel_part.gc:part=1]"

#@description The mapping directives for the "endcap1" daughter volume
mapping.daughter_id.endcap1 : string  = "[vessel_part.gc:part=2]"

#@description The mapping directives for the "inner_volume" daughter volume
mapping.daughter_id.inner_volume : string  = "[vessel_vacuum.gc]"


# End of list of multi-properties.
