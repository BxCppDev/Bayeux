/* ocd_support.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-04-04
 * Last modified : 2013-04-04
 */

#ifndef DPP_OCD_SUPPORT_H_
#define DPP_OCD_SUPPORT_H_ 1

#include <datatools/ocd_macros.h>

#include <dpp/dump_module.h>
#include <dpp/chain_module.h>
#include <dpp/if_module.h>
#include <dpp/input_module.h>
#include <dpp/output_module.h>
#include <dpp/utils_module.h>

DOCD_CLASS_DECLARATION(dpp::dump_module)
DOCD_CLASS_DECLARATION(dpp::chain_module)
DOCD_CLASS_DECLARATION(dpp::if_module)
DOCD_CLASS_DECLARATION(dpp::input_module)
DOCD_CLASS_DECLARATION(dpp::output_module)
DOCD_CLASS_DECLARATION(dpp::utils_module)


#endif // DPP_OCD_SUPPORT_H_

// end of ocd_support.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
