:-module(dict,[dict_unify/1,wordlength_size/2,solve_word_rectangle/4,rect/2]).
:-load_foreign_library(trie).
:-read_dictionary('WORD.LST').

dict_unify(L):-
	maximum_word_length(NMax),
	between(2,NMax,N),
	length(L,N),
	dictionary_unify(L).

wordlength_size(LengthN,NumberOfWordsOfLengthN):-
	maximum_word_length(NMax),
	between(2,NMax,LengthN),
	number_of_words_of_length_deterministic(LengthN,NumberOfWordsOfLengthN),
	\+ NumberOfWordsOfLengthN = 0.

mlength(M,List):-
	length(List,M).
	
generate_rectangle(MRows,NCols,Rows,Cols):-
	length(Rows,MRows),
	maplist(mlength(NCols),Rows),
	length(Cols,NCols),
	maplist(mlength(MRows),Cols),
	unify_rectangle(Rows,Cols).

unify_rectangle(_,[]).
unify_rectangle([],_).
unify_rectangle([[X|Row1]|Rows],[[X|Col1]|Cols]) :-
	   unify_row(Row1,Cols,ColsR),
	      unify_rectangle(Rows,[Col1|ColsR]).

unify_row([],[],[]).
unify_row([X|Row],[[X|Col1]|Cols],[Col1|ColsR]) :-
	unify_row(Row,Cols,ColsR).

solve_word_rectangle(MRows,NCols,Rows,Cols):-
	generate_rectangle(MRows,NCols,Rows,Cols),
	append(Rows,Cols,Words),
	unify_words(Words).

unify_words([]).
unify_words([W|Ws]):-
	dictionary_unify(W),
	unify_words(Ws).

write_word_rectangle(Rows):-
	writeln('__________________'),
	maplist(write_row,Rows),nl.

write_row(Row):-
	maplist(write,Row),nl.

rect(M,N):-
	solve_word_rectangle(M,N,R,_C),
	write_word_rectangle(R),!,
	fail.