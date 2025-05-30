#include "hcmcampaign.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
////////////////////////////////////////////////////////////////////////

////////////////////////////// Class Unit //////////////////////////////
Unit::Unit(int quantity, int weight, const Position pos)
{
    this->quantity = quantity;
    this->weight = weight;
    this->pos = pos;
}

Unit::~Unit() {}

Position Unit::getCurrentPosition() const
{
    return this->pos;
}

int Unit::getQuantity() const
{
    return this->quantity;
}

int Unit::getWeight() const
{
    return this->weight;
}

void Unit::setQuantity(int quantity)
{
    this->quantity = quantity;
}

void Unit::setWeight(int weight)
{
    this->weight = weight;
}

////////////////////////////// Class Vehicle //////////////////////////////
Vehicle::Vehicle(int quantity, int weight, const Position pos, VehicleType vehicleType) : Unit(quantity, weight, pos)
{
    this->vehicleType = vehicleType;
}

Vehicle::~Vehicle() {}

int Vehicle::getAttackScore()
{
    int typeValue = static_cast<int>(this->vehicleType);

    return ceil((typeValue * 304 + this->quantity * this->weight) / 30.0);
}

string Vehicle::str() const
{
    string typeName[7] = {"TANK", "MORTAR", "ANTIAIRCRAFT", "ARMOREDCAR", "APC", "ARTILLERY", "TANK"};
    return "Vehicle[vehicleType=" + typeName[this->vehicleType] + ",quantity=" + to_string(this->quantity) + ",weight=" + to_string(this->weight) + ",pos=" + this->pos.str();
}

VehicleType Vehicle::getVehicleType() const
{
    return this->vehicleType;
}

////////////////////////////// Class Infantry //////////////////////////////
int Infantry::calPersonalNumber(int number)
{
    return (number) ? 1 + (number - 1) % 9 : 0;
}

Infantry::Infantry(int quantity, int weight, const Position pos, InfantryType infantryType) : Unit(quantity, weight, pos)
{
    this->infantryType = infantryType;
}

Infantry::~Infantry() {}

int Infantry::getAttackScore()
{
    int typeValue = static_cast<int>(this->infantryType);

    int score = typeValue * 56 + this->quantity * this->weight;

    if ((this->infantryType == SPECIALFORCES) && (this->weight == sqrt(this->weight) * sqrt(this->weight)))
        score += 75;

    int personalNumber = calPersonalNumber(score + 1975);
    if (personalNumber > 7)
    {
        this->quantity = ceil(quantity * 1.2);
    }
    else if (personalNumber < 3)
    {
        this->quantity *= ceil(quantity * 0.9);
    }

    return typeValue * 56 + this->quantity * this->weight;
}

string Infantry::str() const
{
    string typeName[6] = {"SNIPER", "ANTIAIRCRAFTSQUAD", "MORTARSQUAD", "ENGINEER", "SPECIALFORCES", "REGULARINFANTRY"};
    return "Infantry[infantryType=" + typeName[this->infantryType] + ",quantity=" + to_string(this->quantity) + ",weight=" + to_string(this->weight) + ",pos=" + this->pos.str();
}

InfantryType Infantry::getInfantryType() const
{
    return this->infantryType;
}

////////////////////////////// Class Army //////////////////////////////
Army::Army(Unit **unitArray, int size, string name, BattleField *battleField)
{
    this->LF = 0;
    this->EXP = 0;
    this->name = name;
    this->battleField = battleField;

    for (int i = 0; i < size; i++)
    {
        const Vehicle *vehicle = dynamic_cast<const Vehicle *>(unitArray[i]);
        const Infantry *infantry = dynamic_cast<const Infantry *>(unitArray[i]);

        if (vehicle)
        {
            this->LF += unitArray[i]->getAttackScore();
        }
        else
        {
            this->EXP += unitArray[i]->getAttackScore();
        }
    }

    if (isSpecialNumber(LF + EXP))
        this->unitList = new UnitList(12);
    else
        this->unitList = new UnitList(8);

    for (int i = 0; i < size; i++)
        this->unitList->insert(unitArray[i]);

    calScore();
}

UnitList *Army::getUnitList() const
{
    return this->unitList;
}

int Army::getLF() const
{
    return this->LF;
}

int Army::getEXP() const
{
    return this->EXP;
}

void Army::setLF(int LF)
{
    this->LF = LF;
}

void Army::setEXP(int EXP)
{
    this->EXP = EXP;
}

void Army::recalcIndex()
{
    this->LF = 0;
    this->EXP = 0;

    UnitNode *current = this->unitList->getHead();
    while (current)
    {
        Vehicle *vehicle = dynamic_cast<Vehicle *>(current->unit);
        Infantry *infantry = dynamic_cast<Infantry *>(current->unit);

        if (vehicle)
        {
            this->LF += vehicle->getAttackScore();
        }
        else
        {
            this->EXP += infantry->getAttackScore();
        }

        current = current->next;
    }
}

void Army::removeWeakUnits()
{
    UnitNode *current = this->unitList->getHead();

    while (current)
    {
        if (current->unit->getAttackScore() <= 5)
            unitList->remove(current);

        current = current->next;
    }

    recalcIndex();
}

void Army::calScore() {
    scores = {};
    UnitNode *current = this->unitList->getHead();

    while (current)
    {
        scores.push_back(current->unit->getAttackScore());

        current = current->next;
    }
}

void Army::setScore(int idx, int val) {
    scores[idx] = val;
}

int Army::getScore(int idx) {
    return scores[idx];
}

////////////////////////////// Class LiberationArmy //////////////////////////////
LiberationArmy::LiberationArmy(Unit **unitArray, int size, string name, BattleField *battleField) : Army(unitArray, size, name, battleField) {}

int LiberationArmy::getNearestFibonacci(int number)
{
    if (number <= 1)
    {
        return 1;
    }

    int fib1 = 1, fib2 = 1;
    while (fib2 < number)
    {
        int temp = fib1 + fib2;
        fib1 = fib2;
        fib2 = temp;
    }

    return fib2;
}

void LiberationArmy::fight(Army *enemy, bool defense)
{
    // Attack case
    if (!defense)
    {
        // Liberation Army got a boost
        this->LF = ceil(this->LF * 1.5);
        this->EXP = ceil(this->EXP * 1.5);

        /*
        Get vectors of vehicles & infantries for handling.
        To reduce calculation overhead, we will create the attack score vector for each list.
        */

        vector<Vehicle *> vehicleUnits;
        vector<Infantry *> infantryUnits;

        // Setup to initialize min as the sum of list vehicles/ infantries
        int minVehicleScore = 0;
        int minInfantryScore = 0;

        // Traverse from head to tail
        UnitNode *current = this->unitList->getHead();
        while (current)
        {
            Vehicle *vehicle = dynamic_cast<Vehicle *>(current->unit);
            Infantry *infantry = dynamic_cast<Infantry *>(current->unit);

            if (vehicle)
            {
                vehicleUnits.push_back(vehicle);
                minVehicleScore += scores[vehicleUnits.size() - 1];
            }
            else
            {
                infantryUnits.push_back(infantry);
                minInfantryScore += scores[vehicleUnits.size() + infantryUnits.size() - 1];
            }

            current = current->next;
        }

        // Initialization for combination
        vector<Vehicle *> bestVehicleCombination;
        vector<Infantry *> bestInfantryCombination;
        int vehicleCount = vehicleUnits.size(), infantryCount = infantryUnits.size(),
            minVehicleUsed = vehicleCount + infantryCount, minInfantryUsed = minVehicleUsed;

        bool foundVehicleCombination = minVehicleScore >= enemy->getLF(),
             foundInfantryCombination = minInfantryScore >= enemy->getEXP();

        // We only loop and get in case of there exists a combination
        if (foundVehicleCombination)
        {
            

            // Bitmask technique to get all combination
            for (int mask = 1; mask < (1 << vehicleCount); mask++)
            {
                vector<Vehicle *> combination;
                int totalScore = 0, used = 0;

                for (int i = 0; i < vehicleCount; i++)
                {
                    if (mask & (1 << i))
                    {
                        combination.push_back(vehicleUnits[i]);
                        totalScore += scores[i];
                        ++used;
                    }
                }

                if (totalScore > enemy->getEXP() && (totalScore < minVehicleScore || (totalScore == minVehicleScore &&  used < minVehicleUsed)))
                {
                    minVehicleScore = totalScore;
                    bestVehicleCombination = combination;
                    minVehicleUsed = used;
                }
            }
        }

        if (foundInfantryCombination)
        {
            for (int mask = 1; mask < (1 << infantryCount); mask++)
            {
                vector<Infantry *> combination;
                int totalScore = 0, used = 0;

                for (int i = 0; i < infantryCount; i++)
                {
                    if (mask & (1 << i))
                    {
                        combination.push_back(infantryUnits[i]);
                        totalScore += scores[i + vehicleCount];
                        ++used;
                    }
                }

                if (totalScore > enemy->getEXP() && (totalScore < minInfantryScore || (totalScore == minInfantryScore && used < minInfantryUsed)))
                {
                    minInfantryScore = totalScore;
                    bestInfantryCombination = combination;
                    minInfantryUsed = used;
                }
            }
        }

        bool win = false;

        // Condition check for fight result
        switch (foundVehicleCombination << 1 + foundInfantryCombination)
        {
        // Case found both combination: Will definitely win
        case 3:
            win = true;
            break;
        // Case found Vehicle combination: Check if win
        case 2:
            if (EXP > enemy->getEXP())
            {
                win = true;
                bestInfantryCombination = infantryUnits;
            }
        // Case found Infantry combination: Check if win
        case 1:
            if (LF > enemy->getLF())
            {
                win = true;
                bestVehicleCombination = vehicleUnits;
            }
            break;
            // Default lost, no need to do anything
        }

        if (win)
        {
            // Delete all the combinations, add enemy
            for (Vehicle *unit : bestVehicleCombination)
                this->unitList->deleteVehicle(unit);
            for (Infantry *unit : bestInfantryCombination)
                this->unitList->deleteInfantry(unit);

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

            // Pop all units from enemy into a temporary vector
            vector<Unit *> enemyUnits;
            Unit *unit = enemy->getUnitList()->pop_front();
            while (unit)
            {
                enemyUnits.push_back(unit);
                Unit *unit = enemy->getUnitList()->pop_front();
            }

            // Vector to store units that couldn't be inserted (Case 3)
            /* Some note on case 3 issue
            In UnitList implementation, we always insert vehicle at the end, and infantry at the start
            Since we extract with pop_front, we preserve the order of UnitList
            But if use insert, infantry order will messed up
            For example, we have 0, 1, 2, 3 acting as infantry in UnitList
            When pushing them with insert, they will be inserted in order 0 -> 1 -> 2 -> 3
            Therefore the final result is 3, 2, 1, 0 #BUG
            In order to resolve it, we need to seperate infantries and vehicles
            */
            vector<Unit *> leftoverVehicle, leftoverInfantry;

            // Process each enemy unit
            for (Unit *enemyUnit : enemyUnits)
            {
                bool unitExists = false;

                Vehicle *enemyVehicle = dynamic_cast<Vehicle *>(enemyUnit);
                Infantry *enemyInfantry = dynamic_cast<Infantry *>(enemyUnit);

                // Case 2 & 3: The unit does not exist. Attempting to insert
                if (!unitExists)
                {
                    if (!this->unitList->insert(enemyUnit))
                    {
                        // Insert does not success
                        if (enemyVehicle)
                            leftoverVehicle.push_back(enemyVehicle);
                        else
                            leftoverInfantry.push_back(enemyInfantry);
                    }
                }
            }

            // Resolve leftover units
            // Infantries will be inserted in reversed order
            for (auto it = leftoverInfantry.rbegin(); it != leftoverInfantry.rend(); ++it)
                enemy->getUnitList()->insert(*it);
            // Vehicle just chilling
            for (auto unit : leftoverVehicle)
                enemy->getUnitList()->insert(unit);
        }
        else
        {
            // Lose - each units lost 10% its weight
            UnitNode *current = this->unitList->getHead();
            while (current)
                current->unit->setWeight(ceil(current->unit->getWeight() * 0.9));
        }

        // Update index - This debunk the fight start effect, so no need to worry
        recalcIndex();
    }
    else
    {
        // Defense case

        // Liberation Army has a boost of 1.3
        this->LF = ceil(this->LF * 1.3);
        this->EXP = ceil(this->EXP * 1.3);

        // We loop till the fight end
        bool done = false;
        while (!done)
        {
            done = true;
            // Case 1: If both LF and EXP of Liberation Army are not lower than enemy's, Liberation Army win - do nothing
            // Case 2: If neither LF nor EXP of Liberation Army is lower than enemy's, need reinforcements - increase to the nearest Fibonacci number
            bool bothLower = (this->LF < enemy->getLF() && this->EXP < enemy->getEXP());

            // Case 3: If either LF or EXP of Liberation Army is lower than enemy's, each unit reduces 10% of its quantity
            bool oneLower = (this->LF < enemy->getLF()) ^ (this->EXP < enemy->getEXP());

            // Reduction of each unit's quantity
            if (oneLower)
            {
                UnitNode *current = this->unitList->getHead();
                while (current)
                {
                    current->unit->setQuantity(ceil(current->unit->getQuantity() * 0.9));

                    if (current->unit->getQuantity() <= 1)
                    {
                        this->unitList->remove(current);
                    }

                    current = current->next;
                }
            }

            // Call reinforcements
            if (bothLower)
            {
                UnitNode *current = this->unitList->getHead();
                while (current)
                {
                    current->unit->setQuantity(getNearestFibonacci(current->unit->getQuantity()));
                    current = current->next;
                }
                done = false;
            }

            // Update index
            recalcIndex();
        }
    }
}

string LiberationArmy::str() const
{
    return "LiberationArmy[name=" + this->name + ",LF=" + to_string(this->LF) + ",EXP=" + to_string(this->EXP) + ",unitList=" + this->unitList->str() + ",battleField=" + this->battleField->str() + "]";
}

////////////////////////////// Class ARVN //////////////////////////////
ARVN::ARVN(Unit **unitArray, int size, string name, BattleField *battleField) : Army(unitArray, size, name, battleField) {}

void ARVN::fight(Army *fight, bool defense)
{
    // Attack
    if (!defense)
    {
        // LF and EXP unchange

        UnitNode *current = this->unitList->getHead();
        while (current)
        {
            current->unit->setQuantity(ceil(current->unit->getQuantity() * 0.9));

            if (current->unit->getQuantity() <= 1)
            {
                this->unitList->remove(current);
            }

            current = current->next;
        }
    }
    // Defense
    else
    {
        UnitNode *current = this->unitList->getHead();
        while (current)
        {
            current->unit->setWeight(ceil(current->unit->getWeight() * 0.8));

            current = current->next;
        }
    }

    // Recalculate index
    recalcIndex();
}

string ARVN::str() const
{
    return "ARVN[name=" + this->name + ",LF=" + to_string(this->LF) + ",EXP=" + to_string(this->EXP) + ",unitList=" + this->unitList->str() + ",battleField=" + this->battleField->str() + "]";
}

////////////////////////////// Class UnitList //////////////////////////////
bool Army::isSpecialNumber(int number)
{
    vector<int> power = {
        // Sum of powers of 3
        1, 3, 4, 9, 10, 12, 13, 27, 28, 30, 31, 36, 37, 39, 40,
        81, 82, 84, 85, 90, 91, 93, 94, 108, 109, 111, 112, 117, 118, 120, 121,
        243, 244, 246, 247, 252, 253, 255, 256, 270, 271, 273, 274, 279, 280, 282, 283,
        324, 325, 327, 328, 333, 334, 336, 337, 351, 352, 354, 355, 360, 361, 363, 364,
        729, 730, 732, 733, 738, 739, 741, 742, 756, 757, 759, 760, 765, 766, 768, 769,
        810, 811, 813, 814, 819, 820, 822, 823, 837, 838, 840, 841, 846, 847, 849, 850,
        972, 973, 975, 976, 981, 982, 984, 985, 999, 1000,

        // Sum of powers of 5
        /* 1, */ 5, 6, 25, 26, /* 30, */ /* 31, */ 125, 126, 130, 131, 150, 151, 155, 156,
        625, 626, 630, 631, 650, 651, 655, 656, 750, 751, 755, /* 756, */ 775, 776, 780, 781,

        // Sum of powers of 7
        /* 1, */ 7, 8, 49, 50, 56, 57, 343, 344, 350, /* 351, */ 392, 393, 399, 400};

    for (auto num : power)
        if (num == number)
            return true;

    return false;
}

UnitList::UnitList(int capacity)
{
    this->capacity = capacity;

    this->head = nullptr;
    this->vehicleCount = 0;
    this->infantryCount = 0;
}

bool UnitList::insert(Unit *unit)
{
    if (vehicleCount + infantryCount >= this->capacity)
    {
        return false;
    }

    Vehicle *vehicle = dynamic_cast<Vehicle *>(unit);
    Infantry *infantry = dynamic_cast<Infantry *>(unit);

    UnitNode *newNode = new UnitNode();
    newNode->unit = unit;
    newNode->next = nullptr;

    if (vehicle)
    {
        Unit* exist = getVehicle(vehicle->getVehicleType());
        if (exist) {
            exist->setQuantity(exist->getQuantity() + vehicle->getQuantity());
            exist->setWeight(max(exist->getWeight(), vehicle->getWeight()));
        }
        else {
            if (!this->head)
            {
                this->head = newNode;
            }
            else
            {
                UnitNode *current = this->head;
                while (current->next)
                    current = current->next;
                current->next = newNode;
            }
            vehicleCount++;
        }
    }
    else if (infantry)
    {
        Unit *exist = getInfantry(infantry->getInfantryType());
        if (exist) {
            exist->setQuantity(exist->getQuantity() + infantry->getQuantity());
            exist->setWeight(max(exist->getWeight(), infantry->getWeight()));
        }
        else {
            newNode->next = this->head;
            this->head = newNode;
            this->infantryCount++;
        }
    }

    return true;
}

bool UnitList::isContain(VehicleType vehicleType)
{
    UnitNode *current = this->head;

    while (current)
    {
        Vehicle *vehicle = dynamic_cast<Vehicle *>(current->unit);

        if (vehicle && vehicle->vehicleType == vehicleType)
        {
            return true;
        }

        current = current->next;
    }

    return false;
}

bool UnitList::isContain(InfantryType infantryType)
{
    UnitNode *current = this->head;

    while (current)
    {
        Infantry *infantry = dynamic_cast<Infantry *>(current->unit);

        if (infantry && infantry->infantryType == infantryType)
        {
            return true;
        }

        current = current->next;
    }

    return false;
}

Vehicle *UnitList::getVehicle(VehicleType vehicleType)
{
    if (!this->head)
        return nullptr;

    UnitNode *current = this->head;
    while (current)
    {
        Vehicle *vehicle = dynamic_cast<Vehicle *>(current->unit);
        if (vehicle && vehicle->vehicleType == vehicleType)
        {
            return vehicle;
        }
        current = current->next;
    }
    return nullptr;
}

Infantry *UnitList::getInfantry(InfantryType infantryType)
{
    if (!this->head)
        return nullptr;

    UnitNode *current = this->head;
    while (current)
    {
        Infantry *infantry = dynamic_cast<Infantry *>(current->unit);
        if (infantry && infantry->infantryType == infantryType)
        {
            return infantry;
        }
        current = current->next;
    }
    return nullptr;
}

string UnitList::str() const
{
    string output = "UnitList[count_vehicle=" + to_string(this->vehicleCount) + ";count_infantry=" + to_string(this->infantryCount) + ";" + this->head->unit->str();

    UnitNode *current = this->head->next;
    while (current)
    {
        output += "," + current->unit->str();
        current = current->next;
    }

    output += "]";
    return output;
}

UnitList::~UnitList()
{
    UnitNode *current = this->head;
    while (current)
    {
        UnitNode *next = current->next;
        delete current;
        current = next;
    }
}

void UnitList::remove(UnitNode *node)
{
    if (!node || !this->head)
        return;

    UnitNode *current = this->head;
    while (current)
    {
        if (current = node)
        {
            UnitNode *temp = current;
            current = temp->next;
            delete temp->unit;
            delete temp;
            return;
        }
        current = current->next;
    }

    Vehicle *vehicle = dynamic_cast<Vehicle *>(node->unit);
    Infantry *infantry = dynamic_cast<Infantry *>(node->unit);

    if (vehicle)
    {
        this->vehicleCount--;
    }
    else
    {
        this->infantryCount--;
    }

    delete node->unit;
    delete node;
}

Unit *UnitList::pop_front()
{
    if (this->head == nullptr)
        return nullptr;
    UnitNode *head = this->head;
    this->head = head->next;
    Unit *returnUnit = head->unit;
    delete head;
    if (dynamic_cast<Vehicle *>(returnUnit))
        vehicleCount--;
    else
        infantryCount--;
    return returnUnit;
}

UnitNode *UnitList::getHead() const
{
    return this->head;
}

void UnitList::deleteVehicle(Vehicle *vehicle)
{
    UnitNode *current = this->head;
    UnitNode *prev = nullptr;

    while (current)
    {
        if (current->unit == vehicle)
        {
            // Remove the node from the list
            if (prev)
                prev->next = current->next;
            else
                this->head = current->next;

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

void UnitList::deleteInfantry(Infantry *infantry)
{
    UnitNode *current = this->head;
    UnitNode *prev = nullptr;

    while (current)
    {
        if (current->unit == infantry)
        {
            // Remove the node from the list
            if (prev)
            {
                prev->next = current->next;
            }
            else
            {
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

////////////////////////////// Class Position //////////////////////////////
Position::Position(int r, int c)
{
    this->r = r;
    this->c = c;
}

Position::Position(const string &str_pos)
{
    sscanf(str_pos.c_str(), "(%d,%d)", &this->r, &this->c);
}

int Position::getRow() const
{
    return this->r;
}

int Position::getCol() const
{
    return this->c;
}

void Position::setRow(int r)
{
    this->r = r;
}

void Position::setCol(int c)
{
    this->c = c;
}

string Position::str() const
{
    return "(" + to_string(this->r) + "," + to_string(this->c) + ")";
}

////////////////////////////// Class TerrainElement //////////////////////////////

TerrainElement::TerrainElement(Position pos)
{
    this->pos = pos;
}

TerrainElement::~TerrainElement() {}

double TerrainElement::calcDistance(const Position &pos1, const Position &pos2)
{
    int dr = abs(pos1.getRow() - pos2.getRow());
    int dc = abs(pos1.getCol() - pos2.getCol());
    return sqrt(dr * dr + dc * dc);
}

Position TerrainElement::getPosition() const
{
    return this->pos;
}

////////////////////////////// Class Road //////////////////////////////

Road::Road(Position pos) : TerrainElement(pos) {}

Road::~Road() {}

void Road::getEffect(Army *army) {}

////////////////////////////// Class Mountain //////////////////////////////

Mountain::Mountain(Position pos) : TerrainElement(pos) {}

Mountain::~Mountain() {}

void Mountain::getEffect(Army *army)
{
    LiberationArmy *libArmy = dynamic_cast<LiberationArmy *>(army);

    UnitNode *current = army->getUnitList()->getHead();
    while (current)
    {
        Unit *unit = current->unit;
        Vehicle *vehicle = dynamic_cast<Vehicle *>(unit);
        double distance = calcDistance(this->pos, unit->getCurrentPosition());

        if (libArmy)
        {
            if (distance <= 2.0)
            {
                if (vehicle)
                    army->setLF(ceil(army->getLF() - unit->getAttackScore() * 0.1));
                else
                    army->setEXP(ceil(army->getEXP() + unit->getAttackScore() * 0.3));
            }
        }
        else
        {
            if (distance <= 4.0)
            {
                if (vehicle)
                    army->setLF(ceil(army->getLF() - unit->getAttackScore() * 0.05));
                else
                    army->setEXP(ceil(army->getEXP() + unit->getAttackScore() * 0.2));
            }
        }

        current = current->next;
    }
}

////////////////////////////// Class River //////////////////////////////

River::River(Position pos) : TerrainElement(pos) {}

River::~River() {}

void River::getEffect(Army *army)
{
    int idx = 0;
    UnitNode *current = army->getUnitList()->getHead();
    while (current)
    {
        Unit *unit = current->unit;
        Infantry *infantry = dynamic_cast<Infantry *>(unit);
        double distance = calcDistance(this->pos, unit->getCurrentPosition());

        if (distance <= 2.0 && infantry)
            army->setScore(idx, ceil(army->getScore(idx) * 0.9));

        current = current->next;
        idx++;
    }
}

////////////////////////////// Class Urban //////////////////////////////

Urban::Urban(Position pos) : TerrainElement(pos) {}

Urban::~Urban() {}

void Urban::getEffect(Army *army)
{
    LiberationArmy *libArmy = dynamic_cast<LiberationArmy *>(army);

    int idx = 0;
    UnitNode *current = army->getUnitList()->getHead();
    while (current)
    {
        Unit *unit = current->unit;
        Vehicle *vehicle = dynamic_cast<Vehicle *>(unit);
        Infantry *infantry = dynamic_cast<Infantry *>(unit);
        double distance = calcDistance(this->pos, unit->getCurrentPosition());

        if (libArmy)
        {
            if (vehicle && vehicle->getVehicleType() == ARTILLERY && distance <= 2.0)
                army->setScore(idx, ceil(army->getScore(idx) * 0.5));
            if (infantry && (infantry->getInfantryType() == SPECIALFORCES || infantry->getInfantryType() == REGULARINFANTRY) && distance <= 5.0)
                army->setScore(idx, ceil(army->getScore(idx) + 2 * unit->getAttackScore() / distance));
        }
        else
        {
            if (infantry && infantry->getInfantryType() == REGULARINFANTRY && distance <= 5.0)
                army->setScore(idx, ceil(army->getScore(idx) + 3 * unit->getAttackScore() / (distance * 2)));
        }

        current = current->next;
        idx++;
    }
}

////////////////////////////// Class Fortification //////////////////////////////

Fortification::Fortification(Position pos) : TerrainElement(pos) {}

Fortification::~Fortification() {}

void Fortification::getEffect(Army *army)
{
    LiberationArmy *libArmy = dynamic_cast<LiberationArmy *>(army);

    int idx = 0;
    UnitNode *current = army->getUnitList()->getHead();
    while (current)
    {
        Unit *unit = current->unit;
        double distance = calcDistance(this->pos, unit->getCurrentPosition());

        if (libArmy)
        {
            if (distance <= 2.0)
                army->setScore(idx, ceil(army->getScore(idx) * 0.8));
        }
        else
        {
            if (distance <= 2.0)
                army->setScore(idx, ceil(army->getScore(idx) * 1.2));
        }

        current = current->next;
        idx++;
    }
}

////////////////////////////// Class SpecialZone //////////////////////////////

SpecialZone::SpecialZone(Position pos) : TerrainElement(pos) {}

SpecialZone::~SpecialZone() {}

void SpecialZone::getEffect(Army* army) {
    int idx = 0;
    UnitNode *current = army->getUnitList()->getHead();
    while (current) {
        Unit* unit = current->unit;
        double distance = calcDistance(this->pos, current->unit->getCurrentPosition());

        if (distance <= 1.0)
            army->setScore(idx, 0);

        current = current->next;
        idx++;
    }
}

////////////////////////////// Class BattleField //////////////////////////////

BattleField::BattleField(int n_rows, int n_cols, vector<Position *> arrayForest,
vector<Position *> arrayRiver, vector<Position *> arrayFortification,
vector<Position *> arrayUrban, vector<Position *> arraySpecialZone) {
    this->n_rows = n_rows;
    this->n_cols = n_cols;

    // Initialize terrain pointer and set Road elements
    this->terrain = new TerrainElement**[this->n_rows];
    for (int i = 0; i < this->n_rows; i++) {
        this->terrain[i] = new TerrainElement*[this->n_cols];
        for (int j = 0; j < this->n_cols; j++) {
            this->terrain[i][j] = new Road(Position(i,j));
        }
    }
}

BattleField::~BattleField() {
    for (int i = 0; i < this->n_rows; i++) {
        for (int j = 0; j < this->n_cols; j++) {
            delete this->terrain[i][j];
        }
        delete[] this->terrain[i];
    }
    delete[] this->terrain;
}

TerrainElement* BattleField::getElement(int row, int col) const {
    if (row >= 0 && row < this->n_rows && col >= 0 && col < this->n_cols) {
        return this->terrain[row][col];
    }

    return nullptr;
}

void BattleField::terrainEffect(Army* army) {
    for (int i = 0; i < this->n_rows; i++) {
        for (int j = 0; j < this->n_cols; j++) {
            this->terrain[i][j]->getEffect(army);
        }
    }
}

int BattleField::getRow() const {
    return this->n_rows;
}

int BattleField::getCol() const {
    return this->n_cols;
}

string BattleField::str() const {
    return "BattleField[n_rows=" + to_string(this->n_rows) + ",n_cols=" + to_string(this->n_cols);
}

////////////////////////////// Class Configuration //////////////////////////////

vector<Position*> Configuration::parsePositionArray(const string &str) {
    vector<Position*> positions;
    string content = str;

    // Remove outer brackets
    if (str.front() == '[' && str.back() == ']') {
        content = str.substr(1, str.length() - 2);
    }

    if (content.empty()) return positions;

    // Find all positions
    while (true) {
        int start = str.find('(', start);
        if (start == string::npos) break;

        int end = str.find(')', start);
        if (end == string::npos) break;

        string str = str.substr(start, end - start + 1);
        arrayFortification.push_back(new Position(str));

        start = end + 1;
    }

    return positions;
}

vector<string> Configuration::splitString(const string &str) {
    vector<string> units;
    if (str.empty()) return units;
    
    int pos = 0;
    while (pos < str.length()) {
        // Skip whitespace and commas
        while(pos < str.length() && (str[pos] == ' ' || str[pos] == ',' || str[pos] == '\n' || str[pos] == '\t')) {
            pos++;
        }

        if (pos >= str.length()) break;

        int unitstart = pos;

        // Find the opening string
        int start = str.find('(', pos);
        if (start == string::npos) break;

        // Find the matching closing one
        int count = 1;
        int end = start + 1;

        while (end < str.length() && count > 0) {
            if (str[end] == '(') {
                count++;
            } else if (str[end] == ')') {
                count--;
            }

            end++;
        }

        if (count == 0) {
            string unitstr = str.substr(unitstart, end - start + 1);
            units.push_back(unitstr);
        }

        pos = end;
    }

    return units;
}

vector<string> Configuration::splitParameters(const string &str) {
    vector<string> result;

    // Find the parameter section
    int start = str.find('(');
    int end = str.find(')');

    if (start == string::npos || end == string::npos) {
        return result;
    }

    string params = str.substr(start + 1, end - start - 1);

    int pstart = 0;
    int count = 0;

    for (int i = 0; i < params.length(); i++) {
        if (params[i] == '(') {
            count++;
        } else if (params[i] == ')') {
            count--;
        } else if (params[i] == ',' && count == 0) {
            // Found a separator at the top level
            string param = params.substr(start, i - start);

            // Trim whitespace
            param.erase(0, param.find_first_not_of(" \t\n"));
            param.erase(param.find_last_not_of(" \t\n") + 1);

            result.push_back(param);
            start =  i + 1;
        }
    }

    // Add the last parameter
    if (start < params.length()) {
        string param = params.substr(start, params.length() - 1 - start);

        param.erase(0, param.find_first_not_of(" \t\n"));
        param.erase(param.find_last_not_of(" \t\n") + 1);

        result.push_back(param);
    }

    return result;
}

Unit* Configuration::createUnit(const string &str) {
    int start = str.find('(');
    if (start == string::npos) return nullptr;

    string unitName = str.substr(0, start);
    // Trim whitespace
    unitName.erase(0, unitName.find_first_not_of(" \t\\n"));
    unitName.erase(unitName.find_last_not_of(" \t\n") + 1);

    // Get parameters
    vector<string> params = splitParameters(str);
    if (params.size() < 4) return nullptr;

    int quantity = stoi(params[0]);
    int weight = stoi(params[1]);
    Position pos(params[2]);
    int armyBelong = stoi(params[3]);

    // Create appropriate unit based on name
    // Vehicle
    if (unitName == "TANK") {
        return new Vehicle(quantity, weight, pos, TANK);
    }
    else if (unitName == "ARTILLERY") {
        return new Vehicle(quantity, weight, pos, ARTILLERY);
    }
    else if (unitName == "ARMOREDCAR") {
        return new Vehicle(quantity, weight, pos, ARMOREDCAR);
    }
    else if (unitName == "APC") {
        return new Vehicle(quantity, weight, pos, APC);
    }
    else if (unitName == "TRUCK") {
        return new Vehicle(quantity, weight, pos, TRUCK);
    }
    else if (unitName == "MORTAR") {
        return new Vehicle(quantity, weight, pos, MORTAR);
    }
    else if (unitName == "ANTIAIRCRAFT") {
        return new Vehicle(quantity, weight, pos, ANTIAIRCRAFT);
    }

    // Infantry
    else if (unitName == "SNIPER") {
        return new Infantry(quantity, weight, pos, SNIPER);
    }
    else if (unitName == "ANTIAIRCRAFTSQUAD") {
        return new Infantry(quantity, weight, pos, ANTIAIRCRAFTSQUAD);
    }
    else if (unitName == "MORTARSQUAD") {
        return new Infantry(quantity, weight, pos, MORTARSQUAD);
    }
    else if (unitName == "ENGINEER") {
        return new Infantry(quantity, weight, pos, ENGINEER);
    }
    else if (unitName == "SPECIALFORCES") {
        return new Infantry(quantity, weight, pos, SPECIALFORCES);
    }
    else if (unitName == "REGULARINFANTRY") {
        return new Infantry(quantity, weight, pos, REGULARINFANTRY);
    }
    else return nullptr; // Unknown unit type
}

Configuration::Configuration(const string &filepath = nullptr) {
    this->liberationUnitCount = 0;
    this->arvnUnitCount = 0;
    this->eventCode = 0;

    this->liberationUnits = nullptr;
    this->arvnUnits = nullptr;

    string line; 

    ifstream file(filepath);
    if (!file.is_open()) {
        throw runtime_error("Cannot open configuration file: " + filepath);
    }
    
    while (getline(file, line)) {
        // Find position of equal
        int index = line.find("=");
        
        string key = line.substr(0, index);
        string value = line.substr(index + 1);

        if (key == "NUM_ROWS=") {
            this->num_rows = stoi(value);
        } 
        else if (key == "NUM_COLS=") {
            this->num_cols = stoi(value);
        } 

        else if (key == "ARRAY_FOREST=") {
            this->arrayForest = parsePositionArray(value);
        } 
        else if (key == "ARRAY_RIVER=") {
            this->arrayRiver = parsePositionArray(value);
        } 
        else if (key == "ARRAY_FORTIFICATION=") {
            this->arrayFortification = parsePositionArray(value);
        } 
        else if (key == "ARRAY_URBAN") {
            this->arrayUrban = parsePositionArray(value);
        } 
        else if (key == "ARRAY_SPECIAL_ZONE=") {
            this->arraySpecialZone = parsePositionArray(value);
        } 
        
        else if (key == "UNIT_LIST") {
            // Handle unit list
            string liststr = value;

            // Remove outer brackets
            if (liststr.front() == '[' && liststr.back() == ']') {
                liststr = liststr.substr(1, liststr.length() - 2);
            }

            if (liststr.empty()) return;

            // Split into individual unit strings
            vector<string> unitStr = splitString(liststr);

            // Count units for each army
            for (const string &unitstr : unitStr) {
                vector<string> params = splitParameters(unitstr);
                if (params.size() >= 4) {
                    int armyBelong = stoi(params[3]);

                    if (armyBelong == 0) {
                        this->liberationUnitCount++;
                    } else if (armyBelong == 1) {
                        this->arvnUnitCount++;
                    }
                }
            }

            // Allocate arrays
            if (this->liberationUnitCount > 0) {
                this->liberationUnits = new Unit*[this->liberationUnitCount];
            }

            if (this->arvnUnitCount > 0) {
                this->arvnUnits = new Unit*[this->arvnUnitCount];
            }

            // Create and assign units
            int libIndex = 0;
            int arvnIndex = 0;

            for (const string & unitstr : unitStr) {
                Unit* unit = createUnit(unitstr);
                if (unit == nullptr) continue;

                vector<string> params = splitParameters(unitstr);
                if (params.size() >= 4) {
                    int armyBelong = stoi(params[3]);
                    if (armyBelong == 0 && libIndex < this->liberationUnitCount) {
                        this->liberationUnits[libIndex++] = unit;
                    } else if (armyBelong == 1 && arvnIndex < this->arvnUnitCount) {
                        this->arvnUnits[arvnIndex++] = unit;
                    }
                }
            }
        } else if (key == "EVENT_CODE") {
            int code = stoi(value);

            // Handle event code
            if (code < 0) {
                this->eventCode = 0;
            } else if (code > 99) {
                this->eventCode = code % 100;
            } else {
                this->eventCode = code;
            }
        }

        file.close();
    } 
}

Configuration::~Configuration() {
    // Delete position arrays
    for (Position* pos : this->arrayForest) {
        delete pos;
    }

    for (Position* pos : this->arrayRiver) {
        delete pos;
    }

    for (Position* pos: this->arrayFortification) {
        delete pos;
    }

    for (Position* pos : this->arrayUrban) {
        delete pos;
    }

    for (Position* pos : this->arraySpecialZone) {
        delete pos;
    }

    // Delete Unit & Unit array
    for (int i = 0; i < liberationUnitCount; ++i)
        delete liberationUnits[i];
    delete[] this->liberationUnits;
    this->liberationUnitCount = 0;

    for (int i = 0; i < arvnUnitCount; ++i)
        delete arvnUnits[i];
    delete[] this->arvnUnits;
    this->arvnUnitCount = 0;
}

string Configuration::str() const {
    string result = "Configuration[\n";
    
    // num_rows
    result += "num_rows=" + to_string(this->num_rows) + "\n";
    
    // num_cols
    result += "num_cols=" + to_string(this->num_cols) + "\n";
    
    // arrayForest
    result += "arrayForest=[";
    for (size_t i = 0; i < this->arrayForest.size(); i++) {
        if (i > 0) result += ",";
        result += this->arrayForest[i]->str();
    }
    result += "]\n";
    
    // arrayRiver
    result += "arrayRiver=[";
    for (size_t i = 0; i < this->arrayRiver.size(); i++) {
        if (i > 0) result += ",";
        result += this->arrayRiver[i]->str();
    }
    result += "]\n";
    
    // arrayFortification
    result += "arrayFortification=[";
    for (size_t i = 0; i < this->arrayFortification.size(); i++) {
        if (i > 0) result += ",";
        result += this->arrayFortification[i]->str();
    }
    result += "]\n";
    
    // arrayUrban
    result += "arrayUrban=[";
    for (size_t i = 0; i < this->arrayUrban.size(); i++) {
        if (i > 0) result += ",";
        result += this->arrayUrban[i]->str();
    }
    result += "]\n";
    
    // arraySpecialZone
    result += "arraySpecialZone=[";
    for (size_t i = 0; i < this->arraySpecialZone.size(); i++) {
        if (i > 0) result += ",";
        result += this->arraySpecialZone[i]->str();
    }
    result += "]\n";
    
    // liberationUnits
    result += "liberationUnits=[";
    for (int i = 0; i < this->liberationUnitCount; i++) {
        if (i > 0) result += ",";
        if (this->liberationUnits[i] != nullptr) {
            result += this->liberationUnits[i]->str();
        }
    }
    result += "]\n";
    
    // arvnUnits
    result += "arvnUnits=[";
    for (int i = 0; i < this->arvnUnitCount; i++) {
        if (i > 0) result += ",";
        if (this->arvnUnits[i] != nullptr) {
            result += this->arvnUnits[i]->str();
        }
    }
    result += "]\n";
    
    // liberationUnitCount
    result += "liberationUnitCount=" + to_string(this->liberationUnitCount) + "\n";
    
    // arvnUnitCount
    result += "arvnUnitCount=" + to_string(this->arvnUnitCount) + "\n";
    
    // eventCode
    result += "eventCode=" + to_string(this->eventCode) + "\n";
    
    result += "]";
    
    return result;
}

////////////////////////////// Class HCMCampaign //////////////////////////////

HCMCampaign::HCMCampaign(const string &config_file_path) {
    this->config = new Configuration(config_file_path);
    this->battleField = new BattleField(
        this->config->num_rows, config->num_cols,
        this->config->arrayForest,
        this->config->arrayRiver,
        this->config->arrayFortification,
        this->config->arrayUrban,
        this->config->arraySpecialZone
    );

    this->liberationArmy = new LiberationArmy(
        this->config->liberationUnits,
        this->config->liberationUnitCount,
        "Liberation",
        this->battleField
    );

    this->arvn = new ARVN(
        this->config->arvnUnits,
        this->config->arvnUnitCount,
        "ARVN",
        this->battleField
    );
}

void HCMCampaign::run() {
    // Apply terrain effects
    battleField->terrainEffect(liberationArmy);
    battleField->terrainEffect(arvn);

    // Execute battle based on event code
    if (config->eventCode < 75) {
        // Liberation Army attacks
        liberationArmy->fight(arvn, false);
        arvn->fight(liberationArmy, true);
        
    } else {
        // ARVN attacks
        arvn->fight(liberationArmy, false);
        liberationArmy->fight(arvn, true);
        // Liberation Army counterattacks
        liberationArmy->fight(arvn, false);
        arvn->fight(liberationArmy, true);
    }

    // Remove weak units from both enemies
    liberationArmy->removeWeakUnits();
    arvn->removeWeakUnits();
}

string HCMCampaign::printResult() {
    return "LIBERATIONARMY[LF=" + to_string(this->liberationArmy->getLF()) + ",EXP=" + to_string(this->liberationArmy->getEXP()) 
            + "]-ARVN[LF=" + to_string(this->arvn->getLF()) + ",EXP=" + to_string(this->arvn->getEXP()) + "]";
}

////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////