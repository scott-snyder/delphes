/*
 *  Delphes: a framework for fast simulation of a generic collider experiment
 *  Copyright (C) 2012-2014, 2025  Universite catholique de Louvain (UCL), Belgium
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** \class Delphes
 *
 *  Main Delphes module.
 *  Controls execution of all other modules.
 *
 *  \author P. Demin - UCL, Louvain-la-Neuve
 *
 */

#include "modules/Delphes.h"

#include "classes/DelphesFactory.h"

#include "ExRootAnalysis/ExRootConfReader.h"
#include "ExRootAnalysis/ExRootTreeWriter.h"

#include "TROOT.h"
#include "TRandom3.h"
#include "TString.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

Delphes::Delphes(const char *name)
{
  fModules = new TList();
  fModules->SetOwner(kTRUE);

  fItModules = fModules->MakeIterator();

  SetArrays(new TList);
  SetFactory(new DelphesFactory);

  SetName(name);
}

//------------------------------------------------------------------------------

Delphes::~Delphes()
{
  delete fItModules;
  delete fModules;

  delete GetArrays();
  delete GetFactory();
}

//------------------------------------------------------------------------------

void Delphes::Clear(Option_t * /*option*/)
{
  GetFactory()->Clear();
}

//------------------------------------------------------------------------------

void Delphes::Init()
{
  stringstream message;
  ExRootConfReader *confReader;
  ExRootConfParam param;
  DelphesModule *mod;
  TString cl, name;
  Long_t i, size;

  cout << left;
  cout << setw(30) << "** INFO: initializing module";
  cout << setw(25) << GetName() << endl;

  confReader = GetConfReader();

  if(!confReader)
  {
    message << "can't access configuration reader";
    throw runtime_error(message.str());
  }

  if(!GetTreeWriter())
  {
    message << "can't access tree writer";
    throw runtime_error(message.str());
  }

  gRandom->SetSeed(confReader->GetInt("::RandomSeed", 0));

  param = confReader->GetParam("::ExecutionPath");
  size = param.GetSize();

  for(i = 0; i < size; ++i)
  {
    name = param[i].GetString();
    cl = confReader->GetString(name + "::Class", "");
    if(cl != "")
    {
      AddModule(cl, name);
    }
    else
    {
      message << "module '" << name;
      message << "' is specified in ExecutionPath but not configured.";
      throw runtime_error(message.str());
    }
  }

  fItModules->Reset();
  while((mod = static_cast<DelphesModule *>(fItModules->Next())))
  {
    cout << left;
    cout << setw(30) << "** INFO: initializing module";
    cout << setw(25) << mod->GetName() << endl;

    mod->Init();
  }
}

//------------------------------------------------------------------------------

void Delphes::Process()
{
  DelphesModule *mod;

  fItModules->Reset();
  while((mod = static_cast<DelphesModule *>(fItModules->Next())))
  {
    mod->Process();
  }
}

//------------------------------------------------------------------------------

void Delphes::Finish()
{
  DelphesModule *mod;

  fItModules->Reset();
  while((mod = static_cast<DelphesModule *>(fItModules->Next())))
  {
    mod->Finish();
  }

  fModules->Clear();
}

//------------------------------------------------------------------------------

void Delphes::AddModule(const char *className, const char *moduleName)
{
  stringstream message;
  DelphesModule *mod;
  TClass *cl;

  cl = gROOT->GetClass(className);

  if(!cl)
  {
    message << "can't find class '" << className << "'";
    throw runtime_error(message.str());
  }

  if(!cl->InheritsFrom(DelphesModule::Class()))
  {
    message << "module '" << cl->GetName();
    message << "' does not inherit from DelphesModule";
    throw runtime_error(message.str());
  }

  mod = static_cast<DelphesModule *>(cl->New());

  mod->SetName(moduleName);
  mod->SetArrays(GetArrays());
  mod->SetConfReader(GetConfReader());
  mod->SetTreeWriter(GetTreeWriter());
  mod->SetFactory(GetFactory());

  fModules->Add(mod);
}

//------------------------------------------------------------------------------
