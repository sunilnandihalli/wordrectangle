#include <map>
#include <vector>
static const std::map<char,int>::iterator iter;

int main()
{
  std::map<char,int> s;
  std::map<char,int> s1;
  std::map<char,int>::iterator iter1 = s.begin();
  std::map<char,int>::iterator iter2 = s.end();
  std::map<char,int>::iterator iter3 = iter;
  s1['a']=1;
  std::map<char,int>::iterator iter4 = s1.begin();
  std::map<char,int>::iterator iter5 = s1.end();
  static const std::map<char,int>::iterator iter6;
  std::vector< std::map<char,int>::iterator> v(5);
  v.resize(7);
  std::map<char,int>::iterator &iter7 = v[3];
  std::map<char,int>::iterator &iter8 = v[6];
}
