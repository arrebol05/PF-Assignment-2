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
    if (!defense) {
        this->LF *= 1.5;
        this->EXP *= 1.5;

        vector<Vehicle*> vehicleUnits;
        vector<Infantry*> infantryUnits;
        UnitNode* current = this->unitList->getHead();
        while (current) {
            Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
            Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

            if (vehicle) {
                vehicleUnits.push_back(vehicle);
            } else if (infantry) {
                infantryUnits.push_back(infantry);
            }

            current = current->next;
        }

        vector<Vehicle*> bestVehicleCombination;
        vector<Infantry*> bestInfantryCombination;
        int minVehicleScore = INT_MAX;
        int minInfantryScore = INT_MAX;
        bool foundVehicleCombination = false;
        bool foundInfantryCombination = false;

        int vehicleCount = vehicleUnits.size();
        for (int mask = 1; mask < (1 << vehicleCount); mask++) {
            vector<Vehicle*> combination;
            int totalScore = 0;

            for (int i = 0; i < vehicleCount; i++) {
                if (mask & (1 << i)) {
                    combination.push_back(vehicleUnits[i]);
                    totalScore += vehicleUnits[i]->getAttackScore();
                }
            }

            if (totalScore > enemy->getEXP() && totalScore < minVehicleScore) {
                minVehicleScore = totalScore;
                bestVehicleCombination = combination;
                foundVehicleCombination = true;
            }
        }

        int infantryCount = infantryUnits.size();
        for (int mask = 1; mask < (1 << infantryCount); mask++) {
            vector<Infantry*> combination;
            int totalScore = 0;

            for (int i = 0; i < infantryCount; i++) {
                if (mask & (1 << i)) {
                    combination.push_back(infantryUnits[i]);
                    totalScore += infantryUnits[i]->getAttackScore();
                }
            }

            if (totalScore > enemy->getEXP() && totalScore < minInfantryScore) {
                minInfantryScore = totalScore;
                bestInfantryCombination = combination;
                foundInfantryCombination = true;
            }
        }

        if (foundVehicleCombination && foundInfantryCombination) {
            for (Vehicle* unit : bestVehicleCombination) {
                UnitNode* current = this->unitList->getHead();
                UnitNode* prev = nullptr;

                while (current) {
                    if (current->unit == unit) {
                        if (prev) {
                            prev->next = current->next;
                        } else {
                            this->unitList->setHead(current->next);
                        }
                    }

                    this->unitList->setVehicleCount();
                    delete current;
                    break;
                }

                prev = current;
                current = current->next;
            }

            for (Infantry* unit : bestInfantryCombination) {
                UnitNode* current = this->unitList->getHead();
                UnitNode* prev = nullptr;

                while (current) {
                    if (current->unit == unit) {
                        if (prev) {
                            prev->next = current->next;
                        } else {
                            this->unitList->setHead(current->next);
                        }
                    }

                    this->unitList->setInfantryCount();
                    delete current;
                    break;
                }

                prev = current;
                current = current->next;
            }

            UnitNode* current = enemy->getUnitList()->getHead();
            while (current) {
                Unit* unitCopy = nullptr;

                Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
                Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

                if (vehicle) {
                    unitCopy = new Vehicle(vehicle->getQuantity(), vehicle->getWeight(), vehicle->getCurrentPosition(), vehicle->getVehicleType());
                } else if (infantry) {
                    unitCopy = new Infantry(infantry->getQuantity(), infantry->getWeight(), infantry->getCurrentPosition(), infantry->getInfantryType());
                }

                if (unitCopy) {
                    this->unitList->insert(unitCopy);
                }

                current = current->next;
            }
        } else if (foundVehicleCombination || foundInfantryCombination) {
            bool canWin = false;

            if (foundVehicleCombination && !foundInfantryCombination) {
                canWin = true;

                for (Vehicle* unit : bestVehicleCombination) {
                    UnitNode* current = this->unitList->getHead();
                    UnitNode* prev = nullptr;

                    while (current) {
                        if (current->unit == unit) {
                            if (prev) {
                                prev->next = current->next;
                            } else {
                                this->unitList->setHead(current->next);
                            }
                        }

                        this->unitList->setVehicleCount();
                        delete current;
                        break;
                    }

                    prev = current;
                    current = current->next;
                }
            } else if (!foundVehicleCombination && foundInfantryCombination) {
                canWin = true;

                for (Infantry* unit : bestInfantryCombination) {
                    UnitNode* current = this->unitList->getHead();
                    UnitNode* prev = nullptr;
                    
                    while (current) {
                        if (current->unit == unit) {
                            if (prev) {
                                prev->next = current->next;
                            } else {
                                this->unitList->setHead(current->next);
                            }
                        }

                        this->unitList->setInfantryCount();
                        delete current;
                        break;
                    }

                    prev = current;
                    current = current->next;
                }

                UnitNode* current = this->unitList->getHead();
                UnitNode* prev = nullptr;

                while (current) {
                    if (dynamic_cast<Vehicle*>(current->unit)) {
                        if (prev) {
                            prev->next = current->next;
                        } else {
                            this->unitList->setHead(current->next);
                        }

                        UnitNode* toDelete = current;
                        current = current->next;
                        this->unitList->setInfantryCount();
                        delete toDelete;
                    } else {
                        prev = current;
                        current = current->next;
                    }
                }
            }

            if (canWin) {
                UnitNode* current = enemy->getUnitList()->getHead();
                while (current) {
                    Unit* unitCopy = nullptr;

                    Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
                    Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

                    if (vehicle) {
                        unitCopy = new Vehicle(vehicle->getQuantity(), vehicle->getWeight(), vehicle->getCurrentPosition(), vehicle->getVehicleType());
                    } else if (infantry) {
                        unitCopy = new Infantry(infantry->getQuantity(), infantry->getWeight(), infantry->getCurrentPosition(), infantry->getInfantryType());
                    }

                    if (unitCopy) {
                        this->unitList->insert(unitCopy);
                    }

                    current = current->next;
                }
            } else {
                UnitNode* current = this->unitList->getHead();
                while (current) {
                    int newWeight = (int)ceil(current->unit->getWeight() * 0.9);
                    current->unit->setWeight(newWeight);
                }
            }
        } else {
            UnitNode* current = this->unitList->getHead();
            while (current) {
                int newWeight = (int)ceil(current->unit->getWeight() * 0.9);
                current->unit->setWeight(newWeight);
            }
        }

        this->LF = 0;
        this->EXP = 0;

        UnitNode* current = this->unitList->getHead();
        while (current) {
            Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
            Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

            if (vehicle) {
                this->LF += vehicle->getAttackScore();
            } else if (infantry) {
                this->EXP += infantry->getAttackScore();
            }

            current = current->next;
        }
    } else {
        this->LF = (int)ceil(this->LF * 1.3);
        this->EXP = (int)ceil(this->EXP * 1.3);

        if (this->LF < enemy->getLF() || this->EXP < enemy->getEXP()) {
            if ((this->LF < enemy->getLF() || this->EXP < enemy->getEXP())) {
                UnitNode* current = this->unitList->getHead();
                while (current) {
                    int newQuantity = ceil(current->unit->getQuantity() * 0.9);
                    current->unit->setQuantity(newQuantity); 

                    if (current->unit->getQuantity() <= 0) {
                        this->unitList->remove(current);
                    }

                    current = current->next;
                }

                UnitNode* current = this->unitList->getHead();
                while (current) {
                    int newWeight = (int)ceil(current->unit->getWeight() * 0.9);
                    current->unit->setWeight(newWeight);
                }
            } else if ((this->LF < enemy->getLF() && this->EXP < enemy->getEXP())) {
                UnitNode* current = this->unitList->getHead();
                while (current) {
                    int newQuantity = getNearestFibonacci(current->unit->getQuantity());
                    current->unit->setQuantity(newQuantity);
                    current = current->next;
                }

                this->LF = 0;
                this->EXP = 0;

                UnitNode* current = this->unitList->getHead();
                while (current) {
                    Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
                    Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

                    if (vehicle) {
                        this->LF += vehicle->getAttackScore();
                    } else if (infantry) {
                        this->EXP += infantry->getAttackScore();
                    }

                    current = current->next;
                }

                if ((this->LF < enemy->getLF() || this->EXP < enemy->getEXP())) {
                    UnitNode* current = this->unitList->getHead();
                    while (current) {
                        int newQuantity = ceil(current->unit->getQuantity() * 0.9);
                        current->unit->setQuantity(newQuantity); 

                        if (current->unit->getQuantity() <= 0) {
                            this->unitList->remove(current);
                        }

                        current = current->next;
                    }

                    UnitNode* current = this->unitList->getHead();
                    while (current) {
                        int newWeight = (int)ceil(current->unit->getWeight() * 0.9);
                        current->unit->setWeight(newWeight);
                    }
                }
            }
        }

        this->LF = 0;
        this->EXP = 0;

        UnitNode* current = this->unitList->getHead();
        while (current) {
            Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
            Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

            if (vehicle) {
                this->LF += vehicle->getAttackScore();
            } else if (infantry) {
                this->EXP += infantry->getAttackScore();
            }

            current = current->next;
        }
    }
}

string LiberationArmy::str() const {
    return "LiberationArmy[name=" + this->name + ",LF=" + to_string(this->LF) + ",EXP=" + to_string(this->EXP) + ",unitList=" + this->unitList->str() + ",battleField=" + this->battleField->str() + "]";
}

////////////////////////////// Class ARVN //////////////////////////////
ARVN::ARVN(Unit** unitArray, int size, string name, BattleField* battleField) : Army(unitArray, size, name, battleField) {}

void ARVN::fight(Army* fight, bool defense = false) {
    if (!defense) {
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
                    this->unitList->setVehicleCount();
                } else if (infantry) {
                    this->unitList->setInfantryCount();
                }

                delete current->unit;
                delete current;
            } else {
                prev = current;
            }

            current = current->next;
        }

        this->LF = 0;
        this->EXP = 0;

        current = this->unitList->getHead();
        while (current) {
            Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
            Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

            if (vehicle) {
                this->LF += vehicle->getAttackScore();
            } else if (infantry) {
                this->EXP += infantry->getAttackScore();
            }

            current = current->next;
        }
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
    this->countVehicle = 0;
    this->countInfantry = 0;
}

UnitList::UnitList(int LF, int EXP) {
    int sum = LF + EXP;
    
    if (isSpecialNumber(sum)) {
        this->capacity = 12;
    } else {
        this->capacity = 8;
    }

    this->head = nullptr;
    this->countVehicle = 0;
    this->countInfantry = 0;
}

bool UnitList::insert(Unit *unit) {
    if (countVehicle + countInfantry >= capacity) {
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
        this->countInfantry++;
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

string UnitList::str() const {
    string output = "UnitList[count_vehicle=" + to_string(this->countVehicle) + ";count_infantry=" + to_string(this->countInfantry) + ";" + this->head->unit->str();

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

void UnitList::remove(UnitNode* node) {
    if (!node) return;

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
}
UnitNode* UnitList::getHead() const {
    return this->head;
}

void UnitList::setHead(UnitNode* next) {
    this->head = next;
}

void UnitList::setVehicleCount() {
    this->countVehicle--;
}

void UnitList::setInfantryCount() {
    this->countInfantry--;
}

////////////////////////////// Class BattleField //////////////////////////////

string BattleField::str() const {
    return "BattleField[n_rows=" + to_string(this->n_rows) + ",n_cols=" + to_string(this->n_cols);
}

////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////