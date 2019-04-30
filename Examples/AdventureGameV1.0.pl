h:- dynamic my_location/1, object/2, path/3.


/* TYPE START. TO BEGIN GAME */
/* CREATED BY JORDAN ISAACS - 2019 */


/* Places objects can be used */
object_use(storageDoor,key).
object_use(stream,plank).

/* Game Start */
start :-
    /* remove all locations/objects */
    retractall(my_location(_)), retractall(object(_,_)), retractall(path(_,_,_)),
    /* Creating the paths to rooms */
    assert(path(mainCave,south,mainCaveSouthWall)),
    assert(path(mainCaveSouthWall,north,mainCave)),

    assert(path(mainCave,east,bathroom)),
    assert(path(bathroom,west,mainCave)),

    assert(path(mainCave,west,storageDoor)),
    assert(path(storageDoor,east,mainCave)),

    assert(path(storageDoor,west,storageCloset)),
    assert(path(storageCloset,east,storageDoor)),

    assert(path(mainCave,north,tunnel)),
    assert(path(tunnel,south,mainCave)),

    assert(path(tunnel,north,stream)),
    assert(path(stream,south,tunnel)),

    assert(path(stream,north,stairs)),
    assert(path(stairs,south,stream)),

    assert(path(stairs,west,exit)),

    /* Initialize current location */
    assert(my_location(mainCave)),

    /* Initalize object locations */
    assert(object(bathroom,key)),
    assert(object(storageCloset,plank)),

    /* Prolog Intro */
    write("--IMPORTANT NOTE: PROLOG INTRO--"), nl,
    write("Prolog is the language this game is based on."), nl,
    write("Prolog requires \".\" at the end of all commands."), nl,
    write("For example \"north\" would be an invalid command."), nl,
    write("But \"north.\" is. Make sure to put a \".\" at the end of every command."), nl, nl, nl, nl,

    /* Begin game descriptions */
    write("--BEGIN GAME--"), nl, nl,
    write("You wake up and look around. You have no recollection of how you got to this dark cave."), nl,
    write("Looking down you see you were sleeping on a pristine bed. Checking your pockets, you find a compass."), nl,
    write("Not knowing what to do, you decide to explore."), nl, nl,
    write("There is nothing to pick up here."), nl, nl,
    action(mainCave), nl, nl,
    describe(inventory).

/* Movement Actions */
north :- go(north).
south :- go(south).
east :- go(east).
west :- go(west).
n :- go(north).
s :- go(south).
e :- go(east).
w :- go(west).

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
    assert(object(used, X)),
    usePlankLogic(X),
    useKeyLogic(X), !.

/* Logic for using the key */
useKeyLogic(X) :-
    X == key,
    write("You insert the key into the door. It fits perfectly."), nl, nl,
    action(storageDoor) ;
    true.

/* Text description of placing the plank */
usePlankLogic(X) :-
    X == plank,
    write("You place the plank across the stream. You can now cross"), nl, nl,
    action(stream) ;
    true.

/* If path goes over the stream, ensure the user placed the plank for successful movement */
plankMoveLogic(Place,Direction,Destination) :-
    (   path(Place,Direction,Destination) == path(stream,north,stairs) ->
        (   object(used,plank) ->
                true ;
            false);
        true
    ).

/* Can't go into the storageCloset without a used key */
keyMoveLogic(Place,Direction,Destination) :-
    (   path(Place,Direction,Destination) == path(storageDoor,west,storageCloset) ->
        (   object(used,key) ->
                true;
            false);
        true
    ).


/* Movement/Path Logic */
go(Direction) :-
    /* Change location based on path */
    my_location(Place),
    path(Place,Direction,Destination),
    /* Go to plankMoveLogic/keyMoveLogic */
    plankMoveLogic(Place,Direction,Destination),
    keyMoveLogic(Place,Direction,Destination),
    /* Change location to destination */
    retract(my_location(Place)),
    assert(my_location(Destination)),
    /* Describe the destination */
    describe(Destination), ! ;
    /* If no path exists, let user know they can't go that way */
    my_location(Place),
    write("You can't go that way."), nl, nl,
    action(Place), !.


/* Text description for main cave (calls description of actions and current inventory) */
describe(mainCave) :-
    write("Back to where you started... Where shall you go next."), nl, nl,
    write("There is nothing to pick up here."), nl, nl,
    action(mainCave), nl, nl,
    describe(inventory).

/* Text description + Logic for storage door */
describe(storageDoor) :-
    /* If have the key, rewrite the path to ignore the door, use the key, and jump to storageCloset*/
    write("You run across a steel door. Ornate symbols line the framing."), nl,
    (   object(used,key) ->
            write("The door is unlocked.") ;
        object(in_hand,key) ->
            write("The key you have can unlock the door."), nl ;
        write("You try and knock the door down but it is made of solid steel. You need the key.")
    ),
    nl, nl, write("There is nothing to pick up here."),
    nl, nl, action(storageDoor), nl, nl,
    describe(inventory).

/* Text description for tunnel (calls description of actions and current inventory) */
describe(tunnel) :-
    write("You see torches north of you and head towards them."), nl,
    write("The torches line a damp tunnel. Water drips on your head."),nl,
    write("Etched art lines the walls. They depict a beast guarding a cave."), nl, nl,
    write("There is nothing to pick up here"), nl, nl,
    action(tunnel), nl, nl,
    describe(inventory).

/* Text desription/text logic for the stream + (calls description of actions and current inventory) */
describe(stream) :-
    write("You encounter a fast moving stream. It is too wide to jump."), nl,
    (   object(in_hand,plank) ->
            write("The plank you picked up can be used as a makeshift bridge.") ;
    (   object(used,plank) ->
            write("You already layed down a plank to help you cross.") ;
        write("You need something to help you cross it.")
    )
    ),
    nl, nl, action(stream), nl, nl,
    describe(inventory).


/* Text description for stairs (calls available actions and current inventory) */
describe(stairs) :-
    write("You arrive at stairs that go to your west. At the end of those stairs is light."), nl,
    write("You are relieved. But you are scared to go up the stairs."), nl,
    write("You remember the etching of a beast guarding a cave on the wall of the tunnel."), nl,
    write("It is up to you..."), nl, nl,
    write("There is nothing to pick up here."), nl, nl,
    action(stairs), nl, nl,
    describe(inventory).

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

describe(exit) :-
    write("You inch up the stairs and go out into the light."), nl,
    write("You see a figure in the distance. It is coming towards you..."), nl, nl,
    write("You freak out, the figure is holding something"), nl,
    write("As they get closer you see it is fire wood. Is the figure going to eat you???"), nl, nl,
    write("You now can discern it is a him. There is nowhere for you to hide."), nl,
    write("He yells out, \"Oh howdy! You are awake.\""), nl, nl,
    write("Now you are thoroughly confused. What is the meaning of this."), nl,
    write("You stand there paralyzed as the man approaches. He asks if you are alright."), nl, nl,
    write("You say that you don't remember anything. You can't make sense of the situation."), nl, nl,
    write("The man lets you know that you were in a plane crash. He found you and brought you back to his home in the cave."), nl,
    write("He had been living off the grid for the past few years. You ask about the stream."), nl, nl,
    write("He says has a few planks to cross it but always removes them so animals can't get into the living quarters."), nl, nl,
    write("You then ask about the beast guarding the cave."), nl,
    write("He lets out a bellowing laugh. He motions for you to turn around. There is a stuffed bear's head guarding the cave entrance."), nl, nl,
    write("Its just decoration he says."), nl,
    write("He tells you that a helicopter is coming in the next few days to bring you back to civilization."), nl, nl, nl,
    write("===================================================="),nl,
    write("======================YOU WON======================="),nl,
    write("===================================================="),nl,
    write("            WRITE start. TO PLAY AGAIN              "),nl.

/* Description of what is in inventory */
describe(inventory) :-
    write("Inventory:"), nl,
    forall(object(in_hand,X),invwrite(X)),
    (   object(in_hand,X) ->
            true ;
        write("Nothing")
    ).

/* Describe what objects are available in current room */
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
    write("west             --Return to the door"),
    object(bathroom, key),
    nl, write("pickup(key)      --Pick up the key.") ;
    !.

/* Text descriptions of available actions in storage door */
action(storageDoor) :-
    write("Your available actions:"),nl,
    write("east             --Return to the main cave"),
    object(in_hand,key),
    nl,write("use(key)         --Insert key into door") ;
    object(used,key),
    nl,write("west             --Go through the door") ;
    !.


/* Text descriptions+text logic of available actions in storageCloset */
action(storageCloset) :-
    write("Your available actions:"), nl,
    write("east             --Return to the main cave"),
    object(storageCloset, plank),
    nl, write("pickup(plank)    --Pickup the plank") ;
    !.

/* Text descriptions of available actions in tunnel */
action(tunnel) :-
    write("Your available actions:"), nl,
    write("north            --Continue north"), nl,
    write("south            --Return to the main cave").

/* Text descriptions+text logic of available actions in stream*/
action(stream) :-
    write("south            --Return to the tunnel"),
    (   object(in_hand,plank) ->
            nl, write("use(plank)       --Lay down the plank to allow crossing") ;
    (   object(used,plank) ->
            nl, write("north            --Continue north") ;
        true)
    ).

/* Text descriptions of available actions in stairs */
action(stairs) :-
    write("south            --Return to stream"), nl,
    write("west             --Go up the stairs").

/* Found in describe(inventory), prints out inventory */
invwrite(X) :-
    write(X), nl.












































