/// \file materials/isotope.h
/* Author(s)    : Benoit Guillon  <guillon@lpccaen.in2p3.fr>
 * Creation date: 2010-03-06
 * Last modified: 2014-06-11
 *
 * Description : A simple class to handle only isotopes (ZAI) and their basic properties (mass & decays).
 *
 */

#ifndef MATERIALS_ISOTOPE_H
#define MATERIALS_ISOTOPE_H 1

// Standard Library:
#include <string>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>
#include <datatools/bit_mask.h>

// This project:
#include <materials/chemical_symbol.h>

namespace materials {

  /*! \brief A simple class to handle known isotopes (ZAI) and their basic properties (mass & decays).
   *
   *  Isotope are built with a name and (Z, A, I) attributes :
   *           - name : which should be used as its id  ( = 'ChA-(I)' or '?-0' by default )
   *           - Z :  number of protons equivalent to the chemical symbol     ( = 0 by default )
   *           - A :  number of nucleons    ( = 0 by default )
   *           - I  : Isomeric state if long half-life time/metastable excited states
   *
   *                * GROUND_STATE = ground state      = ' ' (default)
   *                * ISOMERIC_M   = unique metastable = 'm'
   *                * ISOMERIC_M1  = first  metastable = 'm1'
   *                * ISOMERIC_M2  = second metastable = 'm2'
   *                * ISOMERIC_M3  = third  metastable = 'm3'
   *
   *  We suppose we will only use known isotopes, thus it's should be
   *  not possible  to create  an unknow  ZAI (except with default
   *  constructor) :
   *
   *           - For consistency, the 3 attributes (Z,A,I) cannot be initialized separately
   *           - Tabulated value of (Z,A) are searched in data file ( 'resources/mass.mas12' by default )
   *           - Note : Isomeric state (I) stay a 'free' parameter which is not checked for the moment
   *
   *    Isotope may have mass excess data [mass excess with its error] and/or mass data [mass with its error],
   *    which can be initialized with 'set_mass_excess', 'set_mass' or 'find_mass' methods.
   *
   *  Likewise, isotopes have decay data [stability flag, half-life with its error], initialized with
   *  'set_half_life' or 'find_decay' methods.
   *
   *  Note : when the 'set_zai' method is invoked, all others attributes (mass excess, mass & decay) are reinitialized to their default values.
   *
   * \author Benoit Guillon
   */
  class isotope : public datatools::i_tree_dumpable
  {
  public:

    //! Maximum number of protons for an isotope (element 'Ei')
    static int max_number_of_protons();

    //! Undefined number of nucleons for an isotope
    static int undefined_number_of_nucleons();

    //! Maximum number of nucleons for an isotope (element 'Ei')
    static int max_number_of_nucleons();

    //! \brief The identifier for an isotope based on its Z and A>=Z numbers
    class id : public datatools::i_tree_dumpable {
    public:

      //! Default constructor
      id();

      //! Constructor
      id(int z_, int a_);

      //! Reset
      void reset();

      //! Set the number of protons
      void set_z(int z_);

      //! Return the number of protons
      int get_z() const;

      //! Set the number of nucleons
      void set_a(int z_);

      //! Return the number of nucleons
      int get_a() const;

      //! Set the numbers of protons and nucleons
      void set(int z_, int a_);

      //! Return the name associated to the isotope Id
      std::string get_name() const;

      //! Export to a string identifier
      void to_string(std::string &) const;

      //! Return a string identifier
      std::string to_string() const;

      //! Import from a string identifier
      bool from_string(const std::string &);

      //! Check the validity of the isotope id
      bool is_valid() const;

      //! Comparison operator
      bool operator<(const id &) const;

      //! Comparison operator
      bool operator==(const id &) const;

      //! Smart print
      void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_          = false) const override;

    private:

      int _z_; //!< The number of protons
      int _a_; //!< The number of nucleons

    };

    //! \brief Build flags
    enum build_flag_type {
      BF_MASS_DATA  = datatools::bit_mask::bit00, //!< Extract mass excess and mass from tabulated isotopes
      BF_DECAY_DATA = datatools::bit_mask::bit01, //!< Extract decay half-life from tabulated isotopes
      BF_LOCK       = datatools::bit_mask::bit31  //!< Lock after building
    };

    //! \brief Isotope record from AME table
    struct record_type  : public datatools::i_tree_dumpable {
    public:
      //! Return Z
      int get_Z() const;
      //! Return A
      int get_A() const;
      //! Return N
      int get_N() const;
      //! Return nucleus mass
      double get_nucleus_mass() const;
      //! Return mass excess
      double get_mass_excess() const;
      //! Return binding energy per nucleon
      double get_binding_energy_per_nucleon() const;
      //! Return atomic mass
      double get_atomic_mass() const;
      //! Default constructor
      record_type();
      //! Smart print
      void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const override;

    public:

      std::string symbol; //!< Chemical symbol of the element
      int    Z;           //!< The number of protons
      int    A;           //!< The number of nucleons
      double mx;          //!< Mass excess
      double mx_err;      //!< Error on mass excess
      double bea;         //!< Binding energy per nucleon
      double bea_err;     //!< Error on binding energy per nucleon
      double am;          //!< Atomic mass
      double am_err;      //!< Error on atomic mass

   };

    //! Dictionary of isotope records
    typedef std::map<id, record_type> isotope_dict_type;

    //! Print a table of isotopes
    static void print_table_of_isotopes(const isotope_dict_type &,
                                        std::ostream & out_ = std::clog,
                                        const std::string & title_ = "",
                                        const std::string & indent_ = "",
                                        bool inherit_ = false);

    //! Return the isotope database singleton
    static const isotope_dict_type & table_of_isotopes();

    //! Check if the database knows a given isotope id
    static bool id_is_tabulated(const id &);

    enum ame_release_type {
      AME_RELEASE_2003 = 0,
      AME_RELEASE_2012 = 1
    };

    //! Load a dictionary of isotope records from the AME (2003/2012) data file
    static void load_ame_table(isotope::isotope_dict_type &, ame_release_type ame_release_ = AME_RELEASE_2012);

    //! Return the isotope record from the table
    static const record_type & table_record_from_id(const id &);

    //! \brief Isomeric level
    enum isomeric_level_type {
      GROUND_STATE           =  0,                                                   //!< Ground state (default)
      ISOMERIC_STATE         =  datatools::bit_mask::bit00,                          //!< Isomeric level
      ISOMERIC_M             =  ISOMERIC_STATE,                                      //!< Unique isomeric level
      ISOMERIC_M1            =  ISOMERIC_STATE | datatools::bit_mask::bit02,         //!< First isomeric level
      ISOMERIC_M2            =  ISOMERIC_STATE | datatools::bit_mask::bit03,         //!< Second isomeric level
      ISOMERIC_M3            =  ISOMERIC_STATE | datatools::bit_mask::bit04,         //!< Third isomeric level
      ISOMERIC_FISSION_STATE =  datatools::bit_mask::bit01,                          //!< Fission/shape isomeric level
      ISOMERIC_F             =  ISOMERIC_FISSION_STATE,                              //!< Unique fission/shape isomeric level
      ISOMERIC_F1            =  ISOMERIC_FISSION_STATE | datatools::bit_mask::bit02, //!< First fission/shape isomeric level
      ISOMERIC_F2            =  ISOMERIC_FISSION_STATE | datatools::bit_mask::bit03, //!< Second fission/shape isomeric level
      ISOMERIC_F3            =  ISOMERIC_FISSION_STATE | datatools::bit_mask::bit04, //!< Third fission/shape isomeric level
      ISOMERIC_INVALID       =  datatools::bit_mask::bit31                           //!< Invalid isomeric level
    };

    //! Return the label associated to an isomeric level
    static std::string isomeric_to_label(isomeric_level_type);

    //! Return the isomeric level associated to a label
    static isomeric_level_type label_to_isomeric(const std::string &);

    //! Default Constructor
    isotope();

    //! Constructor
    /*!
     * \param name_ : ID name
     */
    explicit isotope(const std::string & name_);

    //! Constructor
    /*!
     * \param name_ : ID name
     */
    explicit isotope(const char * name_);

    //! Constructor
    /*!
     * \param z_    : Number of protons (0 <= Z <= 118)
     * \param a_    : Number of nucleons (Z <= A  <= 293)
     * \param i_    : Isomeric state { 0 = ground state (default) ;
     *                                 1 = first excited ;
     *                                 2 = second excited ;
     *                                 3 = third excited }
     * \param build_flags_  : Flags to store additional data (mass, decay...)
     */
    explicit isotope(int z_ ,
                     int a_ ,
                     isomeric_level_type i_ = GROUND_STATE,
                     unsigned int build_flags_ = 0);

    //! Constructor
    /*!
     * \param ch_symbol_ : Chemical symbol ( "H " <=  Ch  <= "Ei")
     * \param a_    : Number of nucleons (Z <= A  <= 293)
     * \param i_    : Isomeric state { 0 = ground state (default) ;
     *                                 1 = first excited ;
     *                                 2 = second excited ;
     *                                 3 = third excited }
     * \param build_flags_  : Flags to store additional data (mass, decay...)
     */
    explicit isotope(const std::string & ch_symbol_,
                     int a_ ,
                     isomeric_level_type i_ = GROUND_STATE,
                     unsigned int build_flags_ = 0);

    //! Constructor
    /*!
     * \param ch_symbol_ : Chemical symbol ( "H " <=  Ch  <= "Ei")
     * \param a_    : Number of nucleons (Z <= A  <= 293)
     * \param i_    : Isomeric state { 0 = ground state (default) ;
     *                                 1 = first excited ;
     *                                 2 = second excited ;
     *                                 3 = third excited }
     * \param build_flags_  : Flags to store additional data (mass, decay...)
     */
    explicit isotope(const char * ch_symbol_,
                     int a_ ,
                     isomeric_level_type i_ = GROUND_STATE,
                     unsigned int build_flags_ = 0);

    //! Constructor
    /*!
     * \param name_ : ID name
     * \param z_    : Number of protons (0 <= Z <= 118)
     * \param a_    : Number of nucleons (Z <= A  <= 293)
     * \param i_    : Isomeric state { 0 = ground state (default) ;
     *                                 1 = first excited ;
     *                                 2 = second excited ;
     *                                 3 = third excited }
     * \param build_flags_  : Flags to fetch additional data
     */
    explicit isotope(const std::string & name_,
                     int z_ ,
                     int a_ ,
                     isomeric_level_type i_ = GROUND_STATE,
                     unsigned int build_flags_ = 0);

   //! Constructor
    /*!
     * \param name_ : ID name
     * \param z_    : Number of protons (0 <= Z <= 118)
     * \param a_    : Number of nucleons (Z <= A  <= 293)
     * \param i_    : Isomeric state { 0 = ground state (default) ;
     *                                 1 = first excited ;
     *                                 2 = second excited ;
     *                                 3 = third excited }
     * \param build_flags_  : Flags to fetch additional data
     */
    explicit isotope(const char * name_,
                     int z_ ,
                     int a_ ,
                     isomeric_level_type i_ = GROUND_STATE,
                     unsigned int build_flags_ = 0);


    //! Constructor
    /*!
     * \param name_      : ID name
     * \param ch_symbol_ : Chemical symbol ( "H " <=  Ch  <= "Ei")
     * \param a_         : Number of nucleons (Z <= A  <= 293)
     * \param i_         : Isomeric state { 0 = ground state (default);
     *                                      1 = first excited ;
     *                                      2 = second excited ;
     *                                      3 = third excited }
     * \param build_flags_ : Flags to fetch additional data
     */
    explicit isotope(const std::string & name_,
                     const std::string & ch_symbol_,
                     int a_,
                     isomeric_level_type i_ = GROUND_STATE,
                     unsigned int build_flags_ = 0);

    //! Constructor
    /*!
     * \param name_      : ID name
     * \param ch_symbol_ : Chemical symbol ( "H " <=  Ch  <= "Ei")
     * \param a_         : Number of nucleons (Z <= A  <= 293)
     * \param i_         : Isomeric state { 0 = ground state (default);
     *                                      1 = first excited ;
     *                                      2 = second excited ;
     *                                      3 = third excited }
     * \param build_flags_ : Flags to fetch additional data
     */
    explicit isotope(const char * name_,
                     const char * ch_symbol_,
                     int a_,
                     isomeric_level_type i_ = GROUND_STATE,
                     unsigned int build_flags_ = 0);

    //! Destructor
    ~isotope() override;

    //! Return true if isotope is valid, false either
    bool is_valid() const;

    //! Search & set the mass excess and mass from the tabulated isotopes
    bool find_mass_data();

    //! Search & set the half-life from the tabulated isotopes
    bool find_decay_data();

    //! Return the name
    const std::string & get_name() const;

    //! Return the chemical symbol
    const std::string & get_chemical_symbol() const;

    //! Return the zai name : 'ChA(I)'
    std::string get_zai_name() const;

    //! Return Z (number of protons)
    int get_z() const;

    //! Return A (number of nucleons)
    int get_a() const;

    //! Return N (number of neutrons)
    int get_n() const;

    //! Return the isotope Id
    id get_id() const;

    //! Return the mass in unit of energy
    double get_mass() const;

    //! Return the isomeric state
    isomeric_level_type get_isomeric() const;

    //! Check ground state
    bool is_ground_state() const;

    //! Check if the isotope is known from tables
    bool is_known() const;

    //! Return true if atomic mass data have been set properly, false either
    bool has_atomic_mass_data() const;

    //! Return true if mass excess have been set properly, false either
    bool has_mass_excess_data() const;

    //! Return the mass excess in unit of energy
    double get_mass_excess() const;

    //! Return the error on mass excess in unit of energy
    double get_err_mass_excess() const;

    //! Return the atomic mass in unit of [u]
    double get_atomic_mass() const;

    //! Return the error on the atomic mass in unit of [u]
    double get_err_atomic_mass() const;

    //! Set the number of protons  :
    /*!
     * \param z_ :  0 <  Z  <= 119  (check if exist)
     */
    void set_z(int z_);

    //! Set the number of nucleons :
    /*!
     * \param a_ :  0 <  A  <= 293
     */
    void set_a(int a_);

    //! Set the isomeric state :
    /*!
     * \param i_ : isomeric state
     */
    void set_isomeric(isomeric_level_type i_);

    //! Set Z, A, I attributes
    /*!
     * \param z_ : Number of protons  (0 <= Z  <= 119)
     * \param a_ : Number of nucleons (Z <= A  <= 293)
     * \param i_  : Isomeric state { 0 = ground state (default);
     *                               1 = first excited ;
     *                               2 = second excited ;
     *                               3 = third excited }
     */
    void set_zai(int z_, int a_, isomeric_level_type i_ = GROUND_STATE);

    //! Check if the mass of the nucleus is defined
    bool has_mass() const;

    //! Set the name
    /*!
     * \param name_ : user name  of the isotope
     */
    void set_name(const std::string & name_);

    //! Set the mass
    /*!
     * \param mass_     : Mass of the nucleus
     */
    void set_mass(double mass_);

    //! Set the mass excess and its error in explicit unit of energy
    /*!
     * \param mass_excess_     : Mass excess
     * \param err_mass_excess_ : Mass excess error
     */
    void set_mass_excess(double mass_excess_, double err_mass_excess_ = 0.0);

    //! Set the binding energy per nucleon and its error in explicit unit of energy
    /*!
     * \param bea_     : Binding energy per nucleon
     * \param err_bea_ : Binding energy per nucleon error
     */
    void set_binding_energy_per_nucleon(double bea_, double err_bea_ = 0.0);

    //! Set the atomic mass and its error in implicit unit of [u]
    /*!
     * \param mass_     : Atomic mass in implicit unit of [u]
     * \param err_mass_ : Atomic mass error in implicit unit of [u]
     */
    void set_atomic_mass(double mass_, double err_mass_ = 0.0);

    //! Set the decay data
    /*!
     * \param half_life_     : Half-life in explicit unit of time
     * \param err_half_life_ : Half-life error in explicit unit of time
     */
    void set_half_life(double half_life_, double err_half_life_ = 0.0);

    //! Return true if decay data have been set properly, false either
    bool has_decay_data() const;

    //! Return true if isotope is stable, false either
    bool is_stable() const;

    //! Return the half-life in unit of time
    double get_half_life() const;

    //! Return the error on half-life in unit of time
    double get_err_half_life() const;

    //! Give access to properties
    datatools::properties & grab_properties();

    //! Give access to properties
    const datatools::properties & get_properties() const;

    //! Return true if isotope is locked (i.e. fully set), false either
    bool is_locked() const;

    //! Lock the isotope
    void lock();

    //! Unlock the isotope
    void unlock();

    //! Load additional data in the isotope data structure
    void build(unsigned int flags_ = 0);

    //! Set data to default values
    void set_default_data();

    //! Reset
    void reset();

    //! Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_          = false) const override;

    //! Compute the atomic electron binding energy for ion with given Z
    static double compute_electron_binding_energy(int z_);

    //! Compute the mass of the nucleus from its Z, A and binding energy per nucleon (in unit of energy)
    static double compute_nucleus_mass(int z_, int a_, double binding_energy_per_nucleon_);

    //! Compute the mass of the nucleus from its Z and atomic mass (in unit of energy)
    static double compute_nucleus_mass(int z_, double atomic_mass_);

  protected:

    //! Set default attributes
    void _set_defaults();

  private :

    bool _locked_;                  //!< Lock flag
    int _z_;                        //!< Number of protons   (0<Z<=119)
    int _a_;                        //!< Number of nucleons  (Z<=A<=293)
    isomeric_level_type _isomeric_; //!< Isomeric states I={0,1,2,3} ={' ','M','N','O'} for "long half-life" time excited states
    std::string _name_;             //!< Name of the isotope, which could be used as an string identifier
    double _mass_;            //!< Mass of the nucleus in unit of energy
    double _mass_excess_;     //!< Mass excess in unit of energy
    double _err_mass_excess_; //!< Error on mass excess in unit of energy
    double _bea_;             //!< Binding energy per nucleonin unit of energy
    double _err_bea_;         //!< Error on finding energy per nucleonin unit of energy
    double _atomic_mass_;     //!< Atomic mass in unit of [u (or g/mol)]
    double _err_atomic_mass_; //!< Error on the atomic mass in unit of [u (or g/mol)]
    double _half_life_;       //!< Half life time in unit of time
    double _err_half_life_;   //!< Error on the half life time in unit of time
    bool   _is_known_;        //!< Boolean flag [false by default] = true is (Z,A) values are founded in file mass.mas03
    datatools::properties _properties_; //!< Auxiliary properties

  }; // end of class isotope

} // end of namespace materials

#endif // MATERIALS_ISOTOPE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
