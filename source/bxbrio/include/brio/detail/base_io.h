/// \file brio/detail/base_io.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-11-03
 * Last modified: 2012-12-20
 *
 * License: GPL3
 *
 * Description:
 *
 *   A brio reader/writer base class
 *
 */

#ifndef BRIO_DETAIL_BASE_IO_H
#define BRIO_DETAIL_BASE_IO_H 1

// Standard Library:
#include <string>
#include <list>
#include <locale>

// Third Party:
// - Boost:
#include <boost/math/special_functions/nonfinite_num_facets.hpp>

// - Bayeux/datatools:
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>

// This Project:
#include <brio/utils.h>

class TFile;

/// Top-level namespace of the Bayeux/brio module library
namespace brio {

  /// Nested namespace of the Bayeux/brio module library (detail)
  namespace detail {

    //! \brief The internal I/O base class
    class base_io
      : public datatools::i_tree_dumpable
    {
    public:
      static const std::string & pba_label();  /// Label for Boost portable binary archive
      static const std::string & text_label(); /// Label for Boost portable text archive

      enum format_type {
        FORMAT_UNDEFINED = 0,
        FORMAT_PBA       = 1, /// Boost portable binary archive (file extension: .brio)
        FORMAT_TEXT      = 2 /// Boost portable text archive (file extension: .trio)
      };

      enum rw_type {
        RW_UNDEFINED = 0,
        RW_READ      = 1, /// Reader mode
        RW_WRITE     = 2  /// Writer mode
      };

    public:

      base_io(int rw_);

      base_io(int rw_, datatools::logger::priority p_);

      base_io(int rw_, int format_, datatools::logger::priority p_);

      ~base_io() override;

      //! File open
      virtual void open(const std::string & filename_);

      //! File close
      virtual void close();

      virtual bool is_opened() const;

      bool is_reading() const;

      bool is_writing() const;

      bool is_format_pba() const;

      bool is_format_text() const;

      int get_format() const;

      void set_format(int format_);

      void set_format(const std::string & format_str_);

      static int get_format (const std::string & format_str_);

      bool is_debug() const;

      void set_debug(bool);

      bool is_verbose() const;

      void set_verbose(bool);

      int64_t get_number_of_entries(const std::string & label_ = "") const;

      int64_t get_current_entry(const std::string & label_ = "") const;

      const std::string & get_serialization_tag(const std::string & label_ = "") const;

      bool has_store(const std::string & label_) const;

      bool has_store_with_serial_tag(const std::string & label_,
                                     const std::string & serial_tag_) const;

      template <class T>
      bool has_store_with_matching_serial_tag(const std::string & label_) const;

      bool has_mixed_store(const std::string & label_) const;

      void select_store(const std::string & label_);

      bool has_automatic_store() const;

      void select_automatic_store();

      void unselect_store();

      void get_list_of_stores(std::list<std::string> & list_) const;

      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

      void reset();

      void set_logging_priority(datatools::logger::priority);

      datatools::logger::priority get_logging_priority() const;

    protected:
      virtual void _at_open(const std::string & filename_) = 0;

      void _set_default();

      void _reset();

      store_info * _get_store_info(const std::string & label_ = "");

      const store_info * _get_store_info(const std::string & label_ = "") const;

    protected:
      std::string _filename;       ///< Name of the current I/O file (extensions are \b .brio or \b .trio)
      TFile *      _file;           ///< Handle to the current embedded ROOT file
      store_info_dict_type _store_infos; ///< Dictionnary of \e stores
      store_info *          _current_store;  ///< Handle to the current active \e store (if any)
      int _rw;             ///< Read/write mode tag
      std::locale * _default_locale; ///< Default I/O locale (for portable streams)
      std::locale * _locale;         ///< I/O locale (for portable streams)

    private:
      datatools::logger::priority _logging_priority_;
      int  _format_;  /// Format tag (can be Boost portable binary or text archives)
    };

    //----------------------------------------------------------------------
    // Template function definitions
    //
    template <class T>
    bool base_io::has_store_with_matching_serial_tag(const std::string & label_) const
    {
      store_info_dict_type::const_iterator found = _store_infos.find(label_);
      if (found == _store_infos.end()) return false;
      const store_info& the_si = found->second;
      if (the_si.has_dedicated_serialization_tag()) {
        return datatools::check_serial_tag<T>(the_si.get_serialization_tag());
      }
      return false;
    }

  } // end of namespace detail

} // end of namespace brio

#endif // BRIO_DETAIL_BASE_IO_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
