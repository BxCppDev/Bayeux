// -*- mode: c++; -*-
// tts.cxx

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

struct color
{  
public:
  typedef map<string, color> color_map_t;
  string name;
  string hexa;
  int r, g, b;
public:
  color ();
public:
  static color_map_t g_map;
  static void load_map (const string &);
  static void print_map (ostream & out_);
};

color::color ()
{
  name = "black";
  hexa = "#000000";
  r = g = b = 0;
}

color::color_map_t color::g_map;

void color::load_map (const string & f_)
{
  ifstream f (f_.c_str ());
  while (! f.eof ())
    {
      color c;
      f >> c.name >> c.hexa >> c.r >> c.g >> c.b >> ws;
      g_map[c.name] = c;
    }
}

void color::print_map (ostream & out_)
{
  out_ << "# colors map:" << endl;
  for (color_map_t::const_iterator i = g_map.begin ();
       i != g_map.end ();
       i++)
    {
      out_ << "  " << i->second.name << ": (" 
	   << i->second.r << ',' 
	   << i->second.g << ',' 
	   << i->second.b << ") [" << i->second.hexa << "]" << endl ;
    }
}


struct vertex
{
  double x, y ,z;
  map<int, int> ref_facets;
public:
  vertex (double x_ = 0.0, double y_ = 0.0, double z_ = 0.0);
  void print_xyz (ostream & out_, int color_ = 0) const;
  void print (ostream & out_) const;
  void add_ref_facet (int facet_index_, int facet_node_);
};

vertex::vertex (double x_, double y_, double z_)
{
  x = x_;
  y = y_;
  z = z_;
}

void vertex::add_ref_facet (int facet_index_, int facet_node_)
{
  ref_facets[facet_index_] = facet_node_;
}

void vertex::print_xyz (ostream & out_, int color_) const
{
  out_ << x << ' ' << y << ' ' << z << ' ' << color_ << endl;
}

void vertex::print (ostream & out_) const
{
  out_ << '(' << x << ' ' << y << ' ' << z << ')' 
       << " ref=" << ref_facets.size  () << endl;
}

/**************************/

class i_facet
{
private:
  int __color;
public:
  void set_color (int);
  int get_color () const;
  virtual size_t get_number_of_vertices () const = 0;
  virtual const vertex & get_vertex (int i_) const = 0;
  void print (ostream & out_) const;
  double get_color_2 (int node_) const;
};

double i_facet::get_color_2 (int node_) const
{
  return 0.0; // not used.
}

void i_facet::set_color (int color_)
{
  __color = color_;
}

int i_facet::get_color () const
{
  return __color;
}

void i_facet::print (ostream & out_) const
{
  out_ << "#vertices=" << get_number_of_vertices () << endl;
}

/**************************/

class facet3 : public i_facet
{
private:
  const vertex * __v[3];
public:
  virtual size_t get_number_of_vertices () const;
  virtual const vertex & get_vertex (int i_) const;
  facet3 ();
  facet3 (const vertex & v1_, const vertex & v2_, const vertex & v3_);

};

size_t facet3::get_number_of_vertices () const
{
  return 3;
}

const vertex & facet3::get_vertex (int i_) const 
{
  if ((i_ < 0) && (i_ >= get_number_of_vertices ()))
    {
      ostringstream message;
      message << "facet3::get_vertex: "
	      << "Invalid vertex index " << i_ << " !";
      throw runtime_error (message.str());
    }
  return *__v[i_];
}

facet3::facet3 ()
{
  __v[0] = 0;
  __v[1] = 0;
  __v[2] = 0;
}

facet3::facet3 (const vertex & v0_, const vertex & v1_, const vertex & v2_)
{
  __v[0] = &v0_;
  __v[1] = &v1_;
  __v[2] = &v2_;
}

/**************************/

class facet4 : public i_facet
{
private:
  const vertex * __v[4];
private:
  bool __is_valid () const;
public:
  virtual size_t get_number_of_vertices () const;
  virtual const vertex & get_vertex (int i_) const;
  facet4 ();
  facet4 (const vertex & v1_, const vertex & v2_, const vertex & v3_, const vertex & v4_);

};

bool facet4::__is_valid () const
{
  // missing checks...
  return true;
}

size_t facet4::get_number_of_vertices () const
{
  return 4;
}

const vertex & facet4::get_vertex (int i_) const 
{
  if ((i_ < 0) && (i_ >= get_number_of_vertices ()))
    {
      ostringstream message;
      message << "facet4::get_vertex: "
	      << "Invalid vertex index " << i_ << " !";
      throw runtime_error (message.str());
    }
  return *__v[i_];
}

facet4::facet4 ()
{
  __v[0] = 0;
  __v[1] = 0;
  __v[2] = 0;
  __v[3] = 0;
}

facet4::facet4 (const vertex & v0_, const vertex & v1_, const vertex & v2_, const vertex & v3_)
{
  __v[0] = &v0_;
  __v[1] = &v1_;
  __v[2] = &v2_;
  __v[3] = &v3_;
  if (! __is_valid ())
    {
      throw runtime_error ("facet4::facet4: Invalid set of vertices !");
    }
}

/*********************************************/

class tessellated_solid
{
public:
  typedef map<unsigned int, vertex> vertices_col_t;
  typedef map<unsigned int, i_facet *> facets_col_t;
  static const int INVALID_VERTEX = -1.0;
  static const int MAX_VERTEX     = 0x0FFFFFFF;

private:
  bool           __locked;
  vertices_col_t __vertices;
  facets_col_t   __facets;

public:
  static bool validate_index (int);
  bool is_locked () const;
  tessellated_solid ();
  virtual ~tessellated_solid ();
  const vertices_col_t & vertices () const;
  const facets_col_t & facets () const;
  int add_vertex (unsigned int vtx_key_, const vertex & vtx_);
  int add_vertex (unsigned int vtx_key_, 
		   double x_, double y_, double z_);
  void add_facet (unsigned int facet_key_, 
		  int ivtx0_, int ivtx1_, int ivtx2_, 
		  int ivtx3_ = INVALID_VERTEX, 
		  int color_ = 0);
  void add_facet3 (unsigned int facet_key_, 
		   int ivtx0_, int ivtx1_, int ivtx2_, 
		   int color_ = 0);
  void add_facet4 (unsigned int facet_key_, 
		   int ivtx0_, int ivtx1_, int ivtx2_, 
		   int ivtx3_, 
		   int color_ = 0);
  void lock ();
  void unlock ();
  void print_xyz (ostream & out_) const;
  void dump (ostream & out_ = clog) const;

private:
  bool __check () const;
};


bool tessellated_solid::is_locked () const
{
  return __locked;
}

bool tessellated_solid::__check () const
{
  for (vertices_col_t::const_iterator i = __vertices.begin ();
       i != __vertices.end ();
       i++)
    {
      if (i->second.ref_facets.size () == 0)
	{
	  clog << "WARNING: tessellated_solid::lock: Vertex "
	       << i->first << " is not used at all !" << endl;
	}
      else if (i->second.ref_facets.size () < 3) 
	{
	  clog << "WARNING: tessellated_solid::lock: Vertex "
	       << i->first << " is not properly referenced !" << endl;
	  return false;
	}
    }
  // missing check:


  return true;
}

void tessellated_solid::lock ()
{
  if (! __check ())
    {
      throw runtime_error ("tessellated_solid::lock: Solid is not validated !");
    }
  __locked = true;
}

void tessellated_solid::unlock ()
{
  __locked = false;
}

bool tessellated_solid::validate_index (int i_)
{
  return (i_ > INVALID_VERTEX) && (i_ < MAX_VERTEX);
}

tessellated_solid::tessellated_solid ()
{
  __locked = false;
}

tessellated_solid::~tessellated_solid ()
{
  for (facets_col_t::iterator i = __facets.begin ();
       i != __facets.end ();
       i++)
    {
      delete i->second;
    }
  __facets.clear ();
  __vertices.clear ();
}

const tessellated_solid::vertices_col_t & tessellated_solid::vertices () const
{
  return __vertices;
}

const tessellated_solid::facets_col_t & tessellated_solid::facets () const
{
  return __facets;
}

int tessellated_solid::add_vertex (unsigned int vtx_key_, const vertex & vtx_)
{
  if (__locked)
    {
      throw runtime_error ("tessellated_solid::add_vertex: Object is locked !");
    }
  __vertices[vtx_key_] = vtx_;
  return vtx_key_;
}

int tessellated_solid::add_vertex (unsigned int vtx_key_, double x_, double y_, double z_)
{
  vertex vtx;
  vtx.x = x_;
  vtx.y = y_;
  vtx.z = z_;
  return add_vertex (vtx_key_, vtx);
}
  
void tessellated_solid::add_facet3 (unsigned int facet_key_, 
				    int ivtx0_, int ivtx1_, int ivtx2_, 
				    int color_)
{
  add_facet (facet_key_,  ivtx0_, ivtx1_, ivtx2_, INVALID_VERTEX, color_);
}

void tessellated_solid::add_facet4 (unsigned int facet_key_, 
				    int ivtx0_, int ivtx1_, int ivtx2_, int ivtx3_, 
				    int color_)
{
  add_facet (facet_key_,  ivtx0_, ivtx1_, ivtx2_, ivtx3_, color_);
}
   
void tessellated_solid::add_facet (unsigned int facet_key_, 
				   int ivtx0_, 
				   int ivtx1_, 
				   int ivtx2_, 
				   int ivtx3_, 
				   int color_)
{
  if (__locked)
    {
      throw runtime_error ("tessellated_solid::add_vertex: Object is locked !");
    }
  i_facet * f= 0;
  size_t dim = 3;
  if (validate_index (ivtx3_))
    {
      dim = 4;
    }
  int ivtx[4];
  ivtx[0] = ivtx0_;
  ivtx[1] = ivtx1_;
  ivtx[2] = ivtx2_;
  ivtx[3] = ivtx3_;
  vertices_col_t::iterator vtx_it[4];
  for (int i = 0; i < dim; i++)
    {
      vtx_it[i] = __vertices.find (ivtx[i]);
      if (vtx_it[i] == __vertices.end ())
	{
	  ostringstream message;
	  message << "tessellated_solid::add_facet: "
		  << "Invalid vertex index " << ivtx[i] << " !";
	  throw runtime_error (message.str());
	}
    } 
  i_facet * fct = 0;
  if (dim == 3)
    {
      vtx_it[0]->second.add_ref_facet (facet_key_, 0);
      vtx_it[1]->second.add_ref_facet (facet_key_, 1);
      vtx_it[2]->second.add_ref_facet (facet_key_, 2);
      fct = new facet3 ((vtx_it[0]->second),
			(vtx_it[1]->second),
			(vtx_it[2]->second));
    }    
  else
    {
      vtx_it[0]->second.add_ref_facet (facet_key_, 0);
      vtx_it[1]->second.add_ref_facet (facet_key_, 1);
      vtx_it[2]->second.add_ref_facet (facet_key_, 2);
      vtx_it[3]->second.add_ref_facet (facet_key_, 3);
      fct = new facet4 ((vtx_it[0]->second),
			(vtx_it[1]->second),
			(vtx_it[2]->second),
			(vtx_it[3]->second));
    } 
  fct->set_color (color_);   
  __facets[facet_key_] = fct;
}

void tessellated_solid::dump (ostream & out_) const
{
  string tag   = "|-- ";
  string stag  = "|   ";
  string ltag  = "`-- ";
  string sltag = "    ";
  out_ << "tessellated_solid" << endl;
  out_ << tag << "Locked: " << is_locked () << endl;
  out_ << tag << "Vertices: " << __vertices.size () << endl;
  for (vertices_col_t::const_iterator i = __vertices.begin ();
       i != __vertices.end ();
       i++)
    {
      string tag2 = tag;
      {
	vertices_col_t::const_iterator j = i;
	if (++j == __vertices.end ()) tag2 = ltag;
      }
      out_ << stag << tag2 << "Vertex[" << i->first << "] : ";
      i->second.print (out_);
    }
  out_ << ltag << "Facets: " << __facets.size () << endl;
  for (facets_col_t::const_iterator i = __facets.begin ();
       i != __facets.end ();
       i++)
    {
      string tag2 = tag;
      {
	facets_col_t::const_iterator j = i;
	if (++j == __facets.end ()) tag2 = ltag;
      }
      out_ << sltag << tag2 << "Facet[" << i->first << "] : ";
      i->second->print (out_);
    }
  return;
}

void tessellated_solid::print_xyz (ostream & out_) const
{
  bool devel = false;
  if (devel)
    {
      clog << "DEVEL: tessellated_solid::print_xyz: # vertices = " 
	   << __vertices.size()
	   << endl;
      clog << "DEVEL: tessellated_solid::print_xyz: # facets = " 
	   << __facets.size()
	   << endl;
    }
  size_t last = 0;

  int color = 1;
  // facets:
  int count = 0;
  for (facets_col_t::const_iterator i = __facets.begin ();
       i != __facets.end ();
       i++)
    {
      const i_facet * fct = i->second;
      out_ << "# facet " << count 
	   << " with " <<  fct->get_number_of_vertices ()
	   << " vertices" << endl;
      size_t nvtx = fct->get_number_of_vertices ();
      if (devel)
	{
	  clog << "DEVEL: tessellated_solid::print_xyz: # nvtx = " 
	       << nvtx
	       << endl;
	}
      // vertices:
      color = fct->get_color ();
      double dx = fct->get_vertex (1).x - fct->get_vertex (0).x;
      double dy = fct->get_vertex (1).y - fct->get_vertex (0).y;
      double dz = fct->get_vertex (1).z - fct->get_vertex (0).z;
      double m2 = dx * dx + dy * dy + dz * dz;
      double m  = sqrt (m2);
      fct->get_vertex (0).print_xyz (out_, color);
      fct->get_vertex (1).print_xyz (out_, color);
      out_ << endl;
      if (nvtx == 3)
	{	  
	  vertex last;
	  last.x = fct->get_vertex (2).x - 1e-6 * dx / m;
	  last.y = fct->get_vertex (2).y - 1e-6 * dy / m;
	  last.z = fct->get_vertex (2).z - 1e-6 * dz / m;
	  last.print_xyz (out_, color);
	}
      else
	{
	  fct->get_vertex (fct->get_number_of_vertices () - 1)
	    .print_xyz (out_, color);
	}
      fct->get_vertex (2).print_xyz (out_, color);
      out_ << endl;
      out_ << endl;
      if (devel)
	{
	  clog << "DEVEL: tessellated_solid::print_xyz: # last = " 
	       << last
	       << endl;
	}
      count++;
    }
}

/*** ***/
int main (void)
{
  int code = EXIT_SUCCESS;
  try
    {
      color::load_map ("color.data");
      //color::print_map (clog);

      tessellated_solid ts;

      int ivtx = 0;
      int P = ts.add_vertex (ivtx++, 0, 0, 0);
      int Q = ts.add_vertex (ivtx++, 1, 0, 0);
      int R = ts.add_vertex (ivtx++, 1, 1, 0);
      int S = ts.add_vertex (ivtx++, 0, 1, 0);
      int T = ts.add_vertex (ivtx++, 0.5, 0.5, 1);
      //ts.add_vertex (ivtx++, 0.5, 0.5,-1);

      int ifct = 0;
      ts.add_facet4 (ifct++, P, S, R, Q, ifct);
      ts.add_facet3 (ifct++, P, Q, T, ifct);
      ts.add_facet3 (ifct++, Q, R, T, ifct);
      ts.add_facet3 (ifct++, R, S, T, ifct);
      ts.add_facet3 (ifct++, S, P, T, ifct);

      double z0 = 2;
      int A1 = ts.add_vertex (ivtx++, 0, 0, z0);
      int B1 = ts.add_vertex (ivtx++, 2, 0, z0);
      int C1 = ts.add_vertex (ivtx++, 2, 1, z0);
      int D1 = ts.add_vertex (ivtx++, 0, 1, z0);
      int A2 = ts.add_vertex (ivtx++, 0, 0, z0 + 0.5);
      int B2 = ts.add_vertex (ivtx++, 2, 0, z0 + 0.5);
      int C2 = ts.add_vertex (ivtx++, 2, 1, z0 + 0.5);
      int D2 = ts.add_vertex (ivtx++, 0, 1, z0 + 0.5);

      ts.add_facet4 (ifct++, A2, D2, C2, B2, ifct);
      ts.add_facet4 (ifct++, A1, D1, C1, B1, ifct);
      ts.add_facet4 (ifct++, A1, B1, B2, A2, ifct);
      ts.add_facet4 (ifct++, B1, C1, C2, B2, ifct);
      ts.add_facet4 (ifct++, C1, D1, D2, C2, ifct);
      ts.add_facet4 (ifct++, D1, A1, A2, D2, ifct);


      ts.lock ();
      ts.dump (clog);
      
      ts.print_xyz (cout);

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what() << endl;
      code = EXIT_FAILURE;
    }
  return code;
}

// end of tts.cxx
