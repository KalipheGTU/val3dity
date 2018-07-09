/*
  val3dity 

  Copyright (c) 2011-2018, 3D geoinformation research group, TU Delft  

  This file is part of val3dity.

  val3dity is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  val3dity is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with val3dity.  If not, see <http://www.gnu.org/licenses/>.

  For any information or further details about the use of val3dity, contact
  Hugo Ledoux
  <h.ledoux@tudelft.nl>
  Faculty of Architecture & the Built Environment
  Delft University of Technology
  Julianalaan 134, Delft 2628BL, the Netherlands
*/


#include "IndoorModel.h"
#include "definitions.h"
#include "input.h"
#include "Solid.h"

namespace val3dity
{

IndoorModel::IndoorModel(std::string theid)
{
  _id = theid;
  _type = "IndoorModel"; 
  _is_valid = -1;
}


IndoorModel::~IndoorModel()
{}


bool IndoorModel::validate(double tol_planarity_d2p, double tol_planarity_normals, double tol_overlap) 
{
  // 
  // 1. each Cell is valid Solid
  // 2. is dual vertex of each Cell locate inside the Cell
  // 3. is graph valid by itself (faulty xlinks?)
  // 4. do Cells overlap each others?
  // 5. adj in dual == adj in primal
  // 

  if (_is_valid != -1)
    return _is_valid;

  bool bValid = true;
//-- 1. validate each IndoorCell geometry (Solids)
  bValid = Feature::validate_generic(tol_planarity_d2p, tol_planarity_normals, tol_overlap);

//-- 2. is dual vertex of each cell located inside its Cell?
  std::clog << "======== Validating Dual Vertex (Point-in-Solid tests) ========" << std::endl;
  for (auto& el : _cells)
  {
    std::clog << "Cell ID: " << el.first << std::endl;
    Point3 p = std::get<0>(_graphs[0]->get_vertex(std::get<1>(el.second)));
    Solid* s = (Solid*)_lsPrimitives[std::get<0>(el.second)];
    int inside = s->is_point_in_solid(p);
    if (inside == -1)
    {
      std::stringstream msg;
      msg << "CellSpace id=" << el.first;
      this->add_error(702, msg.str(), "");
    }
  }

//-- 3. is dual graph valid
      
//-- bye-bye
  _is_valid = bValid;
  return bValid;
}


void IndoorModel::add_cell(std::string id, Primitive* p, std::string dual)
{
  this->add_primitive(p);
  int pos = (_lsPrimitives.size() - 1);
  _cells[id] = std::make_tuple(pos, dual);
}


void IndoorModel::add_graph(IndoorGraph* g)
{
  _graphs.push_back(g);
}


bool IndoorModel::is_valid() {
  if (_is_valid == 1) 
    return true;
  else
    return false;
}


std::string IndoorModel::get_type() {
  return _type;
}



} // namespace val3dity
