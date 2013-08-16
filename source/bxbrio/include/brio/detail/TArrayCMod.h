// -*- mode: c++; -*-
#ifndef BRIO_DETAIL_TARRAYCMOD_H_
#define BRIO_DETAIL_TARRAYCMOD_H_

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TArrayCMod                                                           //
//                                                                      //
// Array of chars or bytes (8 bits per element).                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// Standard Library

// Third Party
// - ROOT
#ifndef ROOT_TArray
#include "TArray.h"
#endif

// This Project

//! \brief The internal ROOT I/O buffer of bytes
class TArrayCMod : public TArray {
 public:
  Char_t *fArray;    //! Array of bytes
  Int_t   fCapacity; //! Do not store the capacity

 public:
  TArrayCMod();
  virtual ~TArrayCMod();
  void Reserve(Int_t n);
  void Clear();
  void Set(Int_t n);

  // Mandatory by TArray inheritance:
  Double_t GetAt(Int_t i) const {return 0.0;}
  void SetAt(Double_t x, Int_t i) {return;}

  // Workaround
 private:
  void _Streamer_(TBuffer &b);
  ClassDef(TArrayCMod,1)  //Array of chars
};

#endif // BRIO_DETAIL_TARRAYCMOD_H_

// end of TArrayCMod.h

