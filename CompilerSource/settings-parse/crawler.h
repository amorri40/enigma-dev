/********************************************************************************\
**                                                                              **
**  Copyright (C) 2008 Josh Ventura                                             **
**                                                                              **
**  This file is a part of the ENIGMA Development Environment.                  **
**                                                                              **
**                                                                              **
**  ENIGMA is free software: you can redistribute it and/or modify it under the **
**  terms of the GNU General Public License as published by the Free Software   **
**  Foundation, version 3 of the license or any later version.                  **
**                                                                              **
**  This application and its source code is distributed AS-IS, WITHOUT ANY      **
**  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS   **
**  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more       **
**  details.                                                                    **
**                                                                              **
**  You should have recieved a copy of the GNU General Public License along     **
**  with this code. If not, see <http://www.gnu.org/licenses/>                  **
**                                                                              **
**  ENIGMA is an environment designed to create games and other programs with a **
**  high-level, fully compilable language. Developers of ENIGMA or anything     **
**  associated with ENIGMA are in no way responsible for its users or           **
**  applications created by its users, or damages caused by the environment     **
**  or programs made in the environment.                                        **
**                                                                              **
\********************************************************************************/

#include <list>
#include <map>

namespace settings
{
  struct system_descriptor
  {
    string name;
  };
  extern list<string> systems;
  int scour_settings();
}

namespace extensions
{
  string compile_local_string();
  void dump_read_locals(map<string,int>&);
  void crawl_for_locals();
  void determine_target();
  
  struct os_descriptor
  {
    string 
      name, identifier, represents, description, author,
      build_platforms, build_extension,
      run_program, run_params;
  };
  struct api_descriptor
  {
    string
      windowSys, graphicsSys, collisionSys;
  };
  extern os_descriptor targetOS;
  extern api_descriptor targetAPI;
}
