/* 
 * File:   JsonListModel.cpp
 * Author: jay
 * 
 * Created on March 26, 2014
 */
#include "QJsonListModel.h"
#include <QJsonArray>
#include <QJsonObject>
//#include <QDebug>

QJsonListModel::QJsonListModel( QObject *parent )
   : QAbstractListModel( parent )
{
}

// <editor-fold defaultstate="collapsed" desc="json Property">

QJsonDocument QJsonListModel::getJson() const
{
   return _json;
}

void QJsonListModel::setJson( const QJsonDocument value )
{
   if ( _json != value )
   {
      if ( value.isArray() )
      {
         _json = value;
         QJsonArray arr = _json.array();
         beginInsertRows( QModelIndex(), 0, arr.size() - 1 );
         endInsertRows();
         //updateModel();
         emit jsonChanged( _json );
      }
   }
}
// </editor-fold>

/**
 * 
 * inserts count rows into the model before the given row.
 * Items in the new row will be children of the item represented by the parent model index.
 * 
 * If row is 0, the rows are prepended to any existing rows in the parent.
 * 
 * If row is rowCount(), the rows are appended to any existing rows in the parent.
 * 
 * If parent has no children, a single column with count rows is inserted.
 * 
 * Returns true if the rows were successfully inserted; otherwise returns false.
 * 
 * @param row
 * @param count
 * @param parent
 * @return 
 */
bool QJsonListModel::insertRows( int row, int count, const QModelIndex& parent )
{
   // todo: do I need to change the model index of rows that follow the insertion?
   beginInsertRows( parent, row, row + count - 1 );
   endInsertRows();
   return true;
}

QJsonValue QJsonListModel::getItemValue( const QModelIndex& idx ) const
{
   if ( idx.isValid() )
   {
      QJsonValue v = getItemValue( idx.parent() );
      if ( v.isArray() )
         return v.toArray()[ idx.row() ];
      return QJsonValue();
   }
   // if not valid it's the root
   return QJsonValue( _json.array() );
}

int QJsonListModel::rowCount( const QModelIndex& idx ) const
{
   // if not an array then will return zero
   return getItemValue( idx ).toArray().size();
}

// todo: investigate using toVariant() here
QVariant getValue( const QJsonValue& r, const QModelIndex& idx, const char* key )
{
   switch ( r.type() )
   {
      case QJsonValue::Array:
         if ( idx.isValid() )
            return getValue( r.toArray()[ idx.row() ], QModelIndex(), key );
         break;
      case QJsonValue::Object:
         {
            QJsonObject obj = r.toObject();
            QJsonValue val = obj[ key ];
            if ( ! val.isUndefined() )
               return val.toString();
         }
         break;
      case QJsonValue::Bool:
      case QJsonValue::Double:
      case QJsonValue::String:
         return r.toString();
      default:
         break;
   }
   return QVariant();
}

QVariant QJsonListModel::data( const QModelIndex& idx, int role ) const
{
   QJsonValue v = getItemValue( idx );
//   qDebug() << "QJsonListModel::data()"
//      << " idx: " << idx
//      << " v: " << v
//      ;
   switch ( role )
   {
      //Qt::ToolTipRole	3	The data displayed in the item's tooltip. (QString)
      case Qt::ToolTipRole:
         return getValue( v, idx, "ToolTip" );
         break;

      //Qt::StatusTipRole	4	The data displayed in the status bar. (QString)
      case Qt::StatusTipRole:
         return getValue( v, idx, "StatusTip" );
         break;

      //Qt::WhatsThisRole	5	The data displayed for the item in "What's This?" mode. (QString)
      case Qt::WhatsThisRole:
         return getValue( v, idx, "WhatsThis" );
         break;

      //Qt::DisplayRole	0	The key data to be rendered in the form of text. (QString)
      //Qt::EditRole	2	The data in a form suitable for editing in an editor. (QString)
      case Qt::DisplayRole:
      case Qt::EditRole:
      case TextRole:
         return getValue( v, idx, "text" );
         break;
      default:
         break;
   }
   return QVariant();
}

QHash<int, QByteArray> QJsonListModel::roleNames() const
{
   QHash<int, QByteArray> roles;
   roles[TextRole] = "text";
   return roles;
}

