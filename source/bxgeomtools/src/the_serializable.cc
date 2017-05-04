// the_serializable.cc
// Third party:
// - Boost:
#include <boost/serialization/export.hpp>
// - Bayeux/datatools:
#include <datatools/archives_instantiation.h>

// This project:
#include <geomtools/geom_id.ipp>

#include <geomtools/utils.ipp>

#include <geomtools/i_object_3d.ipp>

#include <geomtools/i_shape_1d.ipp>

#include <geomtools/i_shape_2d.ipp>

#include <geomtools/i_shape_3d.ipp>

#include <geomtools/helix_3d.ipp>

#include <geomtools/line_3d.ipp>

#include <geomtools/polyline_3d.ipp>

#include <geomtools/i_placement.ipp>

#include <geomtools/placement.ipp>

#include <geomtools/blur_spot.ipp>

#include <geomtools/base_hit.ipp>

#include <geomtools/display_data.ipp>

#include <geomtools/angular_range.ipp>



DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::vector_2d)

DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::vector_3d)

DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::rotation_3d)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::angular_range)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::angular_range)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::geom_id)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::geom_id)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::i_object_3d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::i_object_3d)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::i_shape_1d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::i_shape_1d)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::i_shape_2d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::i_shape_2d)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::i_shape_3d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::i_shape_3d)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::line_3d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::line_3d)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::helix_3d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::helix_3d)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::polyline_3d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::polyline_3d)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::i_placement)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::i_placement)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::placement)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::placement)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::blur_spot)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::blur_spot)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::base_hit)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::base_hit)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::display_data)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::display_data)


