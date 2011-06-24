#include <iostream>
#include <fstream>
#include <map>
#include <vector>

void mythrow(const char* C)
{
  std::cout<<"ERROR : "<<C<<std::endl;
  throw(C);
}
struct trie
{
  struct node
  {
    bool is_word; 
    node* parent;
    std::map<char,node*> child;
    node(node* _parent)
      :is_word(false),parent(_parent)
    {}
    bool insert(const char* word)
    {
      if(word[0]) // assuming null terminated string
	{
	  if(!child[word[0]])
	    child[word[0]]=new node(this);
	  return child[word[0]]->insert(word+1);
	}
      else
	{
	  if(!is_word)
	    {
	      is_word = true;
	      return true;
	    }
	  else
	    return false;
	}
    }
    int size()
    {
      int ret = is_word?1:0;
      std::map<char,node*>::iterator iter = child.begin();
      while(iter!=child.end())
	{
	  ret+=iter->second->size();
	  ++iter;
	}
      return ret;
    }
    bool next_match(const char* queryString,char* curAnsString) // assumes that ansString has sufficient memory to hold the response string
    {
      if(queryString[0]=='\0' )
	{
	  if(is_word)
	    return true;
	  else
	    return false;
	}
      std::map<char,node*>::iterator iter;
      if(queryString[0]!='_')
	{
	  iter = child.find(queryString[0]);
	  if((curAnsString[0]=='_') &&
	     (iter==child.end()))
	    return false;
	  if(iter->second->next_match(&(queryString[1]),&(curAnsString[1])))
	    {
	      curAnsString[0]=queryString[0];
	      return true;
	    }
	  else
	    {
	      curAnsString[0]='_';
	      return false;
	    }
	}
      else
	{
	  if(curAnsString[0]=='_')
	    iter = child.begin();
	  else
	    {
	      iter = child.find(curAnsString[0]);
	      {
		int i=0;
		while(curAnsString[i]!='\0')
		  curAnsString[i]='_',i++;
	      }
	      ++iter;
	    }
	  while((iter!=child.end()) && 
		!(iter->second->next_match(&(queryString[1]),&(curAnsString[1]))))
	    ++iter;
	  if(iter != child.end())
	    {
	      curAnsString[0]=iter->first;
	      return true;
	    }
	  else
	    {
	      curAnsString[0]='_';
	      return false;
	    }
	}
    }
    ~node()
    {
      std::map<char,node*>::iterator iter = child.begin();
      while(iter!=child.end())
	{
	  delete iter->second;
	  ++iter;
	}
    }
  };
  node* root;
  trie():root(new node(NULL))
  {}
  bool insert(const char* word)
  {
    return root->insert(word);
  }
  int size()
  {
    return root->size();
  }
  bool next_match(const char* queryString,char* curAnsString) 
// assumes that ansString has sufficient memory to hold the response string
  {
    return root->next_match(queryString,curAnsString);
  }
  ~trie()
  {
    delete root;
  }
};

struct trieQuery
{
  trie* t;
  const char* queryString;
  char* ansString;
  trieQuery(trie*& _t,const char* _qString):t(_t),queryString(_qString)
  {
    int queryLength = strlen(queryString);
    ansString = new char[queryLength+1];
    for(int i=0;i<queryLength;++i)
      ansString[i]='_';
    ansString[queryLength]='\0';
  }
  bool next_match(const char*& aString)
  {
    aString = ansString;
    return t->next_match(queryString,ansString);
  }  
  ~trieQuery()
  {
    delete[] ansString;
  }
};

std::map<int,trie*> LengthTrieMap;
void generate_random_query_string(char* q,int length);
int main()
{
  trie FullTrie;
  trie* FullTriePtr = &FullTrie;
  srandom(time(NULL)+random());
  std::ifstream fin("WORD.LST");
  std::string s;
  fin>>s;
  while(fin)
    {
      trie* &Trie = LengthTrieMap[s.size()];
      if(!Trie)
	Trie=new trie;
      Trie->insert(s.c_str());
      FullTrie.insert(s.c_str());
      fin>>s;
    }
  int totalLength = 0;
  int maxLengthOfWord = 0;
  std::map<int,trie*>::iterator iter = LengthTrieMap.begin();
  while(iter!=LengthTrieMap.end())
    {
      int l = iter->second->size();
      totalLength +=l;
      std::cout<<" length : "<<iter->first<<" size : "<<l<<std::endl;
      if(maxLengthOfWord < iter->first)
	maxLengthOfWord = iter->first;
      ++iter;
    }
  std::cout<<" maximum length of words " <<maxLengthOfWord<<std::endl;
  std::cout<<"total Length is "<<totalLength<<std::endl;
  char* queryString = new char[maxLengthOfWord+1];
  for(int wordLength = 2;wordLength<=maxLengthOfWord;++wordLength)
    {
      generate_random_query_string(queryString,wordLength);
      if(LengthTrieMap.find(wordLength)!=LengthTrieMap.end())
	{
	  std::cout<<"__________________________________________________"<<std::endl;
	  trieQuery q(LengthTrieMap[wordLength],queryString);
	  const char* Ans;
	  std::cout<<queryString<<std::endl;
	  while(q.next_match(Ans))
	    std::cout<<Ans<<std::endl;
	  trieQuery qf(FullTriePtr,queryString);
	  const char* AnsF;
	  std::cout<<"--------------------------------------------------"<<std::endl;
	  while(qf.next_match(AnsF))
	    std::cout<<AnsF<<std::endl;
	  std::cout<<"__________________________________________________"<<std::endl;
	}
    }
  {
    std::map<int,trie*>::iterator iter = LengthTrieMap.begin();
    while(iter!=LengthTrieMap.end())
      {
	delete iter->second;
	++iter;
      }
  }
  delete[] queryString;
}

int myrandom(int n)
{
  return random()%n;
}


void generate_random_query_string(char* q,int length)
{
  int N = myrandom(length);
  for(int i=0;i<length;++i)
    {
      if(i==N)
	q[i]='a'+myrandom(26);
      else
	q[i]='_';
    }
  q[length]='\0';
}


void generate_random_query_string1(char* q,int length)
{
  for(int i=0;i<length;++i)
    {
      if(myrandom(2))
	q[i]='a'+myrandom(26);
      else
	q[i]='_';
    }
  q[length]='\0';
}
