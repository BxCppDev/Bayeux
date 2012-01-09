// -*- mode: c++ ; -*-
/* isotope.cc
 */

#include <materials/isotope.h>

#include <fstream>
#include <stdexcept>
#include <sstream>
#include <materials/ch_symbol.h>

double endfline_to_double (const string & endfline_);
double ame3line_to_double (const string & ame3_line_);

namespace mat {

  //________________________________________________________________________// ctor
  isotope::isotope ()
  {
    _z_ = 0;
    _a_ = 0;
    _i_ = 0;

    _is_known_ = false;

    _init_ ();
    return;
  }

  //________________________________________________________________________// ctor
  isotope::isotope (const string & name_)
  {
    /*!
      \param name_ : ID name

      If name_ is set to '', it will be automatically initialize to 'Ch-A(I)' or '?-0'
    */

    _set_name_ ( name_ );

    _z_ = 0;
    _a_ = 0;
    _i_ = 0;

    _is_known_ = false;

    _init_ ();
    return;
  }


  //________________________________________________________________________ // ctor
  isotope::isotope (const string & name_, const int z_ , const int A_ , const int i_, bool build_)
  {
    /*!
      \param name_ : ID name
      \param z_ : Number of protons  ( 0 <  Z  <= chemical_symbol::NB_CHEMICAL_SYMBOLS)
      \param A_ : Number of nucleons (Z <= A  <= 293)
      \param i_  : Isomeric state { 0 = ground state =' ' ; 1 = first excited ='M' ;  2 =.....='N'  ;  3 = third excited='O' }

      Default : Isotope in ground state (I=0)

      If name_ is set to '', it will be automatically initialize to 'Ch-A(I)' or '?-0'
    */
    _set_name_ ( name_ );

    set_zai (z_ , A_, i_);

    if (build_) build ();
    return;
  }

  //________________________________________________________________________ // ctor
  isotope::isotope(const string & name_,
                   const string & ch_symbol_ ,
                   const int A_ ,
                   const int i_,
                   bool build_)
  {
    /*!
      \param name_ : ID name
      \param ch_symbol_ : Chemical symbol ( "H " <=  Ch  <= "Ei")
      \param A_ : Number of nucleons (Z <= A  <= 293)
      \param i_  : Isomeric state { 0 = ground state =' ' ; 1 = first excited ='M' ;  2 =.....='N'  ;  3 = third excited='O' }

      Default : Isotope in ground state (I=0)

      If name_ is set to '', it will be automatically initialize to 'Ch-A(I)' or '?-0'
    */
    _set_name_ ( name_ );

    set_zai (chemical_symbol::z_from_symbol (ch_symbol_) , A_, i_);
    if (build_) build ();
    return;
  }

  //________________________________________________________________________  dtor:
  isotope::~isotope ()
  {
    return;
  }

  //________________________________________________________________________
  void isotope::_init_ ()
  {
    if( _name_.length () == 0  ||  _name_.find ("?-0") != string::npos )
      {
        _set_name_( get_zai_name () );
      }

    _mass_=0.;
    _err_mass_=0.;

    _half_life_time_ = -1.;
    _err_half_life_time_ = -1.;
    return;
  }

  //________________________________________________________________________
  void isotope::_check_za_ ()
  {
    _is_known_ = false;

    //-----------------  Open an ifstream from file mass.mas03  ----------------------------

    string tape_name;

    if (getenv("MATERIALS_ROOT")==NULL)
      {
        ostringstream message;
        message << "isotope::_check_za_ : env. variable '$MATERIALS_ROOT'  not found !";
        throw runtime_error (message.str ());
      }
    else
      {
        tape_name.assign(getenv("MATERIALS_ROOT"));
        tape_name +="/resources/mass.mas03";
      }

    ifstream ifstr_tape;
    ifstr_tape.open(tape_name.c_str ());
    if(! ifstr_tape.is_open ())
      {
        ostringstream message;
        message << "isotope::_check_za_ : ifstream  '" << tape_name<< "'  not found !";
        throw runtime_error (message.str ());
      }


    //----------------------------  Read the ifstream  ----------------------------

    _is_known_  = false;

    string ame03_line;

    for(int i = 0 ; i<=39 ; i++) getline(ifstr_tape, ame03_line);

    while( !_is_known_ && getline(ifstr_tape, ame03_line) )
      {
        if( atoi((ame03_line.substr(9, 5)).c_str ()) == _z_  &&  atoi((ame03_line.substr(14, 5)).c_str ()) == _a_ )
          {
            _is_known_ =true ;
          }
      }

    if(!_is_known_)
      {
        ostringstream message;
        message<< "isotope::_check_za_: " << get_name ()<<endl;
        message << "  -> (Z, A) values : (" << _z_<< ", "<< _a_<< ") not found in database ['" << tape_name<<"'] !"<<endl;
        throw runtime_error (message.str ());
      }
    return;
  }

  //________________________________________________________________________
  void isotope::set_name( const string & name_ )
  {
    /*!
      \param name_ : ID name
    */
    _set_name_( name_ );
    return;
  }

  //________________________________________________________________________
  void isotope::set_zai ( const  int z_, const int A_, const int I_ )
  {
    /*!
      \param z_ : Number of protons  ( 0 <  Z  <= chemical_symbol::NB_CHEMICAL_SYMBOLS)
      \param A_ : Number of nucleons (Z <= A  <= 293)
      \param I_  : Isomeric state { 0 = ground state =' ' ; 1 = first excited ='M' ;  2 =.....='N'  ;  3 = third excited='O' }

      Default : Isotope in ground state (I=0)
    */
    _set_z_ (z_);
    _set_a_ (A_);
    _set_i_ (I_);

    _check_za_ ();

    _init_ ();
    return;
  }

  //________________________________________________________________________
  void isotope::set_mass( const  double mass_, const double err_mass_ )
  {

    /*!
      \param mass_       : Mass in gramm per mol  [g/mol]
      \param err_mass_ : Mass error in gramm per mol  [g/mol]
    */
    _set_mass_ (mass_, err_mass_);
    return;
  }


  //________________________________________________________________________
  void isotope::find_mass (const string & input_file_name_ )
  {
    /*!
      \param input_file_name_ : Name of the input data file
    */

    //-----------------  Open an ifstream from file mass.mas03  ----------------------------

    string tape_name;

    if (getenv ("MATERIALS_ROOT") == NULL)
      {
        ostringstream message;
        message << "isotope::find_mass : env. variable '$MATERIALS_ROOT'  not found !";
        throw runtime_error (message.str ());
      }
    else
      {
        tape_name.assign(getenv ("MATERIALS_ROOT"));
        tape_name += "/resources/";
        tape_name += input_file_name_;
      }

    ifstream ifstr_tape;
    ifstr_tape.open (tape_name.c_str ());
    if (! ifstr_tape.is_open ())
      {
        ostringstream message;
        message << "isotope::find_mass () : ifstream  '" << tape_name << "'  not found !";
        throw runtime_error (message.str ());
      }


    //----------------------------  Read the ifstream  ----------------------------

    bool is_za_found  = false;
    _mass_ = -1.;
    _err_mass_ = -1.;

    string ame03_line;

    for (int i = 0 ; i<=39 ; i++) getline (ifstr_tape, ame03_line);

    while ( ! is_za_found && getline (ifstr_tape, ame03_line) )
      {
        string z_str = ame03_line.substr (9, 5);
        string a_str = ame03_line.substr (14, 5);
        //cerr << "DEVEL: find_mass: z_str=" << z_str << endl;
        //cerr << "DEVEL: find_mass: a_str=" << a_str << endl;
        if ( (atoi (z_str.c_str ()) == _z_) && (atoi (a_str.c_str ()) == _a_))
          {
            is_za_found = true;
            string m_str = ame03_line.substr(96, 4);
            string m2_str = ame03_line.substr(100, 12);
            string m3_str = ame03_line.substr(113, 11);
            //cerr << "DEVEL: find_mass:   m_str=" << m_str << endl;
            //cerr << "DEVEL: find_mass:   m2_str=" << m2_str << endl;
            //cerr << "DEVEL: find_mass:   m3_str=" << m3_str << endl;
            double mass = ame3line_to_double (m_str);
            double mass_default = ame3line_to_double (m2_str) * 1.E-6;
            double error_mass = ame3line_to_double (m3_str) * 1.E-6;
            //cerr << "DEVEL: find_mass:   mass=" << mass << " amu" << endl;
            //cerr << "DEVEL: find_mass:   mass_default=" << mass_default << " amu" << endl;
            //cerr << "DEVEL: find_mass:   error_mass=" << error_mass << " amu" << endl;
            _set_mass_ (mass + mass_default, error_mass);
          }
      }
    //cerr << "DEVEL: find_mass: " << "ZA found: " << is_za_found << endl;

    if (! is_za_found)
      {
        ostringstream message;
        message << "isotope::find_mass: Z A values : '" << _z_ << " " << _a_ << "' not found in '"
                << tape_name << "' !";
        throw runtime_error (message.str ());
        /*
          cerr<<endl<< endl<< "!!! WARNING !!! isotope::find_mass (): Z A values : '" << _z_<< " "<< _a_<< "' not found in file mass.mas03 !"<< endl<< endl;
        */
      }
    return;
  }

  //________________________________________________________________________
  void isotope::set_decay( const double half_life_time_,  const double err_half_life_time_ )
  {

    /*!
      \param half_life_time_       : Half-life time in second [s]
      \param err_half_life_time_ : Half-life time error in second [s]
    */
    _set_half_life_time_(half_life_time_, err_half_life_time_ );
    return;
  }



  //________________________________________________________________________
  const string & isotope::get_ch_symbol () const
  {
    return chemical_symbol::symbol_from_z(_z_);
  }

  //________________________________________________________________________
  string isotope::get_zai_name ()  const
  {

    if( _z_ ==0 && _a_ ==0 && _i_ ==0)
      {
        return "?-0";
      }
    else
      {
        // Chemical symbol :

        string zai_name = get_ch_symbol ();

        // Number of nucleons :

        zai_name += "-";
        std::stringstream s_A;
        s_A<<_a_;
        zai_name += s_A.str ();

        // Isomeric state

        if(_i_>0)
          {
            char I_buffer[][4] = { " ", "M", "N", "O"};
            zai_name+= "(";
            zai_name+= I_buffer[_i_];
            zai_name+= ")";
          }
        return zai_name;
      }
  }


  //________________________________________________________________________
  void isotope::_set_z_( const  int z_)
  {
    /*!
      \param z_ : Number of protons  ( 0 <  Z  <= chemical_symbol::NB_CHEMICAL_SYMBOLS)
    */

    if(  z_ >  0  &&  z_ <= chemical_symbol::NB_CHEMICAL_SYMBOLS )
      {
        _z_ = z_;
      }
    else
      {
        ostringstream message;
        message<< "isotope::_set_z_ (): " << get_name ()<<endl;
        message << "       -> invalid value : '" << _z_<< "' !"<<endl;
        throw logic_error (message.str ());
      }
    return;
  }
  //________________________________________________________________________
  void isotope::_set_a_( const  int A_)
  {
    /*!
      \param A_ : Number of nucleons  ( 0 <  Z  <= chemical_symbol::NB_CHEMICAL_SYMBOLS)
    */

    if(  A_ >= _z_  &&  A_ <= 293)
      {
        _a_ = A_;
      }
    else
      {
        ostringstream message;
        message << "isotope::_set_a_ () : Invalid A value : '" << A_ << "' !";
        throw logic_error (message.str ());
      }
    return;
  }
  //________________________________________________________________________
  void isotope::_set_i_( const  int I_)
  {
    /*!
      \param I_ : Isomeric state { 0 = ground state =' ' ; 1 = first excited ='M' ;  2 =.....='N'  ;  3 = third excited='O' }
    */

    if(  I_ >= GROUND_STATE  &&  I_ <= THIRD_EXCITED )
      {
        _i_ = I_;
      }
    else
      {
        ostringstream message;
        message << "isotope::_set_i_ () : Invalid I value : '" << I_ << "' !";
        throw logic_error (message.str ());
      }
    return;
  }

  //________________________________________________________________________
  void isotope::_set_name_(const string & name_)
  {
    /*!
      \param name_ : ID name
    */
    _name_ = name_;
    return;
  }

  //________________________________________________________________________
  void  isotope::_set_mass_(const double mass_, const double err_mass_)
  {
    if ((mass_ < 0.) || (err_mass_ < 0.))
      {
        ostringstream message;
        message << "isotope:::_set_mass_ () : Invalid mass (+_ error) values : '" << mass_ << "' '" << err_mass_ << " '!";
        throw logic_error (message.str ());
      }
    else
      {
        //cerr << "DEVEL: isotope::_set_mass_: mass=" << mass_ << endl;
        //cerr << "DEVEL: isotope::_set_mass_: error mass=" << err_mass_ << endl;
        _mass_ = mass_;
        _err_mass_ = err_mass_;
      }
    return;
  }

  //________________________________________________________________________
  void   isotope::_set_half_life_time_(const double half_life_time_ , const double err_half_life_time_)
  {
    if(  half_life_time_ < 0. || err_half_life_time_ < 0. )
      {
        ostringstream message;
        message << "isotope:::_set_half_life_time_ () : Invalid half_life_time value : '"
                << half_life_time_  << " +- '"
                << err_half_life_time_<< "' !";
        throw logic_error (message.str ());
      }

    else if ( half_life_time_ == 0. && err_half_life_time_ == 0. )

      {
        _half_life_time_ = half_life_time_;
        _err_half_life_time_ = err_half_life_time_;
      }
    else
      {
        _half_life_time_ = half_life_time_;
        _err_half_life_time_ = err_half_life_time_;
      }
    return;
  }

  //________________________________________________________________________
  void isotope::find_decay(const  string & input_file_name_)
  {
    /*!
      \param input_file_name_ : Name of the input data file
    */
    //-----------------  Open an ifstream from file JEFF311RDD_ALL.OUT ----------------------------

    string tape_name;

    if (getenv("MATERIALS_ROOT")==NULL)
      {
        ostringstream message;
        message << "isotope::find_decay : env. variable '$MATERIALS_ROOT'  not found !";
        throw runtime_error (message.str ());
      }
    else
      {
        tape_name.assign(getenv("MATERIALS_ROOT"));
        tape_name +="/resources/";
        tape_name +=input_file_name_;
      }
    ifstream ifstr_tape;
    ifstr_tape.open(tape_name.c_str ());
    if(!ifstr_tape.is_open ())
      {
        ostringstream message;
        message << "isotope::find_decay () : ifstream  '" << tape_name<< "'  not founded !";
        throw runtime_error (message.str ());
      }


    //----------------------------  Read the ifstream  ----------------------------


    bool is_data_found = false;
    bool is_zai_found  = false;
    string endf6_I[4]={" ", "M", "N", "O"};

    string endf6_line;

    while( !is_zai_found && getline(ifstr_tape, endf6_line) )
      {
        if(endf6_line.find("451    5")!=string::npos)
          {
            if( atoi((endf6_line.substr(0, 3)).c_str ()) == _z_  &&  atoi((endf6_line.substr(7, 3)).c_str ()) == _a_
                &&  !endf6_I[_i_].compare(endf6_line.substr(10, 1))    )
              {
                is_zai_found  = true ;
                /*  symbol  = endf6_line.substr(4, 2)    /    data_base = endf6_line.substr(11, 7)    /    name = endf6_line.substr(0, 11) */
              }
          }
      }
    if(!is_zai_found)
      {
        ostringstream message;
        message<< "isotope::find_decay: " << get_name ()<<endl;
        message << "       -> Z A I values : '" << _z_<< " "<< _a_<< " "<<  _i_<< "' not found in '" << tape_name<<"' !"<<endl;
        throw runtime_error (message.str ());
      }
    else
      {
        while( !is_data_found  && getline(ifstr_tape, endf6_line) )
          {
            if(endf6_line.find("457    1")!=string::npos)
              {
                if((endfline_to_double(endf6_line.substr(0, 11)))!=(1000.*get_z ()+get_a ()))
                  {
                    ostringstream message;
                    message << "isotope::find_decay : zai not consistent !";
                    throw runtime_error (message.str ());
                  }
                else
                  {
                    is_data_found  = true ;

                    getline(ifstr_tape, endf6_line);

                    _set_half_life_time_( endfline_to_double(endf6_line.substr(0, 11)) , endfline_to_double(endf6_line.substr(11, 11)));
                  }
              }
          }

        if (! is_data_found)
          {
            /*  ostringstream message;
                message << "isotope::find_decay () : 457 data not found in jeff-3.1 for Z A I values : '" << _z_<< " "<< _a_<< " "<<  _i_<< "' !"<< endl;
                throw runtime_error (message.str ()); */
            cerr << "isotope::find_decay () : 457 data not found in jeff-3.1 for Z A I values : '" << _z_<< " "<< _a_<< " "<<  _i_<< "' !"<< endl;
          }
        else
          {
          }
      }
    return;
  }

  //________________________________________________________________________
  bool isotope::has_mass_data () const {
    if ((_mass_ > 0.) && (_err_mass_ >= 0.)) return true;
    else  return false;
  }

  //________________________________________________________________________
  bool isotope::has_decay_data () const {
    if ((_half_life_time_ >= 0.) && (_err_half_life_time_ >=0.)) return true;
    else  return false;
  }


  //________________________________________________________________________
  bool isotope::is_stable () const {
    if( _half_life_time_ == 0. && _err_half_life_time_ == 0. ) return true;
    else  return false;
  }


  //________________________________________________________________________
  bool  isotope::is_known () const {
    return _is_known_;
  }


  //________________________________________________________________________
  bool isotope::is_locked () const {
    if(is_known () && has_mass_data ())
      {
        if (is_stable ())
          {
            return true;
          }
        else if (has_decay_data ())
          {
            return true;
          }
        else
          {
            // accept locked isotope without decay data
            return true;
          }
      }
    return false;
  }

  //________________________________________________________________________
  void isotope::_build (bool use_decay_)
  {
    bool use_decay_db = false;
    find_mass ();
    if (use_decay_db)
      {
        find_decay ();
      }
    return;
  }

  //________________________________________________________________________
  void isotope::build ()
  {
    bool use_mass_db = true;
    bool use_decay_db = false;
    _build (use_decay_db);
    return;
  }

  //________________________________________________________________________
  void isotope::tree_dump (ostream & out_,
                           const string & title_,
                           const string & indent_,
                           bool inherit_) const
  {

    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ())
      {
        out_ << indent << title_ << endl;
      }
    out_ << indent << i_tree_dumpable::tag
         << "Name       :'" << get_name () << "'" << endl;
    if (is_known ())
      {
        out_ << indent << i_tree_dumpable::tag
             << "Z, A, I    : " << get_z () << ", "
             <<  get_a () << ", " << get_i () << "" << endl;
        out_ << indent << i_tree_dumpable::tag
             << "zai name   : '" << get_zai_name () << "'"  <<endl;
      }
    else
      {
        out_ << indent << i_tree_dumpable::tag
             << "Z, A, I    : "
             <<  "no data ( default values : "
             << get_z () << ", " << get_a () << ", "
             << get_i () <<" ) " << endl;
        out_ << indent << i_tree_dumpable::tag
             << "zai name   : " << "no data (default values : '"
             << get_zai_name () << "') " << endl;
      }

    if (has_mass_data ())
      {
        out_ << indent << i_tree_dumpable::tag
             << "Mass       : "<<  get_mass () << " +- "
             << get_err_mass () << " [g/mol]" << endl;
      }
    else
      {
        out_ << indent << i_tree_dumpable::tag
             << "Mass       : " <<  "no data ( default values : "
             << get_mass () << " +- " << get_err_mass ()
             << " [g/mol] ) " << endl;
      }

    if (has_decay_data ())
      {
        if (is_stable ()) out_ << indent << i_tree_dumpable::tag << "Stable     : true"   << endl;

        if (! is_stable ())
          {
            out_ << indent << i_tree_dumpable::tag << "Unstable   : ";
            out_ << "T1/2 = "
                 << get_half_life_time () << " +- "
                 << get_err_half_life_time () << " [s]" << endl;
          }
      }
    else
      {
        out_ << indent << i_tree_dumpable::tag
             << "Stability  : " <<  "no data (default values : ";
        if(is_stable ()) out_ << "stable & ";
        if(! is_stable ()) out_ << "unstable & ";
        out_ << " T1/2 = " << get_half_life_time ()
             << " +- " << get_half_life_time () << " [s] )" << endl;
      }

    if(_properties_.size () != 0)
      {
        out_ << indent << du::i_tree_dumpable::tag << "Properties : " << endl;
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << du::i_tree_dumpable::skip_tag;
        _properties_.tree_dump (out_, "", indent_oss.str ());
      }
    else
      {
        out_ << indent << i_tree_dumpable::tag
             << "Properties : " <<  "empty" << endl;
      }


    out_ << indent << i_tree_dumpable::last_tag
         << "Locked     : " << (is_locked ()? "Yes": "No") << endl;

    return;
  }

} // end of namespace mat


//________________________________________________________________________
double endfline_to_double (const string & endf_line)
{
  //  Convert a string (of size 11) written in endf6 format to a double
  //  3 differents formats

  string s_number;
  if(endf_line.find("E")==string::npos)
    {
      int pos;
      if (endf_line.find("+")!=string::npos) pos = endf_line.find_first_of ("+");
      if (endf_line.find("-")!=string::npos) pos = endf_line.find_first_of ("-");
      s_number = endf_line.substr (0, pos) + "E";
      s_number += endf_line.substr (pos, endf_line.size () - pos);
    }
  else s_number = endf_line;

  std::istringstream iss(s_number);

  double number;
  iss >> number;
  if (! iss)
    {
      ostringstream message;
      message << "endfline_to_double: Invalid format for real: '"
              << s_number << "' !";
      throw runtime_error (message.str ());
      //return 0;
    }
  return number;
}

//________________________________________________________________________
double ame3line_to_double(const string & ame3_line)
{
  string s_number;

  if( (ame3_line.find("*")) !=string::npos)  s_number="0";
  else if( (ame3_line.find("#")) !=string::npos)  s_number=ame3_line.substr(0, ame3_line.find_first_of("#"));
  else   s_number = ame3_line;

  std::istringstream iss(s_number);
  double number;
  iss >> number;
  if (! iss)
    {
      ostringstream message;
      message << "ame3line_to_double: Invalid format for real: '"
              << s_number << "' !";
      throw runtime_error (message.str ());
      //return 0;
    }
  //if (!(i >> number))   return 0;
  else return number;
}

// end of isotope.cc
