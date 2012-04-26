# -*- mode: cmake; -*-
###############################################################################
##
## Copyright (C) 2012 François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
## Contact: mauger@lpccaen.in2p3.fr
##
## This file is part of the emfield library.
##
## emfield is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## emfield is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with emfield.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################

if (EXISTS ${PROJECT_SOURCE_DIR}/validation/data )
  message ( STATUS "emfield: validation: Installing validation data..." )
  install ( DIRECTORY ${PROJECT_SOURCE_DIR}/validation/data
            DESTINATION ${CMAKE_INSTALL_DATADIR}/validation
            FILE_PERMISSIONS OWNER_READ 
                             GROUP_READ 
                             WORLD_READ 
            DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
                                  GROUP_READ GROUP_EXECUTE 
                                  WORLD_READ WORLD_EXECUTE 
            PATTERN "*~" EXCLUDE 
           )
endif ()

if (EXISTS ${PROJECT_SOURCE_DIR}/validation/validationDriver.bash )
  message ( STATUS "emfield: validation: Installing validation driver script..." )
  install ( FILES ${PROJECT_SOURCE_DIR}/validation/validationDriver.bash
            DESTINATION ${CMAKE_INSTALL_DATADIR}/validation
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE  
                        GROUP_READ GROUP_EXECUTE
                        WORLD_READ WORLD_EXECUTE 
          )
endif ()

if (EXISTS ${PROJECT_SOURCE_DIR}/validation/scripts )
  message ( STATUS "emfield: validation: Installing validation scripts..." )
  file (GLOB _validation_scripts_bash ${PROJECT_SOURCE_DIR}/validation/scripts/*.bash )
  if ( NOT "${_validation_scripts_bash}" STREQUAL "" )
    install ( FILES ${_validation_scripts_bash}
              DESTINATION ${CMAKE_INSTALL_DATADIR}/validation/scripts
              PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE  
                          GROUP_READ GROUP_EXECUTE
                          WORLD_READ WORLD_EXECUTE 
            )
  endif ()
endif ()

set ( _emfield_validation_tmp_dir "/tmp/\${USER}/emfield/validation/tmp" ) 

add_custom_target ( validation_run
                    COMMAND ${CMAKE_INSTALL_FULL_DATADIR}/validation/validationDriver.bash 
                            --setup ${CMAKE_INSTALL_FULL_ETCDIR}/emfield_setup.sh 
                            --tmp-dir ${_emfield_validation_tmp_dir} 
                            run
                    COMMENT "Run the emfield's validation protocol" 
                  )
#add_dependencies ( validation_run install )

add_custom_target ( validation_report
                    COMMAND ${CMAKE_INSTALL_FULL_DATADIR}/validation/validationDriver.bash 
                            --setup ${CMAKE_INSTALL_FULL_ETCDIR}/emfield_setup.sh 
                            --tmp-dir ${_emfield_validation_tmp_dir} 
                            report
                    COMMENT "Display the emfield's validation protocol report" 
                  )
#add_dependencies ( validation_report validation_run )

add_custom_target ( validation_clean
                    COMMAND ${CMAKE_INSTALL_FULL_DATADIR}/validation/validationDriver.bash 
                            --setup ${CMAKE_INSTALL_FULL_ETCDIR}/emfield_setup.sh 
                            --tmp-dir ${_emfield_validation_tmp_dir} 
                            clean
                    COMMENT "Clean the material from the emfield's validation protocol" 
                  )

unset ( _emfield_validation_tmp_dir )

# end of CMakeLists.txt
