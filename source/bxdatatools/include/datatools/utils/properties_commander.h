/* properties_commander.h */

#ifndef __datatools__utils__properties_commander_h
#define __datatools__utils__properties_commander_h 1

#include <string>
#include <stdexcept>
#include <map>

#include <datatools/utils/i_invokable.h>
#include <datatools/utils/properties.h>

namespace datatools {

  namespace utils {

    class properties_commander_i : public i_invokable
      {
      private:
	
	bool         __debug;
	properties * __prop;

      public:

	bool is_debug () const;

	void set_debug (bool = true);

	void set_properties (properties &);

	properties * get_prop ();

	const properties * get_prop () const;

	void reset ();

	properties_commander_i (bool debug_ = false);

	properties_commander_i (properties &, bool debug_ = false);

	virtual void do_invoke (command & command_);

      };

    /***********************************************/

    class properties_commander
    {
    public:
      typedef std::pair <bool, properties *>   pentry_t;
      typedef std::map <std::string, pentry_t> pmap_t;

    private:
      bool                   __debug;
      properties_commander_i __i;
      pmap_t                 __targets;
      bool                   __allow_map_commands;

    public:

      bool allows_map_commands () const;

      void set_allows_map_commands (bool = true);
      
      bool is_debug () const;
      
      void set_debug (bool = true);

      properties_commander (bool debug_ = false);

      properties_commander (properties & prop_, 
			    bool debug_ = false);

      virtual ~properties_commander ();

      int clear_targets ();

      int switch_target (std::string name_);

      int remove_target (std::string name_);

      int create_target (std::string name_);
      
      int add_target (std::string name_, 
		      properties & prop_,
		      bool current_ = true);

      void set_current_target (properties & prop_);

      int run (std::string name_, command & command_);

      int run (properties & prop_, command & command_);

      int run (command & command_);

      void list_targets (std::ostream & out_ = std::clog) const;

      void print_help (std::ostream & out_ = std::clog) const;

      void reset ();

    private:

      void __reset_targets ();

      void __reset ();
      
    protected:
      int __run (command & command_);

      int _local_run (command & command_);

    };

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__properties_commander_h

/* end of properties_commander.h */
