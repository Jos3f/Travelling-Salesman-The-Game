
# Travelling Salesman: The Game

## Story

You are a pirate in the year 1275 and you are currently in Khanbaliq/Dadu. The game starts in the town square and your mission is to find the hidden chest and unlock it.

The map can be found [here.](map.png)

## Instructions

This is a text based game.

### How to compile

Write "make" in the terminal while in current folder.

Tested on Ubuntu 18.04.1

### How to run

Write ./make.out in the terminal.

### Commands

All options are required.
Everything should be written on the same line and you submit your command by pressing enter.

| Command     | Options                  | Action                                                                                           |
|-------------|--------------------------|--------------------------------------------------------------------------------------------------|
| help        |                          | Get a list of all commands                                                                       |
| look        |                          | Will print your location together with items at the current location and valid exits.            |
| go          | [direction]              | Will go to the room in the given direction.                                                      |
| take/carry  | [item]                   | With take the given item from the current location and carry it with the player.                 |
| carries     |                          | Displays the items the player carries together with the value of the items.                      |
| talk        | [person]                 | Talks to the person.                                                                             |
| swap        | [person] [item1] [item2] | Will swap an item (item1) that the player carries to another item (item2) with the given person. |
| dig         | [item]                   | Dig in the current room using the given item.                                                    |
| unlock      | [item1] [item2]          | Unlock item1 using item2.                                                                        |
| quit        |                          | Quit the game.                                                                                   |

Example command: unlock chest key

You can also use the following shortcuts:

| Shortcut | Corresponding command |
|----------|-----------------------|
| n        | go north              |
| e        | go east               |
| s        | go south              |
| w        | go west               |

A list of command which leads to a win can be found [here.](win_command.txt)
