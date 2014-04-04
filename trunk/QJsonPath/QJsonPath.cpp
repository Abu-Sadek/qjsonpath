// An implementation of JSONPath for use with Qt 5.0+ applications
// Written by Jay Sprenkle ( jsprenkle@gmail )

#include "QJsonPath.h"
#include "JsonPath.h"
#include <QJsonObject>

QJsonPath::QJsonPath( QQuickItem* parent )
   : QQuickItem( parent )
   , _valid( false )
{
}

// <editor-fold defaultstate="collapsed" desc="valid property">

void QJsonPath::setValid( bool v )
{
   if ( _valid != v )
   {
      _valid = v;    
      emit validChanged( _valid );
   }
}

bool QJsonPath::getValid() const
{
   return _valid;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="path property">

QString QJsonPath::getPath() const
{
   return _path;
}

void QJsonPath::setPath( const QString& r )
{
   if ( _path != r )
   {
      _path = r;
      emit pathChanged( _path );
      update();
   }
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="json property">

QString QJsonPath::getJson() const
{
   return _json;
}

void QJsonPath::setJson( const QString& r )
{
   if ( _json != r )
   {
      _json = r;
      emit jsonChanged( _json );
      update();
   }
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="jsonDocument property">

void QJsonPath::update()
{
   try
   {
      if ( !_json.isEmpty() && !_path.isEmpty() )
      {
         //QJsonParseError err;
         QJsonDocument doc = QJsonDocument::fromJson( _json.toLatin1() ); //, &err );
         if ( ! doc.isNull() )
         {
            QJsonObject obj = doc.object();
            QJsonValue root( obj );
            _jsonDocument = ::JsonPath::JsonPath::Parse( root, _path.toLatin1() );

            setValid( true );
            emit jsonDocumentChanged( _jsonDocument );
            return;
         }
      }
   }
   catch (...)
   {
   }
   setValid( false );
}

QJsonDocument QJsonPath::getJsonDocument() const
{
   return _jsonDocument;
}

// </editor-fold>
