// -*- mode: c++; -*-
// version_id.h
#ifndef DATATOOLS_VERSION_ID_H_
#define DATATOOLS_VERSION_ID_H_
// Standard Library
#include <iostream>
#include <string>

// Third Party
// - Boost
#include <boost/cstdlib.hpp>

// This Project

namespace datatools {
/*! \brief A class representing a version ID :
 *
 * Supported formats are:
 *  - "tag" an alpha numeric character string not starting with
 *   a digit or '-' and with no punctuation symbols in it, however
 *   '-' and '_' are allowed.
 *
 *  - "major" a numeric value
 *  - "major.minor" two numeric values
 *  - "major.minor.revision" three numeric values
 *  - "major.minor.revision-tag" three numeric values and a trailing suffix tag
 *     where "tag" have special values:
 *     - "alpha" for 'alpha' version
 *     - "beta" for 'beta' version
 *     - "rcN" for 'release candidate' version
 *        with N a positive integer (<10000)
 *     Examples: "alpha", "beta", "rc0", "rc1"...
 *     "tag" can also be a string reprensenting a positive
 *     integer: "0", "1"...
 *     or any combinaison of alphanumeric character
 *     excluding punctuation symbols but '-' and '_' are still allowed.
 *     Examples:
 *      "3.1.4-15", "3.1.4-test", "3.1.4-007"
 *
 *  Version ID objects can be relatively ordered unless :
 *   - they are composed by a single tag
 *     Example: "Natty"
 *   - they have "major.minor.revision" and a "tag" that cannot be
 *     compared. However an ordering convention is used for special
 *     tags ("alpha" < "beta" < "rcX" < pure number "X")
 *
 *  Version ID objects can also be converted to and parsed from
 *  character strings.
 *
 *  Version ID can be checked to match version requirements
 *  (version rules) :
 *
 *  Examples :
 *   - "> 3.12" or ">3.12"
 *   - ">= 3.12" or ">=3.12"
 *   - "< 3.12" or "<3.12"
 *   - "<= 3.12" or "<=3.12"
 *   - "! 3.1.4-159" or  "!3.1.4-159" or "!= 3.1.4-159" or "!=3.1.4-159"
 *   - "= 3.1.4-159" or  "=3.1.4-159" or "== 3.1.4-159" or "==3.1.4-159"
 *
 *  Version ID can be invalidated : format is then "?"
 *
 */
class version_id {
 public:
  // TODO: could probably all be private or in an PImpl...
  static const int32_t INVALID_NUMBER;    // = -1;
  static const char    INVALID_SYMBOL;    // = '?';
  static const char    NUMERIC_SEPARATOR; // = '.';
  static const char    TAG_SEPARATOR;     // = '-';
  static const int32_t MAX_RC_NUM; //   = 9999;
  static const int32_t ALPHA_NUM; //    = -10003;
  static const int32_t BETA_NUM; //    = -10002;
  static const int32_t RC_BASE_NUM; //  = -10001;

  struct tags {
    static const std::string & alpha();
    static const std::string & beta();
    static const std::string & release_candidate_prefix();
  };

 public:
  // Constructors/Destructors
  version_id();

  version_id(const std::string& tag);

  version_id(int major, int minor = -1, int revision = -1,
             const std::string& tag = "");

  // Global operations
  bool is_valid() const;

  bool has_numbers_only() const;

  void reset();

  // Major number operations
  bool has_major() const;

  bool has_major_only() const;

  int get_major() const;

  void set_major(int);

  // Minor number operations
  bool has_minor() const;

  int get_minor() const;

  void set_minor(int);

  // Revision number operations
  bool has_revision() const;

  int get_revision() const;

  void set_revision(int);

  // Tag number operations
  bool has_tag() const;

  bool has_tag_number() const;

  bool has_tag_only() const;

  bool has_numeric_tag() const;

  bool has_special_tag() const;

  const std::string& get_tag() const;

  int get_tag_number() const;

  void set_tag(const std::string&);

  // Comparisons
  bool equals(const version_id&, bool ignore_tag = false) const;

  bool operator==(const version_id&) const;

  int compare(const version_id&, bool major_only_ = false) const;

  bool matches(const std::string& version_rules, bool major_only_ = false) const;

  // I/O and Conversions
  friend std::ostream& operator<<(std::ostream&, const version_id&);

  friend std::istream& operator>>(std::istream&, version_id&);

  void to_string(std::string& format) const;

  bool from_string(const std::string&);

  void dump(std::ostream& out = std::clog,
            const std::string& title = "",
            const std::string& indent = "") const;


  // These could/should be free functions
  static int compare(const version_id& vid0,
                     const version_id& vid1,
                     bool major_only = false);

  static bool are_orderable(const version_id& vid0,
                            const version_id& vid1,
                            bool major_only = false);

 private:
  int32_t major_;      /// The major number
  int32_t minor_;      /// The minor number
  int32_t revision_;   /// The revision number
  std::string tag_;    /// An additionnal tag (string)
  int32_t tag_number_; /// An additionnal tag (numeric)
};

} // namespace datatools

#endif // DATATOOLS_VERSION_ID_H_

