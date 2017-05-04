# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of the detector volume

#@key_label  "name"
#@meta_label "type"


#############################################################
[name="detector.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to configure the detector logical volume

############
# Geometry #
############

### #@description The default length unit
### length_unit : string = "mm"

#@description The name of the 3D shape of the detector volume
shape_type  : string = "box"

#@description The detector volume X dimension
x           : real   =  60.0 mm

#@description The detector volume Y dimension
y           : real   =  60.0 mm

#@description The detector volume Z dimension
z           : real   =  100.0 mm

############
# Material #
############

#@description The name of the material that fills the detector volume
material.ref : string = "detector_material"

#############
# Sensitive #
#############

#@description The name of the sensitive category associated to the detector volume
sensitive.category : string = "detector_SD"

##############
# Visibility #
##############

#@description The recommended color for the display of the detector volume
visibility.color  : string  = "blue"

#@description The visibility hidden flag for the display of the detector volume
visibility.hidden : boolean = 0

