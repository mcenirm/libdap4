// -*- C++ -*-

// Interface for Url type.
//
// jhrg 9/7/94

/* $Log: Url.h,v $
/* Revision 1.7  1995/01/19 21:59:48  jimg
/* Added read_val from dummy_read.cc to the sample set of sub-class
/* implementations.
/* Changed the declaration of readVal in BaseType so that it names the
/* mfunc read_val (to be consistant with the other mfunc names).
/* Removed the unnecessary duplicate declaration of the abstract virtual
/* mfuncs read and (now) read_val from the classes Byte, ... Grid. The
/* declaration in BaseType is sufficient along with the decl and definition
/* in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
/*
 * Revision 1.6  1995/01/18  18:40:34  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.5  1995/01/11  15:54:38  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.4  1994/11/29  20:16:36  jimg
 * Added mfunc for data transmission.
 * Uses special xdr function for serialization and xdr_coder.
 * Removed `type' parameter from ctor.
 * Added FILE *in and *out to ctor parameter list.
 *
 * Revision 1.3  1994/11/22  14:06:18  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.2  1994/09/23  14:36:16  jimg
 * Fixed errors in comments.
 *
 * Revision 1.1  1994/09/15  21:09:10  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 */

#ifndef _Url_h
#define _Url_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <limits.h>

#include "BaseType.h"

const unsigned int max_url_len = UCHAR_MAX-1;

class Url: public BaseType {
protected:
    char *_buf;

public:
    Url(const String &n = (char *)0);
    virtual ~Url() {}

    virtual BaseType *ptr_duplicate() = 0;
    
    virtual unsigned int size();

#ifdef NEVER
    virtual bool read(String dataset, String var_name, String constraint) = 0;
    virtual bool readVal(void *stuff);
#endif

    virtual bool serialize(bool flush = false, unsigned int num = 0);
    virtual unsigned int deserialize();

    virtual void print_val(ostream &os, String space = "");
};

typedef Url * UrlPtr;

#endif

