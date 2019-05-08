- make retractall() not abort the process if the fact doesn't exist
- finish implementing retract with unbound variables and enable tests

- Make Adventure.pl compile properly
	- (done) Support Block comments /* */
	- (done) Support Assert / Retract
		- As far as I can tell the spec is this:
			- when a predicate is run against the database, each alternative as a result of unification (i.e. each time the tree branches) gets a copy of the state *at that time* and updates to that state are realized immediately ONLY along that branch.
			- Thoughts:
				- if assert is called, this node should get a new database that keeps going
				- the other unifications should keep their existing one
				- The global state should be changed
				- Said otherwise: 
					- All changes to the global database happen immediately (even if the branch fails)
						- If the user chooses not to continue getting solutions, the state isn't changed
					- every predicate in progress took a snapshot when it started that it passes to its children
			- Implementation:
				- (done) use the existing diff model to isolate changes when running a predicate, but "write thru" to a base ruleset with every change so that global changes get seen.
				- Each node is isolated from changes that occur when it is being run, but all other nodes see global changes
				- Start the resolve with a copy so that all nodes are isolated from 
				- A node gets its state from its parent node
				- Assert() and Retract() write all the way through to the "master" which isn't used by anyone
				- the only time you need to copy the ruleset is when you process a dynamic goal, all the others get shared
				- Changes
					- Suspect CreateChildNode() is the place to put the code that gets the right ruleset
						- Gets passed by the caller and things like assert() will pass a copy
					- Need to access the state for the current node, not the global state like it does now
					- Need a way to modify the global state without it appearing to the children. Options:
						- Option 1: Should we just attach generation and track what generation things got added in?
							- Might work for Htn too
						- Option 2: 
							- Use the existing RuleSet infrastructure for HTN AND support a "write-thru" that modifies the master database
								- Would require making a copy of the RuleSet when we start resolving
						- Option 3:
							- Use the current rule update logic for RuleSet BUT: add a new feature that batches up a bunch of changes that can be committed when it is all done
							- Require the caller of ResolveRule to commit the changes when done (in case they don't get all the solutions?)?
						- Option 4: 
							- Shared Rules in Ruleset have a current Generation and rules that get added track their generation.
							- Asking for a rule implies getting it from the current generation and older
							- Then: Leave all the current logic for transacting rules too.
							- Question: Will this make assert() work for HTNs properly?
						- (go this way) Option 5: 
							
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
- retract should unify its term and including the binding for subsequent terms
- do a test where you retract and fail as a poor mans retractall
