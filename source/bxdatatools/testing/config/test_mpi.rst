==============================================================
Test program ``test_multiproperties_include.cxx``
==============================================================

Input files:
============

* ``test_mpi.conf`` :

  .. code:: ascii
     
     <= foo.conf
     <= bar.conf
  ..

* ``config/defaults/foo.conf`` :

  .. code:: ascii

    [name="config" type="section"]
    foo : string = "default"
    pi : const real = 3.14

    [name="params1" type="section"]
    baz : string = "I am baz"
    value : real as length = 42.0 mm
  ..


* ``config/defaults/bar.conf`` :

  .. code:: ascii

    [name="params2" type="section"]
    bar : string = "I am bar"
    m1 : real as mass = 12.0 kg
  ..

* ``config/user/foo.conf`` :

  .. code:: ascii

    [name="config" type="section"]
    foo : string = "OVERRIDDEN"

    [name="params1" type="section"]
    value : real as length = 43.7 mm

    <= baz.conf
  ..
    
* ``~/bar.conf`` :

  .. code:: ascii

    [name="params2" type="section"]
    m1 : real as mass = 34.0 kg
  ..
    
* ``/tmp/baz.conf`` :

  .. code:: ascii

    [name="config" type="section"]
    pi : const real = 3.14159
  ..


Final result:
=============

.. code:: ascii

   |-- Description  : My config
   |-- Key label    : "name"
   |-- Meta label   : "type"
   |-- Entries      : [3]
   |   |-- Entry : "config"
   |   |   |-- Key        : "config"
   |   |   |-- Meta       : "section"
   |   |   `-- Properties : [2]
   |   |       `-- Properties : [2]
   |   |           |-- Name : 'foo'
   |   |           |   |-- Type  : string (scalar)
   |   |           |   `-- Value : 'OVERRIDDEN'
   |   |           `-- Name : 'pi'
   |   |               |-- Type  : const real (scalar)
   |   |               `-- Value : 3.14159
   |   |-- Entry : "params1"
   |   |   |-- Key        : "params1"
   |   |   |-- Meta       : "section"
   |   |   `-- Properties : [1]
   |   |       `-- Properties : [1]
   |   |           `-- Name : 'value'
   |   |               |-- Type  : real (scalar) [explicit unit='mm']
   |   |               `-- Value : 43.7 mm
   |   `-- Entry : "params2"
   |       |-- Key        : "params2"
   |       |-- Meta       : "section"
   |       `-- Properties : [1]
   |           `-- Properties : [1]
   |               `-- Name : 'm1'
   |                   |-- Type  : real (scalar) [explicit unit='kg']
   |                   `-- Value : 34 kg
   `-- Ordered entries      : [3]
	  |-- Entry [rank=0] : "config"
	  |-- Entry [rank=1] : "params1"
	  `-- Entry [rank=2] : "params2"
..
