#ifndef MATVEEV_U2_MEETING_HPP
#define MATVEEV_U2_MEETING_HPP

#include <cstddef>
#include <iosfwd>
#include <string>

#include "../common/array.hpp"
#include "../common/person.hpp"

namespace matveev
{
  struct Meeting
  {
    size_t first;
    size_t second;
    size_t duration;
  };

  bool readMeetings(std::istream &input, Array< Meeting > &meetings);
  bool readPersons(std::istream &input, Array< Person > &persons);
  size_t findPersonIndex(const Array< Person > &persons, size_t id);
  void addMeetingPersons(const Array< Meeting > &meetings, Array< Person > &persons);
  bool executePersonCommand(const std::string &line, Array< Person > &persons, std::ostream &out);
  bool executeMeetingCommand(
  const std::string &line,
  const Array< Person > &persons,
  const Array< Meeting > &meetings,
  std::ostream &out
  );
}

#endif
