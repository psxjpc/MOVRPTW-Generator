/*
 * --------------------------------------------------------------------------
 *
 *                             Copyright (c) 2010
 *                  Juan Castro-Gutierrez <jpcastrog@gmail.com>      (1)
 *             Dario Landa-Silva <dario.landasilva@nottingham.ac.uk> (1)
 *            José A. Moreno Pérez <joseandresmorenoperez@gmail.com> (2)
 *           --------------------------------------------------------
 *            (1) University of Nottingham (UK) - ASAP research group.
 *            (2) Universidad de La Laguna (Spain) - DEIOC.
 *
 * This program is free software (software libre); you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can obtain a copy of the GNU
 * General Public License at:
 *                http://www.gnu.org/copyleft/gpl.html
 * or by writing to:
 *           Free Software Foundation, Inc., 59 Temple Place,
 *                 Suite 330, Boston, MA 02111-1307 USA
 *
 * --------------------------------------------------------------------------
 */

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>

#include "tinyxml/tinyxml.h"

#include "conversions.h"
#include "vrptwinstancegenerator.h"
#include "MersenneTwister.h"

// --- Protected --- //

/**
   Method that ouputs message and force the exit of the app.
   @param errorMessage is the message to be output.
*/
void VRPTWInstanceGenerator::error(const std::string& errorMessage)
{
    std::cout << "[ERROR]: " << errorMessage << std::endl;
    exit(1);
}


/**
   Method that ouputs a warning message.
   @param warningMessage is the message to be output.
*/
void VRPTWInstanceGenerator::warning(const std::string& warningMessage)
{
    std::cout << "[WARNING]: " << warningMessage << std::endl;
}

/**
   Method that given the two first two elements of a table of nx3 returns the third element.
   @param table is the matrix in which we want to look for the third element.
   @param from is the first element or index.
   @param to is the second element or index.
   @return the value of the third element.
*/
double VRPTWInstanceGenerator::getData(const rawInfoVector& table, unsigned from, unsigned to)
{
   if (from == to)
      return 0;
   for (size_t i = 0; i < table.size(); i++)
      if (table[i].from == from && table[i].to == to)
         return table[i].length;

   error("Unexpected error looking for length");
   return 0;
}

/**
   Method that given a vector with accumulate probabilities returns the section in which a random number falls.
   @param randNumber is the random number we want to know the place it falls in
   @param accProbability is the vector containing the accumuate probabilities.
   @return the section in which the random number falls.
*/
unsigned VRPTWInstanceGenerator::getCategoryRandomly(double randNumber, const accProbabilityType& accProbability)
{
   for (size_t i = 0; i < accProbability.size() - 1; i++)
      if (randNumber > accProbability[i] && randNumber < accProbability[i + 1])
         return i;

   error("getCategoryRandomly(). Out of bounds");
   return 0;
}

/**
  Method that returns the index of a given element (id) within the positions vector
  @param id is the id of the element we are looking for
  @return the position (index) within the vector
*/
unsigned VRPTWInstanceGenerator::getPosition(unsigned id)
{
   for (size_t i = 0; i < this->positions.size(); i++)
      if (this->positions[i].id == id)
         return i;

   error("getPosition(). Id not found");
   return 0;
}

// --- Public ---


/**
  Ctor. It sets default values for data members
*/
VRPTWInstanceGenerator::VRPTWInstanceGenerator()
{
   // By default, all seeds will be initilized to time(null)
   unsigned seed = time(NULL);
   this->matrixSeed = seed;
   this->demandSeed = seed;
   this->timeWindowSeed = seed;
   this->serviceTimeSeed = seed;

   // Size by default is 100
   this->size = 100;

   // Prefix will be set to the current time to avoid
   //   file name conflicts.
   std::ostringstream outputStream;
   outputStream << seed;
   this->prefix = "output" + outputStream.str();


}

/**
  Dtor. It does nothing
*/
VRPTWInstanceGenerator::~VRPTWInstanceGenerator()
{ }


/**
  Method that reads the distances from a file between pairs of costumers.
  @param distanceFilanem is the path to the file containing the distances information.
*/
void VRPTWInstanceGenerator::readDistancesFile(const char *distanceFileName)
{
    // Read table of distances.
    std::fstream distanceFile(distanceFileName);
    unsigned from = 0;
    unsigned to = 0;
    double length = 0;
    while (distanceFile >> from >> to >> length)
    {
       tData info;
       info.from = from;
       info.to = to;
       info.length = length;
       distanceTable.push_back(info);
    }
    distanceFile.close();
}

/**
  Method that reads the travel times from a file between pairs of costumers.
  @param timeFileName is the path to the file containing the travel time information.
*/
void VRPTWInstanceGenerator::readTimesFile(const char *timeFileName)
{
    // Read table of times.
    std::fstream timeFile(timeFileName);
    unsigned from = 0;
    unsigned to = 0;
    double length = 0;
    while (timeFile >> from >> to >> length)
    {
       tData info;
       info.from = from;
       info.to = to;
       info.length = length;
       timeTable.push_back(info);
    }
    timeFile.close();
}

/**
  Method that reads the real ids of the costumers.
  @param idsFilename is the path to the file containing the real ids.
*/
void VRPTWInstanceGenerator::readIds(const char *idsFileName)
{
    // Read ids for matching
    std::cout << "Reading ids" << std::endl;
    std::fstream idsFile(idsFileName);
    unsigned trash = 0;
    unsigned id = 0;
    while (idsFile >> trash >> id)
       ids.push_back(id);
    idsFile.close();
}

/**
  Method that reads the real position of the costumers (lat, lng).
  @param idslatlngFilename is the path to the file containing the information.
*/
void VRPTWInstanceGenerator::readPositions(const char* idslatlngFileName)
{
   std::fstream idsLatLngFile(idslatlngFileName);
   tPos row;
   while (idsLatLngFile >> row.id >> row.lat >> row.lng)
      this->positions.push_back(row);
}

/**
  Method that reads the time windows specification
  @para fileName is the path to the file containing the time windows specification
*/
void VRPTWInstanceGenerator::readTimeWindowsFile(const char* fileName)
{
    TiXmlDocument XMLdoc(fileName);
    bool loadOkay = XMLdoc.LoadFile();
    if (loadOkay)
    {
        TiXmlElement *pRoot, *pDepot, *pTimeWindows, *pTimeWindow;
        pRoot = XMLdoc.FirstChildElement( "time-windows-specification");
        pDepot = pRoot->FirstChildElement("depot");
        this->timeWindows.timeWindowDepot.opens = fromStringTo<double>(std::string(pDepot->Attribute("opens")));
        this->timeWindows.timeWindowDepot.closes = fromStringTo<double>(std::string(pDepot->Attribute("closes")));

        pTimeWindows = pRoot->FirstChildElement("time-windows");
        pTimeWindow = pTimeWindows->FirstChildElement("time-window");
        while (pTimeWindow)
        {
            tTimeWindowCostumer timeWindowCostumer;
            timeWindowCostumer.opens = fromStringTo<double>(std::string(pTimeWindow->Attribute("opens")));
            timeWindowCostumer.closes = fromStringTo<double>(std::string(pTimeWindow->Attribute("closes")));
            timeWindowCostumer.probability = fromStringTo<unsigned>(std::string(pTimeWindow->Attribute("probability")));
            this->timeWindows.timeWindowsCostumers.push_back(timeWindowCostumer);
            pTimeWindow = pTimeWindow->NextSiblingElement("time-window");
        }
    }
    else
        error("Unable to read the time windows file");

}

/**
  Method that reads the demand specifications
  @param fileName is the path to the file containing the demands specifications.
*/
void VRPTWInstanceGenerator::readDemandsFile(const char* fileName)
{
    TiXmlDocument XMLdoc(fileName);
    bool loadOkay = XMLdoc.LoadFile();
    if (loadOkay)
    {
        TiXmlElement *pRoot, *pParm, *pDemands, *pDemand;
        pRoot = XMLdoc.FirstChildElement( "demands-specifications" );
        if ( pRoot )
        {
            // Parse looseness
            pParm = pRoot->FirstChildElement("delta");
            this->demands.delta = fromStringTo<unsigned>(std::string(pParm->Attribute("value")));

            // Parse Process
            pDemands = pRoot->FirstChildElement("demands");
            pDemand = pDemands->FirstChildElement("demand");
            while ( pDemand )
            {
               tDemandCostumer demandCostumer;
               demandCostumer.type = fromStringTo<unsigned>(std::string(pDemand->Attribute("type")));
               demandCostumer.probability = fromStringTo<double>(std::string(pDemand->Attribute("probability")));
               this->demands.demandsCostumers.push_back(demandCostumer);
               pDemand = pDemand->NextSiblingElement( "demand" );

            }
        }
        else
            error("Unable to read the demands file");
    }
}

/**
  Method that reads the service times specifications.
  @param fieName is the path to the file containing the specifications.
*/
void VRPTWInstanceGenerator::readServiceTimesFile(const char* fileName)
{
    TiXmlDocument XMLdoc(fileName);
    bool loadOkay = XMLdoc.LoadFile();
    if (loadOkay)
    {
        TiXmlElement *pRoot, *pServiceTimes, *pServiceTime;
        pRoot = XMLdoc.FirstChildElement("service-times-specifications");
        pServiceTimes = pRoot->FirstChildElement("service-times");
        pServiceTime = pServiceTimes->FirstChildElement("service-time");
        while (pServiceTime)
        {
            tServiceTimeCostumer serviceTime;
            serviceTime.type = fromStringTo<double>(std::string(pServiceTime->Attribute("type")));
            serviceTime.probability = fromStringTo<unsigned>(std::string(pServiceTime->Attribute("probability")));
            this->serviceTimes.push_back(serviceTime);
            pServiceTime = pServiceTime->NextSiblingElement("service-time");
        }
    }
    else
        error("Error - Unable to read the service time specifications file.");

}

/**
  Method to set the number of costumers of the instance.
  @param size if the number of costumers.
*/
void VRPTWInstanceGenerator::setSize(unsigned size)
{
    this->size = size;
}

/**
  Method to set the value of a seed.
  @param object is the name of seed to be set up.
  @param value is the value to be set.
*/
void VRPTWInstanceGenerator::setSeed(const std::string& object, unsigned value)
{
    if (object == "Matrices")
        this->matrixSeed = value;
    else if (object == "Demands")
        this->demandSeed = value;
    else if (object == "Time Windows")
        this->timeWindowSeed = value;
    else if (object == "Service Times")
        this->serviceTimeSeed = value;
}

/**
  Method to set the prefix for the output files
  @param prefix is the prefix of the output files.
*/
void VRPTWInstanceGenerator::setPrefix(const std::string& prefix)
{
   this->prefix = prefix;
}

/**
  Method that generates the distance matrix using random ids.
*/
void VRPTWInstanceGenerator::generateDistanceMatrix()
{
    unsigned matrixSize = this->size + 1;
    distanceMatrix.resize(matrixSize);
    for (size_t i = 0; i < (unsigned)matrixSize; i++)
       distanceMatrix[i].resize(matrixSize);

    for (size_t i = 0; i < (unsigned)matrixSize; i++)
       for (size_t j = 0; j < (unsigned)matrixSize; j++)
          distanceMatrix[i][j] = getData(distanceTable, ids[randomIds[i]], ids[randomIds[j]]);
}

/**
  Method that generates the travel time matrix using random ids.
*/
void VRPTWInstanceGenerator::generateTimeMatrix()
{
    unsigned matrixSize = this->size + 1;
    timeMatrix.resize(matrixSize);
    for (size_t i = 0; i < (unsigned)matrixSize; i++)
       timeMatrix[i].resize(matrixSize);

    for (size_t i = 0; i < (unsigned)matrixSize; i++)
       for (size_t j = 0; j < (unsigned)matrixSize; j++)
          timeMatrix[i][j] = getData(timeTable, ids[randomIds[i]], ids[randomIds[j]]);
}

/**
  Method that generates a vector of random ids (size value random ids) and
  invokes generateDistanceMatrix and generateTimeMatrix().
*/
void VRPTWInstanceGenerator::generateMatrices()
{
    // Creation of random ids
    idsType tempIds;
    for (size_t i = 0; i < this->ids.size(); i++)
        tempIds.push_back(i);

    std::random_shuffle(tempIds.begin(), tempIds.end());

    for (size_t i = 0; i < this->size; i++)
        this->randomIds.push_back(tempIds[i]);

    // + depot
    this->randomIds.insert(this->randomIds.begin(), 0);

    // DEBUG: Printing
    std::cout << "Random Ids:" << std::endl;
    std::copy(this->randomIds.begin(), this->randomIds.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;

    std::cout << "Real Ids correspondence:" << std::endl;
    for (size_t i = 0; i < this->randomIds.size(); i++)
       std::cout << ids[randomIds[i]] << " ";
    std::cout << std::endl;

    generateDistanceMatrix();
    generateTimeMatrix();
}

/**
  Method that generates a vector of size (size) with all the time windows.
*/
void VRPTWInstanceGenerator::generateTimeWindows()
{
    std::vector<double> accProbability;
    MTRand randomGenerator(this->timeWindowSeed);

    // Checking integrity, the sum of probs must be 100.
    unsigned sum = 0;
    for (size_t i = 0; i < this->timeWindows.timeWindowsCostumers.size() ; i++)
       sum += this->timeWindows.timeWindowsCostumers[i].probability;

     if (sum != 100)
        warning("Error - The sum of probabilities in the time windows specifications file is not 100.");

    // - Segment for the creation of time windows
    accProbability.resize(this->timeWindows.timeWindowsCostumers.size());
    accProbability[0] = 0;

    for (size_t i = 1; i < accProbability.size(); i++)
       accProbability[i] = accProbability[i - 1] + (this->timeWindows.timeWindowsCostumers[i].probability / (double)sum);
    accProbability.push_back(1);

    unsigned index = 0;
    for (size_t i = 0; i < this->size; i++)
    {
       index = getCategoryRandomly(randomGenerator.rand(), accProbability);
       this->timeWindowsIndexes.push_back(index);
    }
}

/**
  Method that generates a vector of size (size) with all the demands.
*/
void VRPTWInstanceGenerator::generateDemands()
{
   std::vector<double> accProbability;
   MTRand randomGenerator(this->demandSeed);


   // Checking integrity, the sum of probs must be 100.
   unsigned sum = 0;
   // Type of demans start in the second row, so it's from i = 1.
   for (size_t i = 0; i < this->demands.demandsCostumers.size(); i++)
      sum += this->demands.demandsCostumers[i].probability;

   if (sum != 100)
      warning("Error - The sum of probabilities in the demand specification file is not 100.");

   // - Segment for the creation of demands
   accProbability.resize(this->demands.demandsCostumers.size());
   accProbability[0] = 0;
   // First row is reserved, so we start in i = 1
   for (size_t i = 1; i < accProbability.size(); i++)
      accProbability[i] = accProbability[i - 1] + (this->demands.demandsCostumers[i].probability / (double)sum);
   accProbability.push_back(1);

   unsigned index = 0;
   for (size_t i = 0; i < this->size; i++)
   {
      // Demand for the depot is 0, for costumers we throw a die
      index = getCategoryRandomly(randomGenerator.rand(), accProbability);
      this->demandsIndexes.push_back(index);
   }

   // Max capacity for each vehicle.
   capacityType sumOfDemands = 0;
   for (size_t i = 0; i < this->demandsIndexes.size(); i++)
       sumOfDemands += (this->demands.demandsCostumers[this->demandsIndexes[i]].type);

   capacityType maxDemand = 0;
   for (size_t i = 0; i < this->demands.demandsCostumers.size(); i++)
       maxDemand = (this->demands.demandsCostumers[i].type > maxDemand)? this->demands.demandsCostumers[i].type : maxDemand;

   capacityType delta = this->demands.delta;
   capacityType minCapacity = maxDemand;
   this->sizeOfFleet = ceil((double)sumOfDemands / (double)(minCapacity));
   this->vehicleMaxCapacity = minCapacity + ceil((sumOfDemands - minCapacity) * ((double)delta/(double)100));

   return;
}

/**
  Method that generates a vector of size (size) with all the time services.
*/
void VRPTWInstanceGenerator::generateServiceTimes()
{
   accProbabilityType accProbability;
   MTRand randomGenerator(this->serviceTimeSeed);

   // Checking integrity, the sum of probs should be 100.
   unsigned sum = 0;
   for (size_t i = 0; i < this->serviceTimes.size(); i++)
      sum += this->serviceTimes[i].probability;

   if (sum != 100)
      warning("Error - The sum of probabilities in the service time specification file is not 100.");

   // - Segment for the creation of demands.
   accProbability.resize(this->serviceTimes.size() + 1);
   accProbability[0] = 0;
   for (size_t i = 1; i < accProbability.size(); i++)
      accProbability[i] = accProbability[i - 1] + (this->serviceTimes[i - 1].probability / (double)sum);
   accProbability.push_back(1);

   unsigned index = 0;
   for (size_t i = 0; i < this->size + 1; i++)
   {
      index = getCategoryRandomly(randomGenerator.rand(), accProbability);
      this->serviceTimesIndexes.push_back(index);
   }
}

/**
  Method that invokes serveral other methods to generate the instance.
*/
void VRPTWInstanceGenerator::generateAll()
{
    generateMatrices();
    generateTimeWindows();
    generateDemands();
    generateServiceTimes();
}

/**
  Method that prints the distance matrix.
*/
void VRPTWInstanceGenerator::printDistanceMatrix()
{
    for (size_t i = 0; i < this->distanceMatrix.size(); i++)
    {
       for (size_t j = 0; j < this->distanceMatrix[i].size(); j++)
          std::cout << this->distanceMatrix[i][j] << "\t";
       std::cout << std::endl;
    }
}

/**
  Method that prints the time matrix.
*/
void VRPTWInstanceGenerator::printTimeMatrix()
{
    for (size_t i = 0; i < this->timeMatrix.size(); i++)
    {
       for (size_t j = 0; j < this->timeMatrix[i].size(); j++)
          std::cout << this->timeMatrix[i][j] << "\t";
       std::cout << std::endl;
    }
}

/**
  Method that prints the specifications.
*/
void VRPTWInstanceGenerator::printSpecifications()
{
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "Specifications:" << std::endl;

    // Name of the problem
    std::cout << this->prefix << std::endl;
    std::cout << std::endl;
    std::cout << "VEHICLE" << std::endl;
    std::cout << "NUMBER     CAPACITY" << std::endl;
    std::cout << this->sizeOfFleet << "\t" << this->vehicleMaxCapacity << std::endl;
    std::cout << std::endl;
    std::cout << "CUSTOMER" << std::endl;
    std::cout << "CUST NO." << "\t" <<  "XCOORD." << "\t" <<  "YCOORD."  << "\t" <<  "DEMAND" << "\t" <<  "READY TIME" << "\t" << "DUE DATE" << "\t" <<  "SERVICE   TIME" << std::endl;
    std::cout << std::endl;

}

/**
  Method that prints the parameters given by the user (debug purposes).
*/
void VRPTWInstanceGenerator::printParameters()
{
    std::cout << "Parameters" << std::endl;
    std::cout << "Size of the problem (Number of costumers): " << this->size << std::endl;
    std::cout << "Seeds:" << std::endl;
    std::cout << "Seed Matrix: " << this->matrixSeed << std::endl;
    std::cout << "Seed Service Time: " << this->serviceTimeSeed << std::endl;
    std::cout << "Seed Time Windows: " << this->timeWindowSeed << std::endl;
    std::cout << "Seed Demand: " << this->demandSeed << std::endl;
    std::cout << std::endl;
    std::cout << "Random ids: ";
    for (size_t i = 0; this->randomIds.size(); i++)
        std::cout << this->randomIds[i] << " " << std::endl;
    std::cout << std::endl;

    std::cout << "Prefix: " << this->prefix << std::endl;


}

/**
  Method that invokes all printing methods.
*/
void VRPTWInstanceGenerator::printAll()
{
    printParameters();
    printDistanceMatrix();
    printTimeMatrix();
    printSpecifications();
}


/**
  Method that invokes all printing methods.
*/
void VRPTWInstanceGenerator::print()
{
    printAll();
}

/**
  Method that creates the file containing the distance matrix.
*/
void VRPTWInstanceGenerator::writeDistanceMatrix()
{
    // Output to file
    std::string outputFilename = this->prefix + "DistanceMatrix.dat";
    std::ofstream outputFile(outputFilename.c_str());

    std::cout << "Writing distance matrix file: " << outputFilename << std::endl;
    for (size_t i = 0; i < distanceMatrix.size(); i++)
    {
       for (size_t j = 0; j < distanceMatrix[i].size(); j++)
          outputFile << distanceMatrix[i][j] << "\t";
       outputFile << "\n";
    }
    outputFile.close();
}

/**
  Method that creates the file containing the time matrix.
*/
void VRPTWInstanceGenerator::writeTimeMatrix()
{
    // Output to file
    std::string outputFilename = this->prefix + "TimeMatrix.dat";
    std::ofstream outputFile(outputFilename.c_str());

    std::cout << "Writing time matrix file: " << outputFilename << std::endl;
    for (size_t i = 0; i < timeMatrix.size(); i++)
    {
       for (size_t j = 0; j < timeMatrix[i].size(); j++)
          outputFile << timeMatrix[i][j] << "\t";
       outputFile << "\n";
    }
    outputFile.close();
}

/**
  Method that creates the file containing the specifications.
*/
void VRPTWInstanceGenerator::writeSpecifications()
{
    std::string outputFilename = this->prefix + "Specs.dat";
    std::ofstream output(outputFilename.c_str());
    std::cout << "Writing specifications file: " << outputFilename << std::endl;


    double opens, closes;

    // Name of the problem
    output << this->prefix << "\n";
    output << "\n";
    output << "VEHICLE" << "\n";
    output << "NUMBER     CAPACITY" << "\n";
    output << this->sizeOfFleet << "\t" << this->vehicleMaxCapacity << "\n";
    output << "\n";
    output << "CUSTOMER" << "\n";
    output << "CUST NO." << "\t" <<  "XCOORD." << "\t" <<  "YCOORD."  << "\t" <<  "DEMAND" << "\t" <<  "READY TIME" << "\t" << "DUE DATE" << "\t" <<  "SERVICE   TIME" << "\n";
    output << "\n";

    tIndex indexPosition;
    for (size_t i = 0; i < this->randomIds.size(); i++)
    {
        opens = ((i == 0)? this->timeWindows.timeWindowDepot.opens :
                           this->timeWindows.timeWindowsCostumers[this->timeWindowsIndexes[i - 1]].opens);

        closes = ((i == 0)? this->timeWindows.timeWindowDepot.closes :
                            this->timeWindows.timeWindowsCostumers[this->timeWindowsIndexes[i - 1]].closes);

        indexPosition = getPosition(this->ids[this->randomIds[i]]);
        output << ids[this->randomIds[i]] << std::setiosflags(std::ios::fixed) << std::setprecision(4)
                << "\t" << this->positions[indexPosition].lat
                << "\t" << this->positions[indexPosition].lng;

        output << "\t" <<  std::setprecision(0) << ((i == 0)? 0 : this->demands.demandsCostumers[this->demandsIndexes[i]].type)
                << "\t" << opens
                << "\t" << closes
                << "\t" << ((i == 0)? 0 : this->serviceTimes[this->serviceTimesIndexes[i]].type)
                << "\n";

    }
    output.close();
}

/**
  Method that invokes all writing methods.
*/
void VRPTWInstanceGenerator::writeAll()
{
    writeDistanceMatrix();
    writeTimeMatrix();
    writeSpecifications();
}
