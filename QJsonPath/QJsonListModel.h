/* 
 * File:   JsonListModel.h
 * Author: jay
 *
 * Created on March 26, 2014
 */
#ifndef JSONLISTMODEL_H
#define	JSONLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QJsonDocument>

class QJsonListModel
   : public QAbstractListModel
{
   Q_OBJECT
   Q_PROPERTY( QJsonDocument json READ getJson WRITE setJson NOTIFY jsonChanged )

   // <editor-fold defaultstate="collapsed" desc="json Property">
public:
   QJsonDocument getJson() const;
   void setJson( const QJsonDocument json );
signals:
   void jsonChanged( QJsonDocument );
private:
   QJsonDocument _json;
   // </editor-fold>

public:
   enum ListRoles
   {
      TextRole = Qt::UserRole + 1,
      SizeRole
   };

   QJsonListModel( QObject *parent = 0 );
   
   bool insertRows( int row, int count, const QModelIndex & parent = QModelIndex() );

   int rowCount( const QModelIndex & parent = QModelIndex() ) const;

   QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;

protected:
   QHash<int, QByteArray> roleNames() const;
private:
   QJsonValue getItemValue( const QModelIndex& idx ) const;
};

#endif
