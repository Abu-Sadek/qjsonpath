// An implementation of JSONPath for use with Qt 5.0+ applications
// Written by Jay Sprenkle ( jsprenkle@gmail )

#ifndef __QJSONPATH__
#define __QJSONPATH__

#include "JsonPath.h"
#include <QtQuick/QQuickItem>
#include <QJsonDocument>

class QJsonPath
   : public QQuickItem
{
   Q_OBJECT
   Q_PROPERTY( bool valid READ getValid NOTIFY validChanged )
   Q_PROPERTY( QString path READ getPath WRITE setPath NOTIFY pathChanged )
   Q_PROPERTY( QString json READ getJson WRITE setJson NOTIFY jsonChanged )
   Q_PROPERTY( QJsonDocument jsonDocument READ getJsonDocument NOTIFY jsonDocumentChanged )

   // <editor-fold defaultstate="collapsed" desc="valid Property">
public:
   bool getValid() const;
private:
   void setValid( bool );
   bool _valid;
signals:
   /**
    * signal is emitted if property changes
    * @param current property value
    */
   void validChanged( bool );
   // </editor-fold>

   // <editor-fold defaultstate="collapsed" desc="path Property">
public:
   QString getPath() const;
   void setPath( const QString& );
private:
   QString _path;
signals:
   /**
    * signal is emitted if property changes
    * @param current property value
    */
   void pathChanged( QString );
   // </editor-fold>

   // <editor-fold defaultstate="collapsed" desc="json Property">
public:
   QString getJson() const;
   void setJson( const QString& );
private:
   QString _json;
signals:
   /**
    * signal is emitted if property changes
    * @param current property value
    */
   void jsonChanged( QString );
   // </editor-fold>

   // <editor-fold defaultstate="collapsed" desc="jsonDocument Property">
public:
   QJsonDocument getJsonDocument() const;
private:
   QJsonDocument _jsonDocument;
signals:
   /**
    * signal is emitted if property changes and document is valid
    * @param current document
    */
   void jsonDocumentChanged( QJsonDocument );
   // </editor-fold>

public:
   QJsonPath( QQuickItem* parent = 0 );
   
private:
   /**
    * Check for document changes. Emit change signal if updated.
    */
   void update();
};

#include <QMetaType>
// This macro makes the type Type known to QMetaType as long as it provides a public default constructor,
// a public copy constructor and a public destructor. It is needed to use the type Type as a custom type in QVariant.
Q_DECLARE_METATYPE( QJsonPath* )

#endif
