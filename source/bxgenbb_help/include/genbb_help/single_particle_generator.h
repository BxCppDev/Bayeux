// -*- mode: c++ ; -*- 
/* single_particle_generator.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-03
 * Last modified: 2010-10-03
 * 
 * License: 
 * 
 * Description: 
 *   A generator of GENBB-like event with a single particle
 * 
 * History: 
 * 
 */

#ifndef __genbb_help__single_particle_generator_h
#define __genbb_help__single_particle_generator_h 1

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <datatools/utils/properties.h>
#include <datatools/utils/utils.h>
#include <geomtools/utils.h>
#include <genbb_help/i_genbb.h>
#include <datatools/utils/units.h>
#include <mygsl/rng.h>

namespace genbb {

  using namespace std;

  class single_particle_generator : public i_genbb
  {
  public:
    enum mode_t
      {
	MODE_INVALID = 0,
	MODE_MONOKINETIC = 1,
	MODE_GAUSSIAN_ENERGY = 2,
	MODE_ENERGY_RANGE = 3
      };

  private: 
    bool   __debug;
    bool   __initialized;
    int    __particle_type;
    string __particle_name;
    double __particle_mass;
    int    __mode;
    double __mean_energy;
    double __sigma_energy;
    double __min_energy;
    double __max_energy;
    bool   __randomized_direction;
    unsigned long __seed;
    mygsl::rng    __random;

  protected:
    void _check_locked (const string & where_) const;

  public: 
    bool is_debug () const;
    void set_debug (bool);
    bool is_initialized () const;
    bool is_randomized_direction () const;
    void set_randomized_direction (bool);
    const string & get_particle_name () const;
    void set_particle_name (const string &);
    double get_particle_mass () const;
    void set_particle_mass (double);

    int get_mode () const;
    void set_mode (int);

    double get_mean_energy () const;
    double get_sigma_energy () const;
    void set_mean_energy (double mean_, double sigma_ = 0.0);

    double get_min_energy () const;
    double get_max_energy () const;
    void set_energy_range (double min_, double max_);
  
  public: 
    // ctor:
    single_particle_generator ();

    // dtor:
    virtual ~single_particle_generator ();
  
    void initialize (const datatools::utils::properties & config_);

    void reset ();

    virtual bool has_next ();

  protected:

    virtual void _load_next (primary_event & event_, 
			    bool compute_classification_ = true) ;
  private:

    void __at_init ();

    void __at_reset ();

  public:
    
    static double get_particle_mass_from_label (const string & particle_name_);

    static bool particle_name_is_valid (const string & particle_name_);

  };

} // end of namespace genbb

#endif // __genbb_help__single_particle_generator_h

// end of single_particle_generator.h
