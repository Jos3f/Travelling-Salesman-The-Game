#include <iostream>
#include <istream>
#include <algorithm>
#include <math.h>
#include <cmath>
#include <iterator>
#include <vector>
#include <sstream>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <iomanip>
#include <fstream>

struct Location;
struct Player;
struct Item;
struct CarriableItem;

typedef void (*commandFunc)(Player *, std::vector <std::string>); // command function typedef

// Global variables
// std::unordered_map<std::string, std::function<void(Player*, std::vector<std::string>)>> commands;
std::unordered_map <std::string, commandFunc> commands;
std::vector<Location *> rooms;

struct Item {
    std::string name;

    virtual ~Item() {};
};

// ITems that can be carried by a tradesman or the player
struct CarriableItem : public Item {
    int value;
};

// Can be shovel/sword or other
struct Tool : public CarriableItem {
    bool dig;
    bool attack;
};

// Room
struct Location {
    std::string name;
    std::string description;

    std::unordered_map<std::string, Location *> can_go_to;
    std::unordered_map<std::string, Item *> items_in_room;
    std::vector<Item *> items_below_ground;

    void look() {
        std::cout << description << '\n';
    }

    // Print exists with direction and room
    void printExits() {
        std::cout << "Exits:" << '\n';
        for (const auto &direction : can_go_to) {
            std::cout << " " << direction.first << ": " << (direction.second)->name << '\n';
        }
    }

    // Print items currently in the room, but not those below ground
    void printItems() {
        if (items_in_room.size() > 0) {
            std::cout << "Items in this room: " << '\n';
            for (const auto &item : items_in_room) {
                std::cout << " " << item.first << '\n';
            }
        }
    }

    // Dig with a diggable tool in this room.
    // Removes the items below ground and places them above ground
    void dig(Tool *digging_object) {
        if (!(digging_object->dig)) {
            std::cout << "You can't dig with this tool." << '\n';
        } else {
            std::cout << "Digging" << '\n';
            for (int i = 0; i < 10; i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                // std::cout << "┃         ┃" << '\n';
                std::cout << "." << '\n';
            }
            // std::cout << "┗━━━━━━━━━┛" << '\n';
            std::cout << '\n' << "Found: " << '\n';
            if (items_below_ground.size() == 0) {
                std::cout << " Nothing." << '\n';
            }
            for (unsigned int i = 0; i < items_below_ground.size(); i++) {
                std::cout << " " << (items_below_ground[i])->name << '\n';
                items_in_room[(items_below_ground[i])->name] = items_below_ground[i];
            }
            items_below_ground.clear();

        }
    }
};

// Key holds a password and can be used to open locked items with the same password
struct Key : public CarriableItem {
    std::string password;
};

// This is the current player.
struct Player {
    std::string name;
    Location *current_room;
    std::unordered_map<std::string, CarriableItem *> holds_items;
    bool win;
    bool quit;

    void printCarries() {
        std::cout << "You are currently holding: " << '\n';
        for (const auto &holds : holds_items) {
            std::cout << " " << holds.first << ". Value: " << (holds.second)->value << " silver coins." << '\n';
        }
    }
};

// This is a chest.
struct Chest : public Item {
    bool locked;
    std::string password;

    void unlock(Key *key, Player *player) {
        if (key->password == password) {
            locked = false;
            player->win = true;
        } else {
            std::cout << "Wrong key" << '\n';
        }
    }
};

// This is a person. The person can talk.
struct Person : public Item {
    std::string phrase;

    virtual void talk() {
        std::cout << phrase << '\n';
    }
};

// This is a tradesman. The tradesman can hold and swap items with the player.
struct Tradesman : public Person {
    int min_price;
    std::unordered_map<std::string, CarriableItem *> has_items;

    CarriableItem *swap_item(CarriableItem *a, std::string wants) {
        if (a->value > min_price) {
            has_items[a->name] = a;
            CarriableItem *to_return = has_items[wants];
            has_items.erase(wants);
            if (to_return != nullptr) {
                has_items[a->name] = a;
            }
            return to_return;
        }
        return nullptr;
    }

    void talk() {
        std::cout << phrase << '\n';
        std::cout << "You can swap items with me if you're offering something worth at least " << min_price
                  << " silver coins." << '\n';
        std::cout << "I am currently holding: " << '\n';
        for (auto &item: has_items) {
            std::cout << " " << item.first << ". Value: " << item.second->value << " silver coins." << '\n';
        }
    }
};

// This function will take a string and return a vector of strings.
// The words in the string are placed in the vector in the same order.
std::vector <std::string> stringToVector(std::string s) {
    std::vector <std::string> v;
    std::istringstream ss(s + " ");
    std::string word = "";
    while (!ss.eof()) {
        if (ss.peek() == ' ' || ss.peek() == '\n') {
            if (word.length() > 0)
                v.push_back(word);
            word = "";
            ss.get();
        } else {
            word += tolower(ss.get());
        }
    }
    return v;
}

// Print current location
void look(Player *player, std::vector <std::string> words) {
    (player->current_room)->look();
    (player->current_room)->printExits();
    (player->current_room)->printItems();
}

// Print the items that the player carries
void carries(Player *player, std::vector <std::string> words) {
    player->printCarries();
}

// pick up an item from the current location
void take(Player *player, std::vector <std::string> words) {
    std::string item;
    if (words.size() > 0) {
        item = words[0];
        if ((player->current_room)->items_in_room[item] != nullptr) {
            try {
                (player->holds_items)[item] = dynamic_cast<CarriableItem *>(((player->current_room)->items_in_room)[item]);
                if ((player->holds_items)[item] == nullptr) {
                    (player->holds_items).erase(item);
                    std::cout << "There is no carriable item called " << words[0] << " at this location." << '\n';
                } else {
                    ((player->current_room)->items_in_room).erase(item);
                    std::cout << "Picked up " << item << "." << '\n';
                }

            } catch (...) {
                std::cout << "There is no carriable item called " << words[0] << " at this location." << '\n';
            }
        } else {
            ((player->current_room)->items_in_room).erase(item);
            std::cout << "There is no carriable item called " << words[0] << " at this location." << '\n';
        }
    } else {
        std::cout << "You need to specify what to take" << '\n';
    }
}

// Dig at the current location
void dig(Player *player, std::vector <std::string> words) {
    std::string item;

    if (words.size() > 0) {
        item = words[0];
        if ((player->holds_items)[item] != nullptr) {
            Tool *dig_with = dynamic_cast<Tool *>((player->holds_items)[item]);
            if (dig_with != nullptr) {
                (player->current_room)->dig(dig_with);
            } else {
                std::cout << "This is not a tool. You cannot dig with this." << '\n';
            }
        } else {
            (player->holds_items).erase(item);
            std::cout << "You don't carry an item called " << item << '\n';
        }
    } else {
        std::cout << "You need to specify what you want to dig with." << '\n';
    }
}

// Swap item with a tradesman
void swap(Player *player, std::vector <std::string> words) {
    std::string item_from;
    std::string item_to;
    std::string with_person;

    if (words.size() > 2) {
        with_person = words[0];
        item_from = words[1];
        item_to = words[2];
        if ((player->holds_items)[item_from] != nullptr) {
            CarriableItem *swap_from = (player->holds_items)[item_from];

            if (((player->current_room)->items_in_room)[with_person] != nullptr) {

                CarriableItem *swap_to = dynamic_cast<Tradesman *>(((player->current_room)->items_in_room)[with_person])->swap_item(
                        swap_from, item_to);
                if (swap_to != nullptr) {
                    (player->holds_items)[swap_to->name] = swap_to;
                    (player->holds_items).erase(item_from);
                    std::cout << "Thank you for the deal!" << '\n';
                } else {
                    std::cout << "Sorry, can't do that deal." << '\n';
                }
            } else {
                std::cout << "There is no person called " << with_person << " at this location to swap with." << '\n';
                ((player->current_room)->items_in_room).erase(with_person);
            }


        } else {
            ((player->holds_items)).erase(item_from);
            std::cout << "You don't carry an item called " << item_from << "." << '\n';
        }
    } else {
        std::cout
                << "You need to specify the person and two items to swap. First the person, then the item you want to offer and lastly the item you want in exchange."
                << '\n';
    }
}

// Go to another location
void go(Player *player, std::vector <std::string> words) {
    std::string direction;

    if (words.size() > 0) {
        direction = words[0];
        if ((player->current_room)->can_go_to[direction] != nullptr) {
            (player->current_room) = (player->current_room)->can_go_to[direction];
            (player->current_room)->look();
        } else {
            ((player->current_room)->can_go_to).erase(direction);
            std::cout << "You cannot to that." << '\n';
        }
    } else {
        std::cout << "You need to type a direction" << '\n';
    }
}

// Talk to a person
void talk(Player *player, std::vector <std::string> words) {
    std::string item;

    if (words.size() > 0) {
        item = words[0];
        if ((player->current_room)->items_in_room[item] != nullptr) {
            try {
                Person *talkTo = dynamic_cast<Person *>(((player->current_room)->items_in_room)[item]);
                if (talkTo == nullptr) {
                    // (player->holds_items).erase(item);
                    std::cout << "There is no person called " << item << " at this location." << '\n';
                } else {
                    talkTo->talk();
                }

            } catch (...) {
                std::cout << "There is no carriable ite called " << words[0] << " at this location." << '\n';
            }
        } else {
            ((player->current_room)->items_in_room).erase(item);
            std::cout << "There is no person called " << words[0] << " at this location." << '\n';
        }
    } else {
        std::cout << "You need to specify what to take" << '\n';
    }
}

// Unlock a locked item
void unlock(Player *player, std::vector <std::string> words) {
    std::string chest_item;
    std::string key_item;

    if (words.size() > 0) {
        chest_item = words[0];
        key_item = words[1];
        if ((player->current_room)->items_in_room[chest_item] != nullptr) {
            try {
                Chest *chest = dynamic_cast<Chest *>(((player->current_room)->items_in_room)[chest_item]);
                if (chest == nullptr) {
                    // (player->holds_items).erase(item);
                    std::cout << "There is no chest called " << chest_item << " at this location." << '\n';
                } else {
                    if ((player->holds_items)[key_item] != nullptr) {
                        Key *key = dynamic_cast<Key *>((player->holds_items)[key_item]);
                        if (key == nullptr) {
                            std::cout << "You're not carrying a key called " << key_item << "." << '\n';

                        } else {
                            chest->unlock(key, player);
                        }
                    } else {
                        (player->holds_items).erase(key_item);
                        std::cout << "You don't carry an item called " << key_item << "." << '\n';
                    }
                }
            } catch (...) {
                std::cout << "There is no carriable ite called " << words[0] << " at this location." << '\n';
            }
        } else {
            ((player->current_room)->items_in_room).erase(chest_item);
            std::cout << "There is no item called " << chest_item << " at this location." << '\n';
        }
    } else {
        std::cout << "You need to specify what to unlock and which key to use, in that order." << '\n';
    }
}

// Quit game
void quit(Player *player, std::vector <std::string> words) {
    player->quit = true;
}

// Free allocated memory in heap
void cleanUp(Player *player) {
    std::for_each(rooms.begin(), rooms.end(), [](Location *l) {

        for (auto &it: l->items_in_room) {
            if (dynamic_cast<Tradesman *>(it.second) != nullptr) {

                for (auto &holds : (dynamic_cast<Tradesman *>(it.second))->has_items) {
                    delete holds.second;
                }
            }

            delete it.second;
        }
        std::for_each((l->items_below_ground).begin(), (l->items_below_ground).end(), [](Item *below_it) {
            delete below_it;
        });
        delete l;
    });
    for (auto &it:player->holds_items) {
        delete it.second;
    }
    delete player;
}

void print_file(std::string file_path) {
    std::string s;
    std::ifstream file;

    file.open(file_path);
    while (getline(file, s)) {
        std::cout << s << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    }
    file.close();
}

// Print help
void help(Player *player, std::vector <std::string> words) {
    print_file("help_command.txt");
}

// Print ascii chest art
void printChest() {
    print_file("ascii_chest.txt");
}

// Shortcuts
void goNorth(Player *player, std::vector <std::string> words) {
    go(player, {"north"});
}

void goEast(Player *player, std::vector <std::string> words) {
    go(player, {"east"});
}

void goSouth(Player *player, std::vector <std::string> words) {
    go(player, {"south"});
}

void goWest(Player *player, std::vector <std::string> words) {
    go(player, {"west"});
}


// Fill map of commands to function
void fillCommands() {
    commands["look"] = &look;
    commands["go"] = &go;
    commands["take"] = &take;
    commands["carry"] = &take;
    commands["carries"] = &carries;
    commands["talk"] = &talk;
    commands["swap"] = &swap;
    commands["dig"] = &dig;
    commands["unlock"] = &unlock;
    commands["quit"] = &quit;
    commands["help"] = &help;

    commands["n"] = &goNorth;
    commands["e"] = &goEast;
    commands["s"] = &goSouth;
    commands["w"] = &goWest;
}

// Build the rooms and items in them
void buildRooms() {
    rooms.push_back((new Location{"Beach"}));
    rooms.push_back((new Location{"Town square"}));
    rooms.push_back((new Location{"Workshop"}));
    rooms.push_back((new Location{"Marketplace"}));

    rooms[0]->description = "You are on the Beach. There is an X on the ground.";
    rooms[1]->description = "You are in the Town square. This place is very empty today.";
    rooms[2]->description = "You are in the Workshop. You can probably find some tools here.";
    rooms[3]->description = "You are at the Marketplace. You might find someone to talk to here, and maybe even trade something.";


    // Beach
    rooms[0]->can_go_to["east"] = rooms[1];
    // town square
    rooms[1]->can_go_to["north"] = rooms[2];
    rooms[1]->can_go_to["east"] = rooms[3];
    rooms[1]->can_go_to["west"] = rooms[0];
    // Workshop
    rooms[2]->can_go_to["south"] = rooms[1];
    // Marketplace
    rooms[3]->can_go_to["west"] = rooms[1];

    // Add items to beach
    // Add chest
    (rooms[0]->items_below_ground).push_back(new Chest());
    (rooms[0]->items_below_ground)[0]->name = "chest";
    dynamic_cast<Chest *>((rooms[0]->items_below_ground)[0])->locked = true;
    dynamic_cast<Chest *>((rooms[0]->items_below_ground)[0])->password = "ABC123";

    // Add items to Workshop
    // Add sword
    (rooms[2]->items_in_room)["sword"] = new Tool();
    (rooms[2]->items_in_room)["sword"]->name = "sword";
    dynamic_cast<Tool *>((rooms[2]->items_in_room)["sword"])->attack = true;
    dynamic_cast<Tool *>((rooms[2]->items_in_room)["sword"])->value = 15;
    // Add shovel
    (rooms[2]->items_in_room)["shovel"] = new Tool();
    (rooms[2]->items_in_room)["shovel"]->name = "shovel";
    dynamic_cast<Tool *>((rooms[2]->items_in_room)["shovel"])->attack = true;
    dynamic_cast<Tool *>((rooms[2]->items_in_room)["shovel"])->dig = true;
    dynamic_cast<Tool *>((rooms[2]->items_in_room)["shovel"])->value = 5;

    // Add items to Marketplace
    (rooms[3]->items_in_room)["tradesman"] = new Tradesman();
    (rooms[3]->items_in_room)["tradesman"]->name = "tradesman";
    dynamic_cast<Tradesman *>((rooms[3]->items_in_room)["tradesman"])->min_price = 10;
    dynamic_cast<Person *>((rooms[3]->items_in_room)["tradesman"])->phrase = "Hello, my name is Marco Polo.";
    // Add key to tradesman
    (dynamic_cast<Tradesman *>((rooms[3]->items_in_room)["tradesman"])->has_items)["key"] = new Key();
    dynamic_cast<Key *>((dynamic_cast<Tradesman *>((rooms[3]->items_in_room)["tradesman"])->has_items)["key"])->name = "key";
    dynamic_cast<Key *>((dynamic_cast<Tradesman *>((rooms[3]->items_in_room)["tradesman"])->has_items)["key"])->password = "ABC123";
}

int main(int argc, char const *argv[]) {

    Player *player = new Player{"Your name here"};
    fillCommands();
    buildRooms();

    player->current_room = rooms[1];

    std::unordered_map < std::string, CarriableItem * > backpack;


    std::cout
            << "You are a pirate in the year 1275 and you are currently in Khanbaliq/Dadu. The game starts in the town square and your mission is to find the hidden chest and unlock it."
            << "\n\n";
    (player->current_room)->look();
    // Game loop
    while (true) {
        std::cout << '\n';
        std::string in;
        std::cout << ">";
        std::cin >> in;
        try {
            std::string line;
            getline(std::cin, line);
            std::vector <std::string> words = stringToVector(line);
            if (commands.find(in) == commands.end()) {
                std::cout << "You cannot to that. Use help command to view valid commands." << '\n';
            } else {
                commands[in](player, words);
            }
        } catch (...) {
            std::cout << "You cannot to that." << '\n';
        }
        if (player->win) {
            printChest();
            std::cout << "Congratulations. You unlocked the chest and won the game." << '\n';
        }
        if (player->win || player->quit) {
            break;
        }
    }

    cleanUp(player);
    return 0;
}
