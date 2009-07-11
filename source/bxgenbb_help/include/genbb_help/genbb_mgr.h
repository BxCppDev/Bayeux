// -*- mode: c++; -*- 
/* genbb_mgr.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-01-19
 * Last modified: 2009-04-25
 * 
 * License: 
 * 
 * Description: 
 *   Manager for GENBB input data
 * 
 * History: 
 * 
 */

#ifndef __genbb_help__genbb_mgr_h
#define __genbb_help__genbb_mgr_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>

#include <boost/cstdint.hpp>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <CLHEP/Vector/ThreeVector.h>

#include <datatools/serialization/serialization.h>
#include <datatools/serialization/io_factory.h>

#include <geomtools/utils.h>

namespace genbb {

  struct primary_particle 
    : public datatools::serialization::i_serializable 
  {
  public:
    
    static const std::string SERIAL_TAG;
    
    virtual const std::string & get_serial_tag () const;
    
    enum particle_type
      {
	UNDEF    = -1,
	GAMMA    = 1,
	POSITRON = 2,
	ELECTRON = 3,
	ALPHA    = 47
      };

  public:
    
    int32_t              type;
    double               time;
    geomtools::vector_3d momentum;

    void reset ();

    bool is_valid () const
    {
      return type != UNDEF;
    }

    bool is_gamma () const
    {
      return type == GAMMA;
    }

    bool is_positron () const
    {
      return type == POSITRON;
    }

    bool is_electron () const
    {
      return type == ELECTRON;
    }

    bool is_alpha () const
    {
      return type == ALPHA;
    }

    double get_mass () const;

    double get_kinetic_energy () const;

    static std::string get_label (int type_);

    primary_particle ();

    primary_particle (int32_t type_, 
		      double time_, 
		      const geomtools::vector_3d &);


    void dump (std::ostream & out_ = std::clog, 
	       const std::string & indent_ = "") const;

  private:
    friend class boost::serialization::access; 
    template<class Archive>
    void serialize (Archive            & ar_, 
                    const unsigned int   version_)
    {
      ar_ & boost::serialization::make_nvp ("type", type);
      ar_ & boost::serialization::make_nvp ("time", time);
      ar_ & boost::serialization::make_nvp ("momentum", momentum);
    }

  };

  /*******************/
  
  struct primary_event
    : public datatools::serialization::i_serializable 
  {
  public:
    
    static const std::string SERIAL_TAG;
    
    virtual const std::string & get_serial_tag () const;
    
  public:
    typedef std::list<primary_particle> particles_col_t;

    double          time;
    particles_col_t particles;
    std::string     classification;

    const std::string & get_classification () const
    {
      return classification;
    }

    void set_classification (const std::string & c_)
    {
      classification = c_;
    }

    void reset_classification ()
    {
      classification = "";
    }

    void compute_classification ()
    {
      size_t n_eminus = 0;
      size_t n_eplus = 0;
      size_t n_gamma = 0;
      size_t n_alpha = 0;
      size_t n_others = 0;
      for (particles_col_t::const_iterator i = particles.begin ();
	   i != particles.end ();
	   i++)
	{
	  const primary_particle & p = *i;
	  if (p.is_electron ()) n_eminus++;
	  else if (p.is_positron ()) n_eplus++;
	  else if (p.is_gamma ()) n_gamma++;
	  else if (p.is_alpha ()) n_alpha++;
	  else n_others++;
	}
      std::ostringstream cl_ss;
      
      cl_ss << n_eminus << 'e' 
	    << n_eplus << 'p' 
	    << n_gamma << 'g' 
	    << n_alpha << 'a' 
	    << n_others << 'X'; 
      set_classification (cl_ss.str ());
    }

    primary_event ()
    {
      time = -1.;
    }

    bool is_valid () const
    {
      return time >= 0.;
    }

    void reset ()
    {
      time = -1.;
      particles.clear ();
      classification = "";
    }

    void dump (std::ostream & out_ = std::clog,
	       const std::string & indent_ = "") const;

  private:
    friend class boost::serialization::access; 
    template<class Archive>
    void serialize (Archive            & ar_, 
                    const unsigned int   version_)
    {
      ar_ & boost::serialization::make_nvp ("time", time);
      ar_ & boost::serialization::make_nvp ("particles", particles);
      ar_ & boost::serialization::make_nvp ("classification", classification);
    }

  };

  /*******************/

  class genbb_mgr
  {
  public:
    enum
      {
	FORMAT_GENBB = 0,
	FORMAT_BOOST = 1
      };
  private:
    bool __debug;
    bool __initialized;
    std::list<std::string> __filenames;
    int            __format;
    std::istream * __in;
    std::ifstream  __fin;
    datatools::serialization::data_reader __reader;
    primary_event  __current;

  // ctor/dtor:
  public: 

    bool is_debug () const
    {
      return __debug;
    }

    void set_debug (bool d_)
    {
      __debug = d_;
    }

    int get_format () const
    {
      return __format;
    }

    void set_format (int format_ = FORMAT_GENBB);

    bool is_format_genbb () const
    {
      return __format == FORMAT_GENBB;
    }

    bool is_format_boost () const
    {
      return __format == FORMAT_BOOST;
    }

    genbb_mgr (int format_ = FORMAT_GENBB);

    virtual ~genbb_mgr ();
  
    void set (const std::string & filename_);

    void init ();

    void reset ();

    bool has_next ();

    void load_next (primary_event & event_, 
		    bool compute_classification_ = true);

    void dump (std::ostream & out_ = std::clog) const;

  private:

    void __load_next ();

    void __load_next_genbb ();

    void __load_next_boost ();

  };

} // end of namespace genbb

#endif // __genbb_help__genbb_mgr_h

// end of genbb_mgr.h
