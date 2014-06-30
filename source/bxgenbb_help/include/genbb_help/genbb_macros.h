/// \file genbb_help/genbb_macros.h
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-04-24
 * Last modified : 2013-02-25
 */

/* \brief Macros to automatically generate some parts of the particle generator class interface.
 *
 */

#ifndef GENBB_GENBB_MACROS_H
#define GENBB_GENBB_MACROS_H 1

// This project:
#include <genbb_help/i_genbb.h>

#define GENBB_PG_CLASS_DECLARE(T)              \
  class T : public ::genbb::i_genbb \
  /**/

#define GENBB_PG_IS_INITIALIZED_DECLARE()      \
  public:                                       \
  virtual bool is_initialized () const;         \
  /**/

#define GENBB_PG_IS_INITIALIZED_IMPLEMENT_HEAD(T)  \
  bool T::is_initialized () const                   \
  /**/

#define GENBB_PG_HAS_NEXT_VERTEX_DECLARE()     \
  public:                                       \
  virtual bool has_next_vertex () const;        \
  /**/

#define GENBB_PG_HAS_NEXT_VERTEX_IMPLEMENT_HEAD(T) \
  bool T::has_next_vertex () const                  \
  /**/

#define GENBB_PG_INITIALIZE_DECLARE()                      \
  public:                                                   \
  virtual void initialize (const ::datatools::properties &, \
                           ::datatools::service_manager &,  \
                           ::genbb::vg_dict_type &);       \
  /**/

#define GENBB_PG_INITIALIZE_IMPLEMENT_HEAD(T,CONF,SERVICE_MANAGER,DICT) \
  void T::initialize (const ::datatools::properties & CONF,             \
                      ::datatools::service_manager & SERVICE_MANAGER,   \
                      ::genbb::vg_dict_type & DICT)                    \
  /**/

#define GENBB_PG_RESET_DECLARE()               \
  public:                                       \
  virtual void reset ();                        \
  /**/

#define GENBB_PG_RESET_IMPLEMENT_HEAD(T)       \
  void T::reset ()                              \
  /**/

#define GENBB_PG_SHOOT_VERTEX_DECLARE()                                \
  protected :                                                           \
  virtual void _shoot_vertex (::mygsl::rng & random_, ::geomtools::vector_3d & vertex_); \
  /**/

#define GENBB_PG_SHOOT_VERTEX_IMPLEMENT_HEAD(T,Random,Vertex)          \
  void T::_shoot_vertex (::mygsl::rng & Random, ::geomtools::vector_3d & Vertex) \
  /**/

#define GENBB_PG_CONSTRUCTOR_DECLARE(T)        \
  public:                                       \
  T ();                                         \
  /**/

#define GENBB_PG_CONSTRUCTOR_IMPLEMENT_HEAD(T) \
  T::T ()                                       \
    : genvtx::i_genbb ()             \
      /**/

#define GENBB_PG_DESTRUCTOR_DECLARE(T)         \
  public:                                       \
  virtual ~T ();                                \
  /**/

#define GENBB_PG_DESTRUCTOR_IMPLEMENT_HEAD(T)  \
  T::~T ()                                      \
  /**/

#define GENBB_PG_DEFAULT_DESTRUCTOR_IMPLEMENT(T) \
  GENBB_PG_DESTRUCTOR_IMPLEMENT_HEAD (T)         \
  {                                               \
    if (is_initialized ()) reset ();              \
    return;                                       \
  }                                               \
  /**/

/*** Macro for the declaration of the minimal interface ***/
#define GENBB_PG_INTERFACE()                   \
  GENBB_PG_IS_INITIALIZED_DECLARE();           \
  GENBB_PG_INITIALIZE_DECLARE();               \
  GENBB_PG_RESET_DECLARE();                    \
  GENBB_PG_SHOOT_VERTEX_DECLARE();             \
public:                                         \
/**/

/*** Macro for the declaration of the minimal interface ***/
#define GENBB_PG_INTERFACE_CTOR_DTOR(GENBB_CLASS_NAME)  \
  GENBB_PG_CONSTRUCTOR_DECLARE(GENBB_CLASS_NAME);       \
  GENBB_PG_DESTRUCTOR_DECLARE(GENBB_CLASS_NAME);        \
  GENBB_PG_INTERFACE ();                                 \
  /**/

/*** Macros for interface/implementation of static creator methods in EM field classes ***/

/** Registration */

#define GENBB_PG_REGISTRATION_INTERFACE(GENBB_CLASS_NAME)             \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (::genbb::i_genbb,GENBB_CLASS_NAME); \
  /**/

#define GENBB_PG_REGISTRATION_IMPLEMENT(GENBB_CLASS_NAME,GENBB_ID)   \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::genbb::i_genbb,GENBB_CLASS_NAME,GENBB_ID); \
  /**/


#endif // GENBB_GENBB_MACROS_H

// Local Variables: --
// mode: c++ --
// End: --
