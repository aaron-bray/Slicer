/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLFiducialListNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:39 $
Version:   $Revision: 1.3 $

=========================================================================auto=*/
#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLScene.h"

//------------------------------------------------------------------------------
vtkMRMLFiducialListNode* vtkMRMLFiducialListNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLFiducialListNode");
  if(ret)
    {
    return (vtkMRMLFiducialListNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLFiducialListNode;
}

//-----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLFiducialListNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLFiducialListNode");
  if(ret)
    {
    return (vtkMRMLFiducialListNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLFiducialListNode;
}


//----------------------------------------------------------------------------
vtkMRMLFiducialListNode::vtkMRMLFiducialListNode()
{

  this->FiducialList = vtkCollection::New();
  this->Indent = 1;
  this->SymbolScale = 6.0;
  this->TextScale = 4.5;
  this->Visibility = 1;
  this->Color[0]=0.4; this->Color[1]=1.0; this->Color[2]=1.0;
  // from slicer 2: 1.0 0.5 0.5"
  this->SelectedColor[0]=1.0; this->SelectedColor[1]=0.5; this->SelectedColor[2]=0.5;
  this->Name = NULL;
  this->SetName("");

  this->Opacity = 1.0;
  this->Ambient = 0;
  this->Diffuse = 1.0;
  this->Specular = 0;
  this->Power = 1;

//  this->DebugOn();
}

//----------------------------------------------------------------------------
vtkMRMLFiducialListNode::~vtkMRMLFiducialListNode()
{
    if (this->FiducialList)
    {
        this->FiducialList->RemoveAllItems();
        this->FiducialList->Delete();
    }
  if (this->Name) {

      delete [] this->Name;
      this->Name = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMRMLFiducialListNode::WriteXML(ostream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  Superclass::WriteXML(of, nIndent);
  
  vtkIndent indent(nIndent);
  
  of << " symbolScale=\"" << this->SymbolScale << "\"";
  of << " textScale=\"" << this->TextScale << "\"";
  of << " visibility=\"" << this->Visibility << "\"";
  
  of << " color=\"" << this->Color[0] << " " << 
                    this->Color[1] << " " <<
                    this->Color[2] << "\"";

  of << " selectedcolor=\"" << this->SelectedColor[0] << " " << 
                    this->SelectedColor[1] << " " <<
                    this->SelectedColor[2] << "\"";
  
  of << " ambient=\"" << this->Ambient << "\"";

  of << " diffuse=\"" << this->Diffuse << "\"";

  of << " specular=\"" << this->Specular << "\"";

  of << " power=\"" << this->Power << "\"";

  of << " opacity=\"" << this->Opacity << "\"";
  
  if (this->GetNumberOfFiducials() > 0)
  {
      of << " fiducials=\"";
      for (int idx = 0; idx < this->GetNumberOfFiducials(); idx++)
      {
          if (this->GetNthFiducial(idx) != NULL)
          {
              of << "\n";
              this->GetNthFiducial(idx)->WriteXML(of, nIndent);
          }
      }
      of << "\"";
  }
}

//----------------------------------------------------------------------------
void vtkMRMLFiducialListNode::ReadXMLAttributes(const char** atts)
{

  vtkMRMLNode::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL) 
  {
      attName = *(atts++);
      attValue = *(atts++);
      if (!strcmp(attName, "name"))
      {
          this->SetName(attValue);
      }
      else if (!strcmp(attName, "id"))
      {
          this->SetID(attValue);
      }
      else  if (!strcmp(attName, "color")) 
      {
          std::stringstream ss;
          ss << attValue;
          ss >> this->Color[0];
          ss >> this->Color[1];
          ss >> this->Color[2];
      }
      else if (!strcmp(attName, "selectedcolor"))
      {
          std::stringstream ss;
          ss << attValue;
          ss >> this->SelectedColor[0];
          ss >> this->SelectedColor[1];
          ss >> this->SelectedColor[2];
      }
      else if (!strcmp(attName, "symbolScale")) 
      {
          std::stringstream ss;
          ss << attValue;
          ss >> this->SymbolScale;
      }
      else if (!strcmp(attName, "textScale")) 
      {
          std::stringstream ss;
          ss << attValue;
          ss >> this->TextScale;
      }
      else if (!strcmp(attName, "visibility")) 
      {
          std::stringstream ss;
          ss << attValue;
          ss >> this->Visibility;
      }
      else if (!strcmp(attName, "ambient")) 
      {
          std::stringstream ss;
          ss << attValue;
          ss >> Ambient;
      }
      else if (!strcmp(attName, "diffuse")) 
      {
          std::stringstream ss;
          ss << attValue;
          ss >> Diffuse;
      }
      else if (!strcmp(attName, "specular")) 
      {
          std::stringstream ss;
          ss << attValue;
          ss >> Specular;
      }
      else if (!strcmp(attName, "power")) 
      {
          std::stringstream ss;
          ss << attValue;
          ss >> Power;
      }
      else if (!strcmp(attName, "opacity")) 
      {
          std::stringstream ss;
          ss << attValue;
          ss >> Opacity;
      }
      else if (!strcmp(attName, "fiducials"))
      {
          
          // need to add fiducials and parse out the list of fiducial points
          // assume labeltext is first, extract that part of the attValue
          char *fiducials = (char *)attValue;
          char *labelTextPtr;
          labelTextPtr = strstr (fiducials,"labeltext");
          //std::cout << "Starting to parse out the fiducial list, setting it up for tokenisation\n";
          while (labelTextPtr != NULL)
          {
              //std::cout << "current label text pt = " << labelTextPtr << endl;
              
              // find the end of this point, new line or end quote
              labelTextPtr = strstr (fiducials," labeltext");
              if (labelTextPtr != NULL)
              {
                  // replace the space with a carriage return
                  labelTextPtr = strncpy(labelTextPtr, "\nlabeltext", 1);
              }
          }
          // now parse the string into tokens by the newline
          labelTextPtr = strtok(fiducials, "\n");
          while (labelTextPtr != NULL)
          {
              //std::cout << "got a token: " << labelTextPtr << endl;
              // now make a new point
              int pointIndex = this->AddFiducial();
              vtkMRMLFiducial *newPoint = this->GetNthFiducial(pointIndex);
              if (newPoint != NULL)
              {
                  // now pass it the stuff to parse out and set itself from
                  newPoint->ReadXMLString(labelTextPtr);
                  // delete it since the list has a pointer to it
                  newPoint->Delete();
              } else {
                  std::cerr << "ERROR making a new MRML fiducial!\n";
              }
              labelTextPtr = strtok(NULL, "\n");
          }
      }
      else
      {
          std::cerr << "Unknown attribute name " << attName << endl;
      }
  }
  vtkDebugMacro("Finished reading in xml attributes, list id = " << this->GetID() << " and name = " << this->GetName() << endl);
}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, ID
void vtkMRMLFiducialListNode::Copy(vtkMRMLNode *anode)
{
  vtkMRMLNode::Copy(anode);
  vtkMRMLFiducialListNode *node = (vtkMRMLFiducialListNode *) anode;

  this->SetName(node->Name);
  this->SetColor(node->Color);
  this->SetSelectedColor(node->SelectedColor);
  this->SetSymbolScale(node->SymbolScale);
  this->SetTextScale(node->TextScale);
  this->SetVisibility(node->Visibility);

  this->SetOpacity(node->Opacity);
  this->SetAmbient(node->Ambient);
  this->SetDiffuse(node->Diffuse);
  this->SetSpecular(node->Specular);
  this->SetPower(node->Power);
}

//----------------------------------------------------------------------------
void vtkMRMLFiducialListNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "Name: " <<
      (this->Name ? this->Name : "(none)") << "\n";
  
  os << indent << "Symbol scale: (";
  os << indent << this->SymbolScale << ")\n";

  os << indent << "Text scale: (";
  os << indent << this->TextScale << ")\n";

  os << indent << "Visibility: (";
  os << indent << this->Visibility << ")\n";

  os << indent << "Color: (";
  for (idx = 0; idx < 3; ++idx)
    {
        os << indent << this->Color[idx];
        if (idx < 2) { os << ", "; } else { os << ")\n"; }
    }
  os << indent << "Selected color: (";
  for (idx = 0; idx < 3; ++idx)
    {
        os << indent << this->SelectedColor[idx];
        if (idx < 2) { os << ", "; } else { os << ")\n"; }
    }
  
  os << indent << "Opacity:  (" << this->Opacity << ")\n";
  os << indent << "Ambient:  (" << this->Ambient << ")\n";
  os << indent << "Diffuse:  (" << this->Diffuse << ")\n";
  os << indent << "Specular: (" << this->Specular << ")\n";
  os << indent << "Power:    (" << this->Power << ")\n";
  
  if (this->GetNumberOfFiducials() > 0)
  {
      os << indent << "Fiducial points:\n";
      for (idx = 0; idx < this->GetNumberOfFiducials(); idx++)
      {
          os << indent << " Point " << idx << ":\n";
          if (this->GetNthFiducial(idx) != NULL)
          {
              this->GetNthFiducial(idx)->PrintSelf(os,indent.GetNextIndent());
          }
      }
  }
}

//-----------------------------------------------------------

void vtkMRMLFiducialListNode::UpdateScene(vtkMRMLScene *scene)
{
    Superclass::UpdateScene(scene);
    /*
    if (this->GetStorageNodeID() == NULL) 
    {
        //vtkErrorMacro("No reference StorageNodeID found");
        return;
    }

    vtkMRMLNode* mnode = scene->GetNodeByID(this->StorageNodeID);
    if (mnode) 
    {
        vtkMRMLStorageNode *node  = dynamic_cast < vtkMRMLStorageNode *>(mnode);
        node->ReadData(this);
        //this->SetAndObservePolyData(this->GetPolyData());
    }
    */
}

//----------------------------------------------------------------------------
int vtkMRMLFiducialListNode::GetNumberOfFiducials()
{
    return this->FiducialList->vtkCollection::GetNumberOfItems();
}

//-----------------------------------------------------------
vtkMRMLFiducial* vtkMRMLFiducialListNode::GetNthFiducial(int n)
{
    vtkDebugMacro("GetNthFiducial: getting item by index number: " << n);
    if (this->FiducialList == NULL)
    {
        std::cerr << "GetNthFiducial: ERROR: fiducial list is null\n";
        return NULL;
    }
  if(n < 0 || n >= this->FiducialList->GetNumberOfItems()) 
    {
        std::cerr << "vtkMRMLFiducialListNode::GetNthFiducial: index out of bounds, " << n << " is less than zero or more than the number of items: " << this->FiducialList->GetNumberOfItems() << endl;
        return NULL;
    }
  else 
    {
    return (vtkMRMLFiducial*)this->FiducialList->GetItemAsObject(n);
    }
}


//----------------------------------------------------------------------------
int vtkMRMLFiducialListNode::SetNthFiducialXYZ(int n, float x, float y, float z)
{
    vtkMRMLFiducial *node = this->GetNthFiducial(n);
    if (node == NULL)
    {
        vtkErrorMacro("Unable to get fiducial number " << n);
        return 1;
    }
    node->SetXYZ(x,y,z);
    
    // the list contents have been modified
    this->InvokeEvent(vtkMRMLFiducialListNode::FiducialModifiedEvent, NULL);
    return 0;
}

//----------------------------------------------------------------------------
float * vtkMRMLFiducialListNode::GetNthFiducialXYZ(int n)
{
    vtkMRMLFiducial *node = this->GetNthFiducial(n);
    if (node != NULL)
    {
        return node->GetXYZ();
    }
    else
    {
        return NULL;
    }
}

//----------------------------------------------------------------------------
int vtkMRMLFiducialListNode::SetNthFiducialOrientation(int n, float w, float x, float y, float z)
{
    vtkMRMLFiducial *node = this->GetNthFiducial(n);
    if (node == NULL)
    {
        vtkErrorMacro("Unable to get fiducial number " << n);
        return 1;
    }
    node->SetOrientationWXYZ(w, x, y, z);
    
    // the list contents have been modified
    this->InvokeEvent(vtkMRMLFiducialListNode::FiducialModifiedEvent, NULL);
    return 0;
}

//----------------------------------------------------------------------------
float * vtkMRMLFiducialListNode::GetNthFiducialOrientation(int n)    
{
    vtkMRMLFiducial *node = this->GetNthFiducial(n);
    if (node != NULL)
    {
        return node->GetOrientationWXYZ();
    }
    else
    {
        return NULL;
    }
}

//----------------------------------------------------------------------------
int vtkMRMLFiducialListNode::SetNthFiducialLabelText(int n, const char *text)
{
    vtkMRMLFiducial *node = this->GetNthFiducial(n);
    if (node == NULL)
    {
        vtkErrorMacro("Unable to get fiducial number " << n);
        return 1;
    }
    node->SetLabelText(text);
    
    // the list contents have been modified
    this->InvokeEvent(vtkMRMLFiducialListNode::FiducialModifiedEvent, NULL);
    return 0;
}

//----------------------------------------------------------------------------
const char *vtkMRMLFiducialListNode::GetNthFiducialLabelText(int n)
{
    vtkMRMLFiducial *node = this->GetNthFiducial(n);
    if (node != NULL)
    {
        return node->GetLabelText();
    }
    else
    {
        return "(none)";
    }
}

//----------------------------------------------------------------------------
int vtkMRMLFiducialListNode::SetNthFiducialSelected(int n, int flag)
{
    vtkMRMLFiducial *node = this->GetNthFiducial(n);
    if (node == NULL)
    {
        vtkErrorMacro("Unable to get fiducial number " << n);
        return 1;
    }
    node->SetSelected((flag == 0 ? false : true));
    
    // the list contents have been modified
    this->InvokeEvent(vtkMRMLFiducialListNode::FiducialModifiedEvent, NULL);
    return 0;
}

//----------------------------------------------------------------------------
int vtkMRMLFiducialListNode::GetNthFiducialSelected(int n)
{
    vtkMRMLFiducial *node = this->GetNthFiducial(n);
    if (node != NULL)
    {
        return (node->GetSelected() ? 1 : 0);
    }
    else
    {
        return 0;
    }
}

//----------------------------------------------------------------------------
int vtkMRMLFiducialListNode::SetNthFiducialID(int n, const char *id)
{
    vtkMRMLFiducial *node = this->GetNthFiducial(n);
    if (node == NULL)
    {
        vtkErrorMacro("Unable to get fiducial number " << n);
        return 1;
    }
    node->SetID(id);
    
    // the list contents have been modified
    this->InvokeEvent(vtkMRMLFiducialListNode::FiducialModifiedEvent, NULL);
    return 0;
}

//----------------------------------------------------------------------------
const char *vtkMRMLFiducialListNode::GetNthFiducialID(int n)
{
    vtkMRMLFiducial *node = this->GetNthFiducial(n);
    if (node != NULL)
    {
        return node->GetID();
    }
    else
    {
        return "(none)";
    }
}

//----------------------------------------------------------------------------
int vtkMRMLFiducialListNode::AddFiducial()
{
  if ( !this->Scene ) 
    {
    vtkErrorMacro ( << "Attempt to add Fiducial, but no scene set yet");
    return (-1);
    }

  // create a vtkMRMLFiducial and return the fiducial number for later
  // access
  vtkMRMLFiducial * fiducial = vtkMRMLFiducial::New();

  // give the point a unique name based on the list name
  std::stringstream ss;
  ss << this->GetName();
  ss << "-P";
  std::string nameString;
  ss >> nameString;
  fiducial->SetID(this->GetScene()->GetUniqueIDByClass(nameString.c_str()));
  // use the same for the label text for now
  fiducial->SetLabelText(fiducial->GetID());
  
  // add it to the collection
  this->FiducialList->vtkCollection::AddItem(fiducial);
  int itemIndex = this->FiducialList->vtkCollection::IsItemPresent(fiducial);
  // decrement the index, because GetNthFiducial needs a 0 based array
  // index, IsItemPresent returns a 1 based array index
  itemIndex--;

  // then delete it, the collection has registered it and will keep track of
  // it
  fiducial->Delete();

  // let observers know that the node was added
  this->InvokeEvent(vtkMRMLScene::NodeAddedEvent, NULL);
  // this list is now modified...
  this->Modified();

  // return an index for use in getting the item again via GetNthFiducial
  vtkDebugMacro("AddFiducial: added a fiducial to the list at index " << itemIndex << endl);
  return itemIndex;
}
/*
//----------------------------------------------------------------------------
void vtkMRMLFiducialListNode::AddFiducial(vtkMRMLFiducial *o)
{
    if (o != NULL)
    {
        vtkDebugMacro("vtkMRMLFiducialListNode::AddFiducial: adding fiducial " << o->GetLabelText());
    }
    this->FiducialList->vtkCollection::AddItem(o);
    this->Modified();
}
*/

//----------------------------------------------------------------------------
void vtkMRMLFiducialListNode::RemoveFiducial(vtkMRMLFiducial *o)
{
    if (o != NULL)
    {
        vtkDebugMacro("vtkMRMLFiducialListNode::RemoveFiducial: removing fiducial " << o->GetLabelText());
    }
    this->FiducialList->vtkCollection::RemoveItem(o);
    // let interested observers know that a fiducial was removed
    this->InvokeEvent(vtkMRMLScene::NodeRemovedEvent, NULL);
    this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLFiducialListNode::RemoveFiducial(int i)
{
    this->FiducialList->vtkCollection::RemoveItem(i);
    this->InvokeEvent(vtkMRMLScene::NodeRemovedEvent, NULL);
    this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLFiducialListNode::RemoveAllFiducials()
{
    int numPoints = this->GetNumberOfFiducials();
    for (int f = numPoints - 1; f >= 0; f--)
    {
        // as remove them from the end of the list, the size of the list
        // will shrink as the iterator f reduces
        this->FiducialList->vtkCollection::RemoveItem(f);
        this->InvokeEvent(vtkMRMLScene::NodeRemovedEvent, NULL);
    }
    this->Modified();
}

//----------------------------------------------------------------------------
int vtkMRMLFiducialListNode::IsFiducialPresent(vtkMRMLFiducial *o)
{
    return this->FiducialList->vtkCollection::IsItemPresent(o);
}

//-----------------------------------------------------------
void vtkMRMLFiducialListNode::UpdateReferences()
{
   Superclass::UpdateReferences();

/*
  if (this->DisplayNodeID != NULL && this->Scene->GetNodeByID(this->DisplayNodeID) == NULL)
    {
    this->SetAndObserveDisplayNodeID(NULL);
    }
*/
}
/*
//----------------------------------------------------------------------------
vtkMRMLFiducialListDisplayNode* vtkMRMLFiducialListNode::GetDisplayNode()
{
  vtkMRMLFiducialListDisplayNode* node = NULL;
  if (this->GetScene() && this->GetDisplayNodeID() )
    {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID(this->DisplayNodeID);
    node = vtkMRMLFiducialListDisplayNode::SafeDownCast(snode);
    }
  return node;
}

//----------------------------------------------------------------------------
void vtkMRMLFiducialListNode::SetAndObserveDisplayNodeID(const char *displayNodeID)
{
  if (this->DisplayNodeID != NULL)
    {
    vtkMRMLFiducialListDisplayNode *dnode = this->GetDisplayNode();
    if (dnode != NULL)
      {
      dnode->RemoveObservers ( vtkCommand::ModifiedEvent, this->MRMLCallbackCommand );
      }
    }
  this->SetDisplayNodeID(displayNodeID);
  vtkMRMLFiducialListDisplayNode *dnode = this->GetDisplayNode();
  if (dnode != NULL) 
    {
    dnode->AddObserver ( vtkCommand::ModifiedEvent, this->MRMLCallbackCommand );
    }
}
*/
//---------------------------------------------------------------------------
void vtkMRMLFiducialListNode::ProcessMRMLEvents ( vtkObject *caller,
                                           unsigned long event, 
                                           void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);
/*
  vtkMRMLFiducialListDisplayNode *dnode = this->GetDisplayNode();
  if (dnode != NULL && dnode == vtkMRMLFiducialListDisplayNode::SafeDownCast(caller) &&
      event ==  vtkCommand::ModifiedEvent)
    {
        this->InvokeEvent(vtkMRMLFiducialListNode::DisplayModifiedEvent, NULL);
    }
*/
  // check for one of the fiducials being modified, if so, trigger a modified
  // event on the list
  return;
}

//---------------------------------------------------------------------------
void vtkMRMLFiducialListNode::SetColor(double r, double g, double b)
{
    if (this->Color[0] == r &&
        this->Color[1] == g &&
        this->Color[2] == b)
    {
        return;
    }
    vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting Color to " << r << " " << g << " " << b); 
    this->Color[0] = r;
    this->Color[1] = g;
    this->Color[2] = b;

    // invoke a modified event
    this->Modified();
    
    // invoke a display modified event
    this->InvokeEvent(vtkMRMLFiducialListNode::DisplayModifiedEvent);
}

//---------------------------------------------------------------------------
void vtkMRMLFiducialListNode::SetSelectedColor(double r, double g, double b)
{
    if (this->SelectedColor[0] == r &&
        this->SelectedColor[1] == g &&
        this->SelectedColor[2] == b)
    {
        return;
    }
    vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting SelectedColor to " << r << " " << g << " " << b); 
    this->SelectedColor[0] = r;
    this->SelectedColor[1] = g;
    this->SelectedColor[2] = b;

    // invoke a modified event
    this->Modified();
    
    // invoke a display modified event
    this->InvokeEvent(vtkMRMLFiducialListNode::DisplayModifiedEvent);
}

//---------------------------------------------------------------------------
void vtkMRMLFiducialListNode::SetColor(double c[3])
{
    // set the colour
    this->SetColor(c[0], c[1], c[2]);
}

//---------------------------------------------------------------------------
void vtkMRMLFiducialListNode::SetSelectedColor(double c[3])
{
    // set the selected colour
    this->SetSelectedColor(c[0], c[1], c[2]);
}

//---------------------------------------------------------------------------
void vtkMRMLFiducialListNode::SetTextScale(double scale)
{
    if (this->TextScale == scale)
    {
        return;
    }
    vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting TextScale to " << scale);
    this->TextScale = scale;
   
    // invoke a modified event
    this->Modified();
    
    // invoke a display modified event
    this->InvokeEvent(vtkMRMLFiducialListNode::DisplayModifiedEvent);
}

//---------------------------------------------------------------------------
void vtkMRMLFiducialListNode::SetSymbolScale(double scale)
{
    if (this->SymbolScale == scale)
    {
        return;
    }
    vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting SymbolScale to " << scale);
    this->SymbolScale = scale;
   
    // invoke a modified event
    this->Modified();
    
    // invoke a display modified event
    this->InvokeEvent(vtkMRMLFiducialListNode::DisplayModifiedEvent);
}

//---------------------------------------------------------------------------
void vtkMRMLFiducialListNode::SetVisibility(int visible)
{
    if (this->Visibility == visible)
    {
        return;
    }
    vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting Visibility to " << visible);
    this->Visibility = visible;
   
    // invoke a modified event
    this->Modified();
    
    // invoke a display modified event
    this->InvokeEvent(vtkMRMLFiducialListNode::DisplayModifiedEvent);
}

//---------------------------------------------------------------------------
void vtkMRMLFiducialListNode::SetOpacity(double opacity)
{
    if (this->Opacity == opacity)
    {
        return;
    }
    vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting Opacity to " << opacity);
    this->Opacity = opacity;
   
    // invoke a modified event
    this->Modified();
    
    // invoke a display modified event
    this->InvokeEvent(vtkMRMLFiducialListNode::DisplayModifiedEvent);
}
