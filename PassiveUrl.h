
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      pwest       Patrick West (pwest@ucar.edu)

// Interface for PassiveUrl type.
//
// pwest 11/04/03

#ifndef _passiveurl_h
#define _passiveurl_h 1

#ifndef __POWERPC__
#ifdef __GNUG__
#pragma interface
#endif
#endif

#include "PassiveStr.h"
#include "dods-limits.h"
#include <string>

/** @brief Holds an internet address (URL).

    @see BaseType
    @see Str */
class PassiveUrl: public PassiveStr {
public:
    PassiveUrl(const string &n = "");

    virtual ~PassiveUrl();

    virtual BaseType *ptr_duplicate();
};

/* 
 * $Log: PassiveUrl.h,v $
 * Revision 1.1  2004/07/09 16:34:38  pwest
 * Adding Passive Data Model objects
 *
 */

#endif // _passiveurl_h

