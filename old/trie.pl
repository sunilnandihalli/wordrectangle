make_trie([Word|WordList],Trie):-
	make_trielist(Word,Word,WordTrie),
	make_trie(WordList,[WordTrie],Trie).

make_trie([],T,T):-!.
make_trie([Word|WordList],Trie,FinalTrie):-
	insert_word_in_trie(Word,Word,Trie,NewTrie),
	make_trie(WordList,NewTrie,FinalTrie).

make_trielist(Word,Leaf,WordTrie):-
	atom_chars(Word,CharList),
	make_trielist_aux(CharList,Leaf,WordTrie).

make_trielist_aux([X],Leaf,[X,Leaf]):-!.
make_trielist_aux([X|L],Leaf,[X|[LS]]):-
	make_trielist_aux(L,Leaf,LS).

insert_word_in_trie(Word,Leaf,Trie,NewTrie):-
	make_trielist(Word,Leaf,WordTrie),
	insert_wordtrie_in_trie(WordTrie,Trie,NewTrie).

insert_wordtrie_in_trie([H|[T]],
			[[H,Leaf|BT]|LT],
			[[H,Leaf|NB]|LT]):-
	atom(Leaf),!,
	insert_wordtrie_in_trie(T,BT,NB).
insert_wordtrie_in_trie([H|[T]],
			[[H|BT]|LT],
			[[H|NB]|LT]):-
	!,insert_wordtrie_in_trie(T,BT,NB).
insert_wordtrie_in_trie([H|T],
			[[HT|BT]|LT],
			[[HT|BT]|NB]):-
	!,insert_wordtrie_in_trie([H|T],LT,NB).
insert_wordtrie_in_trie(RW,RT,NB):-
	append(RT,[RW],NB),!.

is_word_in_trie([H|T],Trie,Lex):-
	member([H|Branches],Trie),
	is_word_in_trie(T,Branches,Lex).
is_word_in_trie([],Trie,LexList):-
	findall(Lex,(member(Lex,Trie),atom(Lex)),
		LexList),
	LexList\=[].
		       
	