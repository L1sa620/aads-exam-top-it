#include <fstream>
#include <iostream>
#include <string>

#include "array.hpp"
#include "person.hpp"

namespace matveev
{
  bool parseArguments(int argc, char **argv, bool &inSet, std::string &inName, bool &outSet, std::string &outName)
  {
    inSet = false;
    outSet = false;
    inName.clear();
    outName.clear();

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
      else if (arg.compare(0, 4, "out:") == 0)
      {
        if (outSet)
        {
          return false;
        }

        outSet = true;
        outName = arg.substr(4);
      }
      else
      {
        return false;
      }
    }

    return true;
  }

  void writePeople(std::ostream &out, const Array< Person > &people)
  {
    if (people.size == 0)
    {
      out << '\n';
      return;
    }

    for (size_t i = 0; i < people.size; ++i)
    {
      writePerson(out, people.data[i]);
      out << '\n';
    }
  }
}

int main(int argc, char **argv)
{
  if (argc > 3)
  {
    std::cerr << "Invalid arguments\n";
    return 0;
  }

  bool inSet = false;
  bool outSet = false;
  std::string inName;
  std::string outName;

  if (!matveev::parseArguments(argc, argv, inSet, inName, outSet, outName))
  {
    std::cerr << "Invalid arguments\n";
    return 1;
  }

  std::ifstream inputFile;
  std::istream *input = &std::cin;

  if (inSet)
  {
    inputFile.open(inName);
    if (!inputFile)
    {
      std::cerr << "Cannot open file\n";
      return 2;
    }

    input = &inputFile;
  }

  matveev::Array< matveev::Person > people;
  size_t validCount = 0;
  size_t ignoredCount = 0;
  matveev::Person person;
  bool valid = false;

  while (matveev::readPerson(*input, person, valid))
  {
    if (valid)
    {
      bool duplicate = false;

      for (size_t i = 0; i < people.size; ++i)
      {
        if (people.data[i].id == person.id)
        {
          duplicate = true;
          break;
        }
      }

      if (duplicate)
      {
        ++ignoredCount;
      }
      else
      {
        matveev::pushBack(people, person);
        ++validCount;
      }
    }
    else
    {
      ++ignoredCount;
    }
  }

  if (inSet)
  {
    inputFile.close();
  }

  std::cerr << validCount << ' ' << ignoredCount << '\n';

  if (outSet)
  {
    std::ofstream outputFile(outName);
    if (!outputFile)
    {
      std::cerr << "Cannot open file\n";
      return 2;
    }

    matveev::writePeople(outputFile, people);

    std::cout << "in file " << outName << '\n';
    matveev::writePeople(std::cout, people);
  }
  else
  {
    matveev::writePeople(std::cout, people);
  }

  return 0;
}
