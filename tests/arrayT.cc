#ifdef __GNUG__
#pragma implementation
#endif

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

#include <iostream>
#include "TestArray.h"
#include "TestInt16.h"
#include "TestTypeFactory.h"
#include "util.h"
#include "Pix.h"

using std::cerr ;
using std::endl ;

int test_variable_sleep_interval = 0; // Used in Test* classes for testing
				      // timeouts. 

class arrayT : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( arrayT ) ;
CPPUNIT_TEST( arrayT_test ) ;
CPPUNIT_TEST_SUITE_END( ) ;

private:
    /* TEST PRIVATE DATA */
    TestTypeFactory *factory;
    
public:
    void setUp()
    {
        factory = new TestTypeFactory;
    }

    void tearDown() 
    {
        delete factory; factory = 0;
    }

    void arrayT_test()
    {
	TestArray ar( "My Array" ) ;

	int l = ar.length() ;
	CPPUNIT_ASSERT( l == -1 ) ;

	BaseType *bt = factory->NewInt16() ;
	ar.add_var( bt ) ;

	l = ar.length() ;
	CPPUNIT_ASSERT( l == -1 ) ;

	try
	{
	    int w = ar.width() ;
	    CPPUNIT_ASSERT( w == ( l * (int)bt->width() ) ) ;
	}
	catch( InternalErr &e )
	{
	    CPPUNIT_FAIL( "Unable to retrieve width" ) ;
	}

	ar.append_dim( 4, "dim1" ) ;

	l = ar.length() ;
	CPPUNIT_ASSERT( l == 4 ) ;

	try
	{
	    int w = ar.width() ;
	    CPPUNIT_ASSERT( w == ( l * (int)bt->width() ) ) ;
	}
	catch( InternalErr &e )
	{
	    CPPUNIT_FAIL( "Unable to retrieve width" ) ;
	}

	ar.append_dim( 3, "dim2" ) ;

	l = ar.length() ;
	CPPUNIT_ASSERT( l == 12 ) ;

	try
	{
	    int w = ar.width() ;
	    CPPUNIT_ASSERT( w == ( l * (int)bt->width() ) ) ;
	}
	catch( InternalErr &e )
	{
	    CPPUNIT_FAIL( "Unable to retrieve width" ) ;
	}

	ar.append_dim( 2, "dim3" ) ;

	l = ar.length() ;
	CPPUNIT_ASSERT( l == 24 ) ;

	try
	{
	    int w = ar.width() ;
	    CPPUNIT_ASSERT( w == ( l * (int)bt->width() ) ) ;
	}
	catch( InternalErr &e )
	{
	    CPPUNIT_FAIL( "Unable to retrieve width" ) ;
	}

	vector<string> dims ;
	typedef vector<string>::const_iterator citer ;
	dims.push_back( "dim1" ) ;
	dims.push_back( "dim2" ) ;
	dims.push_back( "dim3" ) ;

	vector<int> dimsize ;
	typedef vector<int>::const_iterator dsiter ;
	dimsize.push_back( 4 ) ;
	dimsize.push_back( 3 ) ;
	dimsize.push_back( 2 ) ;

	citer i = dims.begin() ;
	dsiter d = dimsize.begin() ;
	Pix p ;
	p = ar.first_dim() ;
	for( ; p && i != dims.end(); ar.next_dim( p ), i++, d++ )
	{
	    CPPUNIT_ASSERT( ar.dimension_name( p ) == (*i) ) ;
	    if( ar.dimension_name( p ) == (*i) )
	    {
		CPPUNIT_ASSERT( ar.dimension_size( p ) == (*d) ) ;
		CPPUNIT_ASSERT( ar.dimension_start( p ) == 0 ) ;
		CPPUNIT_ASSERT( ar.dimension_stop( p ) == (*d)-1 ) ;
		CPPUNIT_ASSERT( ar.dimension_stride( p ) == 1 ) ;
	    }
	}
	if( p && i == dims.end() )
	{
	    CPPUNIT_FAIL( "too many dimensions" ) ;
	}
	else if( !p && i != dims.end() )
	{
	    CPPUNIT_FAIL( "not enough dimensions" ) ;
	}

	Array::Dim_iter diter = ar.dim_begin() ;
	i = dims.begin() ;
	d = dimsize.begin() ;
	for( ; diter != ar.dim_end() && i != dims.end(); diter++, i++, d++ )
	{
	    CPPUNIT_ASSERT( ar.dimension_name( diter ) == (*i) ) ;
	    if( ar.dimension_name( diter ) == (*i) )
	    {
		CPPUNIT_ASSERT( ar.dimension_size( diter ) == (*d) ) ;
		CPPUNIT_ASSERT( ar.dimension_start( diter ) == 0 ) ;
		CPPUNIT_ASSERT( ar.dimension_stop( diter ) == (*d)-1 ) ;
		CPPUNIT_ASSERT( ar.dimension_stride( diter ) == 1 ) ;
	    }
	}
	if( diter != ar.dim_end() && i == dims.end() )
	{
	    CPPUNIT_FAIL( "too many dimensions" ) ;
	}
	else if( diter == ar.dim_end() && i != dims.end() )
	{
	    CPPUNIT_FAIL( "not enough dimensions" ) ;
	}

	unsigned int numdims = ar.dimensions( ) ;
	CPPUNIT_ASSERT( numdims == 3 ) ;

	p = ar.first_dim() ;
	ar.add_constraint( p, 0, 2, 3 ) ;

	l = ar.length() ;
	CPPUNIT_ASSERT( l == 12 ) ;

	diter = ar.dim_begin() ;
	diter++ ;
	ar.add_constraint( diter, 0, 2, 2 ) ;

	l = ar.length() ;
	CPPUNIT_ASSERT( l == 8 ) ;

	ar.reset_constraint() ;

	l = ar.length() ;
	CPPUNIT_ASSERT( l == 24 ) ;

	bool is_read = ar.read( "dataset" ) ;
	CPPUNIT_ASSERT( is_read == true ) ;

	/* ar.print_val( stdout ) ; */

	delete bt ;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( arrayT ) ;

int main( int argc, char **argv )
{
    CppUnit::TextUi::TestRunner runner ;
    CppUnit::TestFactoryRegistry &registry =
	CppUnit::TestFactoryRegistry::getRegistry() ;
    runner.addTest( registry.makeTest() ) ;
    runner.setOutputter( CppUnit::CompilerOutputter::defaultOutputter( 
                                                        &runner.result(),
                                                        std::cerr ) );
    bool wasSuccessful = runner.run( "", false ) ;
    return wasSuccessful ? 0 : 1;
}

