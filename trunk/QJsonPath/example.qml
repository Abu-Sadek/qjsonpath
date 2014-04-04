import QtQuick 2.0
import QJson 0.1

// example usage for JsonListModel
// This example has a predefined json list in the qml source.
// The intended usage is getting json from a web service or file.
// written by Jay Sprenkle
Item
{
    id: mTop
    width: 800; // root item so give a size
    height: 600;

    // extract the names of people from the json and create a QJsonDocument.
    // The result will be an array
    JsonPath
    {
        id: jpath
        json: "{ \"People\":[ \"bob\", \"ted\", \"carol\", \"alice\" ] }";

        function startupFunction()
        {
            jpath.path = "$.People[*]";
        }
        Component.onCompleted: startupFunction();
    }

    // Create a list model from the QJsonDocument
    JsonListModel
    {
        id: jlist
        json: jpath.jsonDocument
    }

    // display the list using standard Qt 5 controls
    ListView
    {
        id: container
        width: 180;
        height: 200;
        anchors
        {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }

        // what to display
        model: jlist

        // how to display it
        delegate: Rectangle
        {
            width: container.width
            height : 20
            color: "blue"
            Text
            {
                width: container.width
                height : 20
                text: modelData
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
             }
         }
    }
}
