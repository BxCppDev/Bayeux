// check.cc

#include <datatools/serialization/check.h>

namespace datatools {

  namespace serialization {

    void print0 (ostream & a_out, 
		const boost::serialization::extended_type_info & a_eti)
    {
      a_out << "  Key         :  '" << a_eti.get_key() << "'" << endl;
      a_out << "  Debug info  :  '" << a_eti.get_debug_info() << "'" << endl;
      
      return;
    } 

    void check0 ()
    {
      clog << "datatools::serialization::check0 : " << endl;
      boost::serialization::detail::ktmap & x = boost::serialization::singleton<boost::serialization::detail::ktmap>::get_mutable_instance();
      clog << "KTMAP size = " << x.size () << endl;

      for (boost::serialization::detail::ktmap::const_iterator i = x.begin ();
	   i != x.end (); 
	   i++) 
	{
	  const boost::serialization::extended_type_info * eti = *i;
	  print0 (clog, *eti);
	}
      return;
    }

    void print (ostream & a_out, 
		const boost::serialization::typeid_system::extended_type_info_typeid_0 & a_eti)
    {
      if (a_eti.get_key() != NULL)
	{
	  a_out << "  Key          :  '" << a_eti.get_key() << "'" << endl;
	}
      a_out << "  Type ID name :  '" << a_eti.get_typeid().name () << "'" << endl;
      
      return;
    } 

    void check ()
    {
      clog << "datatools::serialization::check : " << endl;
      boost::serialization::typeid_system::tkmap & x = boost::serialization::singleton<boost::serialization::typeid_system::tkmap>::get_mutable_instance();
      clog << "TKMAP size = " << x.size () << endl;

      for (boost::serialization::typeid_system::tkmap::const_iterator i = x.begin ();
	   i != x.end (); 
	   i++)
	{
	  const boost::serialization::typeid_system::extended_type_info_typeid_0 * eti = *i;
	  print (clog, *eti);
	}
      return;
    }

  } // end of namespace serialization

} // end of namespace datatools

// end of check.cc
