% From: https://swish.swi-prolog.org/example/kb.pl
% Knowledge bases
% Usage:
% ?- loves(X, mia).
% ?- jealous(X, Y).

loves(vincent, mia).
loves(marcellus, mia).
loves(pumpkin, honey_bunny).
loves(honey_bunny, pumpkin).

jealous(X, Y) :-
    loves(X, Z),
    loves(Y, Z).



% From: http://ktiml.mff.cuni.cz/~bartak/prolog/bottomup.html
% Calculate the Nth number in the Fibonacci sequence
% Usage (to calculate 5th Fibonacci number):
% ?- fibo(5, X).

fibo(N,F) :- fibo1(0,0,1,N,F).
   
fibo1(N,F,_,N,F).
fibo1(N1,F1,F2,N,F) :-
  <(N1, N), is(N2, +(N1, 1)), is(F3, +(F1, F2)),
  fibo1(N2,F2,F3,N,F).



% From: http://www.cs.toronto.edu/%7Esheila/384/w11/simple-prolog-examples.html
% Recursion: Towers of Hanoi
% A classic table-top puzzle where you try to move discs
% from the left peg to the far right peg
%
% The 3-disk setup is like this:
% 
% 
%            |        |         |
%           xxx       |         |
%          xxxxx      |         |
%         xxxxxxx     |         |
%      _________________________________
%
% move(N,X,Y,Z) - move N disks from peg X to peg Y, with peg Z being the
%                 auxilliary peg
%
% Strategy:
% Base Case: One disc - To transfer a stack consisting of 1 disc from 
%    peg X to peg Y, simply move that disc from X to Y 
% Recursive Case: To transfer n discs from X to Y, do the following: 
%         Transfer the first n-1 discs to some other peg X 
%         Move the last disc on X to Y 
%         Transfer the n-1 discs from X to peg Y
%
% Usage:
% ?- move(3,left,right,center). 

move(1,X,Y,_) :-  
    write("Move top disk from "), 
    write(X), 
    write(" to "), 
    write(Y), 
    nl. 
move(N,X,Y,Z) :- 
    >(N, 1), 
    is(M, -(N, 1)), 
    move(M,X,Z,Y), 
    move(1,X,Y,_), 
    move(M,Z,Y,X).  