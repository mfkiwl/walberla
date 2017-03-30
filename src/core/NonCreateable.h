//======================================================================================================================
//
//  This file is part of waLBerla. waLBerla is free software: you can 
//  redistribute it and/or modify it under the terms of the GNU General Public
//  License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version.
//  
//  waLBerla is distributed in the hope that it will be useful, but WITHOUT 
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
//  for more details.
//  
//  You should have received a copy of the GNU General Public License along
//  with waLBerla (see COPYING.txt). If not, see <http://www.gnu.org/licenses/>.
//
//! \file NonCreateable.h
//! \ingroup core
//! \author Christian Feichtinger
//! \author Christian Godenschwager <christian.godenschwager@fau.de>
//! \author Florian Schornbaum <florian.schornbaum@fau.de>
//
//======================================================================================================================

#pragma once



namespace walberla{

//**********************************************************************************************************************
/*!
*   \brief Ensures that any derived class can not be constructed (static member functions can still be called!)
*/
//**********************************************************************************************************************

class NonCreateable {

#ifndef _MSC_VER

   // non-MSVC-Build

private:

    NonCreateable();
   ~NonCreateable();

   NonCreateable(const NonCreateable&);
   NonCreateable& operator=(const NonCreateable&);

#else

   // MSVC-Build (eliminating warning C4624)

private:

   NonCreateable();
   NonCreateable(const NonCreateable&);

   NonCreateable& operator=(const NonCreateable&);

protected:

   ~NonCreateable();

#endif

};

}// namespace walberla