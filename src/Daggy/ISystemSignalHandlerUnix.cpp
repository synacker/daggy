/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Precompiled.h"
#include "ISystemSignalHandler.h"

void POSIX_handleFunc(int);
int POSIX_physicalToLogical(int);
int POSIX_logicalToPhysical(int);

namespace  {
ISystemSignalHandler* handler_global_pointer(nullptr);
}

ISystemSignalHandler::ISystemSignalHandler(const int mask) : m_mask(mask)
{
  handler_global_pointer = this;

  for (int i = 0; i < num_signals; i++)
  {
    int logical = 0x1 << i;
    if (m_mask & logical)
    {
      int sig = POSIX_logicalToPhysical(logical);
      bool failed = signal(sig, POSIX_handleFunc) == SIG_ERR;
      assert(!failed);
      (void)failed; // Silence the warning in non _DEBUG; TODO: something better
    }
  }
}

ISystemSignalHandler::~ISystemSignalHandler()
{
  for (int i=0;i<num_signals;i++)
  {
    int logical = 0x1 << i;
    if (m_mask & logical)
    {
      signal(POSIX_logicalToPhysical(logical), SIG_DFL);
    }
  }
}

int POSIX_logicalToPhysical(int signal)
{
  int result = -1;
  switch (signal)
  {
  case ISystemSignalHandler::SIG_INT:
    result = SIGINT;
    break;
  case ISystemSignalHandler::SIG_TERM:
    result = SIGTERM;
    break;
  case ISystemSignalHandler::SIG_CLOSE:
    result = SIGTERM;
    break;
  case ISystemSignalHandler::SIG_RELOAD:
    result = SIGHUP;
    break;
  default:;
  }
  return result;
}

int POSIX_physicalToLogical(int signal)
{
  int result = ISystemSignalHandler::SIG_UNHANDLED;
  switch (signal)
  {
  case SIGINT:
    result = ISystemSignalHandler::SIG_INT;
    break;
  case SIGTERM:
    result = ISystemSignalHandler::SIG_TERM;
    break;
  case SIGHUP:
    result = ISystemSignalHandler::SIG_RELOAD;
    break;
  default:;
  }
  return result;
}

void POSIX_handleFunc(int signal)
{
  if (handler_global_pointer)
  {
    const int signo = POSIX_physicalToLogical(signal);
    handler_global_pointer->handleSystemSignal(signo);
  }
}
