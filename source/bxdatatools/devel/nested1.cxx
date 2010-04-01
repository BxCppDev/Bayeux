#include <iostream>
#include <stdexcept>
#include <string>
#include <map>

using namespace std;

class toto
{
private:
  string __name;
  double __value;
public:
  toto ();
  toto (const string & name_, double val_);
  ~toto ();
  const string & get_name () const;
  void set_name (const string & name_);
  double get_value () const;
  void set_value (double val_);
  friend ostream & operator<< (ostream &, const toto & t_);
};

ostream & operator<< (ostream & out_, const toto & t_)
{
  out_ << "[name=" << t_.__name << " value=" << t_.__value << "]";
  return out_;
}

toto::toto ()
{
  __name = "";
  __value = 0.0;
}

toto::toto (const string & name_, double val_)
{
  set_name (name_);
  set_value (val_);
}

toto::~toto ()
{
  //clog << "toto::~toto: Entering..." << endl;
}

const string & toto::get_name () const
{
  return __name;
}

void toto::set_name (const string & name_)
{
  __name = name_;
}

double toto::get_value () const
{
  return __value;
}
void toto::set_value (double val_)
{
  __value = val_;
}

/**********************************************/

class titi
{
public:
  struct entry
  {
    bool         owned;
    const toto * t;
    double       w;
    
    entry ();
    ~entry ();
    bool is_valid () const;
    
    // CRITICAL
    //private:
    entry (const entry &);
    entry & operator= (const entry &);
    
  };
  typedef map<string, entry> entries_dict_t;
private:
  
  string         __name;
  entries_dict_t __composition;
public:
  titi ();
  titi (const string & name_);
  // CRITICAL
private:
  titi (const titi &);
  titi & operator = (const titi &);
public:
  ~titi ();
  const string & get_name () const;
  void set_name (const string & name_);
  void add_toto (const toto & t_, double w_, bool owned_ = false);
  void add_toto (const toto * t_, double w_);
  void print (ostream &) const;
  // special
};

bool titi::entry::is_valid () const
{
  return w > 0.0;
}

titi::entry::entry ()
{
  clog << "DEVEL: titi::entry::entry: Default ctor !" << endl;
  owned = false;
  t = 0;
  w = -1.0;
}

titi::entry & titi::entry::operator= (const titi::entry & e_)
{
  owned = false;
  t = e_.t;
  w = e_.w;
  return *this;
}

titi::entry::entry (const entry & e_)
{
  clog << "DEVEL: titi::entry::entry: Copy ctor !" << endl;
  owned = false;
  t = e_.t;
  w = e_.w;
}
 
titi::entry::~entry ()
{
  if (t != 0)
    {
      if (owned)
	{
	  /*
	  clog << "titi::entry::~entry: Deleting owned instance of '" 
	       << t->get_name () << "'" << endl;
	  */
	  delete t;
	}
      else
	{
	  /*
	  clog << "titi::entry::~entry: Only detaching not-owned instance of '" 
	       << t->get_name () << "'" << endl;
	  */
	}
      t = 0;
    }
  w = -1.0;  
}

titi::titi ()
{
  clog << "DEVEL: titi::titi: Default ctor !" << endl;
  __name = "";
}
 
/* 
titi::titi (const titi & t_)
{
  clog << "DEVEL: titi::titi: Copy ctor !" << endl;
  __name = t_.__name; 
  __composition = t_.__composition;
}
*/

titi::titi (const string & name_)
{
  clog << "DEVEL: titi::titi: ctor 2 !" << endl;
  set_name (name_);
}

titi::~titi ()
{
  //clog << "titi::~titi: Entering..." << endl;
  __composition.clear ();
}

const string & titi::get_name () const
{
  return __name;
}

void titi::set_name (const string & name_)
{
  __name = name_;
}

void titi::add_toto (const toto & t_, double w_, bool owned_)
{
  entry e;
  e.owned = false;
  e.t = &t_;
  e.w = w_;

  /*
  pair<string,entry> p;
  p.first = t_.get_name ();
  __composition.insert (p);
  __composition[e.t->get_name ()].t = &t_;
  __composition[e.t->get_name ()].w = w_;
  __composition[e.t->get_name ()].owned = owned_; 
  */

  __composition[e.t->get_name ()] = e;
  __composition[e.t->get_name ()].owned = owned_; 
}

void titi::add_toto (const toto * t_, double w_)
{
  add_toto (*t_, w_, true);
}

void titi::print (ostream & out_) const
{
  out_ << "titi::print: " << endl;
  out_ << " name = " << __name << endl;
  for (entries_dict_t::const_iterator i = __composition.begin ();
       i !=  __composition.end ();
       i++)
    {
      const string & key = i->first;
      const entry & e = i->second;
      out_ << " key='" << key << "' -> toto = " << *e.t << " weight=" << e.w 
	   << (e.owned? "(owned)": "(not owned)") << endl; 
    }
}

/**********************************************/
class mgr
{
public:

  typedef map<string, toto> toto_dict_t;
  typedef map<string, titi> titi_dict_t;

private:
  
  toto_dict_t __totos;
  titi_dict_t __titis;

public:
  mgr ();
  ~mgr ();

  void add_toto (const string & name_, double val_);
  void add_titi (const string & name_, const map<string, double> & comp_);

  void print (ostream &) const;

  bool has_toto (const string & name_) const;
  const toto & get_toto (const string & name_) const;

  bool has_titi (const string & name_) const;
  const titi & get_titi (const string & name_) const;


};

mgr::mgr ()
{
}

mgr::~mgr ()
{
  __totos.clear ();
  __titis.clear ();
}

bool mgr::has_toto (const string & name_) const
{
  return __totos.find (name_) != __totos.end ();
}

const toto & mgr::get_toto (const string & name_) const
{
  toto_dict_t::const_iterator found = __totos.find (name_);
  if (found == __totos.end ())
    {
      throw runtime_error ("mgr::get_toto: not found !");
    }
  return found->second;
}

bool mgr::has_titi (const string & name_) const
{
  return __titis.find (name_) != __titis.end ();
}

const titi & mgr::get_titi (const string & name_) const
{
  titi_dict_t::const_iterator found = __titis.find (name_);
  if (found == __titis.end ())
    {
      throw runtime_error ("mgr::get_titi: not found !");
    }
  return found->second;
}

void mgr::add_toto (const string & name_, double val_)
{
  if (__totos.find (name_) != __totos.end ())
    {
      throw runtime_error ("mgr::add_toto: already exists !");
    }
  __totos[name_] = toto (name_, val_);
}

void mgr::print (ostream & out_) const
{
  out_ << "mgr::print:" << endl;
  out_ << "  totos:" << endl;
  for (toto_dict_t::const_iterator i = __totos.begin ();
       i != __totos.end ();
       i++)
    {
      out_ << "    " << i->first << " -> " << i->second << endl;
    }  
  out_ << "  titis:" << endl;
  for (titi_dict_t::const_iterator i = __titis.begin ();
       i != __titis.end ();
       i++)
    {
      out_ << "    " << i->first << " -> " << endl;
      i->second.print (out_);
    }  
}

void mgr::add_titi (const string & name_, const map<string, double> & comp_)
{
  if (__titis.find (name_) != __titis.end ())
    {
      throw runtime_error ("mgr::add_titi: already exists !");
    }
  __titis[name_] = titi (name_);
  for (map<string, double>::const_iterator i = comp_.begin ();
       i != comp_.end ();
       i++)
    {
      const string & toto_key = i->first;
      double w = i->second;
      toto_dict_t::const_iterator toto_found = __totos.find (toto_key);
      if (toto_found == __totos.end ())
	{
	  throw runtime_error ("mgr::add_titi: toto dos not exist !");
	}
      const toto & ref_toto = toto_found->second;
      __titis[name_].add_toto (ref_toto, w);
    }
}

/*  for (toto_dict_t::iterator i = __totos.begin ();
       i != __totos.end ();
       i++)
    {
      delete i->second;
    }
*/
/**********************************************/
int main (void)
{
  {
    toto t1 ("a", 1.23);
    toto t2 ("b", 4.56);
    toto t3 ("c", 12.65);
    
    cout << "t1=" << t1 << endl;
    cout << "t2=" << t2 << endl;
    cout << "t3=" << t3 << endl;
    cout << endl;
    
    titi T ("Z");
    T.add_toto (t1, 0.2);
    T.add_toto (t2, 0.5);
    T.add_toto (t3, 0.3);
    T.add_toto (new toto ("d", 666.), 0.1);
    T.print (cout);
    
    titi T2 = T;
    T2.print (cout);
  }

  {
    mgr M;
    M.add_toto ("a", 1.23);
    M.add_toto ("b", 10.23);
    M.add_toto ("c", 12.23);
    M.add_toto ("d", 34.23);
    M.add_toto ("e", 34.23);
    M.add_toto ("f", 0.23);
    map<string, double> z_comp;
    z_comp["a"] = 0.2;
    z_comp["b"] = 0.3;
    z_comp["f"] = 0.5;
    M.add_titi ("Z", z_comp);
    map<string, double> y_comp;
    y_comp["a"] = 0.15;
    y_comp["c"] = 0.05;
    y_comp["e"] = 0.8;
    M.add_titi ("Y", y_comp);
    M.print (cout);
    
    if (M.has_toto ("c"))
      {
	cout << "found toto: " << M.get_toto ("c") << endl;
      }
    if (M.has_titi ("Z"))
      {
	cout << "found titi: " << endl;
	M.get_titi ("Z").print (cout);
      }
  
    titi bad_titi = M.get_titi ("Z");
    cout << "bad titi: " << endl;
    bad_titi.print (cout);
  }

  return 0;
}
