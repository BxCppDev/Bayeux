// -*- mode: c++; -*- 
// test_geom_locator.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/TwoVector.h>
#include <GeomUtils/geom_locator.h>
#include <GeomUtils/box_data.h>
#include <GeomUtils/cylinder_data.h>
#include <GeomUtils/geom_tools.h>
#include <GeomUtils/gnuplot_tools.h>

using namespace geomutils;

struct source_hotspot
{
  int               strip;
  CLHEP::Hep2Vector position;
  double            radius;
};

std::ostream & operator<<( std::ostream & out_ , const source_hotspot & sh_ )
{
  out_ << "(strip=" << sh_.strip;
  out_ << " position=" << sh_.position;
  out_ << " radius=" << sh_.radius;
  out_ << ")";
  return out_;
}

struct gwire_hotspot
{
  int    side, layer, cell, gwire;
  double z;
  double delta_z;
};

std::ostream & operator<<( std::ostream & out_ , const gwire_hotspot & gh_ )
{
  out_ << "(side=" << gh_.side;
  out_ << " layer=" << gh_.layer;
  out_ << " cell=" << gh_.cell;
  out_ << " gwire=" << gh_.gwire;
  out_ << " z=" << gh_.z;
  out_ << " delta_z=" << gh_.delta_z;
  out_ << ")";
  return out_;
}

struct test_locator : public geom_locator
{
public:
  static const int SIDE_BACK  = 0;
  static const int SIDE_FRONT = 1;


public:
  // category index initialized in the '_at_init_do' method:
  int CAT_SOURCE_STRIP;
  int CAT_TC_SIDE;
  int CAT_TC_LAYER;
  int CAT_TC_CELL;
  int CAT_TC_AWIRE;
  int CAT_TC_GWIRE;
  int CAT_HSPOT_SOURCE;
  int CAT_HSPOT_GWIRE;

  int SOURCE_STRIP_ADD; // = 0;
  int SOURCE_SPOT_ADD;  // = 1;
  int SIDE_ADD;         // = 0;
  int LAYER_ADD;        // = 1;
  int CELL_ADD;         // = 2;
  int GWIRE_ADD;        // = 3;
  int GWIRE_SPOT_ADD;   // = 4;

public:
  double strip_width;
  double strip_height;
  double strip_thickness;
  double first_gap;
  double gap_z;
  double cell_length;
  double cell_diameter;
  double awire_diameter;
  double gwire_diameter;
  size_t number_of_strips;
  size_t number_of_layers;
  size_t number_of_cells_per_layer;
  size_t number_of_gwires_per_cell;
  std::vector<source_hotspot> source_hotspots;
  std::vector<gwire_hotspot>  gwire_hotspots;

  box_data source_strip_box;
  box_data tc_layer_box;
  cylinder_data tc_cell_cylinder;
  cylinder_data tc_awire_cylinder;
  cylinder_data tc_gwire_cylinder;

protected:
  void _at_init_do();
  void _at_reset_do();
public:
  test_locator();
  virtual ~test_locator();
  void add_source_hotspot( const source_hotspot & ); 
  void add_gwire_hotspot( const gwire_hotspot & );
  virtual bool validate_id( const geom_id & id_ ) const;
  virtual void locate_id( const geom_id & id_ , 
			  geom_data & gdata_ ) const;
  virtual void find_id( geom_id & id_ , 
			const CLHEP::Hep3Vector & position_ ) const;
  void load_hotspots( const std::string & );
  virtual void dump( std::ostream & , 
		     const std::string & = "test_locator" , 
		     const std::string & = "" , 
		     bool = false ) const;
  virtual void extract_id( const geom_id & , geom_id & ) const;

  std::list<geom_id> get_list_of_id( const std::string & flag_ ) const;

};

void test_locator::_at_init_do()
{
  geom_locator::_at_init_do();
  CAT_SOURCE_STRIP = _categories_dict.get_category_from_label("SOURCE.STRIP");
  CAT_TC_SIDE      = _categories_dict.get_category_from_label("TC.SIDE");
  CAT_TC_LAYER     = _categories_dict.get_category_from_label("TC.LAYER");
  CAT_TC_CELL      = _categories_dict.get_category_from_label("TC.CELL");
  CAT_TC_AWIRE     = _categories_dict.get_category_from_label("TC.AWIRE");
  CAT_TC_GWIRE     = _categories_dict.get_category_from_label("TC.GWIRE");
  CAT_HSPOT_SOURCE = _categories_dict.get_category_from_label("HSPOT.SOURCE");
  CAT_HSPOT_GWIRE  = _categories_dict.get_category_from_label("HSPOT.GWIRE");

}

void test_locator::_at_reset_do()
{
  geom_locator::_at_reset_do();
}

void test_locator::dump( std::ostream & out_ , 
			 const std::string & title_ , 
			 const std::string & indent_ , bool inherit_ ) const
{
  std::string indent=indent_;
  this->geom_locator::dump(out_,title_,indent,true);
  const std::string tag("|-- ");
  const std::string tagc("|   ");
  const std::string last_tag("`-- ");
  const std::string last_tagc("    ");
  const std::string * atag=&tag;
  const std::string * atagc=&tagc;
  
  out_ << indent << *atag << "Strip width                 : " << strip_width/CLHEP::cm << " cm" << std::endl;
  out_ << indent << *atag << "Strip height                : " << strip_height/CLHEP::cm << " cm" << std::endl;
  out_ << indent << *atag << "Strip thickness             : " << strip_thickness/CLHEP::um << " um" << std::endl;
  out_ << indent << *atag << "First gap                   : " << first_gap/CLHEP::cm << " cm" << std::endl;
  out_ << indent << *atag << "Z-gap                       : " << gap_z/CLHEP::cm << " cm" << std::endl;
  out_ << indent << *atag << "Cell length                 : " << cell_length/CLHEP::cm << " cm" << std::endl;
  out_ << indent << *atag << "Cell diameter               : " << cell_diameter/CLHEP::cm << " cm" << std::endl;
  out_ << indent << *atag << "Anode wire diameter         : " << awire_diameter/CLHEP::um << " um" << std::endl;
  out_ << indent << *atag << "Ground wire diameter        : " << gwire_diameter/CLHEP::um << " um" << std::endl;
  out_ << indent << *atag << "Number of strips            : " << number_of_strips << " " << std::endl;
  out_ << indent << *atag << "Number of layers            : " << number_of_layers << " " << std::endl;
  out_ << indent << *atag << "Number of cells/layer       : " << number_of_cells_per_layer << " " << std::endl;
  out_ << indent << *atag << "Number of ground wires/cell : " << number_of_gwires_per_cell << " " << std::endl;

  out_ << indent << *atag << "Source hotspots             : [" << source_hotspots.size() << "]" << std::endl;
  int counter=0;
  for ( std::vector<source_hotspot>::const_iterator i=source_hotspots.begin(); 
	i!=source_hotspots.end(); 
	i++ ) {
    const std::string * atag2=&tag;
    const std::string * atagc2=&tagc;
    if ( counter == source_hotspots.size()-1 ) {
      atag2=&last_tag;
      atagc2=&last_tagc;
    }
    out_ << indent << *atagc << *atag2 << *i << std::endl;
    counter++;
  }
  if ( !inherit_ ) {
    atag=&last_tag;
    atagc=&last_tagc;
  }
  out_ << indent << *atag << "Ground wire hotspots        : [" 
       << gwire_hotspots.size() << "]" << std::endl;
  counter=0;
  for ( std::vector<gwire_hotspot>::const_iterator i=gwire_hotspots.begin(); 
	i!=gwire_hotspots.end(); 
	i++ ) {
    const std::string * atag2=&tag;
    const std::string * atagc2=&tagc;
    if ( counter == gwire_hotspots.size()-1 ) {
      atag2=&last_tag;
      atagc2=&last_tagc;
    }
    out_ << indent << *atagc << *atag2 << *i << std::endl;
    counter++;
  }
}

void test_locator::extract_id( const geom_id & from_id_ , geom_id & to_id_ ) const
{
  // special extract (cast) "TC.CELL" from "HSPOT.GWIRE"
  const cat_entry & from_ce = _categories_dict.get_entry(from_id_.get_category());
  const cat_entry & to_ce   = _categories_dict.get_entry(to_id_.get_category());
  if ( from_ce.get_label() == "HSPOT.GWIRE" && to_ce.get_label() == "TC.CELL" ) {
    int spot = get_address_id(from_id_,"Spot");
    const gwire_hotspot & gwhs = gwire_hotspots.at(spot);
    set_address_id(to_id_,"Side",gwhs.side);
    set_address_id(to_id_,"Layer",gwhs.layer);
    set_address_id(to_id_,"Cell",gwhs.cell);
  }
  else {
    // otherwise, use standard extract rules:
    geom_locator::extract_id(from_id_,to_id_);
  }
  
}

std::list<geom_id> test_locator::get_list_of_id( 
  const std::string & flag_ ) const
{
  std::list<geom_id> id_list;

  if ( flag_ == "SOURCE.STRIP" ) {
    geom_id source_strip_id;
    create_id(source_strip_id,"SOURCE.STRIP");
    for ( int strip =0 ; strip<number_of_strips; strip++ ) {
      set_address_id(source_strip_id,"Strip",strip);
      id_list.push_back(source_strip_id);
    }
  }

  if ( flag_ == "TC.LAYER" ) {
    geom_id layer_id;
    create_id(layer_id,"TC.LAYER");
    for ( int layer =0 ; layer<number_of_layers; layer++ ) {
      set_address_id(layer_id,"Side",SIDE_FRONT);
      set_address_id(layer_id,"Layer",layer);
      id_list.push_back(layer_id);
      set_address_id(layer_id,"Side",SIDE_BACK);
      set_address_id(layer_id,"Layer",layer);
      id_list.push_back(layer_id);
    }
  }

  if ( flag_ == "HSPOT.GWIRE" ) {
    geom_id gw_hs_id;
    create_id(gw_hs_id,"HSPOT.GWIRE");
    int count=0;
    for ( std::vector<gwire_hotspot>::const_iterator i=gwire_hotspots.begin(); 
	  i!=gwire_hotspots.end(); 
	  i++ ) {
      set_address_id(gw_hs_id,"Spot",count);
      id_list.push_back(gw_hs_id);
      count++;
    }
  }
    
  if ( flag_ == "HSPOT.SOURCE" ) {
    geom_id s_hs_id;
    create_id(s_hs_id,"HSPOT.SOURCE");
    int count=0;
    for ( std::vector<source_hotspot>::const_iterator i=source_hotspots.begin(); 
	  i!=source_hotspots.end(); 
	  i++ ) {
      set_address_id(s_hs_id,"Spot",count);
      id_list.push_back(s_hs_id);
      count++;
    }
  }

  return id_list;
}

test_locator::test_locator()
{
  strip_width=10.*CLHEP::cm;
  strip_height=300.*CLHEP::cm;
  strip_thickness=50.*CLHEP::micrometer;
  first_gap=5*CLHEP::cm;
  gap_z=5*CLHEP::cm;
  cell_length=3.*CLHEP::m;
  cell_diameter=3*CLHEP::cm;
  awire_diameter=50*CLHEP::um;
  gwire_diameter=40*CLHEP::um;
  number_of_strips=30;
  number_of_layers=8;
  number_of_cells_per_layer=100;
  number_of_gwires_per_cell=8;
  // initialize geometry volume:
  source_strip_box.set(strip_width,strip_height,strip_thickness);
  tc_layer_box.set(cell_diameter*number_of_cells_per_layer,
		   cell_length,
		   cell_diameter);
  tc_cell_cylinder.set(cell_diameter*0.5,
		       cell_length);
  tc_awire_cylinder.set(awire_diameter*0.5,
			cell_length);
  tc_gwire_cylinder.set(gwire_diameter*0.5,
			cell_length);

  
}

test_locator::~test_locator()
{
  source_hotspots.clear();
  gwire_hotspots.clear();
}

void test_locator::load_hotspots( const std::string & filename_ )
{
  _assert_lock("test_locator::load_hotspots");
  std::ifstream f_hotspots(filename_.c_str());
  if ( !f_hotspots ) {
    std::ostringstream message;
    message << "test_locator::load_hotspots : Cannot open hotsppts file '" << filename_ << "'!";
    throw std::runtime_error(message.str()); 
  }

  int line_counter=0;
  while ( !f_hotspots.eof() ) {
    std::string line;
    std::getline(f_hotspots,line);
    line_counter++;
    std::istringstream iss(line);
    std::string word;
    iss >> word;
    if ( !word.empty() && word[0] != '#' ) {
      std::string hotspot_label;
      std::istringstream line_ss(line);
      line_ss >> hotspot_label;
      cat_entry an_entry;
      try {
	an_entry=_categories_dict.get_entry(hotspot_label);
      }
      catch( std::exception & x) {
	std::ostringstream message;
	message << "test_locator::hotspots: " 
		<< "Format error at line '" 
		<< line_counter
		<< "', unknown category!";
	throw std::logic_error(message.str());	  
	}
      std::string cat_label = an_entry.get_label();
      if ( cat_label == "HSPOT.SOURCE" ) {
	int strip;
	double x,y,radius;
	line_ss >> strip >> x >> y >> radius;
	if ( ! line_ss ) {
	  std::ostringstream message;
	  message << "test_locator::hotspots: " 
		  << "Format error at line '" 
		  << line_counter
		  << "', Invalid source hotspot data!";
	  throw std::logic_error(message.str());	  
	} 
	source_hotspot sh;
	sh.strip=strip;
	sh.position.setX(x);
	sh.position.setY(y);
	sh.radius=radius;
	add_source_hotspot(sh);
      }
      else if ( cat_label == "HSPOT.GWIRE" ) {
	int side, layer, cell, gwire;
	double z, delta_z;
	line_ss >> side >> layer >> cell >> gwire >> z >> delta_z;
	if ( ! line_ss ) {
	  std::ostringstream message;
	  message << "test_locator::hotspots: " 
		  << "Format error at line '" 
		  << line_counter
		  << "', Invalid ground wire hotspot data!";
	  throw std::logic_error(message.str());	  
	} 
	gwire_hotspot gh;
	gh.side=side;
	gh.layer=layer;
	gh.cell=cell;
	gh.gwire=gwire;
	gh.z=z;
	gh.delta_z=delta_z;
	add_gwire_hotspot(gh);
      }
      else {
	std::cerr << "test_locator::hotspots: Ignoring line #" 
		  << line_counter << "!" << std::endl;
      }
    }
  }
}

void test_locator::add_source_hotspot( const source_hotspot & shs_ )
{
  _assert_lock("test_locator::add_source_hotspot");
  source_hotspots.push_back(shs_);
}
 
void test_locator::add_gwire_hotspot( const gwire_hotspot & gwhs_ )
{
  _assert_lock("test_locator::add_source_hotspot");
  gwire_hotspots.push_back(gwhs_);
}

bool test_locator::validate_id( const geom_id & id_ ) const
{
  if ( is_debug() ) std::cerr << "test_locator::validate_id: Entering..." << std::endl;
  // check the category:
  if ( !category_is_valid(id_.get_category()) ) return false;

  // check source strip ID:
  if ( is_id(id_,"SOURCE.STRIP") ) {
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check source strip ID..." << std::endl;
    int strip=get_address_id(id_,"Strip");
    if ( strip <0 || strip >= number_of_strips ) return false;
    return true;
  }

  // check tracking chamber side:
  if ( is_id(id_,"TC.SIDE") ) {
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check tracking chamber side..." << std::endl;
    int side=get_address_id(id_,"Side");
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check tracking chamber side...get_address_id ok" << std::endl;
    if ( side != SIDE_BACK && side != SIDE_FRONT ) return false;
    return true;
  }

  // check tracking chamber layer:
  if ( is_id(id_,"TC.LAYER") ) {
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check tracking chamber layer..." << std::endl;
    geom_id tc_side_id;
    create_id(tc_side_id,"TC.SIDE");
    extract_id(id_,tc_side_id);
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check tracking chamber layer: TC side id is: " << to_string_id(tc_side_id) << std::endl;
    if ( !validate_id(tc_side_id) ) return false; 
    int layer=get_address_id(id_,"Layer");
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check tracking chamber layer...get_address_id ok" << std::endl;
    if ( layer < 0 || layer >= number_of_layers ) return false;
    return true;
  }

  // check drift cell:
  if (  is_id(id_,"TC.CELL") ) {
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check drift cell..." << std::endl;
    geom_id tc_layer_id;
    create_id(tc_layer_id,"TC.LAYER");
    extract_id(id_,tc_layer_id);
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check drift cell: TC layer id is: " << to_string_id(tc_layer_id) << std::endl;
    if ( !validate_id(tc_layer_id) ) return false; 
    int cell=get_address_id(id_,"Cell");
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check drift cell...get_address_id ok" << std::endl;
    if ( cell < 0 || cell >= number_of_cells_per_layer ) return false;
    return true;
  }

  // check anode wire:
  if ( is_id(id_,"TC.AWIRE") ) {
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check anode wire..." << std::endl;
    geom_id tc_cell_id;
    create_id(tc_cell_id,"TC.CELL");
    extract_id(id_,tc_cell_id);
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check anode wire: TC cell id is: " << to_string_id(tc_cell_id) << std::endl;
    if ( !validate_id(tc_cell_id) ) return false; 
    return true;
  }

  // check ground wire:
  if ( is_id(id_,"TC.GWIRE") ) {
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check ground wire..." << std::endl;
    geom_id tc_cell_id;
    create_id(tc_cell_id,"TC.CELL");
    extract_id(id_,tc_cell_id);
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check ground wire: TC cell id is: " << to_string_id(tc_cell_id) << std::endl;
    if ( !validate_id(tc_cell_id) ) return false; 
    int gwire=get_address_id(id_,"Wire");
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check ground wire...get_address_id ok" << std::endl;
    if ( gwire < 0 || gwire >= number_of_gwires_per_cell ) return false;
    return true;
  }

  // check hot spot on source strip:
  if ( is_id(id_,"HSPOT.SOURCE") ) {
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check hot spot on source strip..." << std::endl;
    int spot=get_address_id(id_,"Spot");
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check hot spot on source strip...get_address_id ok" << std::endl;
    if ( spot < 0 || spot >=source_hotspots.size() ) return false;
    return true;
  }

  // check hot spot on ground wire:
  if ( is_id(id_,"HSPOT.GWIRE") ) {
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check hot spot on ground wire... " << std::endl;
    int spot=get_address_id(id_,"Spot");
    if ( is_debug() ) std::cerr << "test_locator::validate_id: check hot spot on ground wire...get_address_id ok" << std::endl;
    if ( spot < 0 || spot >=gwire_hotspots.size() ) return false;
    return true;
  }
  
  return false;
}

void test_locator::locate_id( const geom_id & id_ , geom_data & gdata_ ) const
{
  if ( !validate_id(id_) ) {
    std::ostringstream message;
    std::string id_str;
    encode_id(id_,id_str);
    message << "test_locator::locate_id: ID=" << id_str << " is not valid!";
    throw std::runtime_error(message.str());
  }

  if ( is_id(id_,"SOURCE.STRIP") ) {
    int    strip=get_address_id(id_,"Strip");
    double width=number_of_strips*strip_width;
    double z=0.0;
    double y=0.0;
    double x=-0.5*width+(0.5+strip)*strip_width;
    gdata_.set_position(CLHEP::Hep3Vector(x,y,z));
    gdata_.set_orientation(0,0,0);
    gdata_.set_shape(source_strip_box);
    return;
  } 

  if ( is_id(id_,"TC.LAYER") ) {
    int    side=get_address_id(id_,"Side");
    int    layer=get_address_id(id_,"Layer");
    //double z=first_gap+(0.5+layer)*cell_diameter+gap_z;
    double z=first_gap+layer*gap_z;
    if ( side == SIDE_BACK ) z *= -1; 
    double y=0.0;
    double x=0.0;
    gdata_.set_position(CLHEP::Hep3Vector(x,y,z));
    gdata_.set_orientation(0,0,0);
    gdata_.set_shape(tc_layer_box);
    return;
  } 

  if ( is_id(id_,"TC.CELL") ) {
    geom_id layer_id; 
    create_id(layer_id,"TC.LAYER");
    extract_id(id_,layer_id); // retrieve parent layer id
    locate_id(layer_id,gdata_);
    int    cell=get_address_id(id_,"Cell");
    double width=number_of_cells_per_layer*cell_diameter;
    double x = -0.5*width+(0.5+cell)*cell_diameter;
    gdata_.get_position().setX(x);
    gdata_.set_orientation(90*CLHEP::degree,90*CLHEP::degree,0*CLHEP::degree);
    gdata_.set_shape(tc_cell_cylinder);
    return;
  }

  if ( is_id(id_,"TC.AWIRE") ) {
    geom_id cell_id; 
    create_id(cell_id,"TC.CELL");
    extract_id(id_,cell_id); // retrieve parent cell id
    locate_id(cell_id,gdata_);
    gdata_.set_shape(tc_awire_cylinder);
    return;
  }
 
  if ( is_id(id_,"TC.GWIRE") ) {
    int gwire=get_address_id(id_,"Wire");
    geom_id cell_id; 
    create_id(cell_id,"TC.CELL");
    extract_id(id_,cell_id); // retrieve parent cell id
    locate_id(cell_id,gdata_);
    double delta_theta=CLHEP::twopi/number_of_gwires_per_cell;
    double theta=delta_theta*(0.5+gwire);
    double dz=0.5*cell_diameter*std::cos(theta);
    double dx=0.5*cell_diameter*std::sin(theta);
    gdata_.get_position()+=CLHEP::Hep3Vector(dx,0,dz);
    gdata_.set_shape(tc_gwire_cylinder);
    return;
  }

  if ( is_id(id_,"HSPOT.SOURCE") ) {
    int spot=get_address_id(id_,"Spot");
    const source_hotspot * ss = &source_hotspots.at(spot);
    geom_id strip_id;
    create_id(strip_id,"SOURCE.STRIP");
    set_address_id(strip_id,"Strip",ss->strip);
    // locate the source strip:
    geom_data strip_gdata;
    locate_id(strip_id,strip_gdata);
    // locate the spot on the strip:
    gdata_.set_position(strip_gdata.get_position()+ss->position);
    gdata_.set_orientation(0*CLHEP::degree,0*CLHEP::degree,0*CLHEP::degree);
    gdata_.set_shape(new cylinder_data(ss->radius,strip_thickness));
    return;
  }

  if ( is_id(id_,"HSPOT.GWIRE") ) {
    int spot=get_address_id(id_,"Spot");
    const gwire_hotspot * gs = &gwire_hotspots.at(spot);
    geom_id gwire_id;
    create_id(gwire_id,"TC.GWIRE");
    set_address_id(gwire_id,"Side",gs->side);
    set_address_id(gwire_id,"Layer",gs->layer);
    set_address_id(gwire_id,"Cell",gs->cell);
    set_address_id(gwire_id,"Wire",gs->gwire);
    // locate the ground wire:
    geom_data gwire_gdata;
    locate_id(gwire_id,gwire_gdata);
    // locate the spot on the wire:
    gdata_.set_position(gwire_gdata.get_position()+CLHEP::Hep3Vector(0,gs->z,0));
    gdata_.set_orientation(gwire_gdata.get_rotation());
    gdata_.set_shape(new cylinder_data(gwire_diameter*0.5,gs->delta_z));
    return;
  }

  std::ostringstream message;
  std::string id_str;
  encode_id(id_,id_str);
  message << "test_locator::locate_id: Cannot handle ID=" << id_str << "!";
  throw std::runtime_error(message.str());

}

void test_locator::find_id( geom_id & id_ , 
			    const CLHEP::Hep3Vector & position_ ) const
{
  // check category:
  if ( !category_is_valid(id_.get_category()) ) return;

  // set ID ar invalid address:
  id_.reset_address();

  double x=position_.x();
  double y=position_.y();
  double z=position_.z();
  
  if ( is_id(id_,"SOURCE.STRIP") ) {
    if ( std::abs(z) > 0.5*strip_thickness ) return;
    if ( std::abs(y) > 0.5*strip_height ) return;
    double width=number_of_strips*strip_width;
    if ( std::abs(x) > 0.5*strip_width ) {
      return;
    }
    double x_min=-0.5*width;
    int strip=(int)((x-x_min)/strip_width);
    if ( strip < 0 || strip >=number_of_strips)  return;
    set_address_id(id_,"Strip",strip);
    return;
  }

  if ( is_id(id_,"TC.CELL") ) {
    // check y:
    if ( std::abs(y) > 0.5*cell_length ) return;
    // check x:
    double width=number_of_cells_per_layer*cell_diameter;
    if ( std::abs(x) > 0.5*strip_width ) return;
    int cell = (int) (( x+0.5*width )/cell_diameter);
    if ( cell < 0 || cell >=number_of_cells_per_layer)  return;
    // check z:
    double zp=std::abs(z);
    int l=(int) ( (zp+0.5*cell_diameter-first_gap)/gap_z);
    double dz=zp-first_gap-l*gap_z;
    if ( std::abs(dz) > 0.5*cell_diameter ) return;
    int layer=l;
    if ( layer < 0 || layer >=number_of_layers)  return;
    int side=SIDE_BACK;
    if ( z > 0.0 ) side=SIDE_FRONT;
    set_address_id(id_,"Side",side);
    set_address_id(id_,"Layer",layer);
    set_address_id(id_,"Cell",cell);
    return;
  }

}

int main( int argc_ , char ** argv_ )
{
  int error_code=EXIT_SUCCESS;
  try {
    bool debug=false;

    int iarg=1;
    while ( iarg<argc_ ) {
    
      std::string arg=argv_[iarg];
      
      if ( arg == "-d" || arg == "--debug" ) debug=true;
      
      iarg++;	
    }
    
    std::string dict_filename     = "./resources/test_geom_locator.dict";
    std::string hotspots_filename = "./resources/test_geom_locator.hotspots";
    test_locator my_locator;
    my_locator.set_debug(debug);
    my_locator.load_cat_dictionary(dict_filename);
    my_locator.load_hotspots(hotspots_filename);
    my_locator.init();
    my_locator.dump(std::cout);

    // declare a dummy id:
    std::cout << "declare a dummy id: " << std::endl;
    geomutils::geom_id my_id;

    // make it a "HSPOT.SOURCE" id:
    std::cout << "make it a 'HSPOT.SOURCE' id: " << std::endl;
    my_locator.create_id(my_id,"HSPOT.SOURCE");

    // assign an address to it:
    std::cout << "assign an address to it: " << std::endl;
    my_locator.set_address_id(my_id,"Spot",1);   // set the spot number on this strip
    /* alternative is (because you know the order):
     *   my_locator.set_full_address_id(my_id,10,1);  // set the strip & spot number
     */
    if ( !my_locator.validate_id(my_id) ) {
      std::cout << "id is not valid!" << std::endl;      
    }
    else {
      std::cout << "id is valid!" << std::endl;      
    }
    std::cout << std::endl;      
    std::cout << "convert to a human readable string and print: " << std::endl;
    // convert to a human readable string and print:
    std::cout << "ID=" << my_locator.to_string_id(my_id) << std::endl;
    std::cout << std::endl;      

    // assign a new (invalid) address to it (here spot #666 is invalid!):
    std::cout << "assign a new (invalid) address to it: " << std::endl;
    my_locator.set_address_id(my_id,"Spot",666); // set the spot number
    /* or:
     *   my_locator.set_address_id(my_id,0,666);
     */
    if ( !my_locator.validate_id(my_id) ) {
      std::cout << "id is not valid!" << std::endl;      
    }
    else {
      std::cout << "id is valid!" << std::endl;      
    }
    std::cout << "ID=" << my_locator.to_string_id(my_id) << std::endl;
    std::cout << std::endl;      

    // now make it a "HSPOT.GWIRE" id:
    std::cout << "now make it a 'HSPOT.GWIRE' id: " << std::endl;
    my_locator.create_id(my_id,"HSPOT.GWIRE");
    std::cout << "assign an address to it: " << std::endl;
    my_locator.set_address_id(my_id,"Spot",3);
    std::cout << "ok!" << std::endl;
    if ( !my_locator.validate_id(my_id) ) {
      std::cout << "id is not valid!" << std::endl;      
    }
    else {
      std::cout << "id is valid!" << std::endl;      
    }
    std::cout << "convert to a human readable string and print: " << std::endl;
    std::cout << "ID=" << my_locator.to_string_id(my_id) << std::endl;
    std::cout << std::endl;      

    my_locator.set_address_id(my_id,"Spot",8);
    if ( !my_locator.validate_id(my_id) ) {
      std::cout << "id is not valid!" << std::endl;      
    }
    else {
      std::cout << "id is valid!" << std::endl;      
    }
    std::cout << "ID=" << my_locator.to_string_id(my_id) << std::endl;
    std::cout << std::endl;      

    my_locator.create_id(my_id,"HSPOT.GWIRE");
    my_locator.set_address_id(my_id,"Spot",0);
    if ( !my_locator.validate_id(my_id) ) {
      std::cout << "id is not valid!" << std::endl;      
    }
    else {
      std::cout << "id is valid!" << std::endl;      
    }
    std::cout << "ID=" << my_locator.to_string_id(my_id) << std::endl;
    std::cout << std::endl;      

    std::cout << "Enter ID (examples: [TC.LAYER:0.3], [TC.CELL:1.0.17]): ";
    std::string line;
    std::getline(std::cin,line);
    std::istringstream iss(line);
    std::string word;
    iss >> std::ws >> word >> std::ws;
    if ( !my_locator.decode_id(my_id,word) ) {
      throw std::runtime_error("Invalid format!");
    }
    std::cout << "ID=" << my_locator.to_string_id(my_id) << std::endl;
    if ( !my_locator.validate_id(my_id) ) {
      std::cout << "This id is not valid!" << std::endl;      
    }
    else {
      std::cout << "This id is valid!" << std::endl;      
    }

    
    std::cout << std::endl;      
    std::cout << "Retrieve information about some cell: " << std::endl;
    my_locator.create_id(my_id,"TC.CELL");
    my_locator.set_address_id(my_id,"Side",1); // set the side number
    my_locator.set_address_id(my_id,"Layer",3); // set the layer number
    my_locator.set_address_id(my_id,"Cell",28); // set the cell number
    if ( !my_locator.validate_id(my_id) ) {
      std::cout << "This id is not valid!" << std::endl;      
    }
    else {
      std::cout << "This id is valid!" << std::endl;      
      geom_data gdata;
      my_locator.locate_id(my_id,gdata);
      std::cout << "ID=" << my_locator.to_string_id(my_id) << std::endl;
      gdata.dump(std::cout,"Geometry informations about cell '"+my_locator.to_string_id(my_id)+"':");
    }

    // draw source strips:
    std::ofstream strips_data("strips.data");
    std::list<geom_id> strip_ids = my_locator.get_list_of_id("SOURCE.STRIP");
    std::cout << "Strips:" << std::endl;
    for ( std::list<geom_id>::const_iterator i=strip_ids.begin();
	  i!=strip_ids.end();
	  i++ ) {
      //std::cout << my_locator.to_string_id(*i) << std::endl;
      geomutils::geom_data strip_gd;
      my_locator.locate_id(*i,strip_gd);
      strip_gd.dump(std::cout,
		    "Source strip '"
		    +my_locator.to_string_id(*i)+"' geometry data:");
      std::string shape_name = strip_gd.get_shape_name();
      if ( shape_name == "box" ) {
	const geomutils::box_data & bd=
          static_cast<const geomutils::box_data &>(strip_gd.get_shape());
	geomutils::gnuplot_tools::draw_rect(strips_data,
					    strip_gd.get_position(),
					    strip_gd.get_rotation(),
					    bd.get_x(),bd.get_y());
      }

    }

    // draw TC layers:
    std::ofstream layers_data("layers.data");
    std::list<geom_id> layer_ids = my_locator.get_list_of_id("TC.LAYER");
    std::cout << "TC layers:" << std::endl;
    for ( std::list<geom_id>::const_iterator i=layer_ids.begin();
	  i!=layer_ids.end();
	  i++ ) {
      geomutils::geom_data layer_gd;
      my_locator.locate_id(*i,layer_gd);
      layer_gd.dump(std::cout,
		    "Layer '"
		    +my_locator.to_string_id(*i)+"' geometry data:");
      std::string shape_name = layer_gd.get_shape_name();
      if ( shape_name == "box" ) {
	const geomutils::box_data & bd=
          static_cast<const geomutils::box_data &>(layer_gd.get_shape());
	geomutils::gnuplot_tools::draw_box(layers_data,
					    layer_gd.get_position(),
					    layer_gd.get_rotation(),
					    bd.get_x(),bd.get_y(),bd.get_z());
      }

    }

    // draw Source hot spots:
    std::ofstream shs_data("shs.data");
    std::list<geom_id> shs_ids = my_locator.get_list_of_id("HSPOT.SOURCE");
    std::cout << "Source hot spots:" << std::endl;
    for ( std::list<geom_id>::const_iterator i=shs_ids.begin();
	  i!=shs_ids.end();
	  i++ ) {
      geomutils::geom_data shs_gd;
      my_locator.locate_id(*i,shs_gd);
      shs_gd.dump(std::cout,
		    "Hot spots '"
		    +my_locator.to_string_id(*i)+"' geometry data:");
      std::string shape_name = shs_gd.get_shape_name();
      if ( shape_name == "cylinder" ) {
	const geomutils::cylinder_data & cd=
          static_cast<const geomutils::cylinder_data &>(shs_gd.get_shape());
	geomutils::gnuplot_tools::draw_cyl(shs_data,
					   shs_gd.get_position(),
					   shs_gd.get_rotation(),
					   cd.get_r(),cd.get_z());
      }

    }

    // draw ground wire hot spots:
    std::ofstream gwhs_data("gwhs.data");
    std::list<geom_id> gwhs_ids = my_locator.get_list_of_id("HSPOT.GWIRE");
    std::cout << "Source hot spots:" << std::endl;
    for ( std::list<geom_id>::const_iterator i=gwhs_ids.begin();
	  i!=gwhs_ids.end();
	  i++ ) {
      geomutils::geom_data gwhs_gd;
      my_locator.locate_id(*i,gwhs_gd);
      gwhs_gd.dump(std::cout,
		    "Hot spots '"
		    +my_locator.to_string_id(*i)+"' geometry data:");
      std::string shape_name = gwhs_gd.get_shape_name();
      if ( shape_name == "cylinder" ) {
	const geomutils::cylinder_data & cd=
          static_cast<const geomutils::cylinder_data &>(gwhs_gd.get_shape());
	double r=cd.get_r();
	r = 5.;
	// draw the associated cell
	geomutils::geom_id cell_id;
	my_locator.create_id(cell_id,"TC.CELL");
	my_locator.extract_id(*i,cell_id);
	std::cerr << "DEBUG: GWHS=" << my_locator.to_string_id(*i) << std::endl;
	std::cerr << "DEBUG: Cell=" << my_locator.to_string_id(cell_id) << std::endl;
	geomutils::geom_data cell_gd;
	my_locator.locate_id(cell_id,cell_gd);
	geomutils::gnuplot_tools::draw_circ(gwhs_data,
					   cell_gd.get_position(),
					   cell_gd.get_rotation(),
					   my_locator.cell_diameter*0.5);
	geomutils::gnuplot_tools::draw_cyl(gwhs_data,
					   gwhs_gd.get_position(),
					   gwhs_gd.get_rotation(),
					   r,cd.get_z());
      }

    }

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

// end of test_geom_locator.cxx
