/*************************************************************************************************/
/**
	objectcode.cpp


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

#include <cstring>
#include <iostream>
#include <fstream>

#include "objectcode.h"
#include "symboltable.h"
#include "asmexception.h"
#include "globaldata.h"


ObjectCode* ObjectCode::m_gInstance = NULL;


using namespace std;


/*************************************************************************************************/
/**
	ObjectCode::Create()

	Creates the ObjectCode singleton
*/
/*************************************************************************************************/
void ObjectCode::Create()
{
	assert( m_gInstance == NULL );

	m_gInstance = new ObjectCode;
}


void ObjectCode::StartRecordingIfNeeded()
{
	if ( m_bRecording ) return;

	// Initialise record buffers and copy any bytes already written this pass (USED)
	memset( m_aMemoryRecord, 0, sizeof m_aMemoryRecord );
	memset( m_aFlagsRecord, 0, sizeof m_aFlagsRecord );

	for ( unsigned int i = 0; i < sizeof m_aFlags; ++i )
	{
		if ( m_aFlags[ i ] & USED )
		{
			m_aMemoryRecord[ i ] = m_aMemory[ i ];
			m_aFlagsRecord[ i ] = m_aFlags[ i ];
		}
	}

	m_bRecording = true;
}


void ObjectCode::ApplyRecordedAsBaseline()
{
	if ( !m_bPass2Changed ) return;

	// Replace baseline memory/flags with recorded ones
	memset( m_aMemory, 0, sizeof m_aMemory );
	memset( m_aFlags, 0, sizeof m_aFlags );

	for ( unsigned int i = 0; i < sizeof m_aFlags; ++i )
	{
		if ( m_aFlagsRecord[ i ] & USED )
		{
			m_aMemory[ i ] = m_aMemoryRecord[ i ];
			// promote recorded flags and ensure CHECK is set so next pass verifies
			m_aFlags[ i ] = m_aFlagsRecord[ i ] | CHECK;
		}
	}

	// Clear recording state
	m_bPass2Changed = false;
	m_bRecording = false;
	memset( m_aMemoryRecord, 0, sizeof m_aMemoryRecord );
	memset( m_aFlagsRecord, 0, sizeof m_aFlagsRecord );
}


void ObjectCode::ClearRecorded()
{
	m_bPass2Changed = false;
	m_bRecording = false;
	memset( m_aMemoryRecord, 0, sizeof m_aMemoryRecord );
	memset( m_aFlagsRecord, 0, sizeof m_aFlagsRecord );
}



/*************************************************************************************************/
/**
	ObjectCode::Destroy()

	Destroys the ObjectCode singleton
*/
/*************************************************************************************************/
void ObjectCode::Destroy()
{
	assert( m_gInstance != NULL );

	delete m_gInstance;
	m_gInstance = NULL;
}



/*************************************************************************************************/
/**
	ObjectCode::ObjectCode()

	ObjectCode constructor
*/
/*************************************************************************************************/
ObjectCode::ObjectCode()
	:	m_PC( 0 ),
		m_CPU( CPU_6502 )
{
	memset( m_aMemory, 0, sizeof m_aMemory );
	memset( m_aFlags, 0, sizeof m_aFlags );
	m_bPass2Changed = false;
	m_bRecording = false;
	memset( m_aMemoryRecord, 0, sizeof m_aMemoryRecord );
	memset( m_aFlagsRecord, 0, sizeof m_aFlagsRecord );
	SymbolTable::Instance().AddBuiltInSymbol( "CPU", m_CPU );
}



/*************************************************************************************************/
/**
	ObjectCode::~ObjectCode()

	ObjectCode destructor
*/
/*************************************************************************************************/
ObjectCode::~ObjectCode()
{
}



/*************************************************************************************************/
/**
	ObjectCode::SetCPU()

	Set the CPU type
*/
/*************************************************************************************************/
void ObjectCode::SetCPU( CPU_TYPE cpu )
{
	m_CPU = cpu;
	SymbolTable::Instance().ChangeBuiltInSymbol( "CPU", m_CPU );
}



/*************************************************************************************************/
/**
	ObjectCode::InitialisePass()

	Initialise at the beginning of each pass
*/
/*************************************************************************************************/
void ObjectCode::InitialisePass()
{
	// Reset CPU type and PC

	SetCPU( CPU_6502 );
	SetPC( 0 );
	SymbolTable::Instance().ChangeBuiltInSymbol( "P%", m_PC );

	// Clear flags between passes

	Clear( 0, 0x10000, false );
	// Reset any recording state at the start of each pass
	m_bPass2Changed = false;
	m_bRecording = false;
	memset( m_aMemoryRecord, 0, sizeof m_aMemoryRecord );
	memset( m_aFlagsRecord, 0, sizeof m_aFlagsRecord );

	// initialise ascii mapping table

	for ( int i = 0; i < 96; i++ )
	{
		m_aMapChar[ i ] = i + 32;
	}
}


/*************************************************************************************************/
/**
	ObjectCode::PutByte()

	Puts one byte to memory image, never doing pass consistency checks
*/
/*************************************************************************************************/
void ObjectCode::PutByte( unsigned int byte )
{
	if ( m_PC > 0xFFFF )
	{
		throw AsmException_AssembleError_OutOfMemory();
	}

	assert( m_PC >= 0 && m_PC < 0x10000 );
	assert( byte < 0x100 );

	unsigned char* pFlags = m_bRecording ? m_aFlagsRecord : m_aFlags;
	unsigned char* pMemory = m_bRecording ? m_aMemoryRecord : m_aMemory;

	if ( pFlags[ m_PC ] & GUARD )
	{
		throw AsmException_AssembleError_GuardHit();
	}

	if ( pFlags[ m_PC ] & USED )
	{
		throw AsmException_AssembleError_Overlap();
	}

	pFlags[ m_PC ] |= USED;
	pMemory[ m_PC++ ] = byte;

	SymbolTable::Instance().ChangeBuiltInSymbol( "P%", m_PC );
}



/*************************************************************************************************/
/**
	ObjectCode::Assemble1()

	Assembles one byte to memory image
*/
/*************************************************************************************************/
void ObjectCode::Assemble1( unsigned int opcode )
{
	if ( m_PC > 0xFFFF )
	{
		throw AsmException_AssembleError_OutOfMemory();
	}

	assert( m_PC >= 0 && m_PC < 0x10000 );
	assert( opcode < 0x100 );

	unsigned char* pFlags = m_bRecording ? m_aFlagsRecord : m_aFlags;
	unsigned char* pMemory = m_bRecording ? m_aMemoryRecord : m_aMemory;

	if ( GlobalData::Instance().IsSecondPass() &&
		 ( m_aFlags[ m_PC ] & CHECK ) &&
		 !( m_aFlags[ m_PC ] & DONT_CHECK ) &&
		 m_aMemory[ m_PC ] != opcode )
	{
		// Start recording replacement bytes for pass 2 if not already
		StartRecordingIfNeeded();
		m_bPass2Changed = true;
		pFlags = m_aFlagsRecord;
		pMemory = m_aMemoryRecord;
	}

	if ( pFlags[ m_PC ] & GUARD )
	{
		throw AsmException_AssembleError_GuardHit();
	}

	if ( pFlags[ m_PC ] & USED )
	{
		throw AsmException_AssembleError_Overlap();
	}

	pFlags[ m_PC ] |= ( USED | CHECK );
	pMemory[ m_PC++ ] = opcode;

	SymbolTable::Instance().ChangeBuiltInSymbol( "P%", m_PC );
}



/*************************************************************************************************/
/**
	ObjectCode::Assemble2()

	Assembles two bytes to memory image
*/
/*************************************************************************************************/
void ObjectCode::Assemble2( unsigned int opcode, unsigned int val )
{
	if ( m_PC > 0xFFFE )
	{
		throw AsmException_AssembleError_OutOfMemory();
	}

	assert( m_PC >= 0 && m_PC < 0x10000 );
	assert( opcode < 0x100 );
	assert( val < 0x100 );

	unsigned char* pFlags = m_bRecording ? m_aFlagsRecord : m_aFlags;
	unsigned char* pMemory = m_bRecording ? m_aMemoryRecord : m_aMemory;

	if ( GlobalData::Instance().IsSecondPass() &&
		 ( m_aFlags[ m_PC ] & CHECK ) &&
		 !( m_aFlags[ m_PC ] & DONT_CHECK ) &&
		 m_aMemory[ m_PC ] != opcode )
	{
		StartRecordingIfNeeded();
		m_bPass2Changed = true;
		pFlags = m_aFlagsRecord;
		pMemory = m_aMemoryRecord;
	}

	if ( ( pFlags[ m_PC ] & GUARD ) ||
		 ( pFlags[ m_PC + 1 ] & GUARD ) )
	{
		throw AsmException_AssembleError_GuardHit();
	}

	if ( ( pFlags[ m_PC ] & USED ) ||
		 ( pFlags[ m_PC + 1 ] & USED ) )
	{
		throw AsmException_AssembleError_Overlap();
	}

	pFlags[ m_PC ] |= ( USED | CHECK );
	pMemory[ m_PC++ ] = opcode;
	pFlags[ m_PC ] |= USED;
	pMemory[ m_PC++ ] = val;

	SymbolTable::Instance().ChangeBuiltInSymbol( "P%", m_PC );
}



/*************************************************************************************************/
/**
	ObjectCode::Assemble3()

	Assembles three bytes to memory image
*/
/*************************************************************************************************/
void ObjectCode::Assemble3( unsigned int opcode, unsigned int addr )
{
	if ( m_PC > 0xFFFD )
	{
		throw AsmException_AssembleError_OutOfMemory();
	}

	assert( m_PC >= 0 && m_PC < 0x10000 );
	assert( opcode < 0x100 );
	assert( addr < 0x10000 );

	unsigned char* pFlags = m_bRecording ? m_aFlagsRecord : m_aFlags;
	unsigned char* pMemory = m_bRecording ? m_aMemoryRecord : m_aMemory;

	if ( GlobalData::Instance().IsSecondPass() &&
		 ( m_aFlags[ m_PC ] & CHECK ) &&
		 !( m_aFlags[ m_PC ] & DONT_CHECK ) &&
		 m_aMemory[ m_PC ] != opcode )
	{
		StartRecordingIfNeeded();
		m_bPass2Changed = true;
		pFlags = m_aFlagsRecord;
		pMemory = m_aMemoryRecord;
	}

	if ( ( pFlags[ m_PC ] & GUARD ) ||
		 ( pFlags[ m_PC + 1 ] & GUARD ) ||
		 ( pFlags[ m_PC + 2 ] & GUARD ) )
	{
		throw AsmException_AssembleError_GuardHit();
	}

	if ( ( pFlags[ m_PC ] & USED ) ||
		 ( pFlags[ m_PC + 1 ] & USED ) ||
		 ( pFlags[ m_PC + 2 ] & USED ) )
	{
		throw AsmException_AssembleError_Overlap();
	}

	pFlags[ m_PC ] |= ( USED | CHECK );
	pMemory[ m_PC++ ] = opcode;
	pFlags[ m_PC ] |= USED;
	pMemory[ m_PC++ ] = addr & 0xFF;
	pFlags[ m_PC ] |= USED;
	pMemory[ m_PC++ ] = ( addr & 0xFF00 ) >> 8;

	SymbolTable::Instance().ChangeBuiltInSymbol( "P%", m_PC );
}



/*************************************************************************************************/
/**
	ObjectCode::SetGuard()
*/
/*************************************************************************************************/
void ObjectCode::SetGuard( int addr )
{
	assert( addr >= 0 && addr < 0x10000 );
	m_aFlags[ addr ] |= GUARD;
}



/*************************************************************************************************/
/**
	ObjectCode::Clear()
*/
/*************************************************************************************************/
void ObjectCode::Clear( int start, int end, bool bAll )
{
	assert( start <= end );
	assert( start >= 0 && start < 0x10000 );
	assert( end > 0 && end <= 0x10000 );

	// Nothing to do if start == end
	if ( start == end )
	{
		return;
	}

	if ( bAll )
	{
		// via CLEAR command
		// as soon as we force a block to be cleared, we can no longer do inconsistency checks on
		// the object code, so we flag the whole block as DONT_CHECK
		memset( m_aMemory + start, 0, end - start );
		memset( m_aFlags + start, DONT_CHECK, end - start );
	}
	else
	{
		// between first and second pass
		// we preserve the memory image and the CHECK flags so that we can test for inconsistencies
		// in the assembled code between first and second passes
		for ( unsigned char* i = m_aFlags + start; i < m_aFlags + end; i++ )
		{
			(*i) &= ( CHECK | DONT_CHECK );
		}
	}
}



/*************************************************************************************************/
/**
	ObjectCode::IncBin()
*/
/*************************************************************************************************/
void ObjectCode::IncBin( const char* filename, std::vector<unsigned char>& firstFour )
{
	ifstream binfile;

	binfile.open( filename, ios_base::in | ios_base::binary );

	if ( !binfile )
	{
		throw AsmException_AssembleError_FileOpen();
	}

	char c;

	while ( binfile.get( c ) )
	{
		assert( binfile.gcount() == 1 );
		unsigned char uc = static_cast< unsigned char >( c );
		if ( firstFour.size() < 4 )
		{
			firstFour.push_back(uc);
		}
		Assemble1( uc );
	}

	if ( !binfile.eof() )
	{
		throw AsmException_AssembleError_FileRead();
	}

	binfile.close();
}



/*************************************************************************************************/
/**
	ObjectCode::SetMapping()
*/
/*************************************************************************************************/
void ObjectCode::SetMapping( int ascii, int mapped )
{
	assert( ascii > 31 && ascii < 127 );
	assert( mapped >= 0 && mapped < 256 );

	m_aMapChar[ ascii - 32 ] = mapped;
}



/*************************************************************************************************/
/**
	ObjectCode::GetMapping()
*/
/*************************************************************************************************/
int ObjectCode::GetMapping( int ascii ) const
{
	assert( ascii > 31 && ascii < 127 );
	return m_aMapChar[ ascii - 32 ];
}



/*************************************************************************************************/
/**
	ObjectCode::CopyBlock()
*/
/*************************************************************************************************/
void ObjectCode::CopyBlock( int start, int end, int dest, bool firstPass )
{
	int length = end - start;

	if ( start + length > 0x10000 ||
		 dest + length > 0x10000 )
	{
		throw AsmException_AssembleError_OutOfMemory();
	}

	if (firstPass)
	{
		for ( int i = 0; i < length; i++ )
		{
			if ( m_aFlags[ dest + i ] & GUARD )
			{
				throw AsmException_AssembleError_GuardHit();
			}

			m_aFlags[ dest + i ] |= (m_aFlags[ start + i ] & USED);
		}
	}
	else if ( start < dest )
	{
		for ( int i = length - 1; i >= 0; i-- )
		{
			if ( m_aFlags[ dest + i ] & GUARD )
			{
				throw AsmException_AssembleError_GuardHit();
			}

			m_aMemory[ dest + i ] = m_aMemory[ start + i ];
			m_aFlags[ dest + i ] = m_aFlags[ start + i ];
			m_aFlags[ start + i ] &= ( CHECK | DONT_CHECK );
		}
	}
	else if ( start > dest )
	{
		for ( int i = 0; i < length; i++ )
		{
			if ( m_aFlags[ dest + i ] & GUARD )
			{
				throw AsmException_AssembleError_GuardHit();
			}

			m_aMemory[ dest + i ] = m_aMemory[ start + i ];
			m_aFlags[ dest + i ] = m_aFlags[ start + i ];
			m_aFlags[ start + i ] &= ( CHECK | DONT_CHECK );
		}
	}
}

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

/*************************************************************************************************/
/**
	ObjectCode::AnyUsed() - is any memory USED?
*/
/*************************************************************************************************/
bool ObjectCode::AnyUsed() const
{
	for (unsigned int i = 0; i < ARRAY_LENGTH(m_aFlags); ++i)
	{
		if ( m_aFlags[i] & USED )
		{
			return true;
		}
	}

	return false;
}
