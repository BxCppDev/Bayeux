/*
 * use of typename in methods of a template class
 */

#include <iostream>
#include <list>

template<class T>
class col_t
{
  typename std::list<T> __items;
public:
  col_t()
  {
  }
  virtual ~col_t()
  {
    __items.clear();
  }
  void add(const T & item_)  
  {
    __items.push_back(item_);
  }
  void print() const
  {
    std::cout << "begin" << std::endl;
    for (typename std::list<T>::const_iterator i=__items.begin();
	 i != __items.end();
	 i++)
      {
	std::cout << *i << std::endl;
      }
    std::cout << "end" << std::endl;
  }
};

int main( void )
{
  col_t<int> ints;
  
  for (int i=0; i< 4; i++)
    {
      ints.add(i);
    }
  ints.print();
  return 0;
}

/* end */
