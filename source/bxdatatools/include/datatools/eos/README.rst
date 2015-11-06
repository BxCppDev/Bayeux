EOS Portable Archive
====================

General informations
--------------------
 * URL: https://epa.codeplex.com/
 * Version: eos portable archive v5.0
 * The ``eos_portable_archive/5.0`` directory contains the original code (for up to Boost 1.55)


Important remarks
-----------------

 * No problem up to Boost 1.55
   Bayeux/datatools has used eos portable archive v5.0

 * From Boost 1.56: there is a problem with missing ``shared_ptr_helper`` file (X. Garrido)

   The issue is also known from: https://epa.codeplex.com/workitem/2456

   A patch has been proposed (see ``patch/patch-1.txt`` from this directory).
   Unfortunately it seems to be broken due to some broken CR/CR-LF line endings
   (probably the diff was not run with binary mode).
   So I patched it manually for datatools.
