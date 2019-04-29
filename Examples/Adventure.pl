north :- go(north).
south :- go(south).
east :- go(east).
west :- go(west).


/* Pick up Logic*/
pickup(X) :-
    /* If something to pick up, pick up */
    my_location(Place),
    object(Place,X),
    retract(object(Place,X)),
    assert(object(in_hand,X)),
    write("Picked up "),
    write(X), nl, nl,
    action(Place), ! ;
    /* if nothing to pick up, let player know */
    write("There is nothing to pick up"), nl, nl,
    my_location(Place),
    action(Place), !.

/* Use item logic */
use(X) :-
    my_location(Place),
    object_use(Place, X),
    retract(object(in_hand, X)),
    assert(object(used, X)).

/* Movement/Path Logic */
go(Direction) :-
    /* Change location based on path */
    my_location(Place),
    path(Place,Direction,Destination),
    retract(my_location(Place)),
    assert(my_location(Destination)),
    describe(Destination), ! ;
    /* Let player know can't go that way */
    write("You can't go that way."), nl, nl,
    my_location(Place),
    action(Place).

/* Text description + Action initalizer for main cave */
describe(mainCave) :-
    write("Back to where you started... Where shall you go next."), nl, nl,
    action(mainCave), nl, nl,
    describe(inventory).

/* Text description + Action initializer for kitchen door */
describe(storageDoor) :-
    /* Actions if have the key */
    object(in_hand,key),
    write("The key you picked up unlocks the door. You slowly push the door open..."), nl, nl,
    use(key),
    west,
    retractall(path(storageDoor,_,_)),
    retractall(path(mainCave,west,storageDoor)),
    assert(path(storageCloset,east,mainCave)),
    assert(path(mainCave,west,storageCloset)) ;
    /* Actions if don't have the key */
    write("You stumble across a locked door. Ornate symbols line the framing."), nl,
    write("You try and knock the door down but it is made of solid steel. You need the key."), nl,
    write("You can't explore further and return back to the main cave."), nl, nl,
    east.

/* Description of storageCloset */
describe(storageCloset) :-
    write("The room you enter is dusty storage closet, no one has been in it for a while."), nl,
    write("It looks like a storage room."), nl, nl,
    describe(objects), nl, nl,
    action(storageCloset), nl, nl,
    describe(inventory).

/* Description of mainCaveSouthWall */
describe(mainCaveSouthWall) :-
    write("You blindly walk south. You can't see more than a few feet in front of you."), nl,
    write("After a few seconds you run into a wall."), nl, nl,
    write("You can't explore further and return back to the main cave."), nl, nl,
    north.

/* Description of bathroom */
describe(bathroom) :-
    write("You walk east for a minute and see an open door to a lit room."), nl,
    write("After you enter you realize it is a bathroom."), nl, nl,
    describe(objects), nl, nl,
    action(bathroom), nl, nl,
    describe(inventory).


/* Description of what is in inventory */
describe(inventory) :-
    write("Inventory:"), nl,
    object(in_hand,X),
    write(X) ;
    write("Nothing").


describe(objects) :-
    my_location(Place),
    object(Place, X),
    write("There is "),
    write(X),
    write(" to pick up.") ;
    write("There is nothing more to pick up here.").

/* Text descriptions of available actions in main cave */
action(mainCave) :-
    write("Your available actions:"),nl,
    write("north            --Go North"), nl,
    write("south            --Go South"), nl,
    write("east             --Go East"), nl,
    write("west             --Go West").

/* Text descriptions of available actions in bathroom */
action(bathroom) :-
    write("Your available actions:"),nl,
    write("west             --Return to the main cave"),
    object(bathroom, key),
    nl, write("pickup(key)      --Pick up the key.") ;
    !.

/* Text descriptions of available actions in storageCloset */
action(storageCloset) :-
    write("Your available actions:"), nl,
    write("east             --Return to the main cave"), nl,
    object(storageCloset, plank),
    write("pickup(plank)    --Pickup the plank") ;
    !.













































