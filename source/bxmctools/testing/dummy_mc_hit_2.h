
#ifndef MCTOOLS_TESTING_DUMMY_MC_HIT_2_H
#define MCTOOLS_TESTING_DUMMY_MC_HIT_2_H

// Data model :
#include "dummy_mc_hit.h"

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/ioutils.h>

namespace mctools {
  namespace testing {

    class dummy_mc_hit_2
      : public dummy_mc_hit
    {
    public:
  
      /// \brief Masks to automatically tag the attributes to be stored
      enum dummy_store2_mask_type {
        STORE2_BAR  = datatools::bit_mask::bit00, //!< Serialization mask for the bar attribute
        STORE2_LAST = STORE2_BAR
      };

      /// Default constructor
      dummy_mc_hit_2()
      {
        _bar_ = datatools::invalid_real();
        return;
      }

      /// Destructor
      virtual ~dummy_mc_hit_2()
      {
        return;
      }

      bool has_bar() const
      {
        return datatools::is_valid(_bar_);
      }
  
      void set_bar(const double value_)
      {
        if (datatools::is_valid(value_)) {
          _store2 |= STORE2_BAR;
        } else {
          _store2 &= ~STORE2_BAR;
        }
        _bar_ = value_;
      }
  
      double get_bar() const
      {
        return _bar_;
      }

      void invalidate_bar()
      {
        set_bar(datatools::invalid_real());
        return;
      }

      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = datatools::i_tree_dumpable::empty_options()) const override
      {
        datatools::i_tree_dumpable::base_print_options popts;
        popts.configure_from(options_);
        this->dummy_mc_hit::print_tree(out_,
				       base_print_options::force_inheritance(options_));
   
	out_ << popts.indent << tag
	     << "Store2       : " << datatools::io::to_binary(_store2) << std::endl;

	out_ << popts.indent << inherit_tag(popts.inherit) << "Bar : ";
	if (has_bar()) {
          out_ << _bar_;
        } else {
          out_ << "<none>";
	}
	out_ << std::endl;
       
        return;
      }

    protected:
      
      uint32_t _store2 = 0;
      
    private:
  
      double _bar_; //!< Dummy value

      DATATOOLS_SERIALIZATION_DECLARATION()
    
      GEOMTOOLS_HIT_REGISTRATION_INTERFACE(dummy_mc_hit_2)
  
    };
    
    template<class Archive>
    void dummy_mc_hit_2::serialize(Archive & ar_,
                                 const unsigned int /* version_ */)
    {
      // Inherit from the 'base_step_hit' mother class:
      ar_ & boost::serialization::make_nvp("mctools__testing__dummy_mc_hit",
                                           boost::serialization::base_object<mctools::testing::dummy_mc_hit>(*this));
      ar_ & boost::serialization::make_nvp ("store2",       _store2);
      // The '_store2' field member knows about the field to be stored/loaded from the archive.
      if (_store2 & STORE2_BAR) {
        ar_ & boost::serialization::make_nvp("bar", _bar_);
      }
      return;
    }

  } // end namespace testing 
} // end namespace mctools 

#include <boost/serialization/export.hpp>
#include <datatools/archives_instantiation.h>
BOOST_CLASS_EXPORT_KEY2(mctools::testing::dummy_mc_hit_2, "mctools::testing::dummy_mc_hit_2")
BOOST_CLASS_EXPORT_IMPLEMENT(mctools::testing::dummy_mc_hit_2)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mctools::testing::dummy_mc_hit_2)

namespace mctools {
  namespace testing {
    
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(dummy_mc_hit_2, "mctools::testing::dummy_mc_hit_2")
    GEOMTOOLS_HIT_REGISTRATION_IMPLEMENT(dummy_mc_hit_2, "mctools::testing::dummy_mc_hit_2")

  } // end namespace testing 
} // end namespace mctools 
#endif // MCTOOLS_TESTING_DUMMY_MC_HIT_2_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
