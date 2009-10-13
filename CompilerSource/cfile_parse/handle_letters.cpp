/*********************************************************************************\
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
\*********************************************************************************/

#include <string>
#include <iostream>
using namespace std;
#include "../general/darray.h"
#include "../general/implicit_stack.h"
#include "../externs/externs.h"
#include "cfile_parse_constants.h"

unsigned int macrod=0;
varray<string> inmacros;

typedef implicit_stack<string> iss;
typedef implicit_stack<unsigned int> isui;

void handle_macro_pop(iss &c_file,isui &position,isui &cfile_length)
{
  c_file.pop();
  position.pop();
  cfile_length.pop();
  macrod--;
}

bool handle_macros(const string n,iss &c_file,isui &position,isui &cfile_length)
{
  maciter t;
  if ((t=macros.find(n)) != macros.end())
  {
    bool recurs=0;
    for (unsigned int iii=0;iii<macrod;iii++)
    if (inmacros[iii]==n) { recurs=1; break; }
    if (!recurs)
    {
      c_file.push();
      position.push();
      cfile_length.push();
      position() = 0;
      c_file() = t->second;
      cfile_length() = c_file().length();
      inmacros[macrod++]=n;
      return 1;
    }
  }
  return 0;
}

extern bool is_tflag(string x);
int handle_identifiers(const string n,string &cferr,string &last_identifier,unsigned int &pos,int &last_named,int &last_named_phase,externs* &last_type)
{
  //it's not a macro, so the next thing we'll check for is keyword
  if (n=="struct" or n=="class")
  {
    //Struct can only really follow typedef.
    if (last_named != LN_NOTHING)
    {
      if (last_named != LN_TYPEDEF)
      {
        cferr="Unexpected `"+n+"' token";
        return pos;
      }
      else
      {
        last_named |= LN_STRUCT;
      }
    }
    else last_named = LN_STRUCT;
    return -1;
  }
  if (n=="enum")
  {
    //Same with enum
    if (last_named != LN_NOTHING)
    {
      if (last_named != LN_TYPEDEF)
      {
        cferr="Unexpected `enum' token";
        return pos;
      }
      else
        last_named |= LN_ENUM;
    }
    else
      last_named = LN_ENUM;
    return -1;
  }
  if (n=="typedef")
  {
    //Typedef can't follow anything
    if (last_named != LN_NOTHING)
    {
      cferr="Unexpected `struct' token";
      return pos;
    }
    last_named = LN_TYPEDEF;
    return -1;
  }
  if (n=="extern")
  { //This doesn't tell us anything useful.
    return -1;
  }
  if (n=="union")
  {
    //Struct can only really follow typedef, if it's not on its own.
    if (last_named != LN_NOTHING)
    {
      if (last_named != LN_TYPEDEF)
      {
        cferr="Unexpected `struct' token";
        return pos;
      }
      else
      {
        last_named |= LN_UNION;
      }
    }
    else last_named = LN_UNION;
    return -1;
  }
  if (n=="namespace")
  {
    //Namespace can only follow using, when it's not on its own.
    if (last_named != LN_NOTHING)
    {
      if (last_named != LN_USING or last_named_phase != USE_NOTHING)
      {
        cferr="Unexpected `namespace' token";
        return pos;
      }
      last_named_phase = USE_NAMESPACE; //using namespace...
    }
    else
      last_named = LN_NAMESPACE; //namespace...
    return -1;
  }
  if (n=="explicit")
  { //This is for GCC to know, and us to just be okay with.
    return -1;
  }
  if (n=="operator")
  {
    if (last_named != LN_DECLARATOR)
    {
      cferr="Expected declarator before `operator' token";
      return pos;
    }
    if (last_named_phase<1)
    {
      cferr="Declarator before `operator' token names no type";
      return pos;
    }
    last_named=LN_OPERATOR;
    last_named_phase=0;
    cout << last_named << " = " << last_named_phase << "\r\n";
    return -1;
  }
  if (n=="new")
  {
    //New must only follow keyword "operator" or an = outside of functions
    //In the case of =, it will be skipped anyway. Check for "operator".
    if (last_named != LN_OPERATOR)
    {
      if (last_named != LN_NOTHING)
        cferr="Expect `new' token only after `operator' token or as initializer";
      else
        cferr="Expected identifier before `new' token";
      return pos;
    }
    last_named_phase = OP_NEWORDELETE;
    last_identifier = "operator new";
    return -1;
  }
  if (n=="delete")
  {
    //Delete must only follow keyword "operator" outside of functions
    if (last_named != LN_OPERATOR)
    {
      cferr="Expect `delete' token only after `operator' token";
      return pos;
    }
    last_named_phase = OP_NEWORDELETE;
    last_identifier = "operator delete";
    return -1;
  }
  if (n=="template")
  {
    if (last_named != LN_NOTHING)
    {
      cferr = "Unexpected `template' token";
      return pos;
    }
    last_named = LN_TEMPLATE;
    last_named_phase = 0;
    return -1;
  }
  if (n=="typename")
  {
    if (last_named != LN_TEMPLATE or last_named_phase != TMP_PSTART)
    {
      cferr = "Unexpected `typename' token";
      return pos;
    }
    last_named_phase = TMP_TYPENAME;
    return -1;
  }
  if (n=="class")
  if (n=="friend")
  if (n=="private")
  if (n=="public" or n=="protected" or n=="using")
  if (n=="inline")
  if (n=="virtual")
  if (n=="mutable")
  return -1;

  //Next, check if it's a type name.
  //If flow allows, this should be moved before the keywords section.

  //Check if it's a modifier
  if (is_tflag(n))
  {
    //last_typename += n + " "; Why bother
    last_type = global_scope.members.find("n")->second;
    if (last_named==LN_NOTHING)
    {
      last_named = LN_DECLARATOR;
      if (n=="long")
        last_named_phase = DEC_LONG;
      else
        last_named_phase = DEC_GENERAL_FLAG;
      return -1;
    }
    
    if ((last_named | LN_TYPEDEF) == (LN_DECLARATOR | LN_TYPEDEF))
    {
      if (last_named_phase != DEC_FULL)
      {
        if (last_named_phase != DEC_IDENTIFIER)
        {
          if (n=="long")
          {
            if (last_named_phase == 0)
              last_named_phase = DEC_LONG;
            else if (last_named_phase == DEC_LONG)
              last_named_phase = DEC_LONGLONG;
            else
            {
              if (last_named_phase == DEC_LONGLONG)
                cferr="Type is too long for GCC";
              else
                cferr="Unexpected `long' modifier at this point";
              return pos;
            }
          }
          else if (last_named_phase == 0)
            last_named_phase = DEC_GENERAL_FLAG;
        }
        else
        {
          if (refstack.currentsymbol() != '(')
          {
            cferr = "Expected ';' before new declaration";
            return pos;
          }
          return -1;
        }
      }
      else if ((last_named | LN_TYPEDEF) == (LN_TEMPLATE | LN_TYPEDEF) and (last_named_phase == TMP_EQUALS or last_named_phase == TMP_DEFAULTED))
      {
        last_named_phase = TMP_DEFAULTED;
        return -1;
      }
      else
      {
        cferr="Unexpected declarator at this point";
        return pos;
      }

      return -1;
    }
    if (last_named==LN_TYPEDEF) //if typedef is single, phase==0
    {
      last_named=LN_DECLARATOR | LN_TYPEDEF;
      if (n=="long")
        last_named_phase = DEC_LONG;
      else
        last_named_phase = DEC_GENERAL_FLAG;
      return -1;
    }
    
    if ((last_named | LN_TYPEDEF) == (LN_TEMPLATE | LN_TYPEDEF) and (last_named_phase == TMP_EQUALS or last_named_phase == TMP_DEFAULTED))
    {
      last_named_phase = TMP_DEFAULTED;
      return -1;
    }
    
    cferr="Unexpected declarator at this point";
    return pos;
  }
  
  //Check if it's a primitive or anything user defined that serves as a type.
  if (find_extname(n,EXTFLAG_TYPENAME))
  {
    if (last_named == LN_NOTHING or last_named == LN_TYPEDEF)
    {
      last_type = ext_retriever_var;
      last_named |= LN_DECLARATOR;
      last_named_phase = DEC_FULL;
      return -1;
    }
    
    //If we're declaring a variable by type
    if ((last_named | LN_TYPEDEF) == (LN_DECLARATOR | LN_TYPEDEF))
    {
      if (last_named_phase != DEC_FULL)
      {
        if (last_named_phase != DEC_IDENTIFIER)
        {
          last_type = ext_retriever_var;
          last_named_phase = DEC_FULL;
        }
        else if (refstack.currentsymbol() != '(')
        {
          cferr = "Expected ';' before new declaration";
          return pos;
        }
        return -1;
      } 
      //If error, or if it was declared in this scope
      else if (ext_retriever_var == NULL or ext_retriever_var->parent == current_scope)
      {
        cferr = ext_retriever_var == NULL ? "Type unimplemented in this scope" : "Two types named in declaration";
        return pos;
      }
      //If we made it this far, we are redeclaring something in this scope that is different in higher scopes
      //These next segments of elses are skipped, and the variable is treated like new.
    }
    //Check if we're declaring a new struct or something instead
    else 
    if ((last_named | LN_TYPEDEF) == (LN_STRUCT | LN_TYPEDEF)
    or  (last_named | LN_TYPEDEF) == (LN_ENUM   | LN_TYPEDEF)
    or  (last_named | LN_TYPEDEF) == (LN_CLASS  | LN_TYPEDEF))
    {
      //If we're not right after "struct" (or the like) or are capable of redeclaring it in this scope
      if (last_named_phase != SP_EMPTY or ext_retriever_var == NULL or ext_retriever_var->parent == current_scope)
      {
        cferr = ext_retriever_var == NULL ? "Type unimplemented, but present in this scope" : "Expected identifier";
        return pos;
      }
      //Past this point, the type will be redeclared in this scope.
    }
    //Not declaring var by type, see if we're giving a template parameter a default value
    else if ((last_named | LN_TYPEDEF) == (LN_TEMPLATE | LN_TYPEDEF) and (last_named_phase == TMP_EQUALS or last_named_phase == TMP_DEFAULTED))
    {
      last_named_phase = TMP_DEFAULTED;
      last_type = ext_retriever_var;
      return -1;
    }
    //Not declaring by type or giving default template value
    else //Note: This else is here because the above will need to pass this block     //struct a;
    {    //in the case of the current type being redeclared as scalar in this scope   //namespace b { int a; }
      cferr = "Unexpected declarator at this point";
      return pos;
    }
  }

  //Here's the big part
  //We now assume that what was named is an identifier.
  //This means we do a lot of error checking here.
  if (last_named == LN_NOTHING)
  {
    cferr="Expected type name or keyword before identifier";
    return pos;
  }
  if (last_named == LN_TYPEDEF)
  {
    cferr="Type definition does not specify a type";
    return pos;
  }

  //bool is_td = last_named & LN_TYPEDEF;
  switch (last_named & ~LN_TYPEDEF)
  {
    case LN_DECLARATOR:
        if (last_named_phase == DEC_IDENTIFIER)
        {
          if (refstack.currentsymbol() != '(')
          {
            cferr="Expected ',' or ';' before identifier";
            return pos;
          }
          refstack.inc_current();
          return -1;
        }
        last_named_phase = DEC_IDENTIFIER;
      break;
    case LN_TEMPLATE:
        if (last_named_phase == TMP_TYPENAME)
        {
          last_named_phase = TMP_IDENTIFIER;
          break;
        }
    case LN_CLASS:
    case LN_STRUCT:
    case LN_UNION:
        if (last_named_phase != 0)
        {
          cferr="Unexpected identifier in declaration";
          return pos;
        }
        last_named_phase = 1;
      break;
    case LN_ENUM:
        if (last_named_phase == 1 or last_named_phase == 3)
        {
          cferr="Expected '{' before identifier";
          return pos;
        }
        if (last_named_phase == 0)
          last_named_phase = 1;
        else if (last_named_phase == 2)
          last_named_phase = 3;
      break;
    case LN_NAMESPACE:
        if (last_named_phase == NS_NOTHING)
          last_named_phase = NS_IDENTIFIER;
        else 
        {
          if (last_named_phase == NS_EQUALS)
          cferr = "Nonexisting namspace given in synonymous namespace definition";
          else cferr = "Unexpected identifier in namespace definition";
          return pos;
        }
      break;
    case LN_OPERATOR:
        cferr = "Unexpected identifier in operator statement";
      return pos;
    case LN_USING:
        if (last_named_phase == USE_NAMESPACE_IDENTIFIER)
        {
          cferr = "Namespace to use already specified";
          return pos;
        }
        if (last_named_phase == USE_SINGLE_IDENTIFIER)
        {
          cferr = "Identifier to use already specified";
          return pos;
        }
        if (last_named_phase == USE_NAMESPACE)
        {
          if (!find_extname(n,LN_NAMESPACE))
          {
            cferr = "Expected namespace identifier following `namespace' keyword";
            return pos;
          }
          last_named_phase = USE_NAMESPACE_IDENTIFIER;
          break;
        }
        last_named_phase = USE_SINGLE_IDENTIFIER;
      break;
    default:
      cferr = "Errorness: Unspecified errorzor!@!111";
      return pos;
      //last_named = LN_IDENTIFIER;
      //last_named_phase = 0;
  }

  last_identifier = n;
  return -1;
}
