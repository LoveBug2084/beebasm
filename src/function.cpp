/*************************************************************************************************/
/**
	function.cpp

	Function definition and management for BeebAsm


	Copyright (C) Rich Talbot-Watkins 2007 - 2012

	This file is part of BeebAsm.

	BeebAsm is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the
	License, or (at your option) any later version.

	BeebAsm is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with BeebAsm, as
	COPYING.txt.  If not, see <http://www.gnu.org/licenses/>.
*/
/*************************************************************************************************/

#include <iostream>
#include <cstring>
#include <cstdlib>

#include "function.h"


/*************************************************************************************************/
/**
	Function::Function()
*/
/*************************************************************************************************/
Function::Function( const std::string& filename, int lineNumber ) :
	m_filename( filename ),
	m_lineNumber( lineNumber ),
	m_hasReturnValue( false )
{
}


/*************************************************************************************************/
/**
	FunctionTable methods
*/
/*************************************************************************************************/
FunctionTable* FunctionTable::m_gInstance = NULL;


FunctionTable::FunctionTable()
{
}


FunctionTable::~FunctionTable()
{
	// Delete all functions
	for ( std::map< std::string, Function* >::iterator it = m_map.begin(); it != m_map.end(); ++it )
	{
		delete it->second;
	}
	m_map.clear();
}


void FunctionTable::Create()
{
	assert( m_gInstance == NULL );
	m_gInstance = new FunctionTable();
}


void FunctionTable::Destroy()
{
	assert( m_gInstance != NULL );
	delete m_gInstance;
	m_gInstance = NULL;
}


void FunctionTable::Add( Function* function )
{
	m_map[ function->GetName() ] = function;
}


bool FunctionTable::Exists( const std::string& name ) const
{
	return m_map.find( name ) != m_map.end();
}


Function* FunctionTable::Get( const std::string& name ) const
{
	std::map< std::string, Function* >::const_iterator it = m_map.find( name );
	if ( it != m_map.end() )
	{
		return it->second;
	}
	return NULL;
}


void FunctionTable::Clear()
{
	for ( std::map< std::string, Function* >::iterator it = m_map.begin(); it != m_map.end(); ++it )
	{
		delete it->second;
	}
	m_map.clear();
}

