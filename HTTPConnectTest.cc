
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

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iterator>
#include <string>
#include <algorithm>

#include "Regex.h"

#include "HTTPConnect.h"
#include "RCReader.h"
#include "debug.h"

using namespace CppUnit;
using namespace std;

class HTTPConnectTest : public TestFixture {
private:
    HTTPConnect *http;
    string dodsdev_url;
    string etag;
    string lm;
    string dsp_das_url;

protected:
    bool re_match(Regex &r, const char *s) {
	return r.match(s, strlen(s)) == (int)strlen(s);
    }

public:
    HTTPConnectTest() {}
    ~HTTPConnectTest() {}
    
    void setUp () {
	putenv("DODS_CACHE_INIT=cache-testsuite/dodsrc");
	http = new HTTPConnect(RCReader::instance());

	dodsdev_url = "http://dodsdev.gso.uri.edu/test.html";
	// Two request header values that will generate a 304 response to the
	// above URL.
	etag = "\"6c054-2bf-3d9a1d42\"";
	lm = "Tue, 02 Oct 2002 22:10:10 GMT";
	dsp_das_url = "http://eddy.gso.uri.edu/cgi-bin/nph-dods/avhrr/2001/4/d01093165826.pvu.Z.das";
    }

    void tearDown() {
	delete http;
    }

    CPPUNIT_TEST_SUITE( HTTPConnectTest );

    CPPUNIT_TEST(read_url_test);
    CPPUNIT_TEST(fetch_url_test);
    CPPUNIT_TEST(get_response_headers_test);
    CPPUNIT_TEST(server_version_test);
    CPPUNIT_TEST(type_test);
    CPPUNIT_TEST(cache_test);

    CPPUNIT_TEST_SUITE_END();

    void read_url_test() {
	try {
	    FILE *dump = fopen("/dev/null", "w");
	    long status = http->read_url(dodsdev_url, dump);
	    CPPUNIT_ASSERT(status == 200); 

	    vector<string> headers;

	    // First test using a time with if-modified-since
	    headers.push_back(string("If-Modified-Since: ") + lm);
	    status = http->read_url(dodsdev_url, dump, &headers);
	    CPPUNIT_ASSERT(status == 304); 

	    // Now test an etag
	    headers.clear();
	    headers.push_back(string("If-None-Match: ") + etag);
	    status = http->read_url(dodsdev_url, dump, &headers);
	    CPPUNIT_ASSERT(status == 304); 

	    // now test a combined etag and time4
	    headers.push_back(string("If-Modified-Since: ") + lm);
	    status = http->read_url(dodsdev_url, dump, &headers);
	    CPPUNIT_ASSERT(status == 304); 

	}
	catch(Error &e) {
	    cerr << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(false && "Should not get an Error");
	}
    }

    void fetch_url_test() {
	char c;
	try {
	    Response *stuff = http->fetch_url("http://dcz.dods.org/");
	    CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1 
			   && !ferror(stuff->get_stream()) 
			   && !feof(stuff->get_stream()));
	    delete stuff;

	    stuff = http->fetch_url(dsp_das_url);
	    DBG2(char ln[1024];
		 while (!feof(stuff->get_stream())) {
		     fgets(ln, 1024, stuff->get_stream());
		     cerr << ln;
		 }
		 rewind(stuff->get_stream()));
		
	    CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
			   && !ferror(stuff->get_stream()) 
			   && !feof(stuff->get_stream()));
	    delete stuff;

	    stuff = http->fetch_url("file:///etc/passwd");
	    CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1 
			   && !ferror(stuff->get_stream()) 
			   && !feof(stuff->get_stream()));
	    delete stuff;

	    stuff = http->fetch_url("file://HTTPConnect.cc");
	    CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1 
			   && !ferror(stuff->get_stream()) 
			   && !feof(stuff->get_stream()));
	    delete stuff;
	}
	catch (InternalErr &e) {
	    cerr << "InternalErr: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Caught a DODS exception from fetch_url");
	}
	catch (Error &e) {
	    cerr << "Error: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Caught a DODS exception from fetch_url");
	}
    }

    void get_response_headers_test() {
	Response *r = http->fetch_url(dsp_das_url);

	try {
	    vector<string> h = r->get_headers();

	    DBG(copy(h.begin(), h.end(), 
		     ostream_iterator<string>(cerr, "\n")));
	    delete r;

	    // Should get five headers back.
	    Regex header("XDODS-Server: DAP/.*");
	    CPPUNIT_ASSERT(re_match(header, h[0].c_str()));
	    CPPUNIT_ASSERT(h[4] == "Content-Description: dods_das");
	    CPPUNIT_ASSERT(h.size() == 5);
	}
	catch (InternalErr &e) {
	    delete r;
	    CPPUNIT_ASSERT(!"Caught an exception from get_response_headers");
	}
    }

    void server_version_test() {
	Response *r = http->fetch_url(dsp_das_url);
	Regex version("dap/[0-9]+\\.[0-9]+\\.[0-9]+");
	try {
	    CPPUNIT_ASSERT(re_match(version, r->get_version().c_str()));
	    delete r;
	}
	catch (InternalErr &e) {
	    delete r;
	    CPPUNIT_ASSERT(!"Caught an exception from server_version");
	}
	
    }

    void type_test() {
	Response *r = http->fetch_url(dsp_das_url);
	try {
	    CPPUNIT_ASSERT(r->get_type() == dods_das);
	    delete r;
	}
	catch (InternalErr &e) {
	    delete r;
	    CPPUNIT_ASSERT(!"Caught an exception from type()");
	}

    }

    void set_credentials_test() {
	http->set_credentials("jimg", "was_quit");

	try {
	    char c;
	    Response *stuff = http->fetch_url("http://dcz.dods.org/secret");
	    CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
			   && !ferror(stuff->get_stream()) 
			   && !feof(stuff->get_stream()));
	    delete stuff;
	}
	catch (InternalErr &e) {
	    CPPUNIT_ASSERT(!"Caught exception from output");
	}
    }

    void cache_test() {
	DBG(cerr << endl << "Entering Caching tests." << endl);

	// The cache-testsuite/dodsrc file turns this off; all the other
	// params are set up. 
	http->d_http_cache->set_cache_enabled(true);

	fetch_url_test();
	get_response_headers_test();
	server_version_test();
	type_test();
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HTTPConnectTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}

// $Log: HTTPConnectTest.cc,v $
// Revision 1.8  2003/04/23 21:33:53  jimg
// Changes for the unit tests. This involved merging Rob's VC++ changes
// and fixing a bug in escaping.cc (a call to string::insert invalidated
// an iterator in a loop).
//
// Revision 1.7  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.6  2003/03/04 17:21:47  jimg
// Now uses Response objects.
//
// Revision 1.5  2003/02/21 22:49:52  jimg
// Added fetch_url() tests for "file:" URLs.
//
// Revision 1.4  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.3.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.3  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.5  2002/12/24 02:39:48  jimg
// Added cache test back into code.
//
// Revision 1.1.2.4  2002/11/06 21:53:06  jimg
// I changed the includes of Regex.h from <Regex.h> to "Regex.h". This means
// make depend will include the header in the list of dependencies.
//
// Revision 1.1.2.3  2002/10/18 22:53:17  jimg
// Massive cahnges. Now tests read_url and also includes tests for accesses that use the HTTPCache class (as well as tests that force *not* using the cache).
//
// Revision 1.1.2.2  2002/09/14 03:41:31  jimg
// Added tests for get_response_headers and is_response_present.
//
// Revision 1.1.2.1  2002/07/25 13:24:44  jimg
// Added.
//

