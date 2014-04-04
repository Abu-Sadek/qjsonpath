/* 
 * File:   JsonPath.h
 * Author: Jay
 *
 * Created on March 31, 2014
 */

#ifndef JSONPATH_H
#define	JSONPATH_H

#include <QJsonValue>
#include <QJsonDocument>

namespace JsonPath
{
   // C++ implementation of JsonPath.
   // http://goessner.net/articles/JsonPath/
   //
   // "@", "?()" and "()" are not supported as they require an
   // "eval()" method on the underlying script language.

   class JsonPath
   {
   public:
      // Perform JsonPath query |jsonpath| to the Json node |root|.
      // Results are saved in |output|.
      //
      // Usage:
      //
      // JsonPath::Parse(root, "$.foo.bar[1:2].*.buz", &output);
      static QJsonDocument Parse( const QJsonValue& root, const char* jsonpath );

   private:
      JsonPath()
      {
      }

      ~JsonPath()
      {
      }
   };
}

#endif
