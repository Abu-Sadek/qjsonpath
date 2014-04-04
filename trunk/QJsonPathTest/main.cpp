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
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
//#include <QDebug>

// <editor-fold defaultstate="collapsed" desc="base class for tests">
class QJsonPathTest : public Test::Suite
{
public:
   QJsonDocument strToJsonDoc( const char* JsonStr )
   {
      QByteArray json( JsonStr );
      QJsonParseError err;
      return QJsonDocument::fromJson( json, &err );
   }

   QJsonDocument jsonPath( const QJsonDocument& JsonDoc, const char* JsonPathStr )
   {
      QJsonValue root( JsonDoc.object() );
      return ::JsonPath::JsonPath::Parse( root, JsonPathStr );
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
         QJsonDocument input = jsonPath( strToJsonDoc( "{\"name\":\"bob\",\"value\":\"valuetext\"}" ), "$.name" );
         QJsonDocument expected = strToJsonDoc( "[ \"bob\" ]" );
         TEST_ASSERT_MSG( input == expected, "output is different than expected" );
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
         QJsonDocument input = jsonPath( strToJsonDoc( "{\"Avatars\":[{\"name\":\"zero\"},{\"name\":\"one\"},{\"name\":\"two\"},{\"name\":\"three\"}]}" ), "$.Avatars[*].name" );
         QJsonDocument expected = strToJsonDoc( "[ \"zero\", \"one\", \"two\", \"three\" ]" );
         TEST_ASSERT_MSG( input == expected, "output is different than expected" );
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
         QJsonDocument input = jsonPath( strToJsonDoc( "{\"Avatars\":[{\"name\":\"zero\"},{\"name\":\"one\"},{\"name\":\"two\"},{\"name\":\"three\"}]}" ), "$..*" );
         QJsonDocument expected = strToJsonDoc( "[[{\"name\": \"zero\"},{\"name\": \"one\"},{\"name\": \"two\"},{\"name\": \"three\"}],\"zero\",\"one\",\"two\",\"three\"]" );
//   qDebug() << " input: " << input;
//   qDebug() << " expected: " << expected;
         TEST_ASSERT_MSG( input == expected, "output is different than expected" );
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
      TEST_THROWS_MSG( jsonPath( strToJsonDoc( "{\"name\":\"bob\",\"value\":\"valuetext\"}" ), "" ), ::std::runtime_error&, "Test does not throw expected runtime_error" )
   }

   void test2()
   {
      TEST_THROWS_MSG( jsonPath( strToJsonDoc( "{\"name\":\"bob\",\"value\":\"valuetext\"}" ), "foobar" ), ::std::runtime_error&, "Test does not throw expected runtime_error" )
   }

   void test3()
   {
      TEST_THROWS_MSG( jsonPath( strToJsonDoc( "{\"name\":\"bob\",\"value\":\"valuetext\"}" ), "$." ), ::std::runtime_error&, "Test does not throw expected runtime_error" )
   }

   void test4()
   {
      TEST_THROWS_MSG( jsonPath( strToJsonDoc( "{\"name\":\"bob\",\"value\":\"valuetext\"}" ), "$.name[" ), ::std::runtime_error&, "Test does not throw expected runtime_error" )
   }
};

int main( int, char** )
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

