// -*- mode: c++; -*- 
// test_geom_data.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <cmath>

#include <GeomUtils/geom_data.h>
#include <GeomUtils/geom_tools.h>
#include <GeomUtils/gnuplot_tools.h>
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/Rotation.h>
#include <CLHEP/Geometry/Transform3D.h>

int main( int argc_ , char ** argv_ )
{
  int error_code=EXIT_SUCCESS;
  try {
    bool debug=false;

    int iarg=1;
    while ( iarg<argc_ ) {
    
      std::string arg=argv_[iarg];

      if ( arg == "-d" || arg == "--debug" ) debug=true;

      // <here you may add more switches...>

      iarg++;
    }
    
    geomutils::geom_data my_geom_data;

    my_geom_data.set_position(CLHEP::Hep3Vector(0.0,0.0,4.0));
    my_geom_data.set_orientation(30.*CLHEP::degree,0.0,0.0);
    my_geom_data.dump(std::cout);
    CLHEP::HepRotation r=
      my_geom_data.get_rotation()*my_geom_data.get_inverse_rotation();
    geomutils::geom_tools::dump(r,
		     std::cout,
		     "Rot x Rot^-1");
 
    CLHEP::Hep3Vector world_pos_1(1.0,0.0,5.0);
    CLHEP::Hep3Vector local_pos_1;
    my_geom_data.world_to_local_pos(world_pos_1,local_pos_1);
    std::cout << "World position #1=" << world_pos_1 << std::endl;
    std::cout << "Local position #1=" << local_pos_1 << std::endl;

    CLHEP::Hep3Vector world_pos_2(0.0,1.0,3.0);
    CLHEP::Hep3Vector local_pos_2;
    my_geom_data.world_to_local_pos(world_pos_2,local_pos_2);
    std::cout << "World position #2=" << world_pos_2 << std::endl;
    std::cout << "Local position #2=" << local_pos_2 << std::endl;

    /********************************************************************/
    double cylinder_radius=2.5;
    double cylinder_length=5.0;
    double box_x=3.0;
    double box_y=1.0;
    double box_z=8.0;
    my_geom_data.set_position(CLHEP::Hep3Vector(0.0,0.0,-3.0));
    my_geom_data.set_orientation(30.0*CLHEP::degree,45.0*CLHEP::degree,0.0);
    my_geom_data.dump(std::cout,"Geometry data:");

    std::ofstream fdraw("test_geom_data_draw.data");
    /*
    geomutils::gnuplot_tools::draw_box(fdraw,
			    my_geom_data.get_position(),
			    my_geom_data.get_rotation(),
			    box_x,
			    box_y,
			    box_z);
    fdraw << std::endl << std::endl;
    */
    geomutils::gnuplot_tools::draw_cyl(fdraw,
			    my_geom_data.get_position(),
			    my_geom_data.get_rotation(),
			    cylinder_radius,
			    cylinder_length);
    fdraw << std::endl << std::endl;

    size_t nshots=100000;
    CLHEP::Hep3Vector world_pos;
    CLHEP::Hep3Vector local_pos;
    for ( int i=0; i<nshots; i++ ) {
      double x=-10.+20.*drand48();
      double y=-10.+20.*drand48();
      double z=-10.+20.*drand48();
      world_pos.set(x,y,z);
      my_geom_data.world_to_local_pos(world_pos,local_pos);
      double abs_z = std::abs(local_pos.z());
      double rho   = hypot(local_pos.x(),local_pos.y());
      if ( (abs_z < 0.5*cylinder_length) && (rho < cylinder_radius) ) {
	fdraw << world_pos.x() << ' ' << world_pos.y() << ' ' << world_pos.z() 
		<< std::endl;
      }
    }
    fdraw << std::endl << std::endl;

    nshots=1000;
    double rho2_max=std::pow(cylinder_radius,2);
    for ( int i=0; i<nshots; i++ ) {
      double theta=CLHEP::twopi*drand48();
      double rho2=rho2_max*drand48();
      double rho=std::sqrt(rho2);
      double z=cylinder_length*(-0.5+drand48());
      local_pos.set(rho*std::cos(theta),rho*std::sin(theta),z);   
      my_geom_data.local_to_world_pos(local_pos,world_pos);
      fdraw << world_pos.x() << ' ' << world_pos.y() << ' ' << world_pos.z() 
	    << std::endl;
    }
    fdraw << std::endl << std::endl;

  }
  catch(std::exception & x){
    std::cerr << "error: " << x.what() << std::endl; 
    error_code=EXIT_FAILURE;
  }
  catch(...){
    std::cerr << "error: " << "unexpected error!" << std::endl; 
    error_code=EXIT_FAILURE;
  }
  return error_code;
}

// end of test_geom_data.cxx
