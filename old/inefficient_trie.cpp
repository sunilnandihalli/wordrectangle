#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <SWI-Prolog.h>
#include <cstdio>
foreign_t pl_display(term_t t);

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
    bool next_match(const char* queryString,
		    std::map<char,node*>::iterator* curAnsString)
    {
      return next_match(queryString,curAnsString,false);
    }
    bool next_match(const char* queryString,char* curAnsString,bool changed)
    // assumes that ansString has sufficient memory to hold the response string
    {
      bool locally_changed = false;
      if(queryString[0]=='\0' )
	{
	  if(is_word && changed)
	    return true;
	  else
	    return false;
	}
      std::map<char,node*>::iterator iter;
      if(queryString[0]!='_')
	{
	  iter = child.find(queryString[0]);
	  if((curAnsString[0]->first=='_') &&
	     (iter==child.end()))
	    return false;
	  locally_changed = (curAnsString[0]=='_');
	  if(iter->second->next_match(&(queryString[1]),
				      &(curAnsString[1]),
				      changed || locally_changed))
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
      else if(queryString[0]=='_')
	{
	  if(curAnsString[0]=='_')
	    {
	      locally_changed = true;
	      iter = child.begin();
	    }
	  else
	    iter = child.find(curAnsString[0]);
	  {
	    while((iter!=child.end()) && 
		  !(iter->second->next_match(&(queryString[1]),
					     &(curAnsString[1]),
					     changed || locally_changed)))
	      {
		locally_changed = true;
		++iter;
	      }
	  }
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
  bool next_match(const char* queryString,
		  std::map<char,node*>::iterator* curAnsString) 
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
  char* queryString;
  char* ansString;
  trieQuery(trie*& _t,const char* _qString):t(_t)
  {
    int queryLength = strlen(_qString);
    ansString = new char[queryLength+1];
    queryString = new char[queryLength+1];
    strcpy(queryString,_qString);
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
    delete[] queryString;
  }
};
static std::map<int,trie*> LengthTrieMap;
int dictionaray_maximum_word_length()
{
  std::map<int,trie*>::iterator iter = LengthTrieMap.end();
  --iter;
  return iter->first;
}
int number_of_words_of_length(uint64_t n)
{
  std::map<int,trie*>::iterator iter = LengthTrieMap.find(n);
  if(iter!=LengthTrieMap.end())
    return iter->second->size();
  else
    return 0;
}

int read_dictionary(const char* filename)
{
  try
    {
      std::ifstream fin(filename);
      std::string s;
      fin>>s;
      while(fin)
	{
	  trie* &Trie = LengthTrieMap[s.size()];
	  if(!Trie)
	    Trie=new trie;
	  Trie->insert(s.c_str());
	  fin>>s;
	}
    }
  catch(...)
    {
      return 1;
    }
  return 0;
}


void init_new_dictionary_query(trieQuery*& ctxt,char* queryString)
{
  int n = strlen(queryString);
  std::map<int,trie*>::iterator iter = LengthTrieMap.find(n);
  if(iter==LengthTrieMap.end())
    {
      ctxt = NULL;
      return;
    }
  ctxt = new trieQuery(iter->second,queryString);
}

void clean_up_dictionary_query(trieQuery* ctxt)
{
  delete ctxt;
}

void getQueryString(term_t t,char* buf)
{
  term_t head = PL_new_term_ref();
  term_t list = PL_copy_term_ref(t);
  int i=0;
  char* c;
  while(PL_get_list(list,head,list))
    {
      if(!PL_is_variable(head))
	{
	  PL_get_chars(head,&c,CVT_ATOM|BUF_DISCARDABLE);
	  buf[i]=c[0];
	}
      else
	buf[i]='_';
      ++i;
    }
  buf[i]='\0';
}

extern "C"
{
  foreign_t pl_read_dictionary(term_t filepath_term)
  {
    size_t length;
    char* filename;
    if(PL_is_string(filepath_term))
      return PL_warning("please input a valid string");
    PL_get_chars(filepath_term,&filename,CVT_ALL|BUF_DISCARDABLE);
    printf("reading the file for list of words %s\n",filename);
    if(read_dictionary(filename)==0)
      PL_succeed;
    else
      PL_fail;
  }
  
  foreign_t pl_dictionary_unify(term_t QueryCharList,control_t handle)
  {
    trieQuery* ctxt;
    char* queryString; // assuming that no word is longer than 100 chars
    const char* queryResult;
    switch(PL_foreign_control(handle))
       {
       case PL_FIRST_CALL:
	 queryString = (char*)malloc(sizeof(char)*1000);
	 getQueryString(QueryCharList,queryString);
	 init_new_dictionary_query(ctxt,queryString);
	 free(queryString);
	 if(!ctxt || !ctxt->next_match(queryResult))
	   {
	     if(ctxt)
	       clean_up_dictionary_query(ctxt);
	     PL_fail;
	   }
	 else
	   {
	     PL_unify_list_chars(QueryCharList,queryResult);
	     PL_retry_address(ctxt);
	   }
       case PL_REDO:
	 ctxt=(trieQuery*)PL_foreign_context_address(handle);
	 if(!ctxt->next_match(queryResult))
	   {
	     clean_up_dictionary_query(ctxt);
	     PL_fail;
	   }
	 else
	   {
	     PL_unify_list_chars(QueryCharList,queryResult);
	     PL_retry_address(ctxt);
	   }
       case PL_CUTTED:
	 ctxt==(trieQuery*)PL_foreign_context_address(handle);
	 clean_up_dictionary_query(ctxt);
	 PL_succeed;
       }
   }

  foreign_t pl_get_query_string(term_t t)
  {
    char* buf=(char*)malloc(sizeof(char)*100);
    getQueryString(t,buf);
    printf("\n");
    PL_succeed;
  }
  foreign_t pl_number_of_words_of_length(term_t word_length,
					 term_t nwordsSize_t)
  {
    int n;
    if(!PL_is_integer(word_length))
      {
	PL_warning("word_length should be a bound integer");
      }
    else
      PL_get_integer(word_length,&n);
    int nWordsSize = number_of_words_of_length(n);
    return PL_unify_integer(nwordsSize_t,nWordsSize);
  }
  foreign_t pl_dictionary_max_word_length(term_t t)
  {
    int n = dictionaray_maximum_word_length();
    return PL_unify_integer(t,n);
  }
  install_t install()
  {
    PL_register_foreign_in_module(NULL,"read_dictionary",1,
				  (void*)(pl_read_dictionary),0);
    PL_register_foreign_in_module(NULL,"dictionary_unify",1,
				  (void*)(pl_dictionary_unify),PL_FA_NONDETERMINISTIC);
    PL_register_foreign_in_module(NULL,"query_string",1,
				  (void*)(pl_get_query_string),0);
    PL_register_foreign_in_module(NULL,"maximum_word_length",1,
				  (void*)(pl_dictionary_max_word_length),0);
    PL_register_foreign_in_module(NULL,"number_of_words_of_length_deterministic",2,
				  (void*)(pl_number_of_words_of_length),0);
  }
}
