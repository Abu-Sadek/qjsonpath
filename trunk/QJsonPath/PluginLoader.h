/* 
 * File:   PluginLoader.h
 * Author: jay
 * 
 * Created on August 13, 2013
 */
#ifndef CLIENTPLUGINLOADER_H
#define	CLIENTPLUGINLOADER_H

#include <QQmlExtensionPlugin>

class Loader
   : public QQmlExtensionPlugin
{
   Q_OBJECT
   Q_PLUGIN_METADATA(IID "QJson.QJsonPath" )

public:
   Loader();
   Loader( const Loader& orig );
   virtual ~Loader();

   // Override the registerTypes() method and call qmlRegisterType() to register the types to be exported by the plugin
   void registerTypes( const char* uri );
};
#endif
