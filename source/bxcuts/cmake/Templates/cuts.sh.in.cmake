prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_LIBDIR@
includedir=@CMAKE_INSTALL_INCDIR@

Name: @CMAKE_PROJECT_NAME@
Description: The cuts API
URL: https://nemo.lpc-caen.in2p3.fr/wiki/cuts
Version: @cuts_VERSION_STR@
@cuts_pkgconfig_Requires@
Libs: -L@CMAKE_INSTALL_LIBDIR@ @cuts_pkgconfig_Libs@
Cflags: -I@CMAKE_INSTALL_INCDIR@