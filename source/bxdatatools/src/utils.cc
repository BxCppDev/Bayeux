// -*- mode: c++; -*- 
// utils.cc

#include <datatools/utils/utils.h>

namespace datatools {

  namespace utils {

    void invalidate (double & x_)
    {
       x_ = std::numeric_limits<double>::quiet_NaN ();    
    }

    bool is_valid (double x_)
    {
      return x_ == x_;
    }

    bool fetch_path_with_env (std::string & text_)
    {
      std::string::size_type dollar;
      std::string text = text_;
      if (text.substr (0, 2) == "~/")
	{
	  std::string tmp = text.substr (2);
	  text = "${HOME}/" + tmp;
	}
      else if (text == "~")
	{
	  text = "${HOME}";
	}
      while ((dollar = text.find ('$')) != std::string::npos)
	{
	  std::string::size_type slash = text.find ('/', dollar + 1);
	  std::string::size_type back_slash = text.find ('\\', dollar + 1);
	  /*
	    std::string::size_type another_dollar = text.find ('$',dollar+1);
	  */
	  std::string::size_type pos = std::string::npos;
	  if (slash != std::string::npos) 
	    {
	      if (back_slash != std::string::npos) 
		{
		  pos = std::min (slash, back_slash); 
		  // remove MINIMUM(GB's macro) ==> std::min
		} 
	      else 
		{
		  pos = slash;
		}
	    } 
	  else 
	    {
	      if (back_slash != std::string::npos) 
		{
		  pos = back_slash;
		} 
	      else 
		{
		  pos = std::string::npos;
		}
	    }
	  std::string env;
	  if (pos == std::string::npos) 
	    {
	      env = text.substr (dollar + 1, 
				 text.length () - (dollar + 1));
	    } 
	  else 
	    {
	      //     abc$xxx/ef
	      //     0  3   7 9
	      env = text.substr (dollar + 1, 
				 pos - (dollar + 1));
	    }
	  std::string env_candidate = env;
	  if (! env.empty ()) 
	    {
	      if (env[0] == '{') 
		{
		  if (env[env.size () - 1] != '}') 
		    {
		      return false;
		    }
		  else 
		    {
		      env_candidate = env.substr (1, 
						  env.size () - 2);
		    }
		}
	    }
	  else 
	    {
	      return false;
	    }
	  if (env_candidate.empty ()) 
	    {
	      return false;	  
	    }
	  char* val = ::getenv (env_candidate.c_str());
	  if (val) 
	    {
	      std::string value = text.substr (0, dollar);
	      value += val;
	      if(pos != std::string::npos)
		{
		  value += text.substr (pos, text.length () - pos);
		}
	      text = value;
	    }
	  if (!val) return false; // Stop looping if env variable not found.
	}
      text_ = text;
      return true;
    }


    std::string expand_path (const std::string & path_str_)
    {
      std::string res ("");
      if (path_str_.empty ()) return (res);
      const std::string allowed = 
	"~./_-${}0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
      int found = path_str_.find_first_not_of (allowed);
      if ( found > 0 ) 
	{
	  throw std::runtime_error ("expand_path: found a not allowed character!");
	}
      std::ostringstream sh_cmd;
      char dummy[256];
      sprintf (dummy, "tmp.XXXXXX");
      int tmpfile = mkstemp (dummy);
      if (tmpfile == -1) 
	{
	  throw std::runtime_error ("expand_path: Cannot create temporary file!");
	}
      sh_cmd << "echo \"" << path_str_ << "\" > " << dummy;
      int ret = system (sh_cmd.str ().c_str ());
      if (ret != 0) 
	{
	  throw std::runtime_error ("expand_path: Cannot execute shell command!");
	}
      std::ifstream input (dummy);
      if (! input) 
	{
	  throw std::runtime_error ("expand_path: Cannot open temporary file!");
	} 
      std::string line_get;
      std::getline (input, line_get);
      if (! input)
	{ 
	  throw std::runtime_error ("expand_path: Cannot read temporary file!");
	}
      input.close ();
      close (tmpfile);
      /*
      std::ostringstream sh_cmd2;
      sh_cmd2 << "rm -f " << dummy;
      ret = system (sh_cmd2.str ().c_str ());
      */
      ret = unlink (dummy);
      if (ret != 0) 
	{
	  std::cerr << "expand_path: warning: "
		    << "Cannot delete temporary file '" 
		    << dummy << "'" << std::endl;
	}
      return (line_get);
    }
    
    
    /* From:
       Data Structures in C++ Using the STL
       by Timothy A. Budd
       published by Addison Wesley Longman
       ISBN 0-201-31659-5
       http://web.engr.oregonstate.edu/~budd/Books/stl/info/ReadMe.html
    */
    void 
    split_string (const std::string & text_ ,
		  const std::string & separators_ ,
		  std::list<std::string> & words_)
      // split a string into a list of words
      // 'text_' and 'separators_' are input,
      // list of 'words_' is output
    {
      int text_len = text_.length ();

      // find first non-separator character
      int start = text_.find_first_not_of (separators_, 0);
      // loop as long as we have a non-separator character
      while ((start >= 0) && (start < text_len)) 
	{
	  // find end of current word
	  int stop = text_.find_first_of (separators_, start);
	  if ((stop < 0) || (stop > text_len)) stop = text_len;
	  // add word to list of words
	  words_.push_back (text_.substr (start, stop - start));
	  // find start of next word
	  start = text_.find_first_not_of (separators_, stop+1);
	}
    }

  } // namespace utils

} // namespace datatools 

// end of utils.cc
