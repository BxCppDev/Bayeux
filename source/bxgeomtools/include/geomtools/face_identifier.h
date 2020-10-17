/// \file geomtools/face_identifier.h
/* Author(s):     François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-03-16
 * Last modified: 2016-11-12
 *
 * License: GPL3
 *
 * Description:
 *
 *  The identifier of a face in a simple or composite shape
 *
 */

#ifndef GEOMTOOLS_FACE_IDENTIFIER_H
#define GEOMTOOLS_FACE_IDENTIFIER_H 1

// Standard library:
#include <iostream>
#include <string>
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/bit_mask.h>
#include <datatools/i_tree_dump.h>

namespace geomtools {

  //! \brief Identifier of a face attached to a solid shape
  class face_identifier
    : public datatools::i_tree_dumpable
  {
  public:

    //! \brief Face identification mode
    enum mode_type {
      MODE_FACE_INVALID = 0x0,                             //!< Invalid face identification mode
      MODE_FACE_BITS    = datatools::bit_mask::bit00,      //!< Face identification uses identification bits
      MODE_FACE_INDEX   = datatools::bit_mask::bit01,      //!< Face identification uses indexing
      MODE_FACE_ANY     = MODE_FACE_BITS | MODE_FACE_INDEX //!< Face identification uses any mode
    };

    static const uint32_t FACE_BITS_NONE  = 0x0;        //!< No valid or known face bit
    static const uint32_t FACE_BITS_ANY   = 0xFFFFFFFF; //!< All faces are selected

    // Range [0,1,2.... MAX, ANY, NONE]
    static const uint32_t FACE_INDEX_NONE = 0xFFFFFFFF; //!< Special value for undefined/invalid face index
    static const uint32_t FACE_INDEX_MIN  = 0;          //!< Special value for min face index
    static const uint32_t FACE_INDEX_MAX  = 0xFFFFFFFD; //!< Special value for max face index
    static const uint32_t FACE_INDEX_ANY  = 0xFFFFFFFE; //!< Special value for any face index (wildcard)

    // Range [0,1,2.... MAX, ANY, NONE]
    static const uint32_t PART_INDEX_NONE = 0xFFFFFFFF; //!< Special value for undefined/invalid part index
    static const uint32_t PART_INDEX_MIN  = 0;          //!< Special value for min part index
    static const uint32_t PART_INDEX_DEFAULT = PART_INDEX_MIN; //!< Special value for default part index
    static const uint32_t PART_INDEX_MAX  = 0xFFFFFFFD; //!< Special value for max part index
    static const uint32_t PART_INDEX_ANY  = 0xFFFFFFFE; //!< Special value for any part index (wildcard)

    //! Default constructor
    face_identifier();

    //! Constructor
    face_identifier(uint32_t face_info_, mode_type mode_ = MODE_FACE_BITS);

    //! Check the validity of the identifier
    bool is_valid() const;

    //! Check if the identifier identifies some faces (unique or several/many)
    bool is_ok() const;

    //! Check if the identifier is valid and identifies an unique face
    bool is_unique() const;

    //! Check if the identifier is valid and identifies several faces
    bool is_many() const;

    //! Check if the identifier is invalid or valid but identifies no face
    bool is_none() const;

    //! Invalidate the identifier
    //! @see reset
    void invalidate();

    //! Invalidate the identifier
    void reset();

    //! Return the face identification mode
    mode_type get_mode() const;

    //! Check if the identifier has a valid mode
    bool has_mode() const;

    //! check if the identifier uses face bits
    bool is_face_bits_mode() const;

    //! Check if the identifier uses face index
    bool is_face_index_mode() const;

    //! Return the depth of the parts
    uint32_t get_parts_depth() const;

    //! Return if the face identifier has parts
    bool has_parts() const;

    //! Check if some parts numbers are defined
    bool has_part(unsigned int depth_) const;

    //! Check if a part number is invalid
    bool is_invalid_part(unsigned int depth_) const;

    //! Check if a part number matches the any value
    bool is_any_part(unsigned int depth_) const;

    //! Check if a part number matches a given value
    bool match_part(unsigned int depth_, uint32_t part_) const;

    //! Reset the part number
    void reset_part(unsigned int depth_);

    //! Set the part number to any value
    void set_any_part(unsigned int depth_);

    //! Set all part numbers to any value
    void set_any_parts();

    //! Set the part number
    void set_part(unsigned int depth_, uint32_t);

    //! Return the part number
    uint32_t get_part(unsigned int depth_) const;

    //! Prepend a part
    void prepend_part(uint32_t part_);

    //! Append a part
    void append_part(uint32_t part_);

    //! Append an array of parts
    void append_parts(const std::vector<uint32_t> & parts_);

    //! Check if an inherited
    bool can_inherit_parts(unsigned int depth_ = 1) const;

    //! Build the inherited part
    void inherit_parts(face_identifier & fid_, unsigned int depth_ = 1) const;

    //! Return the array of parts
    const std::vector<uint32_t> & get_parts() const;

    //! Set the face bits is valid
    bool is_face_bits_valid() const;

    //! Set the bits to address any face
    void set_face_bits_any();

    //! Check if the bits address any face
    bool is_face_bits_any() const;

    //! Check if some face bits are active
    bool has_face_bits(uint32_t) const;

    //! Check if a given face bit is active
    bool has_face_bit(uint32_t) const;

    //! Set some face bits
    void set_face_bits(uint32_t bits_);

    //! Set a single face bit
    void set_face_bit(uint32_t bit_);

    //! Return the face bits
    uint32_t get_face_bits() const;

    //! Reset the face bits
    void reset_face_bits();

    //! Check if the face bits number matches some given value
    bool match_face_bits(uint32_t bits_) const;

    //! Reset the index of a face
    void reset_face_index();

    //! Set the index of a face
    void set_face_index(uint32_t);

    //! Return the index of a face
    uint32_t get_face_index() const;

    //! Set the bits to address any index
    void set_face_index_any();

    //! Check if the face identifier address any index
    bool is_face_index_any() const;

    //! Check if the face identifier address a valid index
    bool is_face_index_valid() const;

    //! Check if the face index matches some given value
    bool match_face_index(uint32_t index_) const;

    //! Convert to a string
    void to_string(std::string & word_) const;

    //! Return a string
    std::string to_string() const;

    //! Parse
    bool parse(const std::string & from_);

    //! Check if a given face identifier matches the signature
    bool match(const face_identifier & fid_) const;

    //! Return a singleton face identifier with any bit
    static const face_identifier & face_bits_any();

    //! Return a singleton face identifier with any index
    static const face_identifier & face_index_any();

    //! Return a singleton invalid face identifier
    static const face_identifier & face_invalid();

    //! Output streaming
    friend std::ostream & operator<<(std::ostream & out_, const face_identifier & face_id_);

    //! Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

  protected:

    //! Set default attributes values
    void _set_defaults();

  private:

    mode_type             _mode_;       //!< Mode for face identification
    std::vector<uint32_t> _parts_;      //!< Part identifiers
    uint32_t              _face_bits_;  //!< Face bits (31 bits max)
    uint32_t              _face_index_; //!< Face index

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_FACE_IDENTIFIER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
