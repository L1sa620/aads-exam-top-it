#include "meeting.hpp"

#include <istream>
#include <limits>
#include <string>

namespace
{
  bool isDigit(char symbol)
  {
    return symbol >= '0' && symbol <= '9';
  }

  bool isSpace(char symbol)
  {
    return symbol == ' ' || symbol == '\t';
  }

  void skipSpaces(const std::string &line, size_t &pos)
  {
    while (pos < line.size() && isSpace(line[pos]))
    {
      ++pos;
    }
  }

  bool readSizeT(const std::string &line, size_t &pos, size_t &value)
  {
    skipSpaces(line, pos);

    if (pos == line.size() || !isDigit(line[pos]))
    {
      return false;
    }

    value = 0;

    while (pos < line.size() && isDigit(line[pos]))
    {
      size_t digit = static_cast< size_t >(line[pos] - '0');
      size_t maxValue = std::numeric_limits< size_t >::max();

      if (value > (maxValue - digit) / 10)
      {
        return false;
      }

      value = value * 10 + digit;
      ++pos;
    }

    return true;
  }

  bool readMeetingLine(const std::string &line, matveev::Meeting &meeting, bool &ignored)
  {
    ignored = false;

    size_t pos = 0;
    size_t first = 0;
    size_t second = 0;
    size_t duration = 0;

    if (!readSizeT(line, pos, first))
    {
      return false;
    }

    if (!readSizeT(line, pos, second))
    {
      return false;
    }

    if (!readSizeT(line, pos, duration))
    {
      return false;
    }

    skipSpaces(line, pos);

    if (pos != line.size())
    {
      return false;
    }

    if (first == second)
    {
      ignored = true;
      return true;
    }

    meeting.first = first;
    meeting.second = second;
    meeting.duration = duration;
    return true;
  }
}

bool matveev::readMeetings(std::istream &input, Array< Meeting > &meetings)
{
  std::string line;

  while (std::getline(input, line))
  {
    if (line.empty())
    {
      return false;
    }

    Meeting meeting;
    bool ignored = false;

    if (!readMeetingLine(line, meeting, ignored))
    {
      return false;
    }

    if (!ignored)
    {
      pushBack(meetings, meeting);
    }
  }

  return true;
}
