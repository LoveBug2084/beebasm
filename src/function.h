/*************************************************************************************************/
/**
	function.h

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

#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <cassert>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include "sourcecode.h"
#include "value.h"

class Function
{
public:

	Function( const std::string& filename, int lineNumber );


	void SetName( const std::string& name )
	{
		m_name = name;
	}

	void AddParameter( const std::string& param )
	{
		m_parameters.push_back( param );
	}

	void AddLine( const std::string& line )
	{
		m_body.push_back( line );
	}

	const std::string& GetName() const
	{
		return m_name;
	}

	int GetNumberOfParameters() const
	{
		return static_cast<int>( m_parameters.size() );
	}

	const std::string& GetParameter( int i ) const
	{
		return m_parameters[ i ];
	}

	const std::vector<std::string>& GetBody() const
	{
		return m_body;
	}

	bool HasReturnStatement() const
	{
		// Scan body for RETURN statement
		for (size_t i = 0; i < m_body.size(); i++)
		{
			std::string line = m_body[i];
			// Find first non-whitespace
			size_t pos = 0;
			while (pos < line.length() && (line[pos] == ' ' || line[pos] == '\t'))
				pos++;
			// Check if line starts with RETURN
			if (pos < line.length() && line.substr(pos, 6) == "RETURN")
				return true;
		}
		return false;
	}

	const std::string& GetFilename() const
	{
		return m_filename;
	}

	int GetLineNumber() const
	{
		return m_lineNumber;
	}

	void SetReturnValue( const Value& value )
	{
		m_returnValue = value;
	}

	const Value& GetReturnValue() const
	{
		return m_returnValue;
	}

	bool HasReturnValue() const
	{
		return m_hasReturnValue;
	}

	void SetHasReturnValue( bool hasReturn )
	{
		m_hasReturnValue = hasReturn;
	}


private:

	std::string						m_filename;
	int								m_lineNumber;

	std::string						m_name;
	std::vector< std::string >		m_parameters;
	std::vector< std::string >		m_body;
	Value							m_returnValue;
	bool							m_hasReturnValue;

};


class FunctionTable
{
public:

	static void Create();
	static void Destroy();
	static inline FunctionTable& Instance() { assert( m_gInstance != NULL ); return *m_gInstance; }

	void Add( Function* function );
	bool Exists( const std::string& name ) const;
	Function* Get( const std::string& name ) const;
	void Clear();

private:

	FunctionTable();
	~FunctionTable();

	std::map< std::string, Function* >	m_map;

	static FunctionTable*				m_gInstance;
};



#endif // FUNCTION_H_

