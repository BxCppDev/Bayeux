The Bayeux OCD tutorial
=======================

Building the tutorial
---------------------

Here are some instructions to build the OCD tutorial:

1. Setup Bayeux from your environment: basically the ``bxquery`` utility should be
   executable from your environment: ::

     $ which bxquery
     ...
     $ bxquery --prefix
     ...

2. Build the tutorial sample code in a working directory: ::

     $ mkdir _work
     $ cd _work
     $ cmake -DBayeux_DIR=$(bxquery --prefix) ..
     ...
     $ make
     ...


Using the tutorial
------------------

1. Check the OCD registration for the ``my::algo`` class: ::

     $ bxocd_manual --load-dll BxOCDTutorial@$(pwd) --action list | grep ^my::algo$
     List of registered class IDs :
     my::algo

2. Generate documentation for the ``my::algo`` class: ::

     $ bxocd_manual --load-dll BxOCDTutorial@$(pwd) \
         --action show \
         --class-id my::algo \
         > my_algo.rst
     $ pandoc -w html my_algo.rst > my_algo.html
     $ xdg-open my_algo.html &

3. Run the sample program: ::

     $ ./run_my_algo

4. Run the sample program using a supplied configuration file: ::

     $ ./run_my_algo -c ../config/my_algo.conf

5. Generate a skeleton configuration file for the ``my::algo`` class: ::

     $ bxocd_manual --load-dll BxOCDTutorial@$(pwd) \
         --action skeleton \
         --class-id my::algo \
         --output-file my_algo_skel.conf

   and run the sample program using this configuration file: ::

     $ ./run_my_algo -c my_algo_skel.conf

Suppress the tutorial
---------------------

Remove the _work directory: ::

  $ cd ..
  $ rm -fr _work
