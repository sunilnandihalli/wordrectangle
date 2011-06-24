:-ensure_loaded(readfile).
:-ensure_loaded(trie).

mread_lines(P,LengthTrieMapOut):-
	open(P,read,S),
	mread_stream(S,_LengthTrieMapIn,LengthTrieMapOut).

mread_stream(S,LengthTrieMapIn,LengthTrieMapOut):-
	read_line_to_codes(S,LC),
	\+ LC = end_of_file,!,
	maplist(char_code,L,LC),
	length(L,N),
	get_assoc(N,LengthTrieMapIn,OldNTrie,NewLengthTrieMapIn,NewNTrie),
	insert_word_in_trie(L,L,OldNTrie,NewNTrie),
	mread_stream(S,NewLengthTrieMapIn,LengthTrieMapOut).

mread_stream(_,L,L).


test:-
	read_lines('WORD.LST',L),
	length(L,N),
	writeln([number_of_words,N]).

chars_atom(Cs,A):-
	atom_chars(A,Cs).

t2(Trie):-
	read_lines('Word1.lst',L),
	maplist(chars_atom,L,L1),
	make_trie(L1,Trie).

t(Trie):-
	L =[abc,def,hig,dadf,abde],
	make_trie(L,Trie).

t1(Trie):-
	L =[abc,abe],
	make_trie(L,Trie).

t3(Trie):-
	read_lines('WORD.LST',L),
	maplist(chars_atom,L,L1),
	make_trie(L1,Trie).

t4(Trie):-
	mread_lines('WORD.LST',Trie).


code_char(X,Y):-
	char_code(Y,X).


maplist1(Pred,L1,L2):-!,
	maplist1(Pred,L1,L3,[]),
	reverse(L3,L2).

maplist1(_,[],L2,L2).
maplist1(Pred,[X|Xs],L2,Acc):-!,
	call(Pred,X,FX),!,
	maplist1(Pred,Xs,L2,[FX|Acc]).
