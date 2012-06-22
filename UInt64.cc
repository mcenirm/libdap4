
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2012 OPeNDAP, Inc.
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

#include "config.h"

#include <sstream>

#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Int64.h"
#include "UInt64.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"

#if 0
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"
#endif

#include "Marshaller.h"
#include "UnMarshaller.h"

#include "DDS.h"
#include "util.h"
#include "parser.h"
#include "Operators.h"
#include "dods-limits.h"
#include "debug.h"
#include "InternalErr.h"

using std::cerr;
using std::endl;

namespace libdap {

/** The UInt64 constructor accepts the name of the variable to be created.

    @param n A string containing the name of the variable to be created.
    variable is created
*/
UInt64::UInt64(const string &n)
        : BaseType(n, dods_uint64_c)
{}

/** The UInt64 server-side constructor accepts the name of the variable and
    the dataset name from which this instance is created.

    @param n A string containing the name of the variable to be created.
    @param d A string containing the name of the dataset from which this
    variable is created
*/
UInt64::UInt64(const string &n, const string &d)
        : BaseType(n, d, dods_uint64_c)
{}

UInt64::UInt64(const UInt64 &copy_from) : BaseType(copy_from)
{
    _buf = copy_from._buf;
}

BaseType *
UInt64::ptr_duplicate()
{
    return new UInt64(*this);
}

UInt64 &
UInt64::operator=(const UInt64 &rhs)
{
    if (this == &rhs)
        return *this;

    dynamic_cast<BaseType &>(*this) = rhs;

    _buf = rhs._buf;

    return *this;
}

unsigned int
UInt64::width()
{
    return sizeof(dods_uint64);
}

bool
UInt64::serialize(ConstraintEvaluator &eval, DDS &dds,
                  Marshaller &m, bool ce_eval)
{
    dds.timeout_on();

    if (!read_p())
        read();  // read() throws Error and InternalErr

#if EVAL
    if (ce_eval && !eval.eval_selection(dds, dataset()))
        return true;
#endif

    dds.timeout_off();

    static_cast<DAP4Marshaller*>(&m)->put_uint64( _buf ) ;

    return true;
}

bool
UInt64::deserialize(UnMarshaller &um, DDS *, bool)
{
    // TODO assert
    static_cast<DAP4UnMarshaller*>(&um)->get_uint64( _buf ) ;

    return false;
}

unsigned int
UInt64::val2buf(void *val, bool)
{
    if (!val)
        throw InternalErr(__FILE__, __LINE__,
                          "The incoming pointer does not contain any data.");

    _buf = *(dods_uint64 *)val;

    return width();
}

unsigned int
UInt64::buf2val(void **val)
{
    if (!val)
        throw InternalErr(__FILE__, __LINE__, "NULL pointer.");

    if (!*val)
        *val = new dods_uint64;

    *(dods_uint64 *)*val = _buf;

    return width();
}

dods_uint64
UInt64::value() const
{
    return _buf;
}

bool
UInt64::set_value(dods_uint64 i)
{
    _buf = i;
    set_read_p(true);

    return true;
}

void
UInt64::print_val(FILE *out, string space, bool print_decl_p)
{
    ostringstream oss;
    print_val(oss, space, print_decl_p);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

void
UInt64::print_val(ostream &out, string space, bool print_decl_p)
{
    if (print_decl_p) {
        print_decl(out, space, false);
	out << " = " << (unsigned int)_buf << ";\n" ;
    }
    else
	out << (unsigned int)_buf ;
}

bool
UInt64::ops(BaseType *b, int op)
{
    // Extract the Byte arg's value.
    if (!read_p() && !read())
        throw InternalErr(__FILE__, __LINE__, "This value was not read!");

    // Extract the second arg's value.
    if (!b || !(b->read_p() || b->read()))
        throw InternalErr(__FILE__, __LINE__, "This value was not read!");

    switch (b->type()) {
    case dods_byte_c:
        return rops<dods_uint64, dods_byte, Cmp<dods_uint64, dods_byte> >
               (_buf, static_cast<Byte *>(b)->value(), op);
    case dods_int16_c:
        return rops<dods_uint64, dods_int16, USCmp<dods_uint64, dods_int16> >
               (_buf, static_cast<Int16 *>(b)->value(), op);
    case dods_uint16_c:
        return rops<dods_uint64, dods_uint16, Cmp<dods_uint64, dods_uint16> >
               (_buf, static_cast<UInt16 *>(b)->value(), op);
    case dods_int32_c:
        return rops<dods_uint64, dods_int32, USCmp<dods_uint64, dods_int32> >
               (_buf, static_cast<Int32 *>(b)->value(), op);
    case dods_uint32_c:
        return rops<dods_uint64, dods_uint32, Cmp<dods_uint64, dods_uint32> >
               (_buf, static_cast<UInt32 *>(b)->value(), op);
    case dods_int64_c:
        return rops<dods_uint64, dods_int64, Cmp<dods_uint64, dods_int64> >
               (_buf, static_cast<Int64 *>(b)->value(), op);
    case dods_uint64_c:
        return rops<dods_uint64, dods_uint64, Cmp<dods_uint64, dods_uint64> >
               (_buf, static_cast<Int64 *>(b)->value(), op);
    case dods_float32_c:
        return rops<dods_uint64, dods_float32, Cmp<dods_uint64, dods_float32> >
               (_buf, static_cast<Float32 *>(b)->value(), op);
    case dods_float64_c:
        return rops<dods_uint64, dods_float64, Cmp<dods_uint64, dods_float64> >
               (_buf, static_cast<Float64 *>(b)->value(), op);
    default:
        return false;
    }
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and information about this
 * instance.
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void
UInt32::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "UInt32::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    BaseType::dump(strm) ;
    strm << DapIndent::LMarg << "value: " << _buf << endl ;
    DapIndent::UnIndent() ;
}

} // namespace libdap

