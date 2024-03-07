//import QtQuick 2.10
import QtQuick 2.0
import QtQuick.Window 2.2
//import QtQuick.Controls 2.2
import QtQuick.Controls 1.4

Item {
    id: bezier
    visible: true
    width: 1000
    height: 1000
    /*
    Loader{
        objectName: "mainPageLoader"
        anchors.fill:  parent
    }*/
    property int selectIndex:0
    property bool lockSelect: false
    property bool cubicHideSelect: false
    property bool draggingCubic: false
    property bool change:false
    property var cubicPoints:[]
    property int dragPointIndex: -1
    Row {
        id: colorTool
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 8
        }

        spacing: 4

        Repeater
        {
            id:lineColor
            model:["#FF6432", "#32FF64", "#6432FF", "#00BFFF"]
            ColorSquare
            {
                color: modelData
                Component.onCompleted: active=(selectIndex === index?true:false)
                onClicked: {
                    for (var i = 0; i < lineColor.count; ++i){
                        lineColor.itemAt(i).active=false;
                    }
                    active=true;
                    selectIndex=index;
                }
            }
        }

    }

    Row{
        id: checkBoxes
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: colorTool.bottom
            topMargin: 8
        }

        spacing: 4
        CheckBox
        {
            id:lockSelect
            text: "Lock Control Point Pairs"
            onClicked: {
                doLock(cubicPoints);
            }
        }
        CheckBox
        {
            id:cubicHideSelect
            text: "Hide Controls"
            onClicked: {
                canvas_ruler.requestPaint();
            }
        }
    }

    function drawCross(ctx,rulerWidth){
        var config={
            width: canvas_ruler.width-rulerWidth,
            height: canvas_ruler.height-rulerWidth,
            // 刻度尺相关
            size: 600, // 刻度尺总刻度数
            w: 5, // 刻度线的间隔
            h: 10 // 刻度线基础长度
        }
        var w = config.w || 5
        var h = config.h || 10
        var hx = rulerWidth


        var cy=canvas_ruler.ruleStyle===Main.RuleStyle.Up?0:rulerWidth;
        var size = (config.size || 100) * 10 + 1
        ctx.fillStyle ="#111"    // 设置画笔属性
        ctx.strokeStyle = '#0000FF'
        ctx.lineWidth = 1
        for (var i = 0; i < size; i++) {  //画水平
            ctx.beginPath()
            if(i % 10 == 0&&i!=0){
                ctx.moveTo(hx + i * w,cy)
                ctx.lineTo(hx + i * w,config.height+cy)
            }
            // 画出路径
            ctx.stroke()
        }
        var cx=rulerWidth
        var vx =canvas_ruler.ruleStyle===Main.RuleStyle.Up?config.height:rulerWidth
        var sign=canvas_ruler.ruleStyle===Main.RuleStyle.Up?-1:1
        for (i = 0; i < size; i++) {  //画垂直
            ctx.beginPath()
            if(i % 10 == 0&&i!=0){
                ctx.moveTo(cx,vx + sign*i * w)
                ctx.lineTo( cx+config.width,vx + sign*i * w)
            }
            // 画出路径
            ctx.stroke()
        }
    }

    function drawRuler(ctx,rulerWidth) {
        var config={
            width: canvas_ruler.width-rulerWidth,
            height: canvas_ruler.height-rulerWidth,
            // 刻度尺相关
            size: 600, // 刻度尺总刻度数
            w: 5, // 刻度线的间隔
            h: 10 // 刻度线基础长度
        }
        var size = (config.size || 100) * 10 + 1
        var hx = rulerWidth
        var hy = canvas_ruler.ruleStyle===Main.RuleStyle.Up?config.height:0
        var vx = 0
        var vy = canvas_ruler.ruleStyle===Main.RuleStyle.Up?0:rulerWidth
        var w = config.w || 5
        var h = config.h || 10
        var offset = 3; // 上面数字的偏移量
        // 画之前清空画布
        ctx.clearRect(0, 0, config.width,config.height)
        ctx.fillStyle ="#F5DEB3"
        ctx.fillRect(vx, vy, rulerWidth,config.height)
        ctx.fillRect(hx,hy, config.width,rulerWidth);

        ctx.fillStyle ="#111"    // 设置画笔属性
        ctx.strokeStyle = '#333'
        ctx.lineWidth = 1
        //      ctx.font = 13

        hy+=2*h
        for (var i = 0; i < size; i++) {  //画水平
            ctx.beginPath()          // 开始一条路径
            // 移动到指定位置
            ctx.moveTo(hx + i * w, hy)
            // 满10刻度时刻度线长一些 并且在上方表明刻度
            if (i % 10 == 0) {
                // 计算偏移量
                offset = (String(i / 10).length * 6 / 2)
                ctx.fillText(i / 10,hx + i * w - offset+10 , hy - h * 1.2);
                ctx.lineTo(hx + i * w, hy - h * 2)
            } else {
                // 满5刻度时的刻度线略长于1刻度的
                ctx.lineTo(hx + i * w, hy - (i % 5 === 0 ? 1 : 0.6) * h)
            }
            // 画出路径
            ctx.stroke()
        }
        vy=rulerWidth
        vx=canvas_ruler.ruleStyle===Main.RuleStyle.Up?config.height:rulerWidth
        var sign=canvas_ruler.ruleStyle===Main.RuleStyle.Up?-1:1
        var textSize=5;


        for ( i = 0; i < size; i++) {  //画垂直
            ctx.beginPath()          // 开始一条路径
            // 移动到指定位置
            ctx.moveTo(vy,vx + sign*i * w)
            // 满10刻度时刻度线长一些 并且在上方表明刻度
            if (i % 10 == 0) {
                // 计算偏移量
                offset = (String(i / 10).length * 6 / 2)
                ctx.lineTo( vy - h * 2,vx +sign* i * w)
                ctx.fillText(i / 10, vy - h * 1.2-5,vx + sign*i * w +sign*10+offset);

            } else {
                // 满5刻度时的刻度线略长于1刻度的
                ctx.lineTo(vy - (i % 5 === 0 ? 1 : 0.6) * h,vx + sign*i * w)
            }
            // 画出路径
            ctx.stroke()
        }

    }

    function doLock(cubicPoints) {
        if ( lockSelect.checked ) {
            for(var i=4;i<cubicPoints.length;i+=3){
                cubicPoints[i].x = 2*cubicPoints[i-1].x - cubicPoints[i-2].x;
                cubicPoints[i].y = 2*cubicPoints[i-1].y - cubicPoints[i-2].y;
            }
        }
        canvas_ruler.requestPaint();
    }
    function disk( ctx, x, y, radius ) {
        ctx.beginPath();
        ctx.arc(x,y,radius,0,Math.PI*2);
        ctx.fill();

    }
    function cubicDraw(ctx,cubicPoints,lineColor="#FF0000"){
        var i;
        ctx.fillStyle = "white";
        //ctx.fillRect(0,0,600,600);
        if ( ! cubicHideSelect.checked ) {
            ctx.lineWidth = 1;
            if (lockSelect.checked) {
                ctx.strokeStyle = "#880000";
            }
            else {
                ctx.strokeStyle = "#888888";
            }
            for (i = 0; i < cubicPoints.length-1; i++) {
                if (i % 3 != 1) {
                    ctx.beginPath();
                    ctx.moveTo( cubicPoints[i].x + .5, cubicPoints[i].y + .5 );
                    ctx.lineTo( cubicPoints[i+1].x + .5, cubicPoints[i+1].y + .5 );
                    ctx.stroke();
                }
            }
            for (i = 0; i < cubicPoints.length; i++) {
                if ( i % 3 == 0 ) {
                    ctx.fillStyle="black";
                    disk(ctx, cubicPoints[i].x, cubicPoints[i].y, 5);
                }
                else {
                    ctx.fillStyle= "blue";
                    ctx.fillRect(cubicPoints[i].x - 5, cubicPoints[i].y - 5, 10, 10);

                }
            }
            if(cubicPoints.length>0){
                ctx.beginPath();
                ctx.moveTo(cubicPoints[0].x,cubicPoints[0].y);
                for (i = 1; i <= cubicPoints.length-3; i += 3) {
                    ctx.bezierCurveTo(cubicPoints[i].x,cubicPoints[i].y,
                                      cubicPoints[i+1].x,cubicPoints[i+1].y,
                                      cubicPoints[i+2].x,cubicPoints[i+2].y);
                }
                ctx.lineWidth = 2;
                //ctx.strokeStyle = "black";
                ctx.strokeStyle=lineColor;
                ctx.stroke();
            }
        }
    }
    function doMouseUp() {
        draggingCubic = false;
    }
    function doCubicMouseDown(ctx,cubicPoints,point) {
        if (draggingCubic || cubicHideSelect.checked) {
            return;
        }
        dragPointIndex=-1;
        var x = Math.round(point.x);
        var y = Math.round(point.y);
        if(cubicPoints.length>0){
            for (var i = cubicPoints.length-1; i >= 0; i--) {
                var p = cubicPoints[i];
                if (Math.abs(p.x - x) <= 5 && Math.abs(p.y - y) <= 5) {
                    draggingCubic = true;
                    dragPointIndex = i;
                    return;
                }
            }
        }
    }
    function doCubicMouseMove(ctx,cubicPoints,point) {
        if (!draggingCubic||dragPointIndex==-1) {
            return;
        }
        var x = Math.round(point.x);
        var y = Math.round(point.y);
        var offsetX = x - cubicPoints[dragPointIndex].x;
        var offsetY = y - cubicPoints[dragPointIndex].y;
        cubicPoints[dragPointIndex].x = x;
        cubicPoints[dragPointIndex].y = y;
        if ( dragPointIndex % 3 == 0) {
            if (dragPointIndex > 0) {
                cubicPoints[dragPointIndex - 1].x += offsetX;
                cubicPoints[dragPointIndex - 1].y += offsetY;
            }
            if (dragPointIndex < cubicPoints.length-1) {
                cubicPoints[dragPointIndex + 1].x += offsetX;
                cubicPoints[dragPointIndex + 1].y += offsetY;
            }
        }
        else if (lockSelect.checked) {
            if (dragPointIndex%3 == 2&&dragPointIndex<cubicPoints.length-2) {
                cubicPoints[dragPointIndex+2].x = 2*cubicPoints[dragPointIndex+1].x - cubicPoints[dragPointIndex].x;
                cubicPoints[dragPointIndex+2].y = 2*cubicPoints[dragPointIndex+1].y - cubicPoints[dragPointIndex].y;
            }
            else if (dragPointIndex%3 == 1&&dragPointIndex>= 2) {
                cubicPoints[dragPointIndex-2].x = 2*cubicPoints[dragPointIndex-1].x - cubicPoints[dragPointIndex].x;
                cubicPoints[dragPointIndex-2].y = 2*cubicPoints[dragPointIndex-1].y - cubicPoints[dragPointIndex].y;
            }
        }
    }
    Canvas {

        id: canvas_ruler
        anchors {
            left: parent.left
            right: parent.right
            top: checkBoxes.bottom
            bottom: parent.bottom
            //margins: 50
        }
        property int ruleStyle: Main.RuleStyle.Up
        property real rulerWidth:20

        onPaint: {

            var ctx = getContext('2d')
            ctx.clearRect(0, 0, width,height);
            drawRuler(ctx,rulerWidth);
            drawCross(ctx,rulerWidth);
            cubicDraw(ctx,cubicPoints,lineColor.model[selectIndex]);
        }

        MouseArea {
            id: area
            acceptedButtons:Qt.LeftButton | Qt.RightButton
            anchors.fill: parent


            onPressed: {
                var ctx = canvas_ruler.getContext('2d');
                if(change){
                    doCubicMouseDown(ctx,cubicPoints,Qt.point(mouseX,mouseY));
                }
            }
            onReleased: {
                doMouseUp();
                canvas_ruler.requestPaint()
            }

            /*
                onPositionChanged: {
                    canvas.requestPaint()
                }*/
            onClicked: (mouse)=>{
                           if (mouse.button === Qt.RightButton ) {
                               option_menu.open();
                           }else{
                               if(change)return;
                               cubicPoints.push(Qt.point(mouseX,mouseY));
                           }
                       }

            onPositionChanged:{
                var ctx = canvas_ruler.getContext('2d');
                doCubicMouseMove(ctx,cubicPoints,Qt.point(mouseX,mouseY));
                canvas_ruler.requestPaint();
            }
        }
    }
    Menu {
        id: option_menu


        MenuItem {
            text: "Clear"
            shortcut: "Ctrl+C"
            onTriggered: {
                cubicPoints=[];
                canvas_ruler.requestPaint();
            }
        }
        MenuItem {
            text: "Modify"
            shortcut: "Ctrl+E"
            Component.onCompleted: text=(change?"No Modify":"Modify")
            onTriggered: {
                if(change){
                    change=false;
                    text="Modify"
                }else{
                    change=true;
                    text="No Modify"
                }
                canvas_ruler.requestPaint()
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


