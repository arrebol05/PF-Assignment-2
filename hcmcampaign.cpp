#include "hcmcampaign.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
////////////////////////////////////////////////////////////////////////

////////////////////////////// Class Position //////////////////////////////
Position::Position(int r = 0, int c = 0) {
    this->r = r;
    this->c = c;
}

Position::Position(const string & str_pos) {
    sscanf(str_pos.c_str(), "(%d,%d)", &this->r, &this->c);
}

int Position::getRow() const {
    return this->r;
}

int Position::getCol() const {
    return this->c;
}

void Position::setRow(int r) {
    this->r = r;
}

void Position::setCol(int c) {
    this->c = c;
}

string Position::str() const {
    return "(" + to_string(this->r) + "," + to_string(this->c) + ")";
}

////////////////////////////// Class Unit //////////////////////////////
Unit::Unit(int quantity, int weight, const Position pos) {
    this->quantity = quantity;
    this->weight = weight;
    this->pos = pos;
}

Position Unit::getCurrentPosition() const {
    return this->pos;
}


int Unit::getQuantity() const {
    return this->quantity;
}

int Unit::getWeight() const {
    return this->weight;
}

void Unit::setQuantity(int quantity) {
    this->quantity = quantity;
}

void Unit::setWeight(int weight) {
    this->weight = weight;
}

////////////////////////////// Class Vehicle //////////////////////////////
Vehicle::Vehicle(int quantity, int weight, const Position pos, VehicleType vehicleType) : Unit(quantity, weight, pos) {
    this->vehicleType = vehicleType;
}

int Vehicle::getAttackScore() {
    int typeValue = static_cast<int>(this->vehicleType);

    return (typeValue * 304 + this->quantity * this->weight) / 30;
}

string Vehicle::str() const {
    string typeName[7] = {"TANK", "ARTILLERY", "ARMOREDCAR", "APC", "TRUCK", "MORTAR", "ANTIAIRCRAFT"};
    return "Vehicle[vehicleType=" + typeName[this->vehicleType] + ",quantity=" + to_string(this->quantity) + ",weight=" + to_string(this->weight) + ",pos=" + this->pos.str();
}

VehicleType Vehicle::getVehicleType() const {
    return this->vehicleType;
}

////////////////////////////// Class Infantry //////////////////////////////
int Infantry::calPersonalNumber(int number) {
    int sum = 0;

    while (number > 0) {
        sum += number % 10;
        number = number / 10;
    }

    return sum;
}
Infantry::Infantry(int quantity, int weight, const Position pos, InfantryType infantryType) : Unit(quantity, weight, pos) {
    this->infantryType = infantryType;
}

int Infantry::getAttackScore() {
    int typeValue = static_cast<int>(this->infantryType);

    int score = typeValue * 56 + this->quantity + this->weight;

    if (this->infantryType == SPECIALFORCES && this->weight == sqrt(this->weight) * sqrt(this->weight)) {
        score += 75;
        return score;
    }

    int personalNumber = calPersonalNumber(score + 1975);
    if (personalNumber > 7) {
        this->quantity *= 1.2;
    } else if (personalNumber < 3) {
        this->quantity *= 0.9; 
    }

    return typeValue * 56 + this->quantity + this->weight;
}

string Infantry::str() const {
    string typeName[6] = {"SNIPER", "ANTIAIRCRAFTSQUAD", "MORTARSQUAD", "ENGINEER", "SPECIALFORCES", "REGULARINFANTRY"};
    return "Infantry[infantryType=" + typeName[this->infantryType] + ",quantity=" + to_string(this->quantity) + ",weight=" + to_string(this->weight) + ",pos=" + this->pos.str();
}

InfantryType Infantry::getInfantryType() const {
    return this->infantryType;
}

////////////////////////////// Class Army //////////////////////////////
Army::Army(Unit **unitArray, int size, string name, BattleField *battleField) {
    this->LF = 0;
    this->EXP = 0;
    this->name = name;
    this->battleField = battleField;

    for (int i = 0; i < size; i++) {
        const Vehicle *vehicle = dynamic_cast<const Vehicle*>(unitArray[i]);
        const Infantry *infantry = dynamic_cast<const Infantry*>(unitArray[i]);

        if (vehicle) {
            this->LF += unitArray[i]->getAttackScore();
        } else if (infantry) {
            this->EXP += unitArray[i]->getAttackScore();
        }
    }

    this->unitList = new UnitList(this->LF, this->EXP);

    for (int i = 0; i < size; i++) {
        this->unitList->insert(dynamic_cast<Unit*>(unitArray[i]));
    }
}

UnitList* Army::getUnitList() const {
    return this->unitList;
}

int Army::getLF() const {
    return this->LF;
}

int Army:: getEXP() const {
    return this->EXP;
}

void Army::setLF(int LF) {
    this->LF = LF;
}

void Army::setEXP(int EXP) {
    this->EXP = EXP;
}

void Army::recalcIndex() {
    this->LF = 0;
    this->EXP = 0;

    UnitNode* current = this->unitList->getHead();
    while (current) {
        Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
        Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

        if (vehicle) {
            this->LF += vehicle->getAttackScore();
        } else {
            this->EXP += infantry->getAttackScore();
        }
        
        current = current->next;
    }
}

////////////////////////////// Class LiberationArmy //////////////////////////////
LiberationArmy::LiberationArmy(Unit** unitArray, int size, string name, BattleField* battleField) : Army(unitArray, size, name, battleField) {}

int LiberationArmy::getNearestFibonacci(int number) {
    if (number <= 1) {
        return 1;
    }

    int fib1 = 1, fib2 = 1;
    while (fib2 < number) {
        int temp = fib1 + fib2;
        fib1 = fib2;
        fib2 = temp;
    }

    return fib2;
}

void LiberationArmy::fight(Army* enemy, bool defense = false) {
    // Attack case
    if (!defense) {
        // Liberation Army got a boost
        this->LF = ceil(this->LF * 1.5);
        this->EXP = ceil(this->EXP * 1.5);

        /*
        Get vectors of vehicles & infantries for handling.
        To reduce calculation overhead, we will create the attack score vector for each list.
        */

        vector<Vehicle*> vehicleUnits;
        vector<Infantry*> infantryUnits;
        vector<int> vehicleScores, infantryScores;

        // Setup to initialize min as the sum of list vehicles/ infantries
        int minVehicleScore = 0;
        int minInfantryScore = 0;

        // Traverse from head to tail
        UnitNode* current = this->unitList->getHead();
        while (current) {
            Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
            Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

            if (vehicle) {
                vehicleUnits.push_back(vehicle);
                int score = vehicle->getAttackScore();
                vehicleScores.push_back(score);
                minVehicleScore += score;
            } else {
                infantryUnits.push_back(infantry);
                int score = infantry->getAttackScore();
                infantryScores.push_back(infantry->getAttackScore());
                minInfantryScore += score;
            }

            current = current->next;
        }

        // Initialization for combination
        vector<Vehicle*> bestVehicleCombination;
        vector<Infantry*> bestInfantryCombination; 

        bool foundVehicleCombination = minVehicleScore >= enemy->getLF(),
             foundInfantryCombination = minInfantryScore >= enemy->getEXP();

        // We only loop and get in case of there exists a combination
        if (foundVehicleCombination) {
            int vehicleCount = vehicleUnits.size();

            // Bitmask technique to get all combination
            for (int mask = 1; mask < (1 << vehicleCount); mask++) {
                vector<Vehicle *> combination;
                int totalScore = 0;

                for (int i = 0; i < vehicleCount; i++) {
                    if (mask & (1 << i)) {
                        combination.push_back(vehicleUnits[i]);
                        totalScore += vehicleScores[i];
                    }
                }

                if (totalScore > enemy->getEXP() && totalScore < minVehicleScore) {
                    minVehicleScore = totalScore;
                    bestVehicleCombination = combination;
                }
            }
        }
        
        if (foundInfantryCombination) {
            int infantryCount = infantryUnits.size();
            for (int mask = 1; mask < (1 << infantryCount); mask++) {
                vector<Infantry*> combination;
                int totalScore = 0;

                for (int i = 0; i < infantryCount; i++) {
                    if (mask & (1 << i)) {
                        combination.push_back(infantryUnits[i]);
                        totalScore += infantryScores[i];
                    }
                }

                if (totalScore > enemy->getEXP() && totalScore < minInfantryScore) {
                    minInfantryScore = totalScore;
                    bestInfantryCombination = combination;
                }
            }
        }        
        
        // Condition check for fight result
        // Case found both combination: Will definitely win
        if (foundVehicleCombination && foundInfantryCombination) {
            // Delete all the combinations, add enemy
            for (Vehicle* unit : bestVehicleCombination) {
                this->unitList->deleteVehicle(unit);
            }

            for (Infantry* unit : bestInfantryCombination) {
                this->unitList->deleteInfantry(unit);
            }

            /*
                Repurpose enemy's units - 3 cases:

                Case 1: Successfully find the unit in Liberation unitList
                Update quantity, delete the pointer of enemy's unit (pointer = nullptr)

                Case 2: Not find, still have capacity (check the capacity)
                Call insert, if insert return true (means add to Unit* successfully)

                Case 3: Not find and have no capacity (full)
                Call insert - insert return false (means add fail)
                Add unit* to additional vector to put back into enemy later
            */

            // Create vector of all units in current unitList
            vector<Unit*> currentUnits;
            UnitNode* current = this->unitList->getHead();
            while (current) {
                currentUnits.push_back(current->unit);
                current = current->next;
            }

            // Pop all units from enemy into a temporary vector 
            vector<Unit*> enemyUnits;
            UnitNode* temp = enemy->getUnitList()->getHead();
            while (temp) {
                enemyUnits.push_back(temp->unit);
                temp = temp->next;
            }

            // Clear enemy's unit list
            enemy->getUnitList()->clear();

            // Vector to store units that couldn't be inserted (Case 3)
            vector<Unit*> uninsertUnits;

            // Process each enemy unit
            for (Unit* enemyUnit : enemyUnits) {
                bool unitExists = false;

                Vehicle* enemyVehicle = dynamic_cast<Vehicle*>(enemyUnit);
                Infantry* enemyInfantry = dynamic_cast<Infantry*>(enemyUnit);

                // Case 1: Check if unit exists in current unitList
                for (Unit* existingUnit : currentUnits) {
                    if (enemyVehicle) {
                        Vehicle* existingVehicle = dynamic_cast<Vehicle*>(existingUnit);

                        if (existingVehicle && existingVehicle->getVehicleType() == enemyVehicle->getVehicleType()) {
                            // Update quantity for existing vehicle
                            existingVehicle->setQuantity(existingVehicle->getQuantity() + enemyVehicle->getQuantity());

                            // Delete the enemy unit pointer
                            delete enemyUnit;
                            enemyUnit = nullptr;

                            unitExists = true;
                            break;
                        }
                    } else {
                        Infantry* existingInfantry = dynamic_cast<Infantry*>(existingUnit);

                        if (existingInfantry && existingInfantry->getInfantryType() == enemyInfantry->getInfantryType()) {
                            // Update quantity for exiting infantry
                            existingInfantry->setQuantity(existingInfantry->getQuantity() + enemyInfantry->getQuantity());

                            // Delete the enemy unit pointer
                            delete enemyUnit;
                            enemyUnit = nullptr;

                            unitExists = true;
                            break;
                        }
                    }

                    // Case 2 + 3: Unit doesn't exist, try to insert
                    if (!unitExists && enemyUnit) {
                        bool insertSuccess = this->unitList->insert(enemyUnit);

                        if (insertSuccess) {
                            // Case 2: Successfully inserted
                            // Unit is now managed by unitList, don't delete
                        } else {
                            // Case 3: Insert failed (no capacity)
                            // Add to uninserted vector to put back into enemy
                            uninsertUnits.push_back(enemyUnit);
                        }
                    }

                    // Put uninserted units back into enemy
                    for (Unit* uninsertUnit : uninsertUnits) {
                        enemy->getUnitList()->insert(uninsertUnit);
                    }

                    // Delete enemy if it has no unit
                    if (!enemy->getUnitList()->getHead()) {
                        delete enemy;
                        enemy = nullptr;
                    }
                }
            }

            // Update index 
            recalcIndex();

        // End case found both two combinations
        } else if (foundVehicleCombination || foundInfantryCombination) {
            // Case found one of the two combinations - stalements
            bool canWin = false;

            if (foundVehicleCombination && !foundInfantryCombination && this->LF > enemy->getLF()) {
                canWin = true;
                for (Infantry* unit : bestInfantryCombination) {
                    // Delete infantry combinations
                    this->unitList->deleteInfantry(unit);
                }

                // Delete all vehicle units
            } else if (!foundVehicleCombination && foundInfantryCombination && this->EXP > enemy->getEXP()) {
                canWin = true;
                for (Vehicle* unit : bestVehicleCombination) {
                    // Delete vehicle combinations
                    this->unitList->deleteVehicle(unit);
                }

                // Delete all infantry units
            }

            if (canWin) {
                // Win - repurpose enemy's units

                vector<Unit*> currentUnits;
                UnitNode* current = this->unitList->getHead();
                while (current) {
                    currentUnits.push_back(current->unit);
                    current = current->next;
                }
 
                vector<Unit*> enemyUnits;
                UnitNode* temp = enemy->getUnitList()->getHead();
                while (temp) {
                    enemyUnits.push_back(temp->unit);
                    temp = temp->next;
                }
                enemy->getUnitList()->clear();

                vector<Unit*> uninsertUnits;

                for (Unit* enemyUnit : enemyUnits) {
                    bool unitExists = false;

                    Vehicle* enemyVehicle = dynamic_cast<Vehicle*>(enemyUnit);
                    Infantry* enemyInfantry = dynamic_cast<Infantry*>(enemyUnit);

                    // Case 1: Check if unit exists in current unitList
                    for (Unit* existingUnit : currentUnits) {
                        if (enemyVehicle) {
                            Vehicle* existingVehicle = dynamic_cast<Vehicle*>(existingUnit);

                            if (existingVehicle && existingVehicle->getVehicleType() == enemyVehicle->getVehicleType()) {
                                existingVehicle->setQuantity(existingVehicle->getQuantity() + enemyVehicle->getQuantity());

                                delete enemyUnit;
                                enemyUnit = nullptr;

                                unitExists = true;
                                break;
                            }
                        } else {
                            Infantry* existingInfantry = dynamic_cast<Infantry*>(existingUnit);

                            if (existingInfantry && existingInfantry->getInfantryType() == enemyInfantry->getInfantryType()) {
                                existingInfantry->setQuantity(existingInfantry->getQuantity() + enemyInfantry->getQuantity());

                                delete enemyUnit;
                                enemyUnit = nullptr;

                                unitExists = true;
                                break;
                            }
                        }

                        // Case 2 + 3: Unit doesn't exist, try to insert
                        if (!unitExists && enemyUnit) {
                            bool insertSuccess = this->unitList->insert(enemyUnit);

                            if (insertSuccess) {
                            } else {
                                uninsertUnits.push_back(enemyUnit);
                            }
                        }

                        for (Unit* uninsertUnit : uninsertUnits) {
                            enemy->getUnitList()->insert(uninsertUnit);
                        }

                        if (!enemy->getUnitList()->getHead()) {
                            delete enemy;
                            enemy = nullptr;
                        }
                    }
                }

                    // Update index
                    recalcIndex();

            } else {
                // Lose - each units lost 10% its weight
                UnitNode* current = this->unitList->getHead();
                while (current) {
                    int newWeight = (int)ceil(current->unit->getWeight() * 0.9);
                    current->unit->setWeight(newWeight);
                }

                // Recalculate index
                recalcIndex();
            }

        } else {
            // Case found nothing
            // Lose - each units lost 10% its weight
            UnitNode* current = this->unitList->getHead();
            while (current) {
                int newWeight = (int)ceil(current->unit->getWeight() * 0.9);
                current->unit->setWeight(newWeight);
            }

            // Recalculate index
            recalcIndex();
        }

    } else {
    // Defense case

        // Liberation Army has a boost of 1.3
        this->LF = static_cast<int>(ceil(this->LF * 1.3));
        this->EXP = static_cast<int>(ceil(this->EXP * 1.3));

        // Case 1: If both LF and EXP of Liberation Army are not lower than enemy's, Liberation Army win - do nothing
        // Case 2: If neither LF nor EXP of Liberation Army is lower than enemy's, need reinforcements - increase to the nearest Fibonacci number
        bool bothLower = (this->LF < enemy->getLF() && this->EXP < enemy->getEXP());

        // Case 3: If either LF or EXP of Liberation Army is lower than enemy's, each unit reduces 10% of its quantity
        bool oneLower = ((this->LF < enemy->getLF() || this->EXP < enemy->getEXP()) && !bothLower);
    
        while (bothLower) {
            // Case 2 - Reinforcements
            UnitNode* current = this->unitList->getHead();
            while (current) {
                int newQuantity = getNearestFibonacci(current->unit->getQuantity());
                current->unit->setQuantity(newQuantity);
                current = current->next;
            }

            // Update Indices
            recalcIndex();

            // Case 3 - Reduction of each unit's quantity
            if (oneLower) {
                UnitNode* current = this->unitList->getHead();
                while (current) {
                    int newQuantity = static_cast<int>(ceil(current->unit->getQuantity() * 0.9));
                    current->unit->setQuantity(newQuantity); 

                    if (current->unit->getQuantity() <= 1) {
                        this->unitList->remove(current);
                    }

                    current = current->next;
                }
            }
        }

        // Case 3 - Reduction of each unit's quantity
        if (oneLower) {
            UnitNode* current = this->unitList->getHead();
            while (current) {
                int newQuantity = static_cast<int>(ceil(current->unit->getQuantity() * 0.9));
                current->unit->setQuantity(newQuantity); 

                if (current->unit->getQuantity() <= 1) {
                    this->unitList->remove(current);
                }

                current = current->next;
            }
        }

        // Update index
        recalcIndex();        
    }
}


string LiberationArmy::str() const {
    return "LiberationArmy[name=" + this->name + ",LF=" + to_string(this->LF) + ",EXP=" + to_string(this->EXP) + ",unitList=" + this->unitList->str() + ",battleField=" + this->battleField->str() + "]";
}

////////////////////////////// Class ARVN //////////////////////////////
ARVN::ARVN(Unit** unitArray, int size, string name, BattleField* battleField) : Army(unitArray, size, name, battleField) {}

void ARVN::fight(Army* fight, bool defense = false) {
    if (!defense) {
        // LF and EXP still unchange

        UnitNode* current = this->unitList->getHead();
        UnitNode* prev = nullptr;

        while (current) {
            int newQuantity = (int)ceil(current->unit->getQuantity() * 0.8);
            current->unit->setQuantity(newQuantity);

            if (current->unit->getQuantity() <= 1) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    this->unitList->setHead(current->next);
                }

                Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
                Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

                if (vehicle) {
                    this->unitList->decreaseVehicleCount();
                } else if (infantry) {
                    this->unitList->decreaseInfantryCount();
                }

                delete current->unit;
                delete current;
            } else {
                prev = current;
            }

            current = current->next;
        }

        recalcIndex();
    } else {
        UnitNode* current = this->unitList->getHead();
        while (current) {
            UnitNode* next = current->next;
            delete current->unit;
            delete current;
            current = next;
        }

        this->unitList->setHead(nullptr);
        this->LF = 0;
        this->EXP = 0;
    }
}

string ARVN::str() const {
    return "ARVN[name=" + this->name + ",LF=" + to_string(this->LF) + ",EXP=" + to_string(this->EXP) + ",unitList=" + this->unitList->str() + ",battleField=" + this->battleField->str() + "]";
}

////////////////////////////// Class UnitList //////////////////////////////
bool UnitList::isSpecialNumber(int number) {
    int pow3[6] = {1, 3, 9, 27, 81, 243};
    int pow5[5] = {1, 5, 25, 125, 625};
    int pow7[4] = {1, 7, 49, 343};

    for (int i = 1; i < 64; i++) {
        int sum = 0;
        for (int j = 0; j < 6; j++) {
            if (i & (1 << j)) {
                sum += pow3[j];
            }
        }
        if (sum == number) return true;
    }

    for (int i = 1; i < 32; i++) {
        int sum = 0;
        for (int j = 0; j < 5; j++) {
            if (i & (1 << j)) {
                sum += pow5[j];
            }
        }
        if (sum == number) return true;
    }

    for (int i = 1; i < 16; i++) {
        int sum = 0;
        for (int j = 0; j < 4; j++) {
            if (i & (1 << j)) {
                sum += pow7[j];
            }
        }
        if (sum == number) return true;
    }

    return false;
}

UnitList::UnitList(int capacity) {
    this->capacity = capacity;

    this->head = nullptr;
    this->vehicleCount = 0;
    this->infantryCount = 0;
}

UnitList::UnitList(int LF, int EXP) {
    int sum = LF + EXP;
    
    if (isSpecialNumber(sum)) {
        this->capacity = 12;
    } else {
        this->capacity = 8;
    }

    this->head = nullptr;
    this->vehicleCount = 0;
    this->infantryCount = 0;
}

bool UnitList::insert(Unit *unit) {
    if (vehicleCount + infantryCount >= this->capacity) {
        return false;
    }

    Vehicle* vehicle = dynamic_cast<Vehicle*>(unit);
    Infantry* infantry = dynamic_cast<Infantry*>(unit);

    UnitNode* newNode = new UnitNode;
    newNode->unit = unit;
    newNode->next = nullptr;

    if (vehicle) {
        if (!this->head) {
            this->head = newNode;
        } else {
            UnitNode* current = this->head;
            while (current) {
                current = current->next;
            }
            current->next = newNode;
        }
    } else if (infantry) {
        newNode->next = this->head;
        this->head = newNode;
        this->infantryCount++;
    }

    return true;
}

bool UnitList::isContain(VehicleType vehicleType) {
    UnitNode* current = this->head;

    while (current) {
        Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);

        if (vehicle && vehicle->vehicleType == vehicleType) {
            return true;
        }

        current = current->next;
    }

    return false;
}

bool UnitList::isContain(InfantryType infantryType) {
    UnitNode* current = this->head;

    while (current) {
        Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

        if (infantry && infantry->infantryType == infantryType) {
            return true;
        }

        current = current->next;
    }

    return false;
}

Vehicle* UnitList::getVehicle(VehicleType vehicleType) {
    if (!this->head) return nullptr;

    UnitNode* current = this->head;
    while (current) {
        Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
        if (vehicle && vehicle->vehicleType == vehicleType) {
            return vehicle;
        }
        current = current->next;
    }
    return nullptr;
}

Infantry* UnitList::getInfantry(InfantryType infantryType) {
    if (!this->head) return nullptr;

    UnitNode* current = this->head;
    while (current) {
        Infantry* infantry = dynamic_cast<Infantry*>(current->unit);
        if (infantry && infantry->infantryType == infantryType) {
            return infantry;
        }
        current = current->next;
    }
    return nullptr;
}

string UnitList::str() const {
    string output = "UnitList[count_vehicle=" + to_string(this->vehicleCount) + ";count_infantry=" + to_string(this->infantryCount) + ";" + this->head->unit->str();

    UnitNode* current = this->head->next;
    while (current) {
        output += "," + current->unit->str();
        current = current->next;
    }    

    output += "]";
    return output;
}

UnitList::~UnitList() {
    UnitNode* current = this->head;
    while (current) {
        UnitNode* next = current->next;
        delete current->unit;
        delete current;
        current = next;
    }
}

void UnitList::clear() {
    UnitNode* current = this->head;
    while (current) {
        UnitNode* next = current->next;

        // Delete the unit object
        delete current->unit;
        current->unit = nullptr;

        // Delete the node
        delete current;

        current = next;
    }

    this->head = nullptr;
    this->capacity = 0;
    this->vehicleCount = 0;
    this->infantryCount = 0;
}

void UnitList::remove(UnitNode* node) {
    if (!node || !this->head) return;

    UnitNode* current = this->head;
    while (current) {
        if (current = node) {
            UnitNode* temp = current;
            current = temp->next;
            delete temp->unit;
            delete temp;
            return;
        }
        current = current->next;
    }

    Vehicle* vehicle = dynamic_cast<Vehicle*>(node->unit);
    Infantry* infantry = dynamic_cast<Infantry*>(node->unit);

    if (vehicle) {
        this->vehicleCount--;
    } else {
        this->infantryCount--;
    }

    delete node->unit;
    delete node;
}
UnitNode* UnitList::getHead() const {
    return this->head;
}

void UnitList::setHead(UnitNode* next) {
    this->head = next;
}

void UnitList::decreaseVehicleCount() {
    this->vehicleCount--;
}

void UnitList::decreaseInfantryCount() {
    this->infantryCount--;
}

void UnitList::deleteUnit(Unit* unit) {
    UnitNode* current = this->head;
    UnitNode* prev = nullptr;

    while (current) {
        if (current->unit == unit) {
            // Remove the node from the list
            if (prev) {
                prev->next = current->next;
            } else {
                this->head = current->next;
            }

            // Update appropriate counter
            Vehicle* vehicle = dynamic_cast<Vehicle*>(unit);
            Infantry* infantry = dynamic_cast<Infantry*>(unit);

            if (vehicle) {
                this->vehicleCount--;
            } else {
                this->infantryCount--;
            }

            delete current;

            // Unit found and deleted, exit method
            return;
        }
    
        prev = current;
        current = current->next;
    }
}

void UnitList::deleteVehicle(Vehicle* vehicle) {
    UnitNode* current = this->head;
    UnitNode* prev = nullptr;

    while (current) {
        if (current->unit == vehicle) {
            // Remove the node from the list
            if (prev) {
                prev->next = current->next;
            } else {
                this->head = current->next;
            }

            // Update vehicleCount
            this->vehicleCount--;
            delete current;
            
            // Unit found and deleted, exit method
            return;
        }
    
        prev = current;
        current = current->next;
    }
}

void UnitList::deleteInfantry(Infantry* infantry) {
    UnitNode* current = this->head;
    UnitNode* prev = nullptr;

    while (current) {
        if (current->unit == infantry) {
            // Remove the node from the list
            if (prev) {
                prev->next = current->next;
            } else {
                this->head = current->next;
            }

            // Update vehicleCount
            this->infantryCount--;
            delete current;
            
            // Unit found and deleted, exit method
            return;
        }
    
        prev = current;
        current = current->next;
    }
}

////////////////////////////// Class TerrainElement //////////////////////////////

TerrainElement::TerrainElement(Position pos) {
    this->pos = pos;
}

TerrainElement::~TerrainElement() {}

double TerrainElement::calcDistance(const Position& pos1, const Position& pos2) {
    int dr = abs(pos1.getRow() - pos2.getRow());
    int dc = abs(pos1.getCol() - pos2.getCol());
    return sqrt(dr * dr + dc * dc);
}

Position TerrainElement::getPosition() const {
    return this->pos;
}

////////////////////////////// Class Road //////////////////////////////

Road::Road(Position pos) : TerrainElement(pos) {}

Road::~Road() {}

void Road::getEffect(Army* army) {}

////////////////////////////// Class Mountain //////////////////////////////

Mountain::Mountain(Position pos) : TerrainElement(pos) {}

Mountain::~Mountain() {}

void Mountain::getEffect(Army* army) {
    UnitNode* current = army->getUnitList()->getHead();

    while (current) {
        Unit* unit = current->unit;
        double distance = calcDistance(this->pos, unit->getCurrentPosition());

        LiberationArmy* libArmy = dynamic_cast<LiberationArmy*>(army);
        ARVN* arvnArmy = dynamic_cast<ARVN*>(army);

        if (libArmy) {
            if (distance <= 2.0) {
                Vehicle* vehicle = dynamic_cast<Vehicle*>(unit);
                Infantry* infantry = dynamic_cast<Infantry*>(unit);
                if (vehicle) {
                    army->setLF(ceil(army->getLF() - vehicle->getAttackScore() * 0.1));
                } else if (infantry) {
                    army->setEXP(ceil(army->getEXP() + infantry->getAttackScore() * 0.3));
                } 
            }
        } else if (arvnArmy) {
            if (distance <= 4.0) {
                Vehicle* vehicle = dynamic_cast<Vehicle*>(unit);
                Infantry* infantry = dynamic_cast<Infantry*>(unit);
                if (vehicle) {
                    army->setLF(ceil(army->getLF() - vehicle->getAttackScore() * 0.05));
                } else if (infantry) {
                    army->setEXP(ceil(army->getEXP() + infantry->getAttackScore() * 0.2));
                } 
            }
        }

        current = current->next;
    }
}

////////////////////////////// Class River //////////////////////////////

River::River(Position pos) : TerrainElement(pos) {}

River::~River() {}

void River::getEffect(Army* army) {
    UnitNode* current = army->getUnitList()->getHead();
    while (current) {
        Unit* unit = current->unit;
        double distance = calcDistance(this->pos, unit->getCurrentPosition());

        if (distance <= 2.0) {

        }
    }
}

////////////////////////////// Class BattleField //////////////////////////////

string BattleField::str() const {
    return "BattleField[n_rows=" + to_string(this->n_rows) + ",n_cols=" + to_string(this->n_cols);
}

////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////