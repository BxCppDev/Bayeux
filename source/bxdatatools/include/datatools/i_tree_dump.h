/// \file datatools/i_tree_dump.h
#ifndef DATATOOLS_I_TREE_DUMP_H
#define DATATOOLS_I_TREE_DUMP_H

// Standard Library:
#include <iostream>
#include <string>

// Third party:
// - Boost:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <boost/property_tree/ptree.hpp>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// This project:
#include <datatools/datatools_config.h>
#ifndef Q_MOC_RUN
#include <datatools/reflection_interface.h>
#endif // Q_MOC_RUN

namespace datatools {

  /// \brief An interface with utilities for printable objects.
  class i_tree_dumpable
  {
  public:

    /// \brief Default tags for tree-formated print
    struct tags {
      static const std::string & item();
      static const std::string & last_item();
      static const std::string & skip_item();
      static const std::string & last_skip_item();
      static const std::string & item(bool last_, bool skip_);
    };

    static const boost::property_tree::ptree & empty_options();

    /// \brief Default print options POD
    struct base_print_options {

      bool inherit = false;
      std::string title;
      std::string indent;

      void configure_from(const boost::property_tree::ptree &);
      void export_to(boost::property_tree::ptree &);
      void reset();

      static const std::string & inherit_key();
      static const std::string & title_key();
      static const std::string & indent_key();
      static boost::property_tree::ptree force_inheritance(const boost::property_tree::ptree &);

    };
    
    /// \brief Enumeration for standard output streams
    enum ostream_type {
      OSTREAM_COUT = 1,
      OSTREAM_CERR = 2,
      OSTREAM_CLOG = 3
    };

    /// Constructor
    i_tree_dumpable();

    /// Destructor
    virtual ~i_tree_dumpable();

    static std::ostream & last_skip_tag(std::ostream & out_);

    static std::ostream & skip_tag(std::ostream & out_);

    static std::ostream & last_tag(std::ostream & out_);

    static std::ostream & tag(std::ostream & out_);

    /// Main new interface method for printing
    virtual void print_tree(std::ostream & out_ = std::clog,
                            const boost::property_tree::ptree & options_ = empty_options()) const;

    /// Main old interface method for printing
    virtual void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

    /// Printing with jsonized options
    void print_tree_json(std::ostream & out_ = std::clog,
                         const std::string & json_options_ = "") const;

    /// Printing with jsonized options
    void print_tree_json(std::ostream & out_ = std::clog,
                         const char * json_options_ = nullptr) const;

    void tree_dump_id(const int out_type_ = OSTREAM_CLOG,
                      const std::string & title_ = "",
                      const std::string & indent_ = "",
                      const bool inherit_ = false) const;

    void tree_print_id(const int out_type_ = OSTREAM_CLOG,
                       const std::string & title_ = "") const;

    void smart_print_id(const int out_type_ = OSTREAM_CLOG,
                        const std::string & title_  = "",
                        const std::string & indent_ = "") const;

    /// Output stream manipulator
    class inherit_tag
    {
    public:

      inherit_tag(bool inherit_);

      friend std::ostream & operator<<(std::ostream & out_,
                                       const inherit_tag & last_tag_);

    private:

      bool _inherit_;

    };

    /// Output stream manipulator
    class inherit_skip_tag
    {
    public:

      inherit_skip_tag(bool inherit_);

      friend std::ostream& operator<<(std::ostream & out_,
                                      const inherit_skip_tag & last_tag_);
    private:

      bool _inherit_;

    };

#ifndef Q_MOC_RUN
    DR_CLASS_RTTI()
#endif // Q_MOC_RUN

  };

} // end of namespace datatools

#ifndef Q_MOC_RUN
// Activate reflection layer for the 'datatools::i_tree_dumpable' class:
DR_CLASS_INIT(::datatools::i_tree_dumpable)
#endif // Q_MOC_RUN

#endif // DATATOOLS_I_TREE_DUMP_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
