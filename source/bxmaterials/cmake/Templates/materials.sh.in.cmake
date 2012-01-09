prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_LIBDIR@
includedir=@CMAKE_INSTALL_INCDIR@

Name: @CMAKE_PROJECT_NAME@
Description: The materials API
URL: https://nemo.lpc-caen.in2p3.fr/wiki/materials
Version: @materials_VERSION_STR@
@materials_pkgconfig_Requires@
Libs: -L@CMAKE_INSTALL_LIBDIR@ @materials_pkgconfig_Libs@
Cflags: -I@CMAKE_INSTALL_INCDIR@