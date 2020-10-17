/// \file brio/detail/TArrayCMod.h
#ifndef BRIO_DETAIL_TARRAYCMOD_H
#define BRIO_DETAIL_TARRAYCMOD_H

// ////////////////////////////////////////////////////////////////////////
//                                                                       //
// TArrayCMod                                                            //
//                                                                       //
// Array of chars or bytes (8 bits per element).                         //
//                                                                       //
// ////////////////////////////////////////////////////////////////////////

// Third Party:
// - ROOT:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-long-long"
#endif

#include <TArray.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

//! \brief The internal ROOT I/O buffer of bytes
class TArrayCMod
  : public TArray
{
public:
  Char_t *fArray;    //!< Array of bytes
  Int_t   fCapacity; //!< Do not store the capacity

public:
  TArrayCMod();
  ~TArrayCMod() override;
  void Reserve(Int_t n);
  void Clear();
  void Set(Int_t n) override;

  // Mandatory by TArray inheritance:
  Double_t GetAt(Int_t /*i*/) const override {return 0.0;}
  void SetAt(Double_t /*x*/, Int_t /*i*/) override {return;}

  // Workaround
private:
  void _Streamer_(TBuffer &b);
  ClassDefOverride(TArrayCMod,1)  //Array of chars
};

#endif // BRIO_DETAIL_TARRAYCMOD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
