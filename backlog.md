- Support syntax for x = y instead of =(x, y)
- Support both variants of '""' and "''"
- make retractall() not abort the process if the fact doesn't exist
- do a test where you retract and fail as a poor mans retractall
- finish implementing retract with unbound variables and enable tests
		- https://stackoverflow.com/questions/20027182/gnu-prolog-assert-error/20027252#20027252
		- https://stackoverflow.com/questions/28116244/how-prologs-logical-update-view-works-for-assert-and-retract
		- https://groups.google.com/forum/#!topic/swi-prolog/FPXi_8Ho9nQ
		- https://www.researchgate.net/publication/235357818_Extending_the_logical_update_view_with_transaction_support/download
		- https://groups.google.com/forum/#!topic/swi-prolog/FPXi_8Ho9nQ
		- http://www.swi-prolog.org/pldoc/doc_for?object=retract/1
		- Scenarios:
			
			:- dynamic insect/1, test/0.
			insect(ant).
			insect(bee).
			test :- retract(insect(I)),
			       writeln("first:"),
			       writeln(I),
			       retract(insect(bee)),
			       insect(X),
			       writeln("second:"),
			       writeln(X).

			?- test.
			first:
			ant
			first:
			bee
			false.


			:- dynamic insect/1.
			insect(ant).
			insect(bee).
			test :- retract(insect(I)),
			       writeln("first:"),
			       writeln(I),
			       retract(insect(bee)).
			test :-  writeln("entered second:"),
					insect(I),
			       writeln("second:"),
					writeln(I).

			?- test.
			first:
			ant
			true 
			first:
			bee
			entered second:
			false.


			This seems to imply that changes are kept after backtracking since it will backtrack after the "fail" line, but the second test/0 rule still doesn't see 
			:- dynamic insect/1.
			insect(ant).
			insect(bee).
			test :- retract(insect(I)),
			       writeln("first:"),
			       writeln(I),
			       retract(insect(bee)),
			       fail.
			test :- insect(I),
			       writeln("second:"),
					writeln(I).

			?- test.
			first:
			ant
			first:
			bee
			false.


			This implies that the database is snapshotted before the predicate of test is run, since the second time through test/0, it still sees insect(bee) even though it was retracted:

			:- dynamic insect/1.
			insect(ant).
			insect(bee).
			test :- retract(insect(I)),
			       writeln(I),
			       retract(insect(bee)),
			       fail
			   ;   true.

			?- test.
			ant 
			bee
			true.

			But this seems to prove that the effects of retract/1 are immediate, since it never gets to print "second:"
			:- dynamic insect/1, test/0.
			insect(ant).
			insect(bee).
			test :- retract(insect(I)),
			       writeln("first:"),
			       writeln(I),
			       retract(insect(bee)),
			       insect(X),
			       writeln("second:"),
			       writeln(X).

			?- test.
			first:
			ant
			first:
			bee
			false.

			So now I'm thinking that maybe when it

			But this implies it is *not* snapshotted because the second solution does *not* see insect(bee):
			:- dynamic insect/1, test/0.
			insect(ant).
			insect(bee).
			test :- retract(insect(I)),
			       writeln(I),
			       retract(insect(bee)).
			test :- insect(X), writeln(X).

			?- test.
			ant
			true [enter].	

