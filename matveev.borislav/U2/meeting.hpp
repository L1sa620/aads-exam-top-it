#ifndef MATVEEV_U2_MEETING_HPP
#define MATVEEV_U2_MEETING_HPP

#include <cstddef>
#include <iosfwd>

#include "../common/array.hpp"

namespace matveev
{
  struct Meeting
  {
    size_t first;
    size_t second;
    size_t duration;
  };

  bool readMeetings(std::istream &input, Array< Meeting > &meetings);
}

#endif
