/// @brief service

#include "service.h"
#include "debug.h"


#include "preprocessor.h"	// toUI()

// FIXME: Do something useful
void Service::onError(const char* origin, const char* errorMessage)
{
  EPRINTF("%s: %s", origin, errorMessage);
}

// TODO: send 'event' to 'appliance' for processing
// Meanwhile block each 3-rd event
bool Service::onEvent(tracer::event::Type EventType, const void* eventData)
{

  static unsigned event_count = 0;
  DPRINTF("event[%u]: %u/%s, %p", event_count, toUI(EventType), tracer::event::toString(EventType), eventData);

  const char* path = NULL;
  switch(EventType)
  {
    case tracer::event::Type::OPEN:
      IPRINTF("OPEN");
    case tracer::event::Type::OPENAT:
    {
      path = ((tracer::event::data:: Open*) eventData)->data_path; 
      break;
    }
    case tracer::event::Type::EXEC:
    {
      IPRINTF("OPEN");
      path = ((tracer::event::data:: Exec*) eventData)->child_path;
      break;
    }
    default:
      path = NULL;
      break;
  }


  return controller_.is_allowed(EventType, path);
}

//config file conatin ip address and port of the server
//maybe controller_ should start in Service::start
Service::Service(const char* config_file): // may throw
controller_(config_file)
{
  DPRINTF9("'service' created");
}

Service::~Service() // shall not throw
{
  stop();
  DPRINTF9("destroying 'service'");
}

void Service::start() // may throw
{
  std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
  if (tracer_)
  {
    WPRINTF("tracer is already active");
  }
  else
  {
    IPRINTF("activation tracer");
    tracer_.reset(new tracer::Tracer(*static_cast<tracer::event::source::Observer*>(this)));
  }
}

void Service::stop() // shall not throw
{
  std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
  if (!tracer_)
  {
    DPRINTF("tracer is already inactive");
  }
  else
  {
    IPRINTF("deactivation tracer");
    tracer_.reset();
  }
}
