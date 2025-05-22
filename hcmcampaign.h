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
protected:
    int LF, EXP;
    string name;
    UnitList *unitList;
    BattleField *battleField;

public:
    Army(Unit **unitArray, int size, string name, BattleField *battleField);
    virtual void fight(Army *enemy, bool defense = false) = 0;
    virtual string str() const = 0;
};

// TODO: Class LiberationArmy
class LiberationArmy : public Army {
public:
    LiberationArmy(const Unit** unitArray, int size, string name, BattleField* battleField);
    void fight(Army* enemy, bool defense = false) override;
    string str() const override;
};

// TODO: Class ARVN
class ARVN : public Army {
public:
    ARVN(const Unit** unitArray, int size, string name, BattleField* BattleField);
    void fight(Army* enemy, bool defense = false) override;
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
};

// TODO: Class Vehicle
class Vehicle : public Unit {
private:
    VehicleType vehicleType;

public:
    Vehicle(int quantity, int weight, const Position pos, VehicleType vehicleType);
    int getAttackScore() override;
    string str() const override;
};

// TODO: Class Infantry
class Infantry : public Unit {
private:
    InfantryType infantryType;
    int calPersonalNumber(int number);

public:
    Infantry(int quantity, int weight, const Position pos, InfantryType infantryType);
    int getAttackScore() override;
    string str() const override;
};

// TODO: Struct UnitNode
struct UnitNode {
    Unit* unit;
    UnitNode* next;
};

class UnitList
{
private:
    int capacity;
    // TODO

    UnitNode* head;
    int countVehicle;
    int countInfantry;

    bool isSpecialNumber(int number);

public:
    UnitList(int capacity);
    bool insert(Unit *unit);                   // return true if insert successfully
    bool isContain(VehicleType vehicleType);   // return true if it exists
    bool isContain(InfantryType infantryType); // return true if it exists
    string str() const;
    // TODO

    ~UnitList();
    int getCapacity() const;
    int getVehicleNumber() const;
    int getInfantryNumber() const;
    int getTotalUnitNumber() const;
    UnitNode* getHead() const;
};

class TerrainElement
{
public:
    TerrainElement();
    ~TerrainElement();
    virtual void getEffect(Army *army) = 0;
};

// TODO: Class Road
class Road : public TerrainElement {
public:
    Road();
    ~Road();
    void getEffect(Army *army) override;
};

// TODO: Class Mountain
class Mountain : public TerrainElement {
public:
    Mountain();
    ~Mountain();
    void getEffect(Army *army) override;
};

// TODO: Class River
class River : public TerrainElement {
public:
    River();
    ~River();
    void getEffect(Army *army) override;
};

// TODO: Class Urban
class Urban : public TerrainElement {
public:
    Urban();
    ~Urban();
    void getEffect(Army *army) override;
};

// TODO: Class Fortification
class Fortification : public TerrainElement {
public:
    Fortification();
    ~Fortification();
    void getEffect(Army *army) override;
};

// TODO: Class SpecialZone
class SpecialZone : public TerrainElement {
public:
    SpecialZone();
    ~SpecialZone();
    void getEffect(Army *army) override;
};

class BattleField
{
private:
    int n_rows, n_cols;
    // TODO

    TerrainElement*** terrain;

public:
    BattleField(int n_rows, int n_cols, vector<Position *> arrayForest,
                vector<Position *> arrayRiver, vector<Position *> arrayFortification,
                vector<Position *> arrayUrban, vector<Position *> arraySpecialZone);
    ~BattleField();
};

class Configuration {
private:
    int num_rows, num_cols;
    vector<Position*> arrayForest;
    vector<Position*> arrayRiver;
    vector<Position*> arrayFortification;
    vector<Position*> arrayUrban;
    vector<Position*> arraySpecialZone;
    Unit** liberationUnits;
    Unit** ARVNUnits;
    int liberationUnitCount;
    int ARVNUnitCount;
    int eventCode;
    
public:
    Configuration(const string& filepath);
    ~Configuration();
    string str() const;
};

class HCMCampaign
{
private:
    Configuration *config;
    BattleField *battleField;
    LiberationArmy *liberationArmy;
    ARVN *ARVN;

public:
    HCMCampaign(const string &config_file_path);
    void run();
    string printResult();
};

#endif