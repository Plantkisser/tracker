/// @brief service launcher

#include "service.h"

#include "debug.h"
#include "global.h"
#include "signal_handlers.h"
#include "time.h"

#include <chrono>               // std::chrono::milliseconds()
#include <exception>            // std::exception
#include <thread>               // std::this_thread::sleep_for()

static void report_state(const char* state)
{
  char date_time_string[DATE_TIME_STRING_SEC_LEN+1];
  date_time_string_sec(date_time_string, sizeof(date_time_string), false);
  IPRINTF("service '%s' (%s UTC)", state, date_time_string);
}

// argument is a path to the config
int main(int argc, const char** argv)
{
  if (argc != 2)
  {
    EPRINTF("wrong arguments");
    return 0;
  }

  int status = 0;
  report_state("starting");
  try
  {
    if (!setup_signal_handlers())
    {
      EPRINTF("'%s()' failure", "setup_signal_handlers");
      status = -1;
    }
    else
    {
      Service service(argv[1]);
      service.start();
      report_state("started");
      while (!global::shutdown)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
      }
      report_state("stopping");
      service.stop();
      status = 0;
    }
  }
  catch (const std::exception& e)
  {
    EPRINTF("exception: %s", e.what());
    status = -1;
  }
  catch (...)
  {
    EPRINTF("unexpected exception");
    status = -1;
  }
  report_state("stopped");
  return status;
}
