#include "person.hpp"

#include <istream>
#include <ostream>

std::istream &matveev::readPerson(std::istream &in, Person &person, bool &valid)
{
  valid = false;
  size_t id = 0;
  if (in >> id)
  {
    while (in.peek() == ' ' || in.peek() == '\t')
    {
      in.get();
    }
    std::string info;
    std::getline(in, info);
    if (!info.empty())
    {
      person.id = id;
      person.info = info;
      valid = true;
    }
  }
  else if (!in.eof())
  {
    in.clear();
    std::string skip;
    std::getline(in, skip);
  }
  return in;
}

std::ostream &matveev::writePerson(std::ostream &out, const Person &person)
{
  out << person.id << ' ' << person.info;
  return out;
}
