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


#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include <string>
#include <cassert>
#include <sstream>


/**
   Function that receives a std::string and return a value converted to T.
   @param inputString is a constant reference to the string value we want to convert.
   @return value with the given value converted.
   @note this is specially useful when we read plain-text files with data.
   \code
      // This is just an example of its use.
      std::string stringNumber = "120";
      double doubleNumber = fromStringTo<double>(stringNumber);
   \endcode
*/
template <typename T>
inline T fromStringTo (const std::string& inputString)
{
   T result;
   std::istringstream stream(inputString);
   assert(stream >> result);
   return result;
   // This must be done by launching an exception.
}

/**
   Function that receives a value (T) and return a std::string containing that value.
   @param something is the value we want to convert.
   @return string with the given value.
   \code
      // This is just an example of its use.
      double doubleNumber = 120.05;
      std::string stringNumber = somethingToString<std::string>(doubleNumber);
   \endcode
*/
template <typename T>
inline std::string somethingToString(const T& something)
{
   std::ostringstream outputStream;
   outputStream << something;
   return outputStream.str();
}

#endif
