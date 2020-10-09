/// \file mygsl/histogram_pool.h

#ifndef MYGSL_HISTOGRAM_POOL_H
#define MYGSL_HISTOGRAM_POOL_H 1

// Standard library:
#include <iostream>
#include <string>
#include <map>
#include <vector>

// Third party:
// - Bayeux/datatools :
#include <datatools/i_serializable.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/handle.h>
#include <datatools/logger.h>

// This project:
#include <mygsl/histogram.h>
#include <mygsl/histogram_2d.h>
#include <mygsl/histogram_utils.h>

namespace mygsl {

  /// \brief A pool of histograms
  class histogram_pool :
    public datatools::i_serializable,
    public datatools::i_tree_dumpable
  {
  public:

    /// Histogram dimensionality
    enum histogram_dim_type
      {
        HISTOGRAM_DIM_UNDEFINED = 0,
        HISTOGRAM_DIM_1D = 1,
        HISTOGRAM_DIM_2D = 2
      };

    /// Typedef to handle of 1D histogram
    typedef datatools::handle<histogram_1d> hh1d_type;

    /// Typedef to handle of 2D histogram
    typedef datatools::handle<histogram_2d> hh2d_type;

    /// Simple structure related to histogram attributes
    struct histogram_entry_type : public datatools::i_tree_dumpable
    {
      std::string name;
      std::string title;
      std::string group;
      int32_t     dimension;
      hh1d_type   hh1d;
      hh2d_type   hh2d;

    public:
      /// Constructor
      histogram_entry_type();
      /// Destructor
      virtual ~histogram_entry_type();
      /// Smart print
      virtual void tree_dump(std::ostream      & out_    = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;


      DATATOOLS_SERIALIZATION_DECLARATION_NOINHERIT()
    };

    /// Alias to histogram dictionnary
    typedef std::map<std::string, histogram_entry_type> dict_type;

    /// Return histogram pool description
    const std::string & get_description() const;

    /// Set histogram poool description
    void set_description(const std::string & desc_);

    /// Get a non-mutable reference to auxiliaries
    const datatools::properties & get_auxiliaries() const;

    /// Get a mutable reference to auxiliaries
    datatools::properties & grab_auxiliaries();

    /// Constructor
    histogram_pool();

    /// Constructor with description inside
    histogram_pool(const std::string & desc_);

    /// Destructor
    virtual ~histogram_pool();

    /// Static method to initialize 1D histogram
    static void init_histo_1d(histogram_1d & h1_,
                              const datatools::properties & h1_setups_,
                              const histogram_pool * histo_pool_ = 0);

    /// Static method to initialize 2D histogram
    static void init_histo_2d(histogram_2d & h2_,
                              const datatools::properties & h2_setups_,
                              const histogram_pool * histo_pool_ = 0);

    /// Load histograms given a setup filename
    void load(const std::string & histo_setups_filename_);

    /// Load histograms given properties
    void load(const datatools::multi_properties & histo_setups_);

    /// Check histogram pool initialization
    bool is_initialized() const;

    /// Initialization
    void initialize(const datatools::properties & setup_);

    /// Reset
    void reset();

    /// Clen histogram dictionnary
    void remove_all();

    /// Check dictionnary emptiness
    bool empty() const;

    /// Return the number of histrogrmas stored
    size_t size() const;

    /// Clear auxiliairies
    void clear_auxiliaries();

    /// Check histogram presence given an histogram name
    bool has(const std::string & name_, int dim_ = 0) const;

    /// Check for 1D histogram given its name
    bool has_1d(const std::string & name_) const ;

    /// Check for 2D histogram given its name
    bool has_2d(const std::string & name_) const;

    /// Get a mutable reference to 1D histogram given its name
    histogram_1d & grab_1d(const std::string & name_);

    /// Get a non-mutable reference to 1D histogram given its name
    const histogram_1d & get_1d(const std::string & name_) const;

    /// Get a mutable reference to 2D histogram given its name
    histogram_2d & grab_2d(const std::string & name_);

    /// Get a non-mutable reference to 2D histogram given its name
    const histogram_2d & get_2d(const std::string & name_) const;

    /// Remove an histogram given its name
    void remove(const std::string & name_);

    /// Rename an histogram
    void rename(const std::string & name_, const std::string & new_name_);

    /// Set histogram title
    void set_title(const std::string & name_, const std::string & title_);

    /// Get histogram title
    const std::string & get_title(const std::string & name_) const;

    /// Assign a group to an histogram
    void set_group(const std::string & name_, const std::string & group_);

    /// Get group of an histogram
    const std::string & get_group(const std::string & name_) const;

    /// Build an array containing the names of the histograms stored in the pool :
    void names(std::vector<std::string> &names_, const std::string & filter_ = "", bool reverse_ = false) const;

    /// Add 1D histogram
    histogram_1d & add_1d(const std::string & name_,
                          const std::string & title_ = "",
                          const std::string & group_ = "");

    /// Add 2D histogram
    histogram_2d & add_2d(const std::string & name_,
                          const std::string & title_ = "",
                          const std::string & group_ = "");

    /// Smart print
    virtual void tree_dump(std::ostream      & out    = std::clog,
                           const std::string & title  = "",
                           const std::string & indent = "",
                           bool inherit               = false) const;

    /// Set logging priority
    void set_logging_priority(datatools::logger::priority);

    /// Get logging priority
    datatools::logger::priority get_logging_priority() const ;

  private:

    static const std::string _INITIALIZED_FLAG_KEY_;

    datatools::logger::priority _logging_priority_; //!< Logging priority threshold
    std::string           _description_;            //!< Description of the histogram pool
    dict_type             _dict_;                   //!< Dictionnary of histogram entries
    datatools::properties _auxiliaries_;            //!< Auxiliary peoperties

    DATATOOLS_SERIALIZATION_DECLARATION()

  };

} // end of namespace mygsl

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mygsl::histogram_pool)
DOCD_CLASS_DECLARATION(mygsl::histogram)
DOCD_CLASS_DECLARATION(mygsl::histogram_2d)

#endif // MYGSL_HISTOGRAM_POOL_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
