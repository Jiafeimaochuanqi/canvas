import QtQuick 2.2
import QtQuick.Window 2.2
import QtQuick.Controls 1.0
//import QtQuick.Controls 2.2
//import Qt.labs.platform 1.0


Window {
    id:root
    visible: true
    width: 1200
    height: 1200
    /*
    Loader{
        objectName: "mainPageLoader"
        anchors.fill:  parent
    }*/
	
    enum RuleStyle {
        Up,
        Down
    }


    TabView {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left

        Tab {

            title: "HW1"
            active: true

            source : "qrc:/HW1.qml"
        }
        Tab{
            title:"HW2"
            active: false
            source : "qrc:/HW2.qml"
        }
        Tab{
            title:"HW3"
            active: false
            source : "qrc:/HW3.qml"
        }
        Tab{
            title:"HW4"
            active: false
            source : "qrc:/HW4.qml"
        }
        Tab{
            title:"HW5"
            active: false
            source : "qrc:/HW5.qml"
        }
        Tab{
            title:"Bezier"
            active: false
            source : "qrc:/Bezier.qml"
        }
        Tab{
            title:"Voronoi2d"
            active: false
            source : "qrc:/Voronoi2D.qml"
        }
        Tab{
            title:"Hull2d"
            active: false
            source : "qrc:/Hull2d.qml"
        }
        Tab{
            title:"Delaunay2d"
            active: false
            source : "qrc:/Delaunay2d.qml"
        }
    }





}

