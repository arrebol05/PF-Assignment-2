/*
 * Ho Chi Minh City University of Technology
 * Faculty of Computer Science and Engineering
 * Initial code for Assignment 2
 * Programming Fundamentals Spring 2025
 * Date: 02.02.2025
 */

// The library here is concretely set, students are not allowed to include any other libraries.
#ifndef _H_HCM_CAMPAIGN_H_
#define _H_HCM_CAMPAIGN_H_

#include "main.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

// Forward declaration
class Unit;
class UnitList;
class Army;
class TerrainElement;

class Vehicle;
class Infantry;

class LiberationArmy;
class ARVN;

class Position;

class Road;
class Mountain;
class River;
class Urban;
class Fortification;
class SpecialZone;

class BattleField;

class HCMCampaign;
class Configuration;

enum VehicleType
{
    TRUCK,
    MORTAR,
    ANTIAIRCRAFT,
    ARMOREDCAR,
    APC,
    ARTILLERY,
    TANK
};
enum InfantryType
{
    SNIPER,
    ANTIAIRCRAFTSQUAD,
    MORTARSQUAD,
    ENGINEER,
    SPECIALFORCES,
    REGULARINFANTRY
};

class Army
{
    friend class UnitList;

protected:
    int LF, EXP;
    string name;
    UnitList *unitList;
    BattleField *battleField;

    // Helper
    vector<int> scores;

public:
    Army(Unit **unitArray, int size, string name, BattleField *battleField);
    virtual void fight(Army *enemy, bool defense = false) = 0;
    virtual string str() const = 0;

    // TODO: Helper
    UnitList *getUnitList() const;
    int getLF() const;
    int getEXP() const;
    void setLF(int LF);
    void setEXP(int EXP);
    void recalcIndex();
    void removeWeakUnits();
    void calScore();
    int getScore(int);
    void setScore(int, int);
    bool isSpecialNumber(int);
};

// TODO: Class LiberationArmy
class LiberationArmy : public Army
{
    friend class UnitList;

private:
    int getNearestFibonacci(int number);

public:
    LiberationArmy(Unit **unitArray, int size, string name, BattleField *battleField);
    void fight(Army *enemy, bool defense = false) override;
    string str() const override;
};

// TODO: Class ARVN
class ARVN : public Army
{
    friend class UnitList;

public:
    ARVN(Unit **unitArray, int size, string name, BattleField *BattleField);
    void fight(Army *enemy, bool defense = false) override;
    string str() const override;
};

class Position
{
private:
    int r, c;

public:
    Position(int r = 0, int c = 0);
    Position(const string &str_pos); // Example: str_pos = "(1,15)"
    int getRow() const;
    int getCol() const;
    void setRow(int r);
    void setCol(int c);
    string str() const; // Example: returns "(1,15)"
};

class Unit
{
protected:
    int quantity, weight;
    Position pos;

public:
    Unit(int quantity, int weight, Position pos);
    virtual ~Unit();
    virtual int getAttackScore() = 0;
    Position getCurrentPosition() const;
    virtual string str() const = 0;

    // TODO: Helper
    int getQuantity() const;
    int getWeight() const;
    void setQuantity(int quantity);
    void setWeight(int weight);
};

// TODO: Class Vehicle
class Vehicle : public Unit
{
    friend class UnitList;

private:
    VehicleType vehicleType;

public:
    Vehicle(int quantity, int weight, const Position pos, VehicleType vehicleType);
    ~Vehicle();
    int getAttackScore() override;
    string str() const override;

    // TODO: Helper
    VehicleType getVehicleType() const;
};

// TODO: Class Infantry
class Infantry : public Unit
{
    friend class UnitList;

private:
    InfantryType infantryType;
    int calPersonalNumber(int number);

public:
    Infantry(int quantity, int weight, const Position pos, InfantryType infantryType);
    ~Infantry();
    int getAttackScore() override;
    string str() const override;

    // TODO: Helper
    InfantryType getInfantryType() const;
};

// TODO: Struct UnitNode
struct UnitNode
{
    Unit *unit;
    UnitNode *next;
};

class UnitList
{
private:
    int capacity;
    // TODO

    UnitNode *head;
    int vehicleCount;
    int infantryCount;

public:
    UnitList(int capacity);
    bool insert(Unit *unit);                   // return true if insert successfully
    bool isContain(VehicleType vehicleType);   // return true if it exists
    bool isContain(InfantryType infantryType); // return true if it exists
    string str() const;

    // TODO
    ~UnitList();
    void remove(UnitNode *node);
    Unit *pop_front();

    Vehicle *getVehicle(VehicleType vehicleType);
    Infantry *getInfantry(InfantryType infantryType);

    UnitNode *getHead() const;

    void deleteVehicle(Vehicle *vehicle);
    void deleteInfantry(Infantry *infantry);
};

class TerrainElement
{
private:
    Position pos;

public:
    TerrainElement(Position pos);
    ~TerrainElement();
    virtual void getEffect(Army *army) = 0;

    // TODO: Helper method
    double calcDistance(const Position &pos1, const Position &pos2);
    Position getPosition() const;
};

// TODO: Class Road
class Road : public TerrainElement
{
private:
    Position pos;

public:
    Road(Position pos);
    ~Road();
    void getEffect(Army *army) override;
};

// TODO: Class Mountain
class Mountain : public TerrainElement
{
private:
    Position pos;

public:
    Mountain(Position pos);
    ~Mountain();
    void getEffect(Army *army) override;
};

// TODO: Class River
class River : public TerrainElement
{
private:
    Position pos;

public:
    River(Position pos);
    ~River();
    void getEffect(Army *army) override;
};

// TODO: Class Urban
class Urban : public TerrainElement
{
private:
    Position pos;

public:
    Urban(Position pos);
    ~Urban();
    void getEffect(Army *army) override;
};

// TODO: Class Fortification
class Fortification : public TerrainElement
{
private:
    Position pos;

public:
    Fortification(Position pos);
    ~Fortification();
    void getEffect(Army *army) override;
};

// TODO: Class SpecialZone
class SpecialZone : public TerrainElement
{
private:
    Position pos;

public:
    SpecialZone(Position pos);
    ~SpecialZone();
    void getEffect(Army *army) override;
};

class BattleField
{
private:
    int n_rows, n_cols;
    // TODO

    TerrainElement ***terrain;

public:
    BattleField(int n_rows, int n_cols, vector<Position *> arrayForest,
                vector<Position *> arrayRiver, vector<Position *> arrayFortification,
                vector<Position *> arrayUrban, vector<Position *> arraySpecialZone);
    ~BattleField();

    // TODO
    TerrainElement *getElement(int row, int col) const;
    void terrainEffect(Army *army);
    int getRow() const;
    int getCol() const;
    string str() const;
};

// TODO: Class Configuration
class Configuration
{
    friend class HCMCampaign;

private:
    int num_rows, num_cols;
    vector<Position *> arrayForest;
    vector<Position *> arrayRiver;
    vector<Position *> arrayFortification;
    vector<Position *> arrayUrban;
    vector<Position *> arraySpecialZone;
    Unit **liberationUnits;
    Unit **arvnUnits;
    int liberationUnitCount;
    int arvnUnitCount;
    int eventCode;

    // TODO: Helper
    vector<Position *> parsePositionArray(const string &arraystr);
    vector<string> splitString(const string &str);
    vector<string> splitParameters(const string &str);
    Unit *createUnit(const string &str);

public:
    Configuration(const string &filepath);
    ~Configuration();
    string str() const;
};

class HCMCampaign : public Configuration
{
private:
    Configuration *config;
    BattleField *battleField;
    LiberationArmy *liberationArmy;
    ARVN *arvn;

public:
    HCMCampaign(const string &config_file_path);
    void run();
    void removeWeakUnits(Army *army);
    string printResult();
};

#endif