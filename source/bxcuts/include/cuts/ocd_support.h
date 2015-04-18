/// \file cuts/ocd_support.h
/* Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-04-04
 * Last modified: 2013-04-04
 *
 * License:
 *
 * Description:
 *   AND binary cut
 *
 * History:
 *
 */

#ifndef CUTS_OCD_SUPPORT_H
#define CUTS_OCD_SUPPORT_H 1

// Third party:
// - Bayeux/datatools:
#include <datatools/ocd_macros.h>

// This project:
#include <cuts/accept_cut.h>
#include <cuts/reject_cut.h>
#include <cuts/random_cut.h>
#include <cuts/not_cut.h>
#include <cuts/and_cut.h>
#include <cuts/or_cut.h>
#include <cuts/xor_cut.h>
#include <cuts/nand_cut.h>
#include <cuts/nor_cut.h>
#include <cuts/xnor_cut.h>
#include <cuts/multi_and_cut.h>
#include <cuts/multi_or_cut.h>
#include <cuts/multi_xor_cut.h>

DOCD_CLASS_DECLARATION(cuts::accept_cut)
DOCD_CLASS_DECLARATION(cuts::reject_cut)
DOCD_CLASS_DECLARATION(cuts::random_cut)
DOCD_CLASS_DECLARATION(cuts::not_cut)
DOCD_CLASS_DECLARATION(cuts::and_cut)
DOCD_CLASS_DECLARATION(cuts::or_cut)
DOCD_CLASS_DECLARATION(cuts::xor_cut)
DOCD_CLASS_DECLARATION(cuts::nand_cut)
DOCD_CLASS_DECLARATION(cuts::nor_cut)
DOCD_CLASS_DECLARATION(cuts::xnor_cut)
DOCD_CLASS_DECLARATION(cuts::multi_and_cut)
DOCD_CLASS_DECLARATION(cuts::multi_or_cut)
DOCD_CLASS_DECLARATION(cuts::multi_xor_cut)

#endif // CUTS_OCD_SUPPORT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
