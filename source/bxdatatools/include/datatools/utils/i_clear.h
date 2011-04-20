/* datatools/utils/i_clear.h */

//! \file datatools/utils/i_clear.h

#ifndef __datatools__utils__i_clear_h
#define __datatools__utils__i_clear_h 1

namespace datatools {

  namespace utils {

    //! \brief A pure abstract class (interface) for inherited clearable classes.
    class i_clear
    {
    public:
  
      //virtual ~i_clear () {}

     /**
      * A pure virtual member.
      * Clear (reset) the object's internals (typically empty a container).
      */
      virtual void clear() = 0;
    };

  } // end of namespace utils 

} // end of namespace datatools 
   
//BOOST_IS_ABSTRACT(datatools::utils::i_clear)

#endif // __datatools__utils__i_clear_h

/* end of datatools/utils/i_clear.h */
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
