
// Implementation for the class TestStructure. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestStructure.cc,v $
// Revision 1.2  1995/01/19 21:59:05  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:56  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef _GNUG_
#pragma implementation
#endif

#include "TestStructure.h"

Structure *
NewStructure(const String &n)
{
    return new TestStructure(n);
}

BaseType *
TestStructure::ptr_duplicate()
{
    return new TestStructure(*this);
}

TestStructure::TestStructure(const String &n) : Structure(n)
{
}

TestStructure::~TestStructure()
{
}

bool
TestStructure::read(String dataset, String var_name, String constraint)
{
}

bool
TestStructure::read_val(void *stuff)
{
    return true;
}

