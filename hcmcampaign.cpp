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

////////////////////////////// Class LiberationArmy //////////////////////////////
LiberationArmy::LiberationArmy(Unit** unitArray, int size, string name, BattleField* battleField) : Army(unitArray, size, name, battleField) {}

void LiberationArmy::fight(Army* enemy, bool defense = false) {
    if (!defense) {
        this->LF *= 1.5;
        this->EXP *= 1.5;

        
    }
}

string LiberationArmy::str() const {
    return "LiberationArmy[name=" + this->name + ",LF=" + to_string(this->LF) + ",EXP=" + to_string(this->EXP) + ",unitList=" + this->unitList->str() + ",battleField=" + this->battleField->str() + "]";
}

////////////////////////////// Class ARVN //////////////////////////////
ARVN::ARVN(Unit** unitArray, int size, string name, BattleField* battleField) : Army(unitArray, size, name, battleField) {}

void ARVN::fight(Army* fight, bool defense = false) {

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

////////////////////////////// Class BattleField //////////////////////////////

string BattleField::str() const {
    return "BattleField[n_rows=" + to_string(this->n_rows) + ",n_cols=" + to_string(this->n_cols);
}

////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////