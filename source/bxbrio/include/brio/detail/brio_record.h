// -*- mode: c++ ; -*- 
/* brio_record.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-07-16
 * Last modified: 2010-07-16
 * 
 * License: 
 * 
 * Description: 
 *
 * rootcint brioDict.C -c include/brio/brio_record.h
 *
 * History: 
 * 
 */

#ifndef __brio__brio_record_h
#define __brio__brio_record_h 1

#include <vector>

#include <TObject.h>
#include <TBuffer.h>
#include <TString.h>

#include <brio/detail/TArrayCMod.h>

//
// http://www.codeproject.com/KB/architecture/ratkfactory.aspx
//

class brio_record : public TObject
{
public:
    
  TString  fSerialTag; 
  UInt_t   fVersionTag;
  TArrayCMod fDataBuffer; // Char_t *fArray and fN (from TArray)

public: 

  void reset ();

  // ctor:
  brio_record ();

  // dtor:
  virtual ~brio_record ();
  
  ClassDef(brio_record,1) //Basic BRIO record
};

#endif // __brio__brio_record_h

// end of brio_record.h
