# SetParallelJobs.cmake

include ( FindNumberOfProcessors )
get_number_of_cores ( PROCESSOR_COUNT )
message ( STATUS "Number of processors = ${PROCESSOR_COUNT}" )

option ( BAYEUX_SINGLE_PROCESSOR "Bayeux will use only one processor" OFF )
option ( BAYEUX_TWO_PROCESSORS   "Bayeux will use only two processors" OFF )
option ( BAYEUX_HALF_PROCESSORS  "Bayeux will use only half of the available processors" OFF )

if ( BAYEUX_SINGLE_PROCESSOR )
  set ( PROCESSOR_COUNT 1 )
elseif ( BAYEUX_TWO_PROCESSORS )
  if ( PROCESSOR_COUNT GREATER 1 )
    set ( PROCESSOR_COUNT 2 )
  endif ( )
elseif ( BAYEUX_HALF_PROCESSORS )
  if ( PROCESSOR_COUNT GREATER 1 )
    math ( EXPR PROCESSOR_COUNT "${PROCESSOR_COUNT} / 2" )
    message ( STATUS "Using half the available processors : ${PROCESSOR_COUNT}" )
  else ( )
    set ( PROCESSOR_COUNT 1 )
  endif ( )
endif ( )

set( BAYEUX_PARALLEL_JOBS
  ${PROCESSOR_COUNT}
  CACHE STRING
  "Number of parallel jobs for external project builds"
  )
message( STATUS "External projects will build with ${BAYEUX_PARALLEL_JOBS} parallel jobs" )

# end of SetParallelJobs.cmake