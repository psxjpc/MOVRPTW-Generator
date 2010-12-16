#ifndef VRPTWINSTANCEGENERATOR_H
#define VRPTWINSTANCEGENERATOR_H

#include <string>

#include "dataTypes.h"
#include "MersenneTwister.h"

class VRPTWInstanceGenerator
{
   private:
      //! Seed to generate the set of random costumers.
      unsigned matrixSeed;
      //! Seed to generate the time windows.
      unsigned timeWindowSeed;
      //! Seed to generate the demands.
      unsigned demandSeed;
      //! Seed to generate the service times.
      unsigned serviceTimeSeed;

      //! Size of the instance (number of costumers)
      unsigned size;

      //! Raw info to generate the instance
      rawInfoVector distanceTable;
      rawInfoVector timeTable;

      //! Specifications of the instance
      tTimeWindow timeWindows;
      tDemand demands;
      tServiceTime serviceTimes;

      //! Maximum capacity of each vehicle of the fleet
      capacityType vehicleMaxCapacity;

      //! Number of max allowed vehicles for this instance
      unsigned sizeOfFleet;

      //! Costumer ids
      std::vector<unsigned> ids;

      //! Costumers positions
      costumersPosType positions;

      //! Matrices
      matrixType distanceMatrix;
      matrixType timeMatrix;

      //! Prefix for output-files
      std::string prefix;

      //! Solomon-type output structure
      std::vector<unsigned> timeWindowsIndexes;
      idsType demandsIndexes;
      idsType serviceTimesIndexes;

      //! Vector of costumers appearing in this instance
      std::vector<unsigned> randomIds;

   protected:

      //! Method that, given a table and 'from' 'to' information, returns the length in time or distance
      double getData(const rawInfoVector&, unsigned, unsigned);

      //! This method will receive a random number generator and a vector of accumulative probability and will return a category
      unsigned getCategoryRandomly(double, const accProbabilityType&);

      //! Method to output errors
      void error(const std::string&);

      //! Method to output warnings
      void warning(const std::string&);

      //! Method to get the position of a costumer within vector
      unsigned getPosition(unsigned);

      //! Method to generate the distance matrix for this instance
      void generateDistanceMatrix();

      //! Method to generate the time matrix for this instance
      void generateTimeMatrix();

   public:


      //! Default Ctor.
      VRPTWInstanceGenerator();

      //! Default Destructor.
      ~VRPTWInstanceGenerator();


      //! Method that reads the distance between each par of costumers
      /*!
        \param distanceFileName is the name of the file containing the distances.
      */
      void readDistancesFile(const char* distanceFileName);

      //! Method that reads the time between each par of costumers
      /*!
        \param timeFileName is the name of the file containing the times.
      */
      void readTimesFile(const char* timeFilename);

      //! Method that reads the real ids of the costumers
      /*!
        \param distanceFileName is the name of the file containing the distances.
      */
      void readIds(const char* idsFileName);

      //! Method that reads the position of the costumers
      /*!
        \param idslatlngFileName is the name of the file containing the distances.
      */
      void readPositions(const char* idslatlngFileName);


      //! Method that parsers the time windows specifications
      /*!
        \param fileName is the name of the file containing the time windows specifications.
      */
      void readTimeWindowsFile(const char* fileName);

      //! Method that parsers the demands specifications
      /*!
        \param fileName is the name of the file containing the demands specifications.
      */
      void readDemandsFile(const char* fileName);

      //! Method that parsers the service times specifications
      /*!
        \param fileName is the name of the file containing the service times specifications.
      */
      void readServiceTimesFile(const char* fileName);

      //! Sets the size of the instance
      void setSize(unsigned);

      //! Sets the seeds to generate the data for this instance
      void setSeed(const std::string&, unsigned);

      //! Sets the prefix for the output files
      void setPrefix(const std::string&);

      //! Method that generates the distance and time windows matrices with random costumers
      void generateMatrices();

      //! Method that associates a type of time window to each costumer
      void generateTimeWindows();

      //! Method that associates a type of demand to each costumer
      void generateDemands();

      //! Method that associates a type of service time to each costumer
      void generateServiceTimes();

      //! Method that call all generates
      void generateAll();

      // Print

      //! Method to print the parameters
      void printParameters();

      //! Method to print the time matrix
      void printTimeMatrix();

      //! Method to print the distance matrix
      void printDistanceMatrix();

      //! Method to print the specifications
      void printSpecifications();

      //! Method that prints all
      void print();

      //! Method that prints all
      void printAll();

      // Write output

      //! Method that writes the distance matrix to a file
      void writeDistanceMatrix();

      //! Method that writes the time matrix to a file
      void writeTimeMatrix();

      //! Method that writes the specification to a file
      void writeSpecifications();

      //! Method that call all writes
      void writeAll();

};

#endif // VRPTWINSTANCEGENERATOR_H
