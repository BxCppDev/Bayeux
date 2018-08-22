
#ifndef MCTOOLS_TESTING_DUMMY_MC_HIT_H
#define MCTOOLS_TESTING_DUMMY_MC_HIT_H

// Standard library:
#include <cstdlib>

// Data model :
#include <mctools/base_step_hit.h>

namespace mctools {
  namespace testing {

    class dummy_mc_hit
      : public mctools::base_step_hit
    {
    public:
  
      /// \brief Masks to automatically tag the attributes to be stored
      enum dummy_store_mask_type {
        STORE_FOO  = datatools::bit_mask::bit29, //!< Serialization mask for the foo attribute
        STORE_LAST = STORE_FOO
      };

      /// Default constructor
      dummy_mc_hit()
      {
        _foo_ = datatools::invalid_real();
        return;
      }

      /// Destructor
      virtual ~dummy_mc_hit()
      {
        return;
      }

      bool has_foo() const
      {
        return datatools::is_valid(_foo_);
      }
  
      void set_foo(const double value_)
      {
        if (datatools::is_valid(value_)) {
          _store |= STORE_FOO;
        } else {
          _store &= ~STORE_FOO;
        }
        _foo_ = value_;
      }
  
      double get_foo() const
      {
        return _foo_;
      }

      void invalidate_foo()
      {
        set_foo(datatools::invalid_real());
        return;
      }

      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = datatools::i_tree_dumpable::empty_options()) const override
      {
        datatools::i_tree_dumpable::base_print_options popts;
        popts.configure_from(options_);
        this->mctools::base_step_hit::print_tree(out_,
                                                 base_print_options::force_inheritance(options_));
   
        if (has_foo()) {
          out_ << popts.indent << inherit_tag(popts.inherit)
               << "Foo : " << _foo_ << std::endl;
        }
        
        return;
      }

    private:
  
      double _foo_; //!< Dummy value

      DATATOOLS_SERIALIZATION_DECLARATION()
    
      GEOMTOOLS_HIT_REGISTRATION_INTERFACE(dummy_mc_hit)
  
    };
    
    template<class Archive>
    void dummy_mc_hit::serialize(Archive & ar_,
                                 const unsigned int /* version_ */)
    {
      // Inherit from the 'base_step_hit' mother class:
      ar_ & boost::serialization::make_nvp("mctools__base_step_hit",
                                           boost::serialization::base_object<mctools::base_step_hit>(*this));

      // The '_store' field member from the 'base_hit' mother class
      // knows about the field to be stored/loaded from the archive.
      if (_store & STORE_FOO) {
        ar_ & boost::serialization::make_nvp("foo", _foo_);
      }
      return;
    }

  } // end namespace testing 
} // end namespace mctools 

#include <boost/serialization/export.hpp>
#include <datatools/archives_instantiation.h>
BOOST_CLASS_EXPORT_KEY2(mctools::testing::dummy_mc_hit, "mctools::testing::dummy_mc_hit")
BOOST_CLASS_EXPORT_IMPLEMENT(mctools::testing::dummy_mc_hit)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mctools::testing::dummy_mc_hit)

namespace mctools {
  namespace testing {
    
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(dummy_mc_hit, "mctools::testing::dummy_mc_hit")
    GEOMTOOLS_HIT_REGISTRATION_IMPLEMENT(dummy_mc_hit, "mctools::testing::dummy_mc_hit")

  } // end namespace testing 
} // end namespace mctools 
#endif // MCTOOLS_TESTING_DUMMY_MC_HIT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
