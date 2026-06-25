#include <fstream>
#include <iostream>
#include <string>

#include "meeting.hpp"

namespace matveev
{
  bool parseArguments(int argc, char **argv, bool &inSet, std::string &inName, bool &dataSet, std::string &dataName)
  {
    inSet = false;
    dataSet = false;
    inName.clear();
    dataName.clear();

    if (argc < 2 || argc > 3)
    {
      return false;
    }

    for (int i = 1; i < argc; ++i)
    {
      std::string arg(argv[i]);

      if (arg.compare(0, 3, "in:") == 0)
      {
        if (inSet)
        {
          return false;
        }

        inSet = true;
        inName = arg.substr(3);
      }
      else if (arg.compare(0, 5, "data:") == 0)
      {
        if (dataSet)
        {
          return false;
        }

        dataSet = true;
        dataName = arg.substr(5);
      }
      else
      {
        return false;
      }
    }

    return dataSet;
  }
}

int main(int argc, char **argv)
{
  bool inSet = false;
  bool dataSet = false;
  std::string inName;
  std::string dataName;

  if (!matveev::parseArguments(argc, argv, inSet, inName, dataSet, dataName))
  {
    std::cerr << "Invalid arguments\n";
    return 1;
  }

  std::ifstream inputFile;

  if (inSet)
  {
    inputFile.open(inName);

    if (!inputFile)
    {
      std::cerr << "Cannot open file\n";
      return 2;
    }
  }

  std::ifstream dataFile(dataName);

  if (!dataFile)
  {
    std::cerr << "Cannot open file\n";
    return 2;
  }

  matveev::Array< matveev::Meeting > meetings;

  if (!matveev::readMeetings(dataFile, meetings))
  {
    std::cerr << "Invalid data\n";
    return 3;
  }

  matveev::Array< matveev::Person > persons;

  if (inSet)
  {
    if (!matveev::readPersons(inputFile, persons))
    {
      std::cerr << "Invalid persons\n";
      return 3;
    }
  }

  matveev::addMeetingPersons(meetings, persons);

  std::string command;

  while (std::getline(std::cin, command))
  {
    if (
      !matveev::executePersonCommand(command, persons, std::cout) &&
      !matveev::executeMeetingCommand(command, persons, meetings, std::cout)
    )
    {
      std::cout << "<INVALID COMMAND>\n";
    }
  }

  return 0;
}
