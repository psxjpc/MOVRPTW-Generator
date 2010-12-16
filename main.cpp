/*
 * --------------------------------------------------------------------------
 *
 *                             Copyright (c) 2010
 *                  Juan Castro-Gutierrez <jpcastrog@gmail.com>      (1)
 *             Dario Landa-Silva <dario.landasilva@nottingham.ac.uk> (1)
 *            José A. Moreno Pérez <joseandresmorenoperez@gmail.com> (2)
 *           --------------------------------------------------------
 *            (1) University of Nottingham (UK) - ASAP research group
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
#include <iostream>
#include <iterator>
#include <vector>
#include <string>

#include "dataTypes.h"
#include "vrptwinstancegenerator.h"


/**
  @todo

  - More assertions and warnings to avoid segmentation faults due to invalid data.

  */

int main(int argc, char** argv)
{
    if (argc < 8)
    {
        std::cout << "[ERROR] - Insuffiient parameters." << std::endl;
        std::cout << "Sintax: "<< argv[0] << "<size> <fileTW> <fileD> <fileTS> <seedM> <seedTW> <seedD> <seedST> <outPref>" << std::endl;
        std::cout << "- <size>" << "\t" << "Size of the problem (Number of costumers the problem will have" << std::endl;
        std::cout << "---------------------------------------------------------------------------------------------" << std::endl;
        std::cout << "- <fileTW>" << "\t" << "File that contains the specification of time windows." << std::endl;
        std::cout << "- <fileD>" << "\t" << "File that contains the specification of demands." << std::endl;
        std::cout << "- <fileTS>" << "\t" << "File that contains the specification of time services." << std::endl;
        std::cout << "---------------------------------------------------------------------------------------------" << std::endl;
        std::cout << "- <seedM>" << "\t" << "Seed to generate the matrices." << std::endl;
        std::cout << "- <seedTW>" << "\t" << "Seed to generate the time windows." << std::endl;
        std::cout << "- <seedST>" << "\t" << "Seed to generate the service times." << std::endl;
        std::cout << "- <seedD>" << "\t" << "Seed to generate the demands." << std::endl;
        std::cout << "- <outPref>" << "\t" << "Prefix for output files." << std::endl;
        std::cout << "---------------------------------------------------------------------------------------------" << std::endl;

        exit(1);
    }

    // Fixed file names
    std::string distanceFileName  = "rawDistance.txt";
    std::string timeFileName      = "rawTime.txt";
    std::string idsFileName       = "idRid.txt";
    std::string idslatlngFileName = "idLatLng.dat";

    // Params
    unsigned matrixSize     = (unsigned)atoi(argv[1]);
    char* timeWindowsFile  = argv[2];
    char* demandsFile      = argv[3];
    char* timeServicesFile = argv[4];
    unsigned seedM         = atoi(argv[5]);
    unsigned seedTW        = atoi(argv[6]);
    unsigned seedD         = atoi(argv[7]);
    unsigned seedST        = atoi(argv[8]);
    char* outputFileName   = argv[9];

    // Generator object
    VRPTWInstanceGenerator generator;

    // Read data of the problem
    generator.readDistancesFile(distanceFileName.c_str());
    generator.readTimesFile(timeFileName.c_str());
    generator.readIds(idsFileName.c_str());
    generator.readPositions(idslatlngFileName.c_str());

    // Params
    generator.setSize(matrixSize);

    // Seeds to generate the data for this instance
    generator.setSeed("Matrices", seedM);
    generator.setSeed("Demands", seedD);
    generator.setSeed("Time Windows", seedTW);
    generator.setSeed("Service Times", seedST);

    // Prefix of the output files
    if (outputFileName != NULL)
        generator.setPrefix(std::string(outputFileName));


    // Read specification to build up this instance
    generator.readTimeWindowsFile(timeWindowsFile);
    generator.readDemandsFile(demandsFile);
    generator.readServiceTimesFile(timeServicesFile);

    // Actual generation of the instance
    generator.generateMatrices();

    generator.generateTimeWindows();
    generator.generateDemands();
    generator.generateServiceTimes();

    /**
     You could also use:
     g.generateAll();
     */

    generator.writeDistanceMatrix();
    generator.writeTimeMatrix();
    generator.writeSpecifications();

    /**
      You could also use:
      g.writeAll();
      */

    return 0;
}

