//////////////////////////////////////////////////////////////////////////////////////////
//
//  AcqirisD1Import.h:	Import header file for Acqiris Devices
//
//----------------------------------------------------------------------------------------
//  Copyright 1998-2004 Acqiris. All rights reserved.
//
//	Comments:	Include this file into any project which wants access to the Acqiris
//				Digitizer device drivers
//
//  $Revision: 9 $												$Modtime: 23/04/04 9:07 $
//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef _ACQIRISD1IMPORT_H
#define _ACQIRISD1IMPORT_H


// Calling convention used: __cdecl by default (note: _VI_FUNC is synonymous to __stdcall)
#if !defined( _VXWORKS ) && !defined( _LINUX )
#define ACQ_CC __stdcall
#else
#define ACQ_CC
#endif

// The function qualifier ACQ_DLL is used to declare the import method for the imported
// functions
#ifdef ACQ_DLL
#undef ACQ_DLL
#endif

#ifdef __cplusplus

// Declare the functions as being imported...
#if !defined( _VXWORKS ) && !defined( _LINUX ) && !defined( _ETS )
#define ACQ_DLL __declspec(dllimport)
#else
#define ACQ_DLL
#endif

// ...and declare C linkage for the imported functions if in C++
	extern "C" {

#else

// In C, simply declare the functions as being 'extern' and imported
#if !defined( _VXWORKS ) && !defined( _LINUX ) && !defined( _ETS )
#define ACQ_DLL extern __declspec(dllimport)
#else
#define ACQ_DLL 
#endif

#endif

#include "AcqirisErrorCodes.h"
#include "AcqirisD1Interface.h"


#if defined( __cplusplus ) 
	}
#endif


#endif // _ACQIRISD1IMPORT_H

// End of file ///////////////////////////////////////////////////////////////////////////
