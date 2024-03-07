//import QtQuick 2.10
import QtQuick 2.0
import QtQuick.Window 2.2
//import QtQuick.Controls 2.2
import QtQuick.Controls 1.4

//引入我们注册的模块
import MyCppObject 1.0
Item {
    id: hw5
    visible: true
    width: 1000
    height: 1000
    /*
    Loader{
        objectName: "mainPageLoader"
        anchors.fill:  parent
    }*/
    property int selectIndex:0

    CppObject{
        id: cppObject
        curveType: CppObject.Bezier
        onInputChanged://相应inputChanged信号
        {
            bezier();
            canvas_ruler.requestPaint();
        }
    }


    ExclusiveGroup { id: colorType }
    Row {
        id: lineTool
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

    function cubicDraw(ctx,input,pos,lineColor="#FF0000"){

        if(input.length>0){
            var i,len;

            //ctx.fillStyle="black";
            for(i = 0,len=input.length; i < len; i++) {

                if(i%3 == 0){

                    ctx.beginPath();
                    ctx.fillStyle="black";
                    ctx.moveTo(input[i].x,input[i]);
                    ctx.arc(input[i].x,input[i].y,5,0,2*Math.PI);
                    ctx.fill();
                    ctx.stroke();
                }else{
                    ctx.beginPath();
                    ctx.fillStyle= "blue";
                    ctx.moveTo(input[i].x-5,input[i]-5);
                    ctx.fillRect(input[i].x-5,input[i].y-5,10,10);
                    ctx.fill();
                    ctx.stroke();
                }
            }
            ctx.strokeStyle = lineColor;
            ctx.beginPath();
            for(i = 0,len=input.length; i < len-1; i++) {
                if(i%3!=1){

                    ctx.moveTo(input[i].x,input[i].y)
                    ctx.lineTo(input[i+1].x,input[i+1].y)

                }


            }
            ctx.stroke();

            if(pos.length>0){
                ctx.beginPath();
                ctx.moveTo(pos[0].x,pos[0].y);
                for(i = 1,len=pos.length; i < len; i++) {
                    ctx.lineTo(pos[i].x,pos[i].y);
                    ctx.moveTo(pos[i].x,pos[i].y);
                }

                ctx.lineWidth = 1;
                ctx.stroke();
            }
        }
    }


    Canvas {

        id: canvas_ruler
        anchors {
            left: parent.left
            right: parent.right
            top: lineTool.bottom
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
            cubicDraw(ctx,cppObject.input,cppObject.controlArray.pos,lineColor.model[selectIndex]);
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
                               if(cppObject.change&&cppObject.moveNodeNum!=-1){
                                   cppObject.setControl(Qt.point(mouseX,mouseY))
                                   canvas_ruler.requestPaint();
                                   hoverEnabled=false;
                                   cppObject.moveNodeNum=-1
                               }else{
                                   option_menu.open()
                               }

                           }else{
                               if(cppObject.change){
                                   if(cppObject.moveNodeNum==-1){
                                       cppObject.findSuitableCtrlPoint(Qt.point(mouseX,mouseY))
                                       console.log(cppObject.moveNodeNum)
                                   }
                                   if(cppObject.moveNodeNum!=-1){
                                       hoverEnabled=true
                                   }
                               }else{
                                   cppObject.addInput(Qt.point(mouseX,mouseY))//调用Q_INVOKABLE宏标记的函数
                               }


                           }
                       }
            onReleased: {
                canvas_ruler.requestPaint()
            }
            onPositionChanged:{
                if(cppObject.change&&cppObject.moveNodeNum!=-1){
                    cppObject.setControl(Qt.point(mouseX,mouseY))
                    canvas_ruler.requestPaint();
                }
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
            Component.onCompleted: text=(cppObject.change?"No Modify":"Modify")
            onTriggered: {
                if(cppObject.change){
                    cppObject.change=false;
                    text="Modify"
                }else{
                    cppObject.change=true;
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


