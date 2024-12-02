#include <iostream>
#include <array>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <cmath>

const int tableLength = 113;
// SHOULD ALWAYS BE MULTIPLES OF 4 PLUS 1

const std::string empty = "---";
const std::string invalidSyntaxMessage = "ERROR! Invalid Syntax!";
const std::string overItemCapacityMessage = "ERROR! Item amounts can't extend beyond 64!";
const std::string underItemCapacityMessage = "ERROR! Item amounts can't drop beneath 0!";
const std::string overInventoryCapacityMessage = "ERROR! Inventory slots only go from 1 to 64!";
const std::string emptySlotMessage = "ERROR! That slot is empty!";

struct Item {
    std::string name{empty};
    std::string type{empty};
    std::string description{empty}; 
    int amount{0};
};


std::string decontaminateFatalError (std::string userPrompt) {
    // prevvents crashing from too many whitespaces in prompt
    bool FATAL_ERROR = static_cast<size_t>(userPrompt.find("  ", 0)) != std::string::npos;

    if (FATAL_ERROR) {
        return empty;
    }

    else {
        return userPrompt;
    }

}

void displayBorder () {
    std::string tableBorder;
    tableBorder.assign(tableLength, '='); 


    std::cout << tableBorder << "\n";
}

void displayCell (int index, Item item, char columnSide) {
    int nameSpaceAllocation = (tableLength / 4) - 11;
                                                // increase this value if you're gonna use more spaces in each cell
    std::string name = item.name;
    int amount = item.amount;
    
    if (item.name.size() > nameSpaceAllocation) {
        name = name.substr(0, (nameSpaceAllocation - 5)) + "...";
    }

    std::string indexSpaces;
    indexSpaces.assign(((index < 9) ? 2: 1), ' ');
    //spaces always refer to the ones on the left side of the text
    std::string amountSpaces;
    amountSpaces.assign((nameSpaceAllocation - name.size()) + 1, ' ');
    // 2 spaces after item name
    std::string borderSpaces;
    borderSpaces.assign(((item.amount < 9) ? 2 : 1), ' ');


    std::cout << ((columnSide == 'l') ? "|" : "") << indexSpaces << index+1 << ". " << name << amountSpaces << 'x' << amount << borderSpaces << '|';
}

void displayTable (std::string title, Item inventory[64]) {
    std::string titleSpaces;
    std::string borderSpaces;
    titleSpaces.assign(static_cast<int>(tableLength/2) - static_cast<int>(title.size()/2) - 2, ' ');
    borderSpaces.assign(static_cast<int>(tableLength/2) - static_cast<int>(title.size()/2) - 1, ' ');


    displayBorder();
    std::cout << "||" << titleSpaces << title << borderSpaces << "||" << '\n';
    displayBorder();

    for (int index = 0; index < 64/4; index++) {
        displayCell(index, inventory[index], 'l');
        displayCell(index + (64/4), inventory[index + (64/4)], 'r');
        displayCell(index + ((64/4)*2), inventory[index + ((64/4)*2)], 'r');
        displayCell(index + ((64/4)*3), inventory[index + ((64/4)*3)], 'r');
        std::cout << "\n";
        displayBorder();
    }

}

void checkIfItemEmpty (int index, Item inventory[64]) {
    Item* item = &inventory[index-1];

    if (item->amount <= 0) {
        item->name = empty;
        item->type = empty;
        item->description = empty;
        item->amount = 0;
    }
}

void displayItem (int index, Item inventory[64]) {
    index--;
    
    Item* item = &inventory[index];

    std::string amountSpaces;
    amountSpaces.assign((tableLength - item->type.size()) - ((item->amount > 9) ? 5 : 4), ' ');
    // the 2 is for the brackets

    displayBorder();
    std::cout << item->name << '\n';
    std::cout << '[' << item->type << ']' << amountSpaces << 'x' << item->amount << '\n';
    std::cout << '\n';
    std::cout << item->description << '\n';
    displayBorder();

}

void addItem (std::string name, std::string type, std::string description, int amount, Item inventory[64], int index) {
    index--;
    
    inventory[index].name = name;
    inventory[index].type = type;
    inventory[index].description = description;
    inventory[index].amount = amount;
}

void deleteItem (int index, Item inventory[64]) {
    // not in use?
    Item* item = &inventory[index-1];

    item->name = empty;
    item->type = empty;
    item->description = empty;
    item->amount = 0;
}

void updateItemAmount (int subtrahend, int index, Item inventory[64]) {
    index--;

    int newValue = inventory[index].amount - subtrahend;

    inventory[index].amount = newValue;
}


void displayGuide () {
    std::cout << "Guide:" << "\n";
    std::cout << "-?    HELP                 -?" << "\n";
    std::cout << "-a    ADD ITEM             -a inventory_index <name> <type> <description> amount" << "\n";
    std::cout << "      ADD MORE TO ITEM     -a inventory_index amount_to_be_added" << "\n";
    std::cout << "-dp   DISPLAY ITEM         -dp inventory_index" << "\n";
    std::cout << "      DISPLAY ALL          -dp" << "\n";
    std::cout << "-dr   DROP ITEM            -dr inventory_index amount" << "\n";
    std::cout << "      DROP ALL OF AN ITEM  -dr inventory_index" << "\n";
    std::cout << "-c    CLEAR INVENTORY      -c" << "\n";
    std::cout << "-x    EXIT                 -x" << "\n";
    std::cout << '\n';
    std::cout << "Ex:   DISPLAY ITEM 16      -dp 16 " << '\n';
    std::cout << '\n';
    std::cout << "PS: <input> means to enclose that input in <>";
    std::cout << "\n";
}

void parseAddItemCommand (std::vector<std::string> commandVector, Item inventory[]) {
    try {
        std::istringstream streamAmount(commandVector[5]);
        std::istringstream streamIndex(commandVector[1]);
        int amount;
        int index;
        streamAmount >> amount;
        streamIndex >> index;

        if (amount > 64) {
            throw 1;
        }

        else if (0 >= index || index > 64) {
            throw 2;
        }


        addItem(commandVector[2], commandVector[3], commandVector[4], amount, inventory, index);
        checkIfItemEmpty(index, inventory);
    }

    catch (int i) {
        if (i == 2) {
            std::cout << overInventoryCapacityMessage << '\n';
        } 

        else if (i == 1) {
        std::cout << overItemCapacityMessage << '\n';
        }
    }

    catch (...) {
        std::cout << invalidSyntaxMessage << '\n';
    }
}

void parseUpdateItem (std::vector<std::string> commandVector, Item inventory[]) {
    try {
        std::istringstream streamAmountToBeAdded(commandVector[2]);
        std::istringstream streamIndex(commandVector[1]);
        int amountToBeAdded;
        int index;
        streamAmountToBeAdded >> amountToBeAdded;
        streamIndex >> index;

        amountToBeAdded = std::abs(amountToBeAdded);

        if (amountToBeAdded + inventory[index-1].amount > 64) {
            throw 1;
        }

        else if (0 >= index || index > 64) {
            throw 2;
        }

        else if (inventory[index-1].name == "---") {
            throw 5;
        }

        else {
            updateItemAmount(-(amountToBeAdded), index, inventory);
        }

        checkIfItemEmpty(index, inventory);
    }

    catch (int i) {
        if (i == 1) {
            std::cout << overItemCapacityMessage << '\n';
        }

        else if (i == 2) {
            std::cout << overInventoryCapacityMessage << '\n';
        }

        else if (i == 5) {
            std::cout << emptySlotMessage << '\n';
        }
    }

    catch (...) {
        std::cout << invalidSyntaxMessage << '\n';
    }
}

void parseDisplayItem (std::vector<std::string> commandVector, Item inventory[]) {
    try {    
        std::istringstream streamIndex(commandVector[1]);
        int index;
        streamIndex >> index;


        if (0 >= index || index > 64) {
            throw 2;
        }
        
        else if (inventory[index-1].name == "---") {
            throw 5;
        }

        else {
            displayItem(index, inventory);
        }
        
    }

    catch (int i) {
        if (i == 2) {
            std::cout << overInventoryCapacityMessage << '\n';
        }

        else if (i == 5) {
            std::cout << emptySlotMessage << '\n';
        }
    }

    catch (...) {
        std::cout << invalidSyntaxMessage << '\n';
    }
}

void parseDropItem (std::vector<std::string> commandVector, Item inventory[]) {
    try {
        std::istringstream streamAmountToBeSubtracted(commandVector[2]);
        std::istringstream streamIndex(commandVector[1]);
        int amountToBeSubtracted;
        int index;
        streamAmountToBeSubtracted >> amountToBeSubtracted;
        streamIndex >> index;

        amountToBeSubtracted = std::abs(amountToBeSubtracted);

        if (0 >= index || index > 64) {
            throw 2;
        }

        else if (inventory[index-1].amount - amountToBeSubtracted < 0) {
            throw 3;
        }

        else if (inventory[index-1].name == "---") {
            throw 5;
        }

        else {
            updateItemAmount(amountToBeSubtracted, index, inventory);
        }
        
        checkIfItemEmpty(index, inventory);
    }

    catch (int i) { 
        if (i == 2) {
            std::cout << overInventoryCapacityMessage << '\n';
        }

        else if (i == 3) {
            std::cout << underItemCapacityMessage << '\n';
        }

        else if (i == 5) {
            std::cout << emptySlotMessage << '\n';
        }
    }

    catch (...) {
        std::cout << invalidSyntaxMessage << '\n';
    }
}
void parseDropAllOfItem (std::vector<std::string> commandVector, Item inventory[]) {
    try {
        std::istringstream streamIndex(commandVector[1]);
        int index;
        streamIndex >> index;

        if (0 >= index || index > 64) {
            throw 2;
        }

        else if (inventory[index-1].name == "---") {
            throw 5;
        }
        
        else {
            inventory[index-1].amount = 0;
        }
        
        checkIfItemEmpty(index, inventory);
    }

    catch (int i) {
        if (i == 2) {
            std::cout << overInventoryCapacityMessage << '\n';
        }

        else if (i == 5) {
            std::cout << emptySlotMessage << '\n';
        }
    }

    catch (...) {
        std::cout << invalidSyntaxMessage << '\n';
    }


}

void promptAction (Item inventory[64]) {
    std::string userPrompt;
    std::string command;
    // mostly just for catching stuff in loops
    std::vector<std::string> initialCommands;
    // before combining the quoted parts
    std::vector<std::string> finalCommands;
    bool stringShouldCombine{false};

    std::cout << "> ";
    getline(std::cin, userPrompt);

    userPrompt = decontaminateFatalError(userPrompt);


    std::istringstream userAction(userPrompt);
    std::cout << "\n";

    while (getline(userAction, command, ' ')) {
        initialCommands.push_back(command);
    }

    command = "";

    for (int index = 0; index < initialCommands.size(); index++) {
        // parses string for multi-worded stuff

        if (stringShouldCombine){
            command.append(std::string(" ") + initialCommands[index]);
        } 

        else {
            command = initialCommands[index];
        }

        if (initialCommands[index].rfind('<') == 0) {
            stringShouldCombine = true;
            command.erase(0, 1);
        }

        if (initialCommands[index].rfind('>') == initialCommands[index].size() - 1) {
            stringShouldCombine = false;
            command.erase(command.size() - 1, 1);
        }

        if (!stringShouldCombine) {
            finalCommands.push_back(command);
        }

    }

    if (finalCommands[0] == "-?") {
        displayGuide();
    } 
    
    else if (finalCommands[0] == "-a" && finalCommands.size() == 6) {
        parseAddItemCommand(finalCommands, inventory);
    }

    else if (finalCommands[0] == "-a" && finalCommands.size() == 3) {
        parseUpdateItem(finalCommands, inventory);
    }

    else if (finalCommands[0] == "-dp" && finalCommands.size() == 2) {
        parseDisplayItem(finalCommands, inventory);
    }

    else if (finalCommands[0] == "-dp" && finalCommands.size() == 1) {
        displayTable("YOUR INVENTORY", inventory);
    }

    else if (finalCommands[0] == "-dr" && finalCommands.size() == 3) {
        parseDropItem(finalCommands, inventory);
    }

    else if (finalCommands[0] == "-dr" && finalCommands.size() == 2) {
        parseDropAllOfItem(finalCommands, inventory);
    }

    else if (finalCommands[0] == "-c" && finalCommands.size() == 1) {
        for (int index = 1; index < 65; index++) {
            updateItemAmount(64, index, inventory);
            checkIfItemEmpty(index, inventory);
        }
    }

    else if (finalCommands[0] == "-x" && finalCommands.size() == 1) {
        exit(1);
    }

    else {
        std::cout << invalidSyntaxMessage << '\n';
    }

    std::cout << "\n";
}

int main () {
    Item inventory[64];

    displayGuide();
    std::cout << '\n';
    std::cout << '\n';



    addItem("Sword of Darkness", "Weapon", "Used by the Green Ranger back when he was evil.", 1, inventory, 4);
    addItem("Hurricane-in-a-Bottle", "Artifact", "Unleashes an arcane hurricane to devastate the battlefield.", 1, inventory, 17);
    addItem("Healing Potion", "Consumable", "Gives you life, which  is ironic since it tastes like death.", 5, inventory, 27);
    addItem("Herbalism Potion", "Consumable", "Increase your attunement to nature for a few hours.", 1, inventory, 16);
    addItem("Wood Planks", "Material", "Freshly chopped.", 10, inventory, 28);
    addItem("Overpriced Rations", "Consumable", "Half the usual portions.", 15, inventory, 50);
    addItem("Leather Cap", "Armor", "Worn leather.", 1, inventory, 61);

    displayTable("YOUR INVENTORY", inventory);
    std::cout << '\n';

    while (true) {
        promptAction(inventory);
    }
    return 1;
}