// -*- mode: c++; -*-
// histogram_pool.h

#ifndef MYGSL_HISTOGRAM_POOL_H_
#define MYGSL_HISTOGRAM_POOL_H_ 1

#include <iostream>
#include <string>
#include <map>
#include <vector>

// Serialization interfaces :
#include <datatools/i_serializable.h>

#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/handle.h>
#include <datatools/logger.h>

#include <mygsl/histogram.h>
#include <mygsl/histogram_2d.h>
#include <mygsl/histogram_utils.h>

namespace mygsl {

  class histogram_pool :
    DATATOOLS_SERIALIZABLE_CLASS,
    public datatools::i_tree_dumpable
  {
  public:

    enum histogram_dim_type
      {
        HISTOGRAM_DIM_UNDEFINED = 0,
        HISTOGRAM_DIM_1D = 1,
        HISTOGRAM_DIM_2D = 2
      };

    typedef datatools::handle<histogram_1d> hh1d_type;
    typedef datatools::handle<histogram_2d> hh2d_type;

    struct histogram_entry_type : public datatools::i_tree_dumpable
    {
      std::string name;
      std::string title;
      std::string group;
      int32_t     dimension;
      hh1d_type   hh1d;
      hh2d_type   hh2d;

    public:

      histogram_entry_type ();
      virtual ~histogram_entry_type ();
      /// Main interface method for smart dump
      virtual void tree_dump (std::ostream      & out_    = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_               = false) const;


      DATATOOLS_SERIALIZATION_DECLARATION();

    };

    typedef std::map<std::string, histogram_entry_type> dict_type;

    const std::string & get_description () const;

    void set_description (const std::string & desc_);

    const datatools::properties & get_auxiliaries () const;

    datatools::properties & grab_auxiliaries ();

    histogram_pool ();

    histogram_pool (const std::string & desc_);

    virtual ~histogram_pool ();

    static void init_histo_1d (histogram_1d & h1_,
                               const datatools::properties & h1_setups_,
                               const histogram_pool * histo_pool_ = 0);

    static void init_histo_2d (histogram_2d & h2_,
                               const datatools::properties & h2_setups_,
                               const histogram_pool * histo_pool_ = 0);

    void load (const std::string & histo_setups_filename_);

    void load (const datatools::multi_properties & histo_setups_);

    bool is_initialized () const;

    void initialize (const datatools::properties & setup_);

    void reset ();

    void remove_all ();

    bool empty () const;

    unsigned int size () const;

    void clear_auxiliaries ();

    bool has (const std::string & name_, int dim_ = 0) const;

    bool has_1d (const std::string & name_) const ;

    bool has_2d (const std::string & name_) const;

    histogram_1d & grab_1d (const std::string & name_);

    const histogram_1d & get_1d (const std::string & name_) const;

    histogram_2d & grab_2d (const std::string & name_);

    const histogram_2d & get_2d (const std::string & name_) const;

    void remove (const std::string & name_);

    void rename (const std::string & name_, const std::string & new_name_);

    void set_title (const std::string & name_, const std::string & title_);

    const std::string & get_title (const std::string & name_) const;

    void set_group (const std::string & name_, const std::string & group_);

    const std::string & get_group (const std::string & name_) const;

    /// Build an array containing the names of the histograms stored in the pool :
    void names (std::vector<std::string> &names_, const std::string & filter_ = "", bool reverse_ = false) const;

    histogram_1d & add_1d (const std::string & name_,
                           const std::string & title_ = "",
                           const std::string & group_ = "");

    histogram_2d & add_2d (const std::string & name_,
                           const std::string & title_ = "",
                           const std::string & group_ = "");

    /// Main interface method for smart dump
    virtual void tree_dump (std::ostream      & out    = std::clog,
                            const std::string & title  = "",
                            const std::string & indent = "",
                            bool inherit               = false) const;

    void set_logging_priority(datatools::logger::priority);

    datatools::logger::priority get_logging_priority() const ;

  private:

    static const std::string _INITIALIZED_FLAG_KEY_;

    datatools::logger::priority _logging_priority_;
    std::string           _description_;
    dict_type             _dict_;
    datatools::properties _auxiliaries_;

    DATATOOLS_SERIALIZATION_DECLARATION();

  };

} // end of namespace mygsl

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mygsl::histogram_pool)
DOCD_CLASS_DECLARATION(mygsl::histogram)
DOCD_CLASS_DECLARATION(mygsl::histogram_2d)

#endif // MYGSL_HISTOGRAM_POOL_H_

// end of mygsl::histogram_pool.h
