# ASHARIS

## Goal
Make a game where players will live adventures, and just have meaningfull fun. A open masive multiplayer game.
There will be no rules, no lvs, players will be born from others grow up, train and learn, improve chose their lifestiles. But at the end they will all die. Time gives the game meaning, time spent with friends adventuring, growing or just having fun.

## How
* 2d simple to death to ease development
* Online, masive persistance. They game will have server meshing, player based so no need for servers (forget about balancing cheats etc). 
* World and history made by players, each world is procedural
* Everything in the world responds to time and player interaction. nothing is static ever.
* No recepies no skill trees. characters will have atributes, genes, phisic (phisical and magical) that dictate what they can or cannot do. there are phisics, chemistry and magic that interact with the world. Players will interact with a crafting system that allows them complete freedom. Ex, backing bread will infact be 'simulated', from gathering the weed, turning it into flour, making dough and backing. Not manually implemented but by a phisics and chemistry engine.

how will players take input? Its a game with no rules yet how do we let players interact with everything? If we dont make a tree cut mechanic and decide we will use a simulation between the wood of the tree and the axe movement then how does the player
make that movement, the exact movement the wish for? a tablet?

### MVP

* create a 'round' map, a huge one
* multiplayer, each player can connect to another
* server meshing
* persistance
* phisics chemistry and magic simulation
* unity game engine.
* proximity voice chat

### Implemetation
* The 2d world will be the same stile as a pokemon game, but its not tilled it uses normal coordinates

* There will be no textures, instead everything will have to be made with math, and simulations

* simulation:
    * temperature
    * preasure
    * oxigen, co2, h20
    * magic density
    * basic minerals and nutrients
    * gravity, weight