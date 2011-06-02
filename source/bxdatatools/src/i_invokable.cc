/* i_invokable.cc */

#include <datatools/utils/i_invokable.h>
#include <datatools/utils/command.h>

#include <iostream>
#include <stdexcept>

namespace datatools {

  namespace utils {

    // default implementation:
    void 
    i_invokable::do_invoke (datatools::utils::command & command_)
    {
      throw command_not_implemented ("'i_invokable::do_invoke' method should be implemented from a concrete daughter class !"); 
    }

    int 
    i_invokable::invoke (datatools::utils::command & command_)
    {
      try
	{
	  command_.reset_output ();
	  /*
          std::clog << "DEVEL: i_invokable::invoke: Command is (before):"
		    << std::endl;
	 
	  command_.dump (std::clog);
	  */
	  this->do_invoke (command_); 
	  /*
	  std::clog << "DEVEL: i_invokable::invoke: Command is (after):"
		    << std::endl;
	  command_.dump (std::clog);
	  std::clog << "DEVEL: i_invokable::invoke: "
		    << "code=" << command_.get_error_code ()
		    << " msg=" << command_.get_error_message ()
		    << std::endl;
	  */
	}
      catch (std::runtime_error & x)
	{
	  if (command_.is_success ())
	    {
	      command_.set_error ();
	      if (command_.get_error_message ().empty ())
		{
		  command_.set_error_message (x.what ());
		}
	    }
	}
      catch (...)
	{
	  if (command_.is_success ())
	    {
	      command_.set_error ();
	      if (command_.get_error_message ().empty ())
		{
		  command_.set_error_message ("unexpected error!");
		}
	    }
	}
      return command_.get_error_code ();
    }

    int 
    i_invokable::invoke (i_invokable & invokable_, 
			 datatools::utils::command & command_)
    {
      return invokable_.invoke (command_);
    }

  } // end of namespace utils 

} // end of namespace datatools 

/* end of i_invokable.cc */
