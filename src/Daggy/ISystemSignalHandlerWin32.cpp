/*
MIT License

Copyright (c) 2020 Mikhail Milovidov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "Precompiled.h"
#include "ISystemSignalHandler.h"


BOOL WINAPI WIN32_handleFunc(DWORD);
int WIN32_physicalToLogical(DWORD);
DWORD WIN32_logicalToPhysical(int);
std::set<int> g_registry;

ISystemSignalHandler* handler_global_pointer(NULL);


ISystemSignalHandler::ISystemSignalHandler(int mask)
  : m_mask(mask)
{
  assert(handler_global_pointer == NULL);
  handler_global_pointer = this;
  SetConsoleCtrlHandler(WIN32_handleFunc, TRUE);
  for (int i = 0; i < num_signals; i++)
  {
    int logical = 0x1 << i;
    if (m_mask & logical)
    {
      g_registry.insert(logical);
    }
  }

}

ISystemSignalHandler::~ISystemSignalHandler()
{
  SetConsoleCtrlHandler(WIN32_handleFunc, FALSE);
}

DWORD WIN32_logicalToPhysical(int signal)
{
  DWORD result = ~(unsigned int)0;
  switch (signal)
  {
  case ISystemSignalHandler::SIG_INT:
    result = CTRL_C_EVENT;
    break;
  case ISystemSignalHandler::SIG_TERM:
    result = CTRL_BREAK_EVENT;
    break;
  case ISystemSignalHandler::SIG_CLOSE:
    result = CTRL_CLOSE_EVENT;
    break;
  default:;
  }
  return result;
}

int WIN32_physicalToLogical(DWORD signal)
{
  int result = ISystemSignalHandler::SIG_UNHANDLED;
  switch (signal)
  {
  case CTRL_C_EVENT:
    result = ISystemSignalHandler::SIG_INT;
    break;
  case CTRL_BREAK_EVENT:
    result = ISystemSignalHandler::SIG_TERM;
    break;
  case CTRL_CLOSE_EVENT:
    result = ISystemSignalHandler::SIG_CLOSE;
    break;
  }
  return result;
}

BOOL WINAPI WIN32_handleFunc(DWORD signal)
{
  BOOL result = FALSE;
  if (handler_global_pointer)
  {
    const int signo = WIN32_physicalToLogical(signal);
    // The std::set is thread-safe in const reading access and we never
    // write to it after the program has started so we don't need to
    // protect this search by a mutex
    std::set<int>::const_iterator found = g_registry.find(signo);
    if (signo != -1 && found != g_registry.end())
    {
      result = handler_global_pointer->handleSystemSignal(signo) ? TRUE : FALSE;
    }
  }
  return result;
}