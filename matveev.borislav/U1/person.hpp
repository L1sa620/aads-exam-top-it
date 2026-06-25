#ifndef PERSON_HPP
#define PERSON_HPP

#include <cstddef>
#include <iosfwd>
#include <string>

namespace matveev
{
  struct Person
  {
    size_t id;
    std::string info;
  };
  bool readPerson(std::istream &in, Person &person, bool &valid);
  std::ostream &writePerson(std::ostream &out, const Person &person);
}

#endif
