//import QtQuick 2.10
import QtQuick 2.0
import QtQuick.Window 2.2
//import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import Qt.labs.platform 1.1
//引入我们注册的模块
import MyCppObject 1.0
Item {
    id: delaunay2D
    visible: true
    width: 500
    height: 500
    CppObject{
        id: cppObject
        width:500
        height:500
    }
    onWidthChanged:{
        cppObject.width=width;
    }
    onHeightChanged: {
        cppObject.height=height;
    }
    /*
    Loader{
        objectName: "mainPageLoader"
        anchors.fill:  parent
    }*/
    property int selectIndex:0
    property bool lockSelect: false
    property bool cubicHideSelect: false
    property bool draggingCubic: false
    property bool start:false
    property bool play:false
    property var points:[]
    property int dragPointIndex: -1


    function disk( ctx, x, y, radius ) {
        ctx.beginPath();
        ctx.arc(x,y,radius,0,Math.PI*2);
        ctx.fill();

    }
    function pointsDraw(ctx,points,type="rect",color="#FF0000"){
        var i;
        ctx.fillStyle = color;

        for (i = 0; i < points.length; i++) {
            if(type==="rect"){
                ctx.fillRect(points[i].x - 5, points[i].y - 5, 10, 10);
            }else{
                disk(ctx,points[i].x,points[i].y,2);
            }
        }
    }
    function lineDraw(ctx,lines,lineColor="#0000FF"){
        var i;
        ctx.fillStyle = lineColor;
        ctx.beginPath()
        for (i = 0; i < lines.length/2; i++) {


            ctx.moveTo(lines[2*i+0].x,lines[2*i+0].y)
            ctx.lineTo( lines[2*i+1].x,lines[2*i+1].y)

            // 画出路径

        }
        ctx.stroke()
    }

    Canvas {

        id: canvas_ruler
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            //margins: 50
        }

        onPaint: {

            var ctx = getContext('2d')
            ctx.clearRect(0, 0, width,height);
            pointsDraw(ctx,points);
            if(play){
                pointsDraw(ctx,[cppObject.centroid],"cirlce","#0000FF");
            }
            lineDraw(ctx,cppObject.output);
            pointsDraw(ctx,cppObject.centroids,"cirlce","red");
        }


        MouseArea {
            id: area
            acceptedButtons:Qt.LeftButton | Qt.RightButton
            anchors.fill: parent


            onPressed: {
                var ctx = canvas_ruler.getContext('2d');

            }
            onReleased: {
                canvas_ruler.requestPaint()
            }

            /*
                onPositionChanged: {
                    canvas.requestPaint()
                }*/
            onClicked: (mouse)=>{
                           if(!start)return;
                           if (mouse.button === Qt.RightButton ) {
                               option_menu.open();
                           }else if(!play){
                               points.push(Qt.point(mouseX,mouseY));
                               cppObject.addPoint(Qt.point(mouseX,mouseY));
                               cppObject.delaunay2d();
                           }else{
                               cppObject.playDelaunay2d();
                           }
                       }

//            onPositionChanged:{
//                var ctx = canvas_ruler.getContext('2d');

//                canvas_ruler.requestPaint();
//            }
        }
    }
    Button {
        id:button
        text: "Click To Add Points"
        anchors {
            left: parent.center
            right: parent.center
            top: parent.center
            bottom: parent.center
            //margins: 50
        }
        onClicked: {
            button.destroy();
            start=true;
        }
    }
    Menu {
        id: option_menu


        MenuItem {
            text: "Clear"
            shortcut: "Ctrl+C"
            onTriggered: {
                points=[];
                canvas_ruler.requestPaint();
            }
        }
        MenuItem {
            text: "Play"
            shortcut: "P"
            Component.onCompleted: text=(play?"Stop Play":"Play")
            onTriggered: {
                if(play){
                    play=false;
                    text="Play"
                }else{
                    play=true;
                    text="Stop Play"
                }
            }
        }

        MenuSeparator { }

        Menu {
            title: "More Stuff"

            MenuItem {
                text: "Do Nothing"
            }
        }
    }





}


