/// \file brio/detail/brio_record.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-07-16
 * Last modified: 2010-07-16
 *
 * Description:
 *
 * rootcint brioDict.C -c include/brio/brio_record.h
 *
 */

#ifndef BRIO_DETAIL_BRIO_RECORD_H
#define BRIO_DETAIL_BRIO_RECORD_H 1

// Standard Library:
#include <vector>

// Third Party:
// - ROOT:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-long-long"
#pragma clang diagnostic ignored "-Wshadow"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <TObject.h>
#include <TBuffer.h>
#include <TString.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// This Project:
#include <brio/detail/TArrayCMod.h>

/// The internal ROOT I/O record class
class brio_record
  : public TObject
{
public:

  TString    fSerialTag;  ///< The serialization tag of the data class
  UInt_t     fVersionTag; ///< The serialization version number of the data class
  TArrayCMod fDataBuffer; ///< The buffer of bytes that contains the Boost archive associated to the serialized data

public:

  //! Constructor
  brio_record();

  //! Destructor
  ~brio_record() override;

  //! Reset the internal data
  void reset();

  ClassDefOverride(brio_record,1) // Basic BRIO record

};

#endif // BRIO_DETAIL_BRIO_RECORD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
