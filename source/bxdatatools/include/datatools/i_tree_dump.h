/// \file datatools/i_tree_dump.h
#ifndef DATATOOLS_I_TREE_DUMP_H
#define DATATOOLS_I_TREE_DUMP_H

// Standard Library:
#include <iostream>
#include <string>

// This project:
#include <datatools/datatools_config.h>
#ifndef Q_MOC_RUN
#include <datatools/reflection_interface.h>
#endif // Q_MOC_RUN

namespace datatools {

  /// \brief An abstract interface with utilities for printable objects.
  class i_tree_dumpable
  {
  public:

    struct tags {
      static const std::string & item();
      static const std::string & last_item();
      static const std::string & skip_item();
      static const std::string & last_skip_item();
    };

    enum ostream_type {
      OSTREAM_COUT=1,
      OSTREAM_CERR=2,
      OSTREAM_CLOG=3
    };

    /// Constructor
    i_tree_dumpable();

    /// Destructor
    virtual ~i_tree_dumpable();

    static std::ostream & last_skip_tag(std::ostream& out_);

    static std::ostream & skip_tag(std::ostream& out_);

    static std::ostream & last_tag(std::ostream& out_);

    static std::ostream & tag(std::ostream& out_);

    /// Main interface method for smart dump
    virtual void tree_dump(std::ostream& out_ = std::clog,
                           const std::string& title_  = "",
                           const std::string& indent_ = "",
                           bool inherit_ = false) const = 0;

    void tree_dump(int out_type_ = OSTREAM_CLOG,
                   const std::string& title_  = "",
                   const std::string& indent_ = "",
                   bool inherit_ = false) const;

    void tree_print(int out_type_ = OSTREAM_CLOG,
                    const std::string& title_ = "") const;

    void smart_print(int out_type_ = OSTREAM_CLOG,
                     const std::string& title_  = "",
                     const std::string& indent_ = "") const;

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
