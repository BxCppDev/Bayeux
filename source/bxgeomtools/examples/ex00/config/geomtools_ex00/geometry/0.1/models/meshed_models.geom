# -*- mode: conf-unix; -*-
# List of multi-properties (datatools::multi_properties):

#@description The geometry model of a meshed volume
#@key_label   "name"
#@meta_label  "type"


######################################################################
[name="micro_magenta_box.model" type="geomtools::simple_shaped_model"]

############
# Geometry #
############

#@description The name of the 3D shape of the detector volume
shape_type  : string = "box"

#@description The X dimension of the box
x           : real as length =  10.0 mm

#@description The Y dimension of the box
y           : real as length =  10.0 mm

#@description The Z dimension of the box
z           : real as length =  10.0 mm

############
# Material #
############

#@description The name of the material that fills the box
material.ref : string = "copper"

##############
# Visibility #
##############

#@description The recommended color for the display of the box
visibility.color  : string  = "magenta"

#@description The visibility hidden flag for the display of the box
visibility.hidden : boolean = 0




#################################################################
[name="tiny_red_box.model" type="geomtools::simple_shaped_model"]

############
# Geometry #
############

#@description The name of the 3D shape of the detector volume
shape_type  : string = "box"

#@description The X dimension of the box
x           : real as length =  20.0 mm

#@description The Y dimension of the box
y           : real as length =  20.0 mm

#@description The Z dimension of the box
z           : real as length =  20.0 mm

############
# Material #
############

#@description The name of the material that fills the box
material.ref : string = "copper"

##############
# Visibility #
##############

#@description The recommended color for the display of the box
visibility.color  : string  = "red"

#@description The visibility hidden flag for the display of the box
visibility.hidden : boolean = 0


#####################################################################
[name="small_orange_box.model" type="geomtools::simple_shaped_model"]

############
# Geometry #
############

#@description The name of the 3D shape of the detector volume
shape_type  : string = "box"

#@description The X dimension of the box
x           : real as length =  40.0 mm

#@description The Y dimension of the box
y           : real as length =  40.0 mm

#@description The Z dimension of the box
z           : real as length =  40.0 mm

############
# Material #
############

#@description The name of the material that fills the box
material.ref : string = "copper"

##############
# Visibility #
##############

#@description The recommended color for the display of the box
visibility.color  : string  = "orange"

#@description The visibility hidden flag for the display of the box
visibility.hidden : boolean = 0


####################################################################
[name="medium_blue_box.model" type="geomtools::simple_shaped_model"]

############
# Geometry #
############

#@description The name of the 3D shape of the detector volume
shape_type  : string = "box"

#@description The X dimension of the box
x           : real as length =  200.0 mm

#@description The Y dimension of the box
y           : real as length =  100.0 mm

#@description The Z dimension of the box
z           : real as length =  50.0 mm

############
# Material #
############

#@description The name of the material that fills the box
material.ref : string = "copper"

##############
# Visibility #
##############

#@description The recommended color for the display of the box
visibility.color  : string  = "blue"

#@description The visibility hidden flag for the display of the box
visibility.hidden : boolean = 0



####################################################################
[name="large_green_box.model" type="geomtools::simple_shaped_model"]

############
# Geometry #
############

#@description The name of the 3D shape of the detector volume
shape_type  : string = "box"

#@description The X dimension of the box
x           : real as length =  200.0 mm

#@description The Y dimension of the box
y           : real as length =  500.0 mm

#@description The Z dimension of the box
z           : real as length =  200.0 mm

############
# Material #
############

#@description The name of the material that fills the box
material.ref : string = "water"

##################
# Internal items #
##################

internal_item.labels : string[4] = "Wall0" "Wall1" "Wall2" "Wall3"

#@description The model of the "Wall0" daughter volume
internal_item.model.Wall0       : string  = "medium_blue_box.model"

#@description The placement of the "Wall0" daughter volume
internal_item.placement.Wall0   : string  = "0 -15 -5 (cm) / x 90 (degree)"

#@description The model of the "Wall1" daughter volume
internal_item.model.Wall1       : string  = "medium_blue_box.model"

#@description The placement of the "Wall1" daughter volume
internal_item.placement.Wall1   : string  = "0 0 -5 (cm) / x 90 (degree)"

#@description The model of the "Wall2" daughter volume
internal_item.model.Wall2       : string  = "medium_blue_box.model"

#@description The placement of the "Wall2" daughter volume
internal_item.placement.Wall2   : string  = "0 +15 +5 (cm) / x 90 (degree)"

#@description The model of the "Wall3" daughter volume
internal_item.model.Wall3       : string  = "medium_blue_box.model"

#@description The placement of the "Wall3" daughter volume
internal_item.placement.Wall3   : string  = "-5 -5 +7.5 (cm) @ 90 0 0 (degree)"

###################
# Internal meshes #
###################

#@description The logging priority of the internal mesh handler
internal_mesh.logging.priority : string = "debug"

#@description The list of internal meshes
internal_mesh.labels : string[3] = "Voxel" "TinyVoxel" "MicroVoxel"

#@description The model of the "Voxel" daughter volume
internal_mesh.model.Voxel  : string  = "small_orange_box.model"

# #@description The logging priority of the mesh placement of the "Voxel" daughter volume
# internal_mesh.placement.Voxel.logging.priority : string = "debug"

#@description The rank of the "Voxel" daughter volume
internal_mesh.rank.Voxel   : integer  = 0

#@description The placement cache file parameter of the "Voxel" daughter volume
internal_mesh.placement.Voxel.cache_file_path : string as path = \
  "~/.cache/Bayeux/geomtools/ex00/Voxels.xml"

#@description The placement overlapping sampling parameter of the "Voxel" daughter volume
internal_mesh.placement.Voxel.overlapping.high_sampling: boolean = true

# #@description The placement step Z parameter of the "Voxel" daughter volume
# internal_mesh.placement.Voxel.step_z : real as length = 6 cm

# #@description The placement step Z parameter of the "Voxel" daughter volume
# internal_mesh.placement.Voxel.start_x : real as length = 0 cm


# #@description The logging priority of the mesh placement of the "TinyVoxel" daughter volume
# internal_mesh.placement.TinyVoxel.logging.priority : string = "debug"

#@description The model of the "TinyVoxel" daughter volume
internal_mesh.model.TinyVoxel  : string  = "tiny_red_box.model"

#@description The rank of the "TinyVoxel" daughter volume
internal_mesh.rank.TinyVoxel   : integer  = 1

#@description The placement cache file parameter of the "TinyVoxel" daughter volume
internal_mesh.placement.TinyVoxel.cache_file_path : string as path = \
  "~/.cache/Bayeux/geomtools/ex00/TinyVoxels.xml"

#@description The placement overlapping sampling parameter of the "TinyVoxel" daughter volume
internal_mesh.placement.TinyVoxel.overlapping.low_sampling: boolean = true


# #@description The logging priority of the mesh placement of the "MicroVoxel" daughter volume
# internal_mesh.placement.MicroVoxel.logging.priority : string = "debug"

#@description The model of the "MicroVoxel" daughter volume
internal_mesh.model.MicroVoxel  : string  = "micro_magenta_box.model"

#@description The rank of the "MicroVoxel" daughter volume
internal_mesh.rank.MicroVoxel   : integer  = 2

#@description The placement cache file parameter of the "MicroVoxel" daughter volume
internal_mesh.placement.MicroVoxel.cache_file_path : string as path = \
  "~/.cache/Bayeux/geomtools/ex00/MicroVoxels.xml"

#@description The placement overlapping sampling parameter of the "MicroVoxel" daughter volume
internal_mesh.placement.MicroVoxel.overlapping.low_sampling: boolean = true


##############
# Visibility #
##############

#@description The recommended color for the display of the box
visibility.color  : string  = "green"

#@description The visibility hidden flag for the display of the box
visibility.hidden : boolean = 0


# end