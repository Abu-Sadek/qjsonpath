/* 
 * File:   main.cpp
 * Author: jay
 *
 * Created on March 28, 2014
 */
#include <cstdlib>

#include <cpptest.h>
#include "QJsonPath.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

// <editor-fold defaultstate="collapsed" desc="base class for tests">
class QJsonPathTest : public Test::Suite
{
public:
   ::std::vector< QJsonValue > output;

   void test( const char* JsonStr, const char* JsonPathStr )
   {
      QByteArray json( JsonStr );
      QJsonParseError err;
      QJsonDocument doc = QJsonDocument::fromJson( json, &err );
      TEST_ASSERT_MSG( err.error == QJsonParseError::NoError, err.errorString( ).toStdString( ).c_str( ) );

      TEST_ASSERT_MSG( doc.isObject( ), "document is not an object" );
      QJsonObject obj = doc.object( );
      QJsonValue root( obj );
      output = QJsonPath::JsonPath::Parse( root, JsonPathStr );
   }
};
// </editor-fold>


class HappyPathTest : public QJsonPathTest
{
public:
   HappyPathTest()
   {
      TEST_ADD( HappyPathTest::test1 )
      TEST_ADD( HappyPathTest::test2 )
      TEST_ADD( HappyPathTest::test3 )
   }

   void test1()
   {
      try
      {
         test( "{\"name\":\"bob\",\"value\":\"valuetext\"}", "$.name" );
         TEST_ASSERT_MSG( output.size() == 1, "does not have one node as output" );
         {
            const QJsonValue value = output[0];
            TEST_ASSERT_MSG( value.isString(), "does not have a string as output" );
            TEST_ASSERT_MSG( value.toString() == QString("bob"), "does not have correct output" );
         }
      }
      catch ( std::runtime_error& r )
      {
         std::cout << r.what() << "\n";
         TEST_FAIL( "Test throws exception" );
      }
      catch ( ... )
      {
         TEST_FAIL( "Test throws unknown exception" );
      }
   }

   void test2()
   {
      try
      {
         test( "{\"Avatars\":[{\"name\":\"zero\"},{\"name\":\"one\"},{\"name\":\"two\"},{\"name\":\"three\"}]}", "$.Avatars[*].name" );
         TEST_ASSERT_MSG( output.size() == 4, "output has wrong size" );
         {
            const QJsonValue value = output[0];
            TEST_ASSERT_MSG( value.isString(), "output is not a string" );
            TEST_ASSERT_MSG( value.toString() == QString("zero"), "does not have correct output" );
         }
         {
            const QJsonValue value = output[1];
            TEST_ASSERT_MSG( value.isString(), "output is not a string" );
            TEST_ASSERT_MSG( value.toString() == QString("one"), "does not have correct output" );
         }
         {
            const QJsonValue value = output[2];
            TEST_ASSERT_MSG( value.isString(), "output is not a string" );
            TEST_ASSERT_MSG( value.toString() == QString("two"), "does not have correct output" );
         }
         {
            const QJsonValue value = output[3];
            TEST_ASSERT_MSG( value.isString(), "output is not a string" );
            TEST_ASSERT_MSG( value.toString() == QString("three"), "does not have correct output" );
         }
      }
      catch ( std::runtime_error& r )
      {
         std::cout << r.what() << "\n";
         TEST_FAIL( "Test throws exception" );
      }
      catch ( ... )
      {
         TEST_FAIL( "Test throws unknown exception" );
      }
   }

   void test3()
   {
      try
      {
         test( "{\"Avatars\":[{\"name\":\"zero\"},{\"name\":\"one\"},{\"name\":\"two\"},{\"name\":\"three\"}]}", "$..*" );
         TEST_ASSERT_MSG( output.size() == 5, "output has wrong size" );

         {
            const QJsonValue value = output[0];
            TEST_ASSERT_MSG( value.isArray(), "output is not an Array" );
         }
         {
            const QJsonValue value = output[1];
            TEST_ASSERT_MSG( value.isString(), "output is not a string" );
            TEST_ASSERT_MSG( value.toString() == QString("zero"), "does not have correct output" );
         }
      }
      catch ( std::runtime_error& r )
      {
         std::cout << r.what() << "\n";
         TEST_FAIL( "Test throws exception" );
      }
      catch ( ... )
      {
         TEST_FAIL( "Test throws unknown exception" );
      }
   }
};

class FailPathTest : public QJsonPathTest
{
public:
   FailPathTest()
   {
      TEST_ADD( FailPathTest::test1 )
      TEST_ADD( FailPathTest::test2 )
      TEST_ADD( FailPathTest::test3 )
      TEST_ADD( FailPathTest::test4 )
   }

   void test1()
   {
      TEST_THROWS_MSG( test( "{\"name\":\"bob\",\"value\":\"valuetext\"}", "" ), ::std::runtime_error&, "Test does not throw expected runtime_error" )
   }

   void test2()
   {
      TEST_THROWS_MSG( test( "{\"name\":\"bob\",\"value\":\"valuetext\"}", "foobar" ), ::std::runtime_error&, "Test does not throw expected runtime_error" )
   }

   void test3()
   {
      TEST_THROWS_MSG( test( "{\"name\":\"bob\",\"value\":\"valuetext\"}", "$." ), ::std::runtime_error&, "Test does not throw expected runtime_error" )
   }

   void test4()
   {
      TEST_THROWS_MSG( test( "{\"name\":\"bob\",\"value\":\"valuetext\"}", "$.name[" ), ::std::runtime_error&, "Test does not throw expected runtime_error" )
   }
};

int main( int argc, char** argv )
{
   Test::CompilerOutput output( Test::CompilerOutput::GCC );
   Test::Suite* p[] =
   {
      new HappyPathTest
      ,new FailPathTest
   };
   int rc = 0;
   unsigned int TotalTests = sizeof(p) / sizeof(p[0]);
   for ( unsigned int i = 0; i < TotalTests; ++i )
   {
      std::cout
         << "Start test suite: " << p[i]->_name
         << " containing " << p[i]->total_tests() << " sub tests"
         << "\n";
      rc += p[i]->run( output, false ) ? 0 : 1;
   }
   std::cout << "Completed " << TotalTests << " Test suites with " << rc << " failures\n";

   return rc;
}

