#include <cstdio>
#include "ros/file_log.h"
#include "ros/this_node.h"

#include "ros/platform.h"

#include <ros/io.h>
#include <ros/console.h>

#include <boost/filesystem.hpp>

#ifdef _MSC_VER
  #ifdef snprintf
    #undef snprintf
  #endif
  #define snprintf _snprintf_s
#endif

namespace fs = boost::filesystem;

namespace xcros
{

namespace file_log
{

std::string g_log_directory;

const std::string& getLogDirectory()
{
  return g_log_directory;
}

void init(const M_string& remappings)
{
  std::string log_file_name;
  M_string::const_iterator it = remappings.find("__log");
  if (it != remappings.end())
  {
    log_file_name = it->second;
  }

  {
    // Log filename can be specified on the command line through __log
    // If it's been set, don't create our own name
    if (log_file_name.empty())
    {
      // Setup the logfile appender
      // Can't do this in rosconsole because the node name is not known
      pid_t pid = getpid();
      std::string ros_log_env;
      if ( get_environment_variable(ros_log_env, "ROS_LOG_DIR"))
      {
        log_file_name = ros_log_env + std::string("/");
      }
      else
      {
        if ( get_environment_variable(ros_log_env, "ROS_HOME"))
        {
          log_file_name = ros_log_env + std::string("/log/");
        }
        else
        {
          // Not cross-platform?
          if( get_environment_variable(ros_log_env, "HOME") )
          {
            std::string dotros = ros_log_env + std::string("/.ros/");
            fs::create_directory(dotros);
            log_file_name = dotros + "log/";
            fs::create_directory(log_file_name);
          }
        }
      }

      // sanitize the node name and tack it to the filename
      for (size_t i = 1; i < this_node::getName().length(); i++)
      {
        if (!isalnum(this_node::getName()[i]))
        {
          log_file_name += '_';
        }
        else
        {
          log_file_name += this_node::getName()[i];
        }
      }

      char pid_str[100];
      snprintf(pid_str, sizeof(pid_str), "%d", pid);
      log_file_name += std::string("_") + std::string(pid_str) + std::string(".log");
    }

    log_file_name = fs::system_complete(log_file_name).string();
    g_log_directory = fs::path(log_file_name).parent_path().string();
  }
}

} // namespace file_log

} // namespace xcros
