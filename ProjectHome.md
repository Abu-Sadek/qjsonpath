This project provides an interface between JSON and Qt 5.x QML Model/View ListViews. It is implemented as a QML plugin so an application can be extended to use it without recompilation.

It consists of several parts:

  * JsonPath refactors and extends the C++ version of JsonPath to make it usable with Qt QML (version 5 or later). To reduce the number of dependencies this code was refactored to use the [Qt](Qt.md) Json Classes.

  * QJsonPath creates a QML compatible item that allows QML/javascripted applications to provide JSON strings and retrieve a parsed QJsonDocument object. The item uses JsonPath to extract a subset of the JSON string as an array.

  * QJsonListModel creates a QML compatible item that implements a standard Qt Model from a QJsonDocument object.

[Example](Example.md) QML is provided to illustrate usage.

Unit tests and [Netbeans](Netbeans.md) projects are included but not necessary.

[JsonPath](JsonPath.md) is a proposal by Stefan Gössner to implement XPath for JSON.