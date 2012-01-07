prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_LIBDIR@
includedir=@CMAKE_INSTALL_INCDIR@

Name: @CMAKE_PROJECT_NAME@
Description: The geomtools API
URL: https://nemo.lpc-caen.in2p3.fr/wiki/geomtools
Version: @geomtools_VERSION_STR@
@geomtools_pkgconfig_Requires@
Libs: -L@CMAKE_INSTALL_LIBDIR@ @_geomtools_pkgconfig_Libs@
Cflags: -I@CMAKE_INSTALL_INCDIR@