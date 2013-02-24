# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of the laboratory area 

#@key_label   "name"
#@meta_label  "type"

########################################################
[name="lab.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to configure the laboratory experimental area

#@description The name of the 3D shape of the lab area
shape_type  : string = "box"

#@description The length unit for the experimental setup placement
length_unit : string = "mm"

#@description The X dimension of the box
x           : real = 1900.0

#@description The Y dimension of the box
y           : real = 1900.0

#@description The Z dimension of the box
z           : real = 1900.0

#@description The name of the material that fills the lab atmosphere
material.ref : string = "air"

#@description The recommended color for the display of the lab area
visibility.color            : string  = "cyan"

#@description The visibility hidden flag for the display of the lab area
visibility.hidden           : boolean = 0

#@description The daughters' visibility hidden flag for the display of the lab area
visibility.daughters.hidden : boolean = 0

#@description The list of daughter volumes by labels
internal_item.labels 	    : string[1] = "vacuum_chamber"

#@description The model of the "vacuum_chamber" daughter volume
internal_item.model.vacuum_chamber     : string  = "vessel.model"

#@description The placement of the "vacuum_chamber" daughter volume
internal_item.placement.vacuum_chamber : string  = "0 0 0 (mm) / x +90 (degree)"

#@description The mapping directives for the "vacuum_chamber" daughter volume
mapping.daughter_id.vacuum_chamber : string  = "[vessel.gc:chamber=3]"


# End of list of multi-properties.
