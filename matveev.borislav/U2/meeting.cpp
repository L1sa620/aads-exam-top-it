#include "meeting.hpp"

#include <fstream>
#include <istream>
#include <limits>
#include <ostream>
#include <string>

namespace
{
  struct MeetingOutput
  {
    size_t id;
    size_t duration;
  };

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

  bool readWord(const std::string &line, size_t &pos, std::string &word)
  {
    skipSpaces(line, pos);

    if (pos == line.size())
    {
      return false;
    }

    size_t begin = pos;

    while (pos < line.size() && !isSpace(line[pos]))
    {
      ++pos;
    }

    word = line.substr(begin, pos - begin);
    return true;
  }

  bool isEnd(const std::string &line, size_t &pos)
  {
    skipSpaces(line, pos);
    return pos == line.size();
  }

  bool readDescription(const std::string &line, size_t &pos, std::string &description)
  {
    skipSpaces(line, pos);

    if (pos == line.size() || line[pos] != '"')
    {
      return false;
    }

    ++pos;
    size_t begin = pos;

    while (pos < line.size() && line[pos] != '"')
    {
      ++pos;
    }

    if (pos == line.size())
    {
      return false;
    }

    description = line.substr(begin, pos - begin);
    ++pos;
    return !description.empty();
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

  void writeAnons(std::ostream &out, const matveev::Array< matveev::Person > &persons)
  {
    bool hasPrevious = false;
    size_t previous = 0;

    while (true)
    {
      bool found = false;
      size_t next = 0;

      for (size_t i = 0; i < persons.size; ++i)
      {
        if (!persons.data[i].info.empty())
        {
          continue;
        }

        if (hasPrevious && persons.data[i].id <= previous)
        {
          continue;
        }

        if (!found || persons.data[i].id < next)
        {
          next = persons.data[i].id;
          found = true;
        }
      }

      if (!found)
      {
        break;
      }

      out << next << '\n';
      previous = next;
      hasPrevious = true;
    }
  }

  bool getOtherId(const matveev::Meeting &meeting, size_t id, size_t &otherId)
  {
    if (meeting.first == id)
    {
      otherId = meeting.second;
      return true;
    }

    if (meeting.second == id)
    {
      otherId = meeting.first;
      return true;
    }

    return false;
  }

  bool isDurationNeeded(size_t duration, size_t time, int mode)
  {
    if (mode == 1)
    {
      return duration < time;
    }

    if (mode == 2)
    {
      return duration > time;
    }

    return true;
  }

  bool isBefore(const MeetingOutput &lhs, const MeetingOutput &rhs)
  {
    if (lhs.id != rhs.id)
    {
      return lhs.id < rhs.id;
    }

    return lhs.duration < rhs.duration;
  }

  void swapMeetingOutput(MeetingOutput &lhs, MeetingOutput &rhs)
  {
    MeetingOutput tmp = lhs;
    lhs = rhs;
    rhs = tmp;
  }

  void sortMeetingOutput(matveev::Array< MeetingOutput > &output)
  {
    for (size_t i = 0; i < output.size; ++i)
    {
      size_t minIndex = i;

      for (size_t j = i + 1; j < output.size; ++j)
      {
        if (isBefore(output.data[j], output.data[minIndex]))
        {
          minIndex = j;
        }
      }

      if (minIndex != i)
      {
        swapMeetingOutput(output.data[i], output.data[minIndex]);
      }
    }
  }

  void writeMeetingOutput(std::ostream &out, matveev::Array< MeetingOutput > &output)
  {
    sortMeetingOutput(output);

    for (size_t i = 0; i < output.size; ++i)
    {
      out << output.data[i].id << ' ' << output.data[i].duration << '\n';
    }
  }

  void collectMeetingOutput(
    const matveev::Array< matveev::Meeting > &meetings,
    size_t id,
    size_t time,
    int mode,
    matveev::Array< MeetingOutput > &output
  )
  {
    for (size_t i = 0; i < meetings.size; ++i)
    {
      size_t otherId = 0;

      if (
        getOtherId(meetings.data[i], id, otherId) &&
        isDurationNeeded(meetings.data[i].duration, time, mode)
      )
      {
        MeetingOutput row;
        row.id = otherId;
        row.duration = meetings.data[i].duration;
        pushBack(output, row);
      }
    }
  }

  bool containsId(const matveev::Array< size_t > &ids, size_t id)
  {
    for (size_t i = 0; i < ids.size; ++i)
    {
      if (ids.data[i] == id)
      {
        return true;
      }
    }

    return false;
  }

  void collectPartners(
    const matveev::Array< matveev::Meeting > &meetings,
    size_t id,
    matveev::Array< size_t > &partners
  )
  {
    for (size_t i = 0; i < meetings.size; ++i)
    {
      size_t otherId = 0;

      if (getOtherId(meetings.data[i], id, otherId) && !containsId(partners, otherId))
      {
        pushBack(partners, otherId);
      }
    }
  }

  void sortIds(matveev::Array< size_t > &ids)
  {
    for (size_t i = 0; i < ids.size; ++i)
    {
      size_t minIndex = i;

      for (size_t j = i + 1; j < ids.size; ++j)
      {
        if (ids.data[j] < ids.data[minIndex])
        {
          minIndex = j;
        }
      }

      if (minIndex != i)
      {
        size_t tmp = ids.data[i];
        ids.data[i] = ids.data[minIndex];
        ids.data[minIndex] = tmp;
      }
    }
  }

  void writeCommonPartners(
    std::ostream &out,
    const matveev::Array< matveev::Meeting > &meetings,
    size_t firstId,
    size_t secondId
  )
  {
    matveev::Array< size_t > firstPartners;
    matveev::Array< size_t > secondPartners;
    matveev::Array< size_t > commonPartners;

    collectPartners(meetings, firstId, firstPartners);
    collectPartners(meetings, secondId, secondPartners);

    for (size_t i = 0; i < firstPartners.size; ++i)
    {
      if (containsId(secondPartners, firstPartners.data[i]) && !containsId(commonPartners, firstPartners.data[i]))
      {
        pushBack(commonPartners, firstPartners.data[i]);
      }
    }

    sortIds(commonPartners);

    for (size_t i = 0; i < commonPartners.size; ++i)
    {
      out << commonPartners.data[i] << '\n';
    }
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

bool matveev::readPersons(std::istream &input, Array< Person > &persons)
{
  while (input)
  {
    Person person;
    bool valid = false;

    if (!readPerson(input, person, valid))
    {
      return !input.bad();
    }

    if (valid && findPersonIndex(persons, person.id) == persons.size)
    {
      pushBack(persons, person);
    }
  }

  return !input.bad();
}

size_t matveev::findPersonIndex(const Array< Person > &persons, size_t id)
{
  for (size_t i = 0; i < persons.size; ++i)
  {
    if (persons.data[i].id == id)
    {
      return i;
    }
  }

  return persons.size;
}

void matveev::addMeetingPersons(const Array< Meeting > &meetings, Array< Person > &persons)
{
  for (size_t i = 0; i < meetings.size; ++i)
  {
    if (findPersonIndex(persons, meetings.data[i].first) == persons.size)
    {
      Person person;
      person.id = meetings.data[i].first;
      person.info.clear();
      pushBack(persons, person);
    }

    if (findPersonIndex(persons, meetings.data[i].second) == persons.size)
    {
      Person person;
      person.id = meetings.data[i].second;
      person.info.clear();
      pushBack(persons, person);
    }
  }
}

bool matveev::executePersonCommand(const std::string &line, Array< Person > &persons, std::ostream &out)
{
  size_t pos = 0;
  std::string command;

  if (!readWord(line, pos, command))
  {
    return false;
  }

  if (command == "anons")
  {
    if (!isEnd(line, pos))
    {
      return false;
    }

    writeAnons(out, persons);
    return true;
  }

  if (command == "desc")
  {
    size_t id = 0;

    if (!readSizeT(line, pos, id) || !isEnd(line, pos))
    {
      return false;
    }

    size_t index = findPersonIndex(persons, id);

    if (index == persons.size)
    {
      return false;
    }

    if (persons.data[index].info.empty())
    {
      out << "<ANON>\n";
    }
    else
    {
      out << persons.data[index].info << '\n';
    }

    return true;
  }

  if (command == "redesc")
  {
    size_t id = 0;
    std::string description;

    if (!readSizeT(line, pos, id) || !readDescription(line, pos, description) || !isEnd(line, pos))
    {
      return false;
    }

    size_t index = findPersonIndex(persons, id);

    if (index == persons.size)
    {
      return false;
    }

    persons.data[index].info = description;
    return true;
  }

  if (command == "out-persons")
  {
    std::string filename;

    if (!readWord(line, pos, filename) || !isEnd(line, pos))
    {
      return false;
    }

    std::ofstream output(filename);

    if (!output)
    {
      return false;
    }

    for (size_t i = 0; i < persons.size; ++i)
    {
      if (!persons.data[i].info.empty())
      {
        writePerson(output, persons.data[i]) << '\n';
      }
    }

    return static_cast< bool >(output);
  }

  return false;
}

bool matveev::executeMeetingCommand(
  const std::string &line,
  const Array< Person > &persons,
  const Array< Meeting > &meetings,
  std::ostream &out
)
{
  size_t pos = 0;
  std::string command;

  if (!readWord(line, pos, command))
  {
    return false;
  }

  if (command == "meets")
  {
    size_t id = 0;

    if (!readSizeT(line, pos, id) || !isEnd(line, pos))
    {
      return false;
    }

    if (findPersonIndex(persons, id) == persons.size)
    {
      return false;
    }

    Array< MeetingOutput > output;
    collectMeetingOutput(meetings, id, 0, 0, output);
    writeMeetingOutput(out, output);
    return true;
  }

  if (command == "commons")
  {
    size_t firstId = 0;
    size_t secondId = 0;

    if (!readSizeT(line, pos, firstId) || !readSizeT(line, pos, secondId) || !isEnd(line, pos))
    {
      return false;
    }

    if (findPersonIndex(persons, firstId) == persons.size || findPersonIndex(persons, secondId) == persons.size)
    {
      return false;
    }

    writeCommonPartners(out, meetings, firstId, secondId);
    return true;
  }

  if (command == "less" || command == "greater")
  {
    size_t time = 0;
    size_t id = 0;

    if (!readSizeT(line, pos, time) || !readSizeT(line, pos, id) || !isEnd(line, pos))
    {
      return false;
    }

    if (findPersonIndex(persons, id) == persons.size)
    {
      return false;
    }

    int mode = command == "less" ? 1 : 2;
    Array< MeetingOutput > output;
    collectMeetingOutput(meetings, id, time, mode, output);
    writeMeetingOutput(out, output);
    return true;
  }

  return false;
}
