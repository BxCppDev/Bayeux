/* utils.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-02-15
 * Last modified :
 *
 */

#ifndef DPP_UTILS_H_
#define DPP_UTILS_H_ 1

namespace dpp {

  /// Processing status flags used as the returned value of data processing method through the pipeline
  enum processing_status_flags_type {
    PROCESS_OK       = 0,                          /** Module has processed normally and processing
                                                    *  can go on
                                                    */
    PROCESS_SUCCESS  = PROCESS_OK,                 /** idem PROCESS_OK */
    PROCESS_CONTINUE = PROCESS_OK,                 /** idem PROCESS_OK */
    PROCESS_ERROR    = datatools::bit_mask::bit00, /** Module has met an error (considered as
                                                    * non critical)
                                                    */
    PROCESS_FAILURE  = PROCESS_ERROR,              /** idem PROCESS_ERROR */
    PROCESS_STOP     = datatools::bit_mask::bit01, /** Module asks for the stop of the processing
                                                    *  of the current data model in the current
                                                    *  pipeline branch
                                                    */
    PROCESS_FATAL    = datatools::bit_mask::bit02, /** Module has met an error and requests a total
                                                    *  abortion of the processing session
                                                    */
    PROCESS_INVALID  = datatools::bit_mask::bit31  /** Invalid process status */
  };


} // end of namespace dpp

#endif // DPP_UTILS_H_

// end of utils.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
