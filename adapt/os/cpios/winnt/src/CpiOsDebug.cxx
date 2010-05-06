// Copyright (c) 2009 Mercury Federal Systems.
// 
// This file is part of OpenCPI.
// 
// OpenCPI is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// OpenCPI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with OpenCPI.  If not, see <http://www.gnu.org/licenses/>.

#include <CpiOsDebug.h>
#include <iostream>
#include <windows.h>
#include "CpiOsWin32DumpStack.h"

void
CPI::OS::dumpStack (std::ostream & out)
  throw ()
{
  Win32::dumpStack (out);
}

void
CPI::OS::debugBreak ()
  throw ()
{
  DebugBreak ();
}