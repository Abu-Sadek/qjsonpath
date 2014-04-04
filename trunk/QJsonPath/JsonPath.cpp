/* 
 * File:   JsonPath.cpp
 * Author: jay
 * 
 * Created on March 31, 2014
 */
#include "JsonPath.h"

#include <sstream>
#define __STDC_LIMIT_MACROS 1
#include <stdint.h>
#include <stdlib.h>     /* atoi */
#include <stdexcept>

#include <string>
#include <vector>

#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

namespace JsonPath
{
   struct JsonPathNode
   {
      enum Type
      {
         UNDEFINED_INDEX,
         OBJECT_INDEX,
         ARRAY_INDEX,
         SLICE_INDEX
      };

      Type type;
      ::std::string object_index;
      int array_index;
      int slice_start;
      int slice_end;
      int slice_step;

      static const int kSliceUndef = INT32_MAX;

      static bool IsUndef( int n )
      {
         return n == kSliceUndef;
      }

      ::std::string DebugString() const
      {
         ::std::ostringstream os;
         os << "{" << type << ":" << array_index << ":" << object_index
            << ":(" << slice_start << ":" << slice_end << ":" << slice_step << ")}";
         return os.str();
      }

      JsonPathNode()
         : type( UNDEFINED_INDEX )
         , array_index( 0 )
         , slice_start( 0 )
         , slice_end( 0 )
         , slice_step( 0 )
      {
      }

      ~JsonPathNode()
      {
      }
   };

   bool GetDigit( const ::std::string& str, int* output )
   {
      //DCHECK( output );
      if ( str.empty() )
         return false;

      const char *begin = str.data();
      const char *end = str.data() + str.size();
      if ( str[0] == '-' )
      {
         if ( str.size() == 1 )
            return false;
         ++begin;
      }

      while ( begin < end )
      {
         if ( !isdigit( *begin ) )
            return false;
         ++begin;
      }

      *output = atoi( str.c_str() );
      return true;
   }

   bool GetSliceDigit( const ::std::string& str, int* output )
   {
      if ( str.empty() )
      {
         *output = JsonPathNode::kSliceUndef;
         return true;
      }
      return GetDigit( str, output );
   }

   bool GetQuotedString( const ::std::string& str, const char c, ::std::string *output )
   {
      if ( str.size() >= 2 && str[0] == c && str[str.size() - 1] == c )
      {
         *output = str.substr( 1, str.size() - 2 );
         return true;
      }
      return false;
   }

   ::std::vector< ::std::string > split( const ::std::string& s, const ::std::string& delim, const bool keep_empty = true )
   {
      ::std::vector< ::std::string > result;
      if ( delim.empty() )
      {
         result.push_back( s );
         return result;
      }
      ::std::string::const_iterator substart = s.begin(), subend;
      while ( true )
      {
         subend = ::std::search( substart, s.end(), delim.begin(), delim.end() );
         ::std::string temp( substart, subend );
         if ( keep_empty || !temp.empty() )
         {
            result.push_back( temp );
         }
         if ( subend == s.end() )
         {
            break;
         }
         substart = subend + delim.size();
      }
      return result;
   }         

   bool EndsWith( const ::std::string& str, const ::std::string& suffix )
   {
      if ( str.size() < suffix.size() )
         return false;
      return memcmp( str.data() + str.size() - suffix.size(), suffix.data(), suffix.size() ) == 0;
   }

   typedef ::std::vector<JsonPathNode> JsonPathNodes;
   class JsonPathExp : public ::std::vector< ::std::vector<JsonPathNode> >
   {
   public:

      bool Parse( const ::std::string& jsonpath )
      {
         clear();
         if ( jsonpath.size() <= 1 || jsonpath[0] != '$' )
            throw ::std::runtime_error( "Does not start with $" );

         if ( EndsWith( jsonpath, "." ) || jsonpath.find( "..." ) != ::std::string::npos )
            throw ::std::runtime_error( ::std::string( "Parse error: " ) + jsonpath );

         if ( jsonpath.find( "(" ) != ::std::string::npos ||
              jsonpath.find( ")" ) != ::std::string::npos ||
              jsonpath.find( "@" ) != ::std::string::npos ||
              jsonpath.find( "?" ) != ::std::string::npos )
            throw ::std::runtime_error( ::std::string( "script expression/current node are not supported: " ) + jsonpath );

         const char *begin = jsonpath.data() + 1;
         const char *end = jsonpath.data() + jsonpath.size();

         ::std::string item;
         for ( ; begin < end; ++begin )
         {
            if ( *begin == ']' )
               return false;
            if ( *begin == '.' || *begin == '[' )
            {
               if ( !item.empty() )
               {
                  if ( !AddNodes( item, OUT_BRACKET ) )
                     return false;
                  item.clear();
               }
               if ( *begin == '.' && begin + 1 < end && *( begin + 1 ) == '.' )
               {
                  ++begin;
                  if ( !AddNodes( ".", OUT_BRACKET ) )
                     return false;
               }
               else
                  if ( *begin == '[' )
                  {
                     ++begin;
                     ::std::string exp;
                     while ( *begin != ']' )
                     {
                        if ( begin == end )
                           throw ::std::runtime_error( "Cannot find closing \"]\"" );
                        exp += *begin;
                        ++begin;
                     }
                     if ( !AddNodes( exp, IN_BRACKET ) )
                        return false;
                  }
               }
               else
               {
                  item += *begin;
               }
         }

         if ( !item.empty() )
         {
            if ( !AddNodes( item, OUT_BRACKET ) )
               return false;
         }

         return !empty();
      }

      ::std::string DebugString() const
      {
         ::std::ostringstream os;
         for ( size_t i = 0; i < size(); ++i )
         {
            os << "[";
            for ( size_t j = 0; j < ( *this )[i].size(); ++j )
            {
               os << ( *this )[i][j].DebugString();
            }
            os << "]";
         }
         return os.str();
      }

   private:
      enum NodesType
      {
         IN_BRACKET,
         OUT_BRACKET
      };

      bool AddNodes( const ::std::string& nodes_exp, NodesType nodes_type )
      {
         if ( nodes_exp.empty() )
         {
            return false;
         }

         resize( size() + 1 );
         JsonPathNodes* nodes = &( back() );
         //DCHECK( nodes );

         if ( nodes_type == OUT_BRACKET )
         {
            JsonPathNode node;
            node.type = JsonPathNode::OBJECT_INDEX;
            node.object_index = nodes_exp;
            nodes->push_back( node );
         }
         else
            if ( nodes_type == IN_BRACKET )
            {
               ::std::vector< ::std::string > nodes_exps;

               nodes_exps = split( nodes_exp, ",", false );
               //SplitStringUsing( nodes_exp, ",", &nodes_exps );
               for ( size_t i = 0; i < nodes_exps.size(); ++i )
               {
                  JsonPathNode node;
                  node.type = JsonPathNode::UNDEFINED_INDEX;
                  ::std::string in_nodes_exp;
                  if ( GetQuotedString( nodes_exps[i], '\'', &in_nodes_exp ) ||
                       GetQuotedString( nodes_exps[i], '\"', &in_nodes_exp ) )
                  {
                     node.type = JsonPathNode::OBJECT_INDEX;
                     node.object_index = in_nodes_exp;
                  }
                  else
                     if ( nodes_exps[i] == "*" )
                     {
                        node.type = JsonPathNode::OBJECT_INDEX;
                        node.object_index = "*";
                     }
                     else
                     {
                        ::std::vector< ::std::string > slice;
                        slice = split( nodes_exps[i], ":", true );
                        //Util::SplitStringAllowEmpty( nodes_exps[i], ":", &slice );
                        if ( slice.size() == 1 )
                        {
                           if ( GetDigit( slice[0], &node.array_index ) )
                           {
                              node.type = JsonPathNode::ARRAY_INDEX;
                           }
                           else
                           {
                              // fallback to OBJECT_INDEX.
                              node.type = JsonPathNode::OBJECT_INDEX;
                              node.object_index = slice[0];
                           }
                        }
                        else
                           if ( slice.size() == 2 && GetSliceDigit( slice[0], &node.slice_start ) && GetSliceDigit( slice[1], &node.slice_end ) )
                           {
                              node.slice_step = JsonPathNode::kSliceUndef;
                              node.type = JsonPathNode::SLICE_INDEX;
                           }
                           else
                              if ( slice.size() == 3 &&
                                 GetSliceDigit( slice[0], &node.slice_start ) &&
                                 GetSliceDigit( slice[1], &node.slice_end ) &&
                                 GetSliceDigit( slice[2], &node.slice_step ) )
                              {
                                 node.type = JsonPathNode::SLICE_INDEX;
                              }
                     }

                     if ( node.type == JsonPathNode::UNDEFINED_INDEX )
                        return false;

                     nodes->push_back( node );
                  }
               }
               else
                  throw ::std::runtime_error( "Unknown nodes type" );

         return true;
      }
   };

   // Find all QJsonValue(s) from root node |value|,
   //  which matches to |nodes|.
   void CollectValuesRecursively( const QJsonValue& value, const JsonPathNodes& nodes, ::std::vector< QJsonValue >* output )
   {
      //DCHECK( output );
      for ( size_t node_index = 0; node_index < nodes.size(); ++node_index )
      {
         if ( nodes[node_index].type != JsonPathNode::OBJECT_INDEX )
            continue;

         switch ( value.type() )
         {
            case QJsonValue::Object:
               {
                  QJsonObject obj = value.toObject();
                  const QStringList members = obj.keys();

                  const ::std::string& object_index = nodes[node_index].object_index;
                  if ( object_index != "*" && obj.contains( object_index.c_str() ) )
                     output->push_back( obj[ QString( object_index.c_str() ) ] );

                  // really? returning a signed integer for a size that cannot be negative?
                  for ( size_t i = 0; i < (size_t) members.size(); ++i )
                  {
                     const QJsonValue& v = obj[ members[i] ];
                     if ( object_index == "*" )
                        output->push_back( v );
                     CollectValuesRecursively( v, nodes, output );
                  }
               }
               break;
            case QJsonValue::Array:
               {
                  QJsonArray array = value.toArray();
                  QJsonArray::const_iterator i;
                  for ( i = array.begin(); i != array.end(); ++i )
                     CollectValuesRecursively( *i, nodes, output );
               }
               break;
            default:
               break;
         }
      }
   }

   bool IsArrayElementDefined( const QJsonArray& array, size_t i )
   {
      QJsonValue child = array.at( i );
      return ( ! child.isUndefined() );
   }

   void CollectNodesFromJson( const QJsonValue& value, const JsonPathExp& jsonpathexp, size_t depth, QJsonArray* output )
   {
      if ( depth >= jsonpathexp.size() )
      {
         output->push_back( value );
         return;
      }

      const JsonPathNodes &nodes = jsonpathexp[depth];

      for ( size_t node_index = 0; node_index < nodes.size(); ++node_index )
      {
         const JsonPathNode& node = nodes[node_index];
         switch ( node.type )
         {
            case JsonPathNode::OBJECT_INDEX:
               if ( node.object_index == "*" )
               {
                  switch ( value.type() )
                  {
                     case QJsonValue::Object:
                        {
                           QJsonObject obj = value.toObject();
                           QJsonObject::const_iterator i;
                           for ( i = obj.begin(); i != obj.end(); ++i )
                              CollectNodesFromJson( i.value(), jsonpathexp, depth + 1, output );
                        }
                        break;
                     case QJsonValue::Array:
                        {
                           QJsonArray array = value.toArray();
                           QJsonArray::const_iterator i;
                           for ( i = array.begin(); i != array.end(); ++i )
                              CollectNodesFromJson( *i, jsonpathexp, depth + 1, output );
                        }
                        break;
                     default:
                        CollectNodesFromJson( value, jsonpathexp, depth + 1, output );
                  }
               }
               else
                  if ( node.object_index == "." && depth + 1 < jsonpathexp.size() )
                  {
                     ::std::vector< QJsonValue > matched_values;
                     CollectValuesRecursively( value, jsonpathexp[depth + 1], &matched_values );
                     for ( size_t i = 0; i < matched_values.size(); ++i )
                        CollectNodesFromJson( matched_values[i], jsonpathexp, depth + 2, output );
                  }
                  else
                     if ( value.isObject() )
                     {
                        // contains named child object?
                        QJsonObject obj = value.toObject();
                        if ( obj.contains( node.object_index.c_str() ) )
                           CollectNodesFromJson( obj[ node.object_index.c_str() ], jsonpathexp, depth + 1, output );
                     }
               break;
            case JsonPathNode::ARRAY_INDEX:
               if ( value.isArray() )
               {
                  QJsonArray array = value.toArray();
                  const size_t i = node.array_index >= 0 ? node.array_index : array.size() + node.array_index;
                  {
                     QJsonValue child = array.at( i );
                     if ( ! child.isUndefined() )
                        CollectNodesFromJson( child, jsonpathexp, depth + 1, output );
                  }
               }
               break;
            case JsonPathNode::SLICE_INDEX:
               {
                  if ( value.isArray() )
                  {
                     QJsonArray array = value.toArray();
                     const int size = static_cast<int> ( array.size() );

                     int start = IsArrayElementDefined( array, node.slice_start ) ? 0 : node.slice_start;
                     int end = IsArrayElementDefined( array, node.slice_end ) ? size : node.slice_end;
                     // if this is a step size while iterating the array how can it be undefined?
                     const int step = IsArrayElementDefined( array, node.slice_step ) ? 1 : node.slice_step;

                     // don't allow exceeding bounds of the array
                     start = ( start < 0 ) ? ::std::max( 0, start + size ) : ::std::min( size, start );
                     end = ( end < 0 ) ? ::std::max( 0, end + size ) : ::std::min( size, end );
                     if ( step > 0 && end > start )
                     {
                        for ( int i = start; i < end; i += step )
                           if ( IsArrayElementDefined( array, i ) )
                              CollectNodesFromJson( array[i], jsonpathexp, depth + 1, output );
                     }
                     else
                        if ( step < 0 && end < start )
                        {
                           for ( int i = start; i > end; i += step )
                              if ( IsArrayElementDefined( array, i ) )
                                 CollectNodesFromJson( array[i], jsonpathexp, depth + 1, output );
                        }
                  }
               }
               break;
            default:
               throw ::std::runtime_error( "Unknown type" );
               break;
         }
      }
   }

   QJsonDocument JsonPath::Parse( const QJsonValue& root, const char* jsonstr )
   {
      JsonPathExp jsonpathexp;
      if ( !jsonpathexp.Parse( ::std::string( jsonstr ) ) )
         throw ::std::runtime_error( "Error parsing jsonpath" );

      QJsonArray output;
      CollectNodesFromJson( root, jsonpathexp, 0, &output );
      return QJsonDocument( output );
   }
}
