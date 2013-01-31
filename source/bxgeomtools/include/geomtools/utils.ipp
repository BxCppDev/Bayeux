// -*- mode: c++; -*- 
//! \file geomtools/utils.ipp

#ifndef GEOMTOOLS_UTILS_IPP_
#define GEOMTOOLS_UTILS_IPP_ 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>

#include <datatools/utils.h>
#include <geomtools/utils.h>

//! Serialization stuff for CLHEP 'vector_2d'
namespace boost {

  namespace serialization {
    
    // Vector 2D:
    template<class Archive>
    void save (Archive & a_ar , 
               const geomtools::vector_2d & a_vector,
               const unsigned int a_version)
    {
      double x = a_vector.x ();
      double y = a_vector.y ();
      a_ar & boost::serialization::make_nvp ("x", x);
      a_ar & boost::serialization::make_nvp ("y", y);
      return;
    }
    
    template<class Archive>
    void load (Archive & a_ar , 
               geomtools::vector_2d & a_vector,
               const unsigned int a_version)
    {
      double x;
      double y;
      a_ar & boost::serialization::make_nvp ("x", x);
      a_ar & boost::serialization::make_nvp ("y", y);
      a_vector.set (x, y);
      return;
    }
    
    template<class Archive>
    void serialize (Archive & a_ar,
                    geomtools::vector_2d  & a_vector,
                    const unsigned int a_version)
    {
      boost::serialization::split_free (a_ar, a_vector, a_version);
      return;
    } 
    
  } // namespace serialization

} // namespace boost


//! Serialization stuff for CLHEP 'vector_3d'
namespace boost {

  namespace serialization {
    
    // Vector 3D:
    template<class Archive>
    void save (Archive & a_ar , 
               const geomtools::vector_3d & a_vector,
               const unsigned int a_version)
    {
      double x = a_vector.x ();
      double y = a_vector.y ();
      double z = a_vector.z ();
      a_ar & boost::serialization::make_nvp ("x", x);
      a_ar & boost::serialization::make_nvp ("y", y);
      a_ar & boost::serialization::make_nvp ("z", z);
      return;
    }
    
    template<class Archive>
    void load (Archive & a_ar , 
               geomtools::vector_3d & a_vector,
               const unsigned int a_version)
    {
      double x;
      double y;
      double z;
      a_ar & boost::serialization::make_nvp ("x", x);
      a_ar & boost::serialization::make_nvp ("y", y);
      a_ar & boost::serialization::make_nvp ("z", z);
      a_vector.set (x, y, z);
      return;
    }
    
    template<class Archive>
    void serialize (Archive & a_ar,
                    geomtools::vector_3d  & a_vector,
                    const unsigned int a_version)
    {
      boost::serialization::split_free (a_ar, a_vector, a_version);
      return;
    } 
    
  } // namespace serialization

} // namespace boost

//! Serialization stuff for CLHEP 'rotation'
namespace boost {

  namespace serialization {
    
    // Rotation 3D :
    template<class Archive>
    void save (Archive & a_ar , 
               const geomtools::rotation_3d & a_rotation,
               const unsigned int a_version)
    {
      double rxx = a_rotation.xx ();
      a_ar & boost::serialization::make_nvp ("xx", rxx);
      if (geomtools::is_valid (a_rotation))
        {
          double rxy, rxz, ryx, ryy, ryz, rzx, rzy, rzz;
          rxy = a_rotation.xy ();
          rxz = a_rotation.xz ();
          ryx = a_rotation.yx ();
          ryy = a_rotation.yy ();
          ryz = a_rotation.yz ();
          rzx = a_rotation.zx ();
          rzy = a_rotation.zy ();
          rzz = a_rotation.zz ();
          a_ar & boost::serialization::make_nvp ("xy", rxy);
          a_ar & boost::serialization::make_nvp ("xz", rxz);
          a_ar & boost::serialization::make_nvp ("yx", ryx);
          a_ar & boost::serialization::make_nvp ("yy", ryy);
          a_ar & boost::serialization::make_nvp ("yz", ryz);
          a_ar & boost::serialization::make_nvp ("zx", rzx);
          a_ar & boost::serialization::make_nvp ("zy", rzy);
          a_ar & boost::serialization::make_nvp ("zz", rzz);
        }
      return;
    }
    
    template<class Archive>
    void load (Archive & a_ar , 
               geomtools::rotation_3d & a_rotation,
               const unsigned int a_version)
    {
      double rxx (0.0), rxy (0.0), rxz (0.0);
      double ryx (0.0), ryy (0.0), ryz (0.0);
      double rzx (0.0), rzy (0.0), rzz (0.0);
      a_ar & boost::serialization::make_nvp ("xx", rxx); 
      if (rxx == rxx)
        {
          a_ar & boost::serialization::make_nvp ("xy", rxy);
          a_ar & boost::serialization::make_nvp ("xz", rxz);
          a_ar & boost::serialization::make_nvp ("yx", ryx);
          a_ar & boost::serialization::make_nvp ("yy", ryy);
          a_ar & boost::serialization::make_nvp ("yz", ryz);
          a_ar & boost::serialization::make_nvp ("zx", rzx);
          a_ar & boost::serialization::make_nvp ("zy", rzy);
          a_ar & boost::serialization::make_nvp ("zz", rzz);
          a_rotation = geomtools::rotation_wrapper_t (rxx, rxy, rxz, ryx, ryy, ryz, rzx, rzy, rzz);
        }
      else 
        {
          a_rotation = geomtools::rotation_3d ();
          geomtools::invalidate (a_rotation);
        }
      return;
    }
    
    template<class Archive>
    void serialize (Archive & a_ar,
                    geomtools::rotation_3d  & a_rotation,
                    const unsigned int a_version)
    {
      boost::serialization::split_free (a_ar, a_rotation, a_version);
      return;
    } 
    
  } // namespace serialization

} // namespace boost


#endif // GEOMTOOLS_UTILS_IPP_

// end of utils.ipp
