// This Project:
#include <datatools/i_tree_dump.h>

// Standard Library:
#include <iostream>
#include <string>
#include <exception>

struct A
  : public datatools::i_tree_dumpable
{
  int         k;
  double      x;
  std::string s;

  A()
    : k(1)
    , x(3.14)
    , s("foo")
  {
  }

};

struct B
  : public datatools::i_tree_dumpable
{
  int         l;
  double      y;
  std::string t;

  B()
    : l(2)
    , y(6.28)
    , t("bar")
  {
  }

  virtual void print_tree(std::ostream & out_ = std::clog,
                          const boost::property_tree::ptree & options_ = datatools::i_tree_dumpable::empty_options()) const
  {
    datatools::i_tree_dumpable::base_print_options opts;
    opts.configure_from(options_);
    if (! opts.title.empty ()) {
       out_ << opts.indent << opts.title << std::endl;
    }
    out_ << opts.indent << datatools::i_tree_dumpable::tag
         << "l = '" << l << "'" << std::endl;
    out_ << opts.indent << datatools::i_tree_dumpable::tag
         << "y = '" << y << "'" << std::endl;
    out_ << opts.indent << datatools::i_tree_dumpable::inherit_tag(opts.inherit)
         << "t = '" << t << "'" << std::endl;

    return;
  }

};

struct C
  : public B
{
  int m;

  C()
    : B()
    , m(666)
  {
  }

  virtual void print_tree(std::ostream & out_ = std::clog,
                          const boost::property_tree::ptree & options_ = datatools::i_tree_dumpable::empty_options()) const
  {
    datatools::i_tree_dumpable::base_print_options opts;
    opts.configure_from(options_);

    boost::property_tree::ptree base_options = datatools::i_tree_dumpable::base_print_options::force_inheritance(options_);
    this->B::print_tree(out_, base_options);

    out_ << opts.indent << datatools::i_tree_dumpable::inherit_tag(opts.inherit)
         << "m = '" << m << "'" << std::endl;

    return;
  }
};


int main (void)
{
  try {

    A a;
    B b;
    C c;

    a.print_tree_json(std::cout, std::string("{\"title\" : \"A\", \"indent\" : \"--> \", \"inherit\" : false}"));
    std::cout << std::endl;

    std::string opts("{\"title\" : \"B\", \"indent\" : \"--> \", \"inherit\" : true}");
    b.print_tree_json(std::cout, opts);
    std::cout << std::endl;

    {
      boost::property_tree::ptree poptions;
      poptions.put("title", "Test B: ");
      poptions.put("indent", "[debug] ");
      b.print_tree(std::cout, poptions);
      std::cout << std::endl;
    }

    {
      boost::property_tree::ptree poptions;
      poptions.put("title", "Test C: ");
      poptions.put("indent", "[debug] ");
      c.print_tree(std::cout, poptions);
      std::cout << std::endl;
    }

    {
      boost::property_tree::ptree poptions;
      poptions.put("title", "Test C: ");
      c.print_tree(std::cout, poptions);
      std::cout << std::endl;
    }
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
  }

  return 0;
}
