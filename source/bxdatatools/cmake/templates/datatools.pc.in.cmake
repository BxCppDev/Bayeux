prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_LIBDIR@
includedir=@CMAKE_INSTALL_INCDIR@

Name: @CMAKE_PROJECT_NAME@
Description: The datatools API
URL: https://nemo.lpc-caen.in2p3.fr/wiki/datatools
Version: @datatools_VERSION_STR@
@datatools_pkgconfig_Requires@
Libs: -L@CMAKE_INSTALL_LIBDIR@ @_datatools_pkgconfig_Libs@
Cflags: -I@CMAKE_INSTALL_INCDIR@