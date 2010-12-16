#ifndef DATATYPES_H
#define DATATYPES_H

#include <vector>

/**
  Cost between nodes:
     Structure to store the cost between pairs of node.
*/
struct tData
{
   unsigned from;
   unsigned to;
   double length;
};

typedef std::vector<tData> rawInfoVector;


/**
Position of the costumers:
  For each costumer, we will hold the the
  original id and its position (lat, lng).
*/

struct tPos
{
   unsigned id;
   double lat;
   double lng;
};

typedef std::vector<tPos> costumersPosType;


/**
Time Window Scepecifications Structure:
  In order to avoid the use of inheritance and reduce
  the possible overhead to the minimum, we will use two
  structs, one for the depot and another for the costumers.
*/
struct tTimeWindowDepot
{
    double opens;
    double closes;
};

struct tTimeWindowCostumer
{
    double opens;
    double closes;
    unsigned probability;
};

struct tTimeWindow
{
    tTimeWindowDepot timeWindowDepot;
    std::vector<tTimeWindowCostumer> timeWindowsCostumers;
};

/**
Demand Specification Structure:
  "delta" is defined as the percentage change in capacity of the fleet of vehicles.
  This value will be bounded between 0 and 100. The bigger this value is,
  the more capacity the vehicles will have. Moreover, we will
  also specify the demands with their respective probabilites.
*/
struct tDemandCostumer
{
    double type;
    unsigned probability;
};

struct tDemand
{
    unsigned delta;
    std::vector<tDemandCostumer> demandsCostumers;
};

/**
Service Time Specification Structure
  For each costumer, a type of time service is defined with its
  probability
*/

struct tServiceTimeCostumer
{
   double type;
   unsigned probability;
};

typedef std::vector<tServiceTimeCostumer> tServiceTime;

/**
Solomon-like Specification Structure
  In order to output the general specifications of the costumers,
  we will use the structure emplied in the VRP Solomon's instance-set.
*/

struct tSolomon
{
    unsigned id;
    double X;
    double Y;
    double demand;
    unsigned long opens;
    unsigned long closes;
    unsigned long serviceTime;
};

/**
 Generic Data Types
*/
typedef unsigned tIndex;
typedef long capacityType;
typedef std::vector<tIndex> idsType;

/**
 Matrix to store both distance and time matrices
*/
typedef std::vector<std::vector<double> > matrixType;

/**
 Vector to apply roulette wheel selection
*/
typedef std::vector<double> accProbabilityType;

#endif // DATATYPES_H
