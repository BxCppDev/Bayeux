# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description List of geometry models that describe an optical module
#
# This geometry sample setup describes an optical module
# based on the coupling of a PMT and a scintillator block
# through a light guide.
#
#@key_label   "name"
#@meta_label  "type"

#################################################################
[name="front_window.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to describe the film in front of the scintillator block

#@description The name of the 3D shape
shape_type :   string =  "box"

#@description The X dimension
x   : real   = 300.0

#@description The Y dimension
y   : real   = 300.0

#@description The Z dimension
z   : real   = 12.0e-3

#@description The length unit
length_unit       : string  = "mm"

#@description The name of the material
material.ref      : string  = "mylar"

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "green"


#######################################################################
[name="scintillator_block.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to describe the scintillator block

#@description The name of the 3D shape
shape_type :   string =  "box"

#@description The X dimension
x   : real   = 300.0

#@description The Y dimension
y   : real   = 300.0

#@description The Z dimension
z   : real   =  10.0

#@description The length unit
length_unit       : string    = "mm"

#@description The name of the material
material.ref      : string = "polystyrene"

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "cyan"

#@description The 'sensitive' category attached to this detection volume
sensitive.category  : string  = "scin.sd"


################################################################
[name="light_guide.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to describe the light guide

#@description The name of the 3D shape
shape_type : string = "polyhedra"

#@description The polyhedra build mode
build_mode : string = "points"

#@description The polyhedra number of sides
sides        : integer = 4

#@description The list of Z coordinates for the shape
list_of_z    : real [3] = -80.     +60.     +80.

#@description The list of inner radius coordinates for the shape
list_of_rmin : real [3] =    0.      0.       0.

#@description The list of outer radius coordinates for the shape
list_of_rmax : real [3] =   60.    150.     150.

#@description The length unit
length_unit  : string = "mm"

#@description The name of the material
material.ref : string  = "plexiglass"

#@description The visibility hidden flag for the display
visibility.hidden          : boolean = 0

#@description The recommended color for the display
visibility.color           : string  = "cyan"


########################################################################
[name="rotated_light_guide.model" type="geomtools::rotated_boxed_model"]

#@config The list of properties to describe the rotated light guide

#@description The rotation axis
rotated.axis  : string = "z"

#@description The angular unit
angle_unit    : string = "degree"

#@description The rotation angle
rotated.angle : real   = 45.0

#@description The length unit
length_unit   : string = "mm"

#@description The X dimension
x             : real   = 300.0

#@description The Y dimension
y             : real   = 300.0

#@description The name of the model to be rotated
rotated.model : string = "light_guide.model"

#@description The label associated to the rotated daughter model
rotated.label : string = "light_guide"

#@description The visibility hidden flag for the display
visibility.hidden          : boolean = 0

#@description The recommended color for the display
visibility.color           : string  = "grey"

#@description The visibility hidden flag for the envelope
visibility.hidden_envelope  : boolean = 0


#######################################################################
[name="focusing_electrode.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to describe the rotated light guide

#@description The shape
shape_type   : string =  "tube"

#@description The length unit
length_unit   : string = "mm"

#@description The Z dimension
z             : real   = 1.0

#@description The inner radius dimension
inner_r      : real   = 30.0

#@description The outer radius dimension
outer_r      : real   = 40.0

#@description The material name
material.ref      : string  = "copper"

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "red"


###########################################################
[name="dynode.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to describe the dynode

#@description The shape
shape_type   : string =  "box"

#@description The length unit
length_unit  : string = "mm"

#@description The X dimension
x            : real   = 35.0

#@description The Y dimension
y            : real   = 20.0

#@description The Z dimension
z            : real   =  0.5

#@description The material name
material.ref      : string  = "copper"

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "red"


########################################################
[name="PMT.model" type="geomtools::simple_shaped_model"]

shape_type  : string = "polycone"

#filled_mode : string = "by_extrusion"
filled_mode : string = "by_envelope"

#@description The length unit
length_unit : string = "mm"

#@description The material name
material.ref        : string = "glass"

#@description The inner material name
material.filled.ref : string = "vacuum"

#@description The polycone build mode
build_mode  : string = "datafile"

#@description The polycone coordinates filename
datafile    : string = "${MCTOOLS_DATA_DIR}/testing/config/g4/test-1.0/geometry/pmt_hamamatsu_R5912MOD_polycone.data"

#@description The visibility hidden flag for the display
visibility.hidden           : boolean = 0

#@description The visibility hidden flag for the envelope
visibility.hidden_envelop   : boolean = 0

#@description The recommended color for the display
visibility.color            : string  = "magenta"

#@description The visibility hidden flag for the daughters volumes
visibility.daughters.hidden : boolean = 0

#@description The list of daughter volumes by labels
internal_item.labels 	    : string[12] = \
				"focusing_electrode" \
				"dynode_0" \
				"dynode_1" \
				"dynode_2" \
				"dynode_3" \
				"dynode_4" \
				"dynode_5" \
				"dynode_6" \
				"dynode_7" \
				"dynode_8" \
				"dynode_9" \
				"anode"

#@description The placement of the "focusing_electrode" daughter volume
internal_item.placement.focusing_electrode : string  = "0 0 +20 (mm)"

#@description The model of the "focusing_electrode" daughter volume
internal_item.model.focusing_electrode     : string  = "focusing_electrode.model"

internal_item.model.dynode_0     : string  = "dynode.model"
internal_item.placement.dynode_0 : string  = "0 -15 +0 (mm) / x +45 (degree)"

internal_item.model.dynode_1     : string  = "dynode.model"
internal_item.placement.dynode_1 : string  = "0 +15 +0 (mm) / x +45 (degree)"

internal_item.model.dynode_2     : string  = "dynode.model"
internal_item.placement.dynode_2 : string  = "0 -15 -20 (mm) / x +45 (degree)"

internal_item.model.dynode_3     : string  = "dynode.model"
internal_item.placement.dynode_3 : string  = "0 +15 -20 (mm) / x +45 (degree)"

internal_item.model.dynode_4     : string  = "dynode.model"
internal_item.placement.dynode_4 : string  = "0 -15 -40 (mm) / x +45 (degree)"

internal_item.model.dynode_5     : string  = "dynode.model"
internal_item.placement.dynode_5 : string  = "0 +15 -40 (mm) / x +45 (degree)"

internal_item.model.dynode_6     : string  = "dynode.model"
internal_item.placement.dynode_6 : string  = "0 -15 -60 (mm) / x +45 (degree)"

internal_item.model.dynode_7     : string  = "dynode.model"
internal_item.placement.dynode_7 : string  = "0 +15 -60 (mm) / x +45 (degree)"

internal_item.model.dynode_8     : string  = "dynode.model"
internal_item.placement.dynode_8 : string  = "0 -15 -80 (mm) / x +45 (degree)"

internal_item.model.dynode_9     : string  = "dynode.model"
internal_item.placement.dynode_9 : string  = "0 +15 -80 (mm) / x +45 (degree)"

internal_item.placement.anode : string  = "0 0 -95 (mm) / x +20 (degree)"
internal_item.model.anode     : string  = "dynode.model"

# The mapping directives for all "electrodes/dynodes" daughter volumes

mapping.daughter_id.dynode_0 : string  = "[pmt_dynode.gc:dynode=0]"
mapping.daughter_id.dynode_1 : string  = "[pmt_dynode.gc:dynode=1]"
mapping.daughter_id.dynode_2 : string  = "[pmt_dynode.gc:dynode=2]"
mapping.daughter_id.dynode_3 : string  = "[pmt_dynode.gc:dynode=3]"
mapping.daughter_id.dynode_4 : string  = "[pmt_dynode.gc:dynode=4]"
mapping.daughter_id.dynode_5 : string  = "[pmt_dynode.gc:dynode=5]"
mapping.daughter_id.dynode_6 : string  = "[pmt_dynode.gc:dynode=6]"
mapping.daughter_id.dynode_7 : string  = "[pmt_dynode.gc:dynode=7]"
mapping.daughter_id.dynode_8 : string  = "[pmt_dynode.gc:dynode=8]"
mapping.daughter_id.dynode_9 : string  = "[pmt_dynode.gc:dynode=9]"
mapping.daughter_id.anode    : string  = "[pmt_dynode.gc:dynode=10]"
mapping.daughter_id.focusing_electrode : string = "[pmt_focus_electrode.gc]"


#############################################################
[name="PMT_base.model" type="geomtools::simple_shaped_model"]

#@config The list of properties to describe the PMT base

#@description The name of the 3D shape
shape_type   : string =  "cylinder"

#@description The length unit
length_unit       : string  = "mm"

#@description The R dimension
r            : real   = 30.0

#@description The Z dimension
z            : real   = 40.0

#@description The name of the material
material.ref : string = "polystyrene"

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "orange"

#@description The mapping directives for the "detectors" daughter volumes
#mapping.daughter_id.scintillator_block : string  = "[scin_block.gc]"


##########################################################################
[name="diaphragm.model" type="geomtools::cylindric_extrusion_boxed_model"]

#@config The list of properties to describe the diaphragm

#@description The length unit
length_unit       : string  = "mm"

#@description The extrusion radius dimension
extrusion_radius : real   =  40.0

#@description The X dimension
x                : real   = 320.0

#@description The Y dimension
y                : real   = 320.0

#@description The Z dimension
z                : real   =   1.0

#@description The name of the material
material.ref : string = "copper"

#@description The visibility hidden flag for the display
visibility.hidden : boolean = 0

#@description The recommended color for the display
visibility.color  : string  = "red"


#############################################################
[name="optical_module.model" type="geomtools::stacked_model"]

#@description The name of the material
material.ref      : string    = "copper"

#@description The stacking axis
stacked.axis            : string = "z"

#@description The number of stacked volumes
stacked.number_of_items : integer = 6

#@description The model of the stacked volume #5
stacked.model_5   : string  = "diaphragm.model"

#@description The label of the stacked volume #5
stacked.label_5   : string  = "diaphragm"

stacked.model_4   : string  = "front_window.model"
stacked.label_4   : string  = "front_window"

stacked.model_3   : string  = "scintillator_block.model"
stacked.label_3   : string  = "scintillator_block"

stacked.model_2   : string  = "rotated_light_guide.model"
stacked.label_2   : string  = "light_guide"

stacked.model_1   : string  = "PMT.model"
stacked.label_1   : string  = "PMT"

stacked.model_0   : string  = "PMT_base.model"
stacked.label_0   : string  = "PMT_base"

#@description The visibility hidden flag for the display
visibility.hidden : boolean  = 0

#@description The recommended color for the display
visibility.color  : string   = "grey"

#@description The mapping directives for the "detectors" daughter volumes
mapping.daughter_id.scintillator_block : string  = "[scin_block.gc]"


###################################################################
[name="detector_column.model" type="geomtools::replicated_model"]

#@description The replication axis
replicated.axis            : string = "y"

#@description The number of replicated volumes
replicated.number_of_items : integer = 2

#@description The model of the replicated volumes
replicated.model           : string = "optical_module.model"

#@description The label associated to the replicated volumes
replicated.label           : string = "optical_modules"

#@description The length unit
length_unit                : string = "cm"

#@description The step between replicated volumes
replicated.step            : real = 30

#@description The mapping directives for the "detectors" daughter volumes
mapping.daughter_id.optical_modules : string  = "[detector.gc:row+0]"


###################################################################
[name="detector_array.model" type="geomtools::replicated_model"]

#@description The replication axis
replicated.axis            : string = "x"

#@description The number of replicated volumes
replicated.number_of_items : integer = 2

#@description The model of the replicated volumes
replicated.model           : string = "detector_column.model"

#@description The label associated to the replicated volumes
replicated.label           : string = "detectors"

#@description The length unit
length_unit                : string = "cm"

#@description The step between replicated volumes
replicated.step            : real = 30

#@description The mapping directives for the "detectors" daughter volumes
mapping.daughter_id.detectors : string  = "[detector_column.gc:column+0]"


# End of list of multi-properties.
