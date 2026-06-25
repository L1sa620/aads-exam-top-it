#include <iostream>

#include "person.hpp"

int main()
{
  matveev::Person person;
  bool valid = false;
  while (matveev::readPerson(std::cin, person, valid))
  {
    if (valid)
    {
      matveev::writePerson(std::cout, person);
      std::cout << '\n';
    }
  }
  return 0;
}
