#include <SWI-Prolog.h>
#include <cstdio>
int read_word_list(const char* filename);
extern "C"
{
  foreign_t pl_read_word_list(term_t filepath_term)
  {
    size_t length;
    char* filename;
    if(PL_is_string(filepath_term))
      return PL_warning("please input a valid string");
    else
      printf("input term is a string\n");
    PL_get_chars(filepath_term,&filename,CVT_ALL|BUF_DISCARDABLE);
    printf("reading the file for list of words %s\n",filename);
    if(read_word_list(filename)==0)
      PL_succeed;
    else
      PL_fail;
  }
  
  foreign_t pl_number_of_equal_words(term_t lengthOfWord,
				     term_t numberOfWords,
				     control_t handle)
  {
    //    numberOfEqualWordsContext* ctxt
  }
  
  foreign_t pl_dictionary_unify(term_t QueryCharList,control_t handle)
  {
    trieQuery* ctxt;
    char queryString[100]; // assuming that no word is longer than 100 chars
    char* queryResult;
    switch(PL_foreign_control(handle))
       {
       case PL_FIRST_CALL:
	 getQueryString(QueryCharList,queryString);
	 init_new_dictionary_query(ctxt,queryString);
	 if(!ctxt->next(queryResult))
	   {
	     clean_up_dictionary_query(ctxt);
	     PL_fail;
	   }
	 else
	   {
	     PL_unify_list_chars(QueryCharList,queryResult);
	     PL_retry_address(ctxt);
	   }
       case PL_REDO:
	 ctxt=PL_foreign_context_address(handle);
	 if(!ctxt->next(queryResult))
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
	 ctxt=PL_foreign_context_address(handle);
	 clean_up_dictionary_query(ctxt);
	 PL_succeed;
       }
   }
  
  install_t install()
  {
    PL_register_foreign("read_word_list",1,(void*)(pl_read_word_list),0);
    PL_register_foreign("unify_list",1,(void*)(pl_unify_list),0);
  }
}

void init_new_dictionary_query(trieQuery*& ctxt,char* queryString)
{
  ctxt = new trieQuery(queryString)
}
void getQueryString(term_t t,char* buf)
{
  int i=0;
  char* c;
  term_t h;
  while(PL_get_list(t,h,t))
    {
      if(!PL_is_variable(h))
	{
	  PL_get_chars(h,&c,CVT_ATOM|BUF_DISCARDABLE);
	  buf[i]=c[0];
	}
      else
	buf[i]='_';
      ++i;
    }
  buf[i]='\0';
  printf("buf : %s\n",buf);
}
