#include "names.h"
#include "this_node.h"
#include "file_log.h"

#include <ros/console.h>
#include <ros/assert.h>

#include <cstring>

namespace xcros
{
namespace names
{

M_string g_remappings;
M_string g_unresolved_remappings;

const M_string& getRemappings()
{
  return g_remappings;
}

const M_string& getUnresolvedRemappings()
{
  return g_unresolved_remappings;
}

bool isValidCharInName(char c)
{
  if (isalnum(c) || c == '/' || c == '_')
  {
    return true;
  }

  return false;
}

bool validate(const std::string& name, std::string& error)
{
  if (name.empty())
  {
    return true;
  }

  // First element is special, can be only ~ / or alpha
  char c = name[0];
  if (!isalpha(c) && c != '/' && c != '~')
  {
    std::stringstream ss;
    ss << "Character [" << c << "] is not valid as the first character in Graph Resource Name [" << name << "].  Valid characters are a-z, A-Z, / and in some cases ~.";
    error = ss.str();
    return false;
  }

  for (size_t i = 1; i < name.size(); ++i)
  {
    c = name[i];
    if (!isValidCharInName(c))
    {
      std::stringstream ss;
      ss << "Character [" << c << "] at element [" << i << "] is not valid in Graph Resource Name [" << name <<"].  Valid characters are a-z, A-Z, 0-9, / and _.";
      error = ss.str();

      return false;
    }
  }

  return true;
}

std::string clean(const std::string& name)
{
  std::string clean = name;

  size_t pos = clean.find("//");
  while (pos != std::string::npos)
  {
    clean.erase(pos, 1);
    pos = clean.find("//", pos);
  }

  if (*clean.rbegin() == '/')
  {
    clean.erase(clean.size() - 1, 1);
  }

  return clean;
}

std::string append(const std::string& left, const std::string& right)
{
  return clean(left + "/" + right);
}

std::string remap(const std::string& name)
{
  std::string resolved = resolve(name, false);

  M_string::const_iterator it = g_remappings.find(resolved);
  if (it != g_remappings.end())
  {
    return it->second;
  }

  return name;
}

std::string resolve(const std::string& name, bool _remap)
{
  std::string s = resolve(this_node::getNamespace(), name, _remap);
  return s;
}

std::string resolve(const std::string& ns, const std::string& name, bool _remap)
{
  std::string error;
  if (!validate(name, error))
  {
  	throw InvalidNameException(error);
  }

  if (name.empty())
  {
    if (ns.empty())
    {
      return "/";
    }

    if (ns[0] == '/')
    {
      return ns;
    }

    return append("/", ns);
  }

  std::string copy = name;

  if (copy[0] == '~')
  {
    copy = append(this_node::getName(), copy.substr(1));
  }

  if (copy[0] != '/')
  {
    copy = append("/", append(ns, copy));
  }

  copy = clean(copy);

  if (_remap)
  {
    copy = remap(copy);
  }

  return copy;
}

void init(const M_string& remappings)
{
  M_string::const_iterator it = remappings.begin();
  M_string::const_iterator end = remappings.end();
  for (; it != end; ++it)
  {
    const std::string& left = it->first;
    const std::string& right = it->second;

    if (!left.empty() && left[0] != '_' && left != this_node::getName())
    {
      std::string resolved_left = resolve(left, false);
      std::string resolved_right = resolve(right, false);
      g_remappings[resolved_left] = resolved_right;
      g_unresolved_remappings[left] = right;
    }
  }
}

std::string parentNamespace(const std::string& name)
{
  std::string error;
  if (!validate(name, error))
  {
  	throw InvalidNameException(error);
  }

  if (!name.compare(""))  return "";
  if (!name.compare("/")) return "/"; 

  std::string stripped_name;

  // rstrip trailing slash
  if (name.find_last_of('/') == name.size()-1)
    stripped_name = name.substr(0, name.size() -2);
  else
    stripped_name = name;

  //pull everything up to the last /
  size_t last_pos = stripped_name.find_last_of('/');
  if (last_pos == std::string::npos)
  {
    return "";
  }
  else if (last_pos == 0)
    return "/";
  return stripped_name.substr(0, last_pos);
}

} // namespace names
}