//import QtQuick 2.10
import QtQuick 2.0
import QtQuick.Window 2.2
//import QtQuick.Controls 2.2
import QtQuick.Controls 1.4


//引入我们注册的模块
import MyCppObject 1.0
Item {
    id: hw2
    visible: true
    width: 500
    height: 500
    /*
    Loader{
        objectName: "mainPageLoader"
        anchors.fill:  parent
    }*/

    CppObject{
        id: cppObject
        onInputChanged://相应inputChanged信号
        {
            processInput(canvas_ruler.rulerWidth,canvas_ruler.width,canvas_ruler.rulerWidth/2);
        }
        onInterPolyChanged:
        {
            canvas_ruler.requestPaint()
        }
    }



    Row {
        id: colorTools
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 8
        }

        spacing: 4

        Repeater
        {
            id:colorRepeater
            model:["#33B5E5", "#99CC00", "#FFBB33", "#FFB4B4"]
            ColorSquare
            {
                id:index
                color: modelData
            }
        }
    }
    function getSate(text){
        if(text==="Lagrange"){
            return cppObject.interPoly.visible;
        }else if(text==="LeastSquares"){
            return cppObject.leastSquares.visible;
        }else if(text==="Regression"){
            return cppObject.regression.visible;
        }else if(text==="Gauss"){
            return cppObject.interGauss.visible;
        }else{
            console.error("unknow checkbox")
            return false;
        }
    }

    Row{
        id: checkBoxes
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: colorTools.bottom
            topMargin: 8
        }

        spacing: 4
        Repeater
        {
            id:checkBoxRepeater
            model: ["Lagrange","LeastSquares","Regression","Gauss"]
            CheckBox
            {
                text: modelData
                Component.onCompleted: checked =getSate(text)
                onClicked:
                {
                    if(text=="Lagrange"){
                        cppObject.interPoly.visible=checked;
                    }else if(text=="LeastSquares"){
                        cppObject.leastSquares.visible=checked;
                    }else if(text=="Regression"){
                        cppObject.regression.visible=checked;
                    }else if(text=="Gauss"){
                        cppObject.interGauss.visible=checked;
                    }

                    canvas_ruler.requestPaint();
                }

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
    function drawPoint(ctx,pos){
        ctx.strokeStyle = "#FF0000"
        ctx.fillStyle = "red";
        if(pos.length>0){
            ctx.beginPath()
            for(var j = 0,len=pos.length; j < len; j++) {
                ctx.moveTo(pos[j].x, pos[j].y)
                ctx.arc(pos[j].x, pos[j].y,5,0,2*Math.PI)
                ctx.lineTo(pos[j].x, pos[j].y);
                ctx.fill()

            }

            ctx.stroke()
        }
    }

    function drawCurve(ctx,pos,lineColor="#FF0000"){
        ctx.strokeStyle = lineColor

        if(pos.length>0){
            ctx.beginPath()

            ctx.moveTo(pos[0].x, pos[0].y)
            for(var j = 1,len=pos.length; j < len; j++) {
                ctx.lineTo(pos[j].x, pos[j].y)
                ctx.moveTo(pos[j].x, pos[j].y)
            }

            ctx.stroke()
        }
    }

    Canvas {

        id: canvas_ruler
        anchors {
            left: parent.left
            right: parent.right
            top: checkBoxes.bottom
            bottom: parent.bottom
            margins: 50
        }

        property int ruleStyle: Main.RuleStyle.Up
        property real lastX:600
        property real lastY:600
        property real scaleX:1
        property real scaleY:1
        property color paintColor: "green"
        property real rulerWidth:20

        onPaint: {

            scaleX=parent.width/lastX;
            scaleY=parent.height/lastY;
            var ctx = getContext('2d')
            ctx.clearRect(0, 0, width,height)
            drawRuler(ctx,rulerWidth)
            drawCross(ctx,rulerWidth)
            drawPoint(ctx,cppObject.input)

            if(cppObject.interPoly.visible){
                drawCurve(ctx,cppObject.interPoly.pos,colorRepeater.model[0])
            }
            if(cppObject.leastSquares.visible){
                drawCurve(ctx,cppObject.leastSquares.pos,colorRepeater.model[1])
            }
            if(cppObject.regression.visible){
                drawCurve(ctx,cppObject.regression.pos,colorRepeater.model[2])
            }
            if(cppObject.interGauss.visible){
                drawCurve(ctx,cppObject.interGauss.pos,colorRepeater.model[3])
            }
        }

        MouseArea {
            id: area
            acceptedButtons:Qt.LeftButton | Qt.RightButton
            anchors.fill: parent
            /*
            onPressed: {
                canvas.lastX = mouseX
                canvas.lastY = mouseY
            }
            onPositionChanged: {
                canvas.requestPaint()
            }*/
            onClicked: (mouse)=>{
                           if (mouse.button === Qt.RightButton ) {
                               option_menu.open()
                           }else{
                               //canvas_ruler.pointArray.push({x:mouseX,y:mouseY})
                               cppObject.addInput(Qt.point(mouseX,mouseY))//调用Q_INVOKABLE宏标记的函数
                           }
                       }
            onReleased: {
                canvas_ruler.requestPaint()
            }
        }
    }
    Menu {
        id: option_menu


        MenuItem {
            text: "Clear"
            shortcut: "Ctrl+C"
            onTriggered: {
                cppObject.clearInput()
                canvas_ruler.requestPaint()
            }
        }

        MenuItem {
            text: "Modify"
            shortcut: "Ctrl+E"
            onTriggered: {}
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


