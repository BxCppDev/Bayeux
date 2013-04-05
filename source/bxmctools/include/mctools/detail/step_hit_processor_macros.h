/* step_hit_processor_macros.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-05-03
 * Last modified : 2013-03-08
 *
 * Copyright (C) 2012-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 *
 * Description:
 *
 *   Macros to help build step hit processor classes.
 *
 * History:
 *
 */

/*! \file step_hit_processor_macros.h
 * \brief Macros to automatically generate some parts 
 *        of the step hit processor interface.
 *
 */

#ifndef MCTOOLS_DETAIL_STEP_HIT_PROCESSOR_H_
#define MCTOOLS_DETAIL_STEP_HIT_PROCESSOR_H_ 1

#include <mctools/base_step_hit_processor.h>

#define MCTOOLS_STEP_HIT_PROCESSOR_CLASS_DECLARE(T)   \
  class T : public ::mctools::base_step_hit_processor \
  /**/

#define MCTOOLS_STEP_HIT_PROCESSOR_INITIALIZE_DECLARE()           \
  public:                                                         \
  virtual void initialize (const ::datatools::properties & config_,     \
                           ::datatools::service_manager & service_mgr_); \
  /**/

#define MCTOOLS_STEP_HIT_PROCESSOR_INITIALIZE_IMPLEMENT_HEAD(T,Config,ServiceMgr) \
  void T::initialize (const ::datatools::properties & Config,           \
                      ::datatools::service_manager & ServiceMgr         \
                      )                                                 \
  /**/

#define MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_HANDLE_DECLARE()             \
  public:                                                               \
  virtual void process (const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits, \
                        ::mctools::simulated_data::hit_handle_collection_type & the_handle_hits); \
  /**/

#define MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_HANDLE_IMPLEMENT_HEAD(T,BaseStepHits,HandleHits) \
  void T::process (const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & BaseStepHits, \
                   ::mctools::simulated_data::hit_handle_collection_type & HandleHits) \
  /**/

#define MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_PLAIN_DECLARE()              \
  public:                                                               \
  virtual void process (const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits, \
                        ::mctools::simulated_data::hit_collection_type & the_plain_hits); \
  /**/

#define MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_PLAIN_IMPLEMENT_HEAD(T,BaseStepHits,PlainHits) \
  void T::process (const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & BaseStepHits, \
                   ::mctools::simulated_data::hit_collection_type & PlainHits) \
  /**/


/** Registration */

#define MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_INTERFACE(SHP_CLASS_NAME) \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (::mctools::base_step_hit_processor,SHP_CLASS_NAME); \
  /**/

#define MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_IMPLEMENT(SHP_CLASS_NAME,SHP_ID) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::mctools::base_step_hit_processor,SHP_CLASS_NAME,SHP_ID); \
  /**/

#endif // MCTOOLS_DETAIL_STEP_HIT_PROCESSOR_H_

// end of step_hit_processor_macros.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
