/* 
 * File:   PluginLoader.cpp
 * Author: jay
 * 
 * Created on August 13, 2013
 */
#include "PluginLoader.h"
#include "QJsonPath.h"
#include "QJsonListModel.h"

#include <QQmlEngine>
#include <QtQml>        // qmlRegisterType<>

Loader::Loader()
{
}

Loader::Loader( const Loader& orig )
   : QQmlExtensionPlugin( 0 )
{
   setParent( orig.parent() );
}

Loader::~Loader()
{
}

void Loader::registerTypes( const char* )
{
   // in qml:   "import QJson 0.1"
   qmlRegisterType< QJsonPath >( "QJson", 0, 1, "JsonPath" );
   qmlRegisterType< QJsonListModel >( "QJson", 0, 1, "JsonListModel" );
}
