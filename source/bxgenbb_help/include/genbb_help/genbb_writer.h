// -*- mode: c++; -*- 
/* genbb_writer.h
 * Author(s):     Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 * Creation date: 2010-01-20
 * Last modified: 2010-01-20
 * 
 * License: 
 * 
 * Description: 
 *   Create GENBB input data
 * 
 * History: 
 * 
 */

#ifndef __genbb_help__genbb_writer_h
#define __genbb_help__genbb_writer_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <CLHEP/Vector/ThreeVector.h>

#include <datatools/serialization/serialization.h>
#include <datatools/serialization/io_factory.h>

#include <geomtools/utils.h>

#include <genbb_help/genbb_mgr.h>

namespace genbb {

  class genbb_writer
  {

 private:

    bool        __debug;
    std::string __fout;


  private:

    int    __Npart, __part_per_evt;
    int    __type, __seed;
    double __Emin, __Emax;
    double __phi_min, __phi_max;


  public:

    void run();

    void set_seed ( int seed_ )
    { __seed = seed_; }
    int get_seed() const { return __seed; }

    void set_npart ( int npart_ )
    { __Npart = npart_; }
    int get_npart() const { return __Npart; }

    void set_part_per_evt ( int part_per_evt_ )
    { __part_per_evt = part_per_evt_; }
    int get_part_per_evt() const { return __part_per_evt; }

    void set_type ( int type_ )
    { __type = type_; }
    int get_type() const { return __type; }

    void set_Emin ( double Emin_ )
    { __Emin = Emin_; }
    double get_Emin() const { return __Emin; }

    void set_Emax ( double Emax_ )
    { __Emax = Emax_; }
    double get_Emax() const { return __Emax; }

    void set_phi_min ( double phi_min_ )
    { __phi_min = phi_min_; }
    double get_phi_min() const { return __phi_min; }

    void set_phi_max ( double phi_max_ )
    { __phi_max = phi_max_; }
    double get_phi_max() const { return __phi_max; }

    void set_outfile ( std::string fout_ ) { __fout = fout_; }
    std::string get_outfile() const { return __fout; }


  public:

    void set_debug ( bool d_ )
    {__debug = d_; }
    bool is_debug () const { return __debug; }

    void dump ( std::ostream & out_ ) const;

    // ctor/dtor:
    genbb_writer ();
    
    virtual ~genbb_writer ();
    

  private:

    double myrandom ( double min_, double max_ );

    void init();

  };

} // end of namespace genbb

#endif // __genbb_help__genbb_writer_h

// end of genbb_writer.h
