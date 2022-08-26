import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.4 as Controls14
import QtQuick.Controls.Styles 1.4 as ControlsStyle14
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.15
import OpenGLItem 1.0

Window {
    id:mainWindow
    visible: true
    visibility: "Maximized"
    title: qsTr("Structed Light Viewer-A visualizable tool designed by Liu Yunhuang")

    color: "black"

    property color theamColor: "#3e4d59"

    RowLayout{
        id: topLayout
        spacing: 0.5
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width
        height: 25

        //AddSouce控件
        Rectangle{
            id: addSourceRec
            Layout.fillHeight: true
            Layout.preferredWidth: 150
            color: theamColor

            Text {
                id: addDeviceText
                text: qsTr("Add Source")
                anchors.left: icon.right
                anchors.top: parent.top
                anchors.topMargin: 8
                anchors.leftMargin: 5
                color: "white"
                font{
                    family: "Arial"
                    pixelSize: 8
                }
            }

            Image{
                id: icon
                width: 10
                height: 10
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.topMargin: 7
                source:"qrc:/icon/icon/addDevice.png"
            }

            MouseArea{
                anchors.fill: parent
                hoverEnabled: true

                onEntered:
                    parent.color = "#586773"

                onExited:
                    parent.color = theamColor

                 onPressed:{
                    tipConnect.text = "";
                    //realTimeCamera.visible = true;
                    cameraShowAnimation.start();
                 }
            }
        }

        //2D、3D切换;设置工具
        Rectangle{
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: theamColor

            //2D
            Rectangle{
                anchors.right: vertiaclSpace.left
                anchors.top: parent.top
                height: parent.height
                width: 25
                color: parent.color

                Text {
                    id: text2D
                    anchors.centerIn: parent
                    text: qsTr("2D")
                    font{
                        family: "Arial"
                        pixelSize: 8
                    }
                    color: "white"
                }

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true

                    onEntered:
                        parent.color = parent.color = "#586773"

                    onExited:
                        parent.color = theamColor

                    onClicked:{
                        text2D.color = "#158dbc";
                        text3D.color = "white";
                        opengl.setDisplayState(0);
                        opengl.magCofficient = 1;
                        opengl.displayState = false;
                    }

                }
            }

            //分隔符
            Rectangle{
                id: vertiaclSpace
                anchors.right: threeDimension.left
                anchors.top: parent.top
                width: 7
                height: parent.height
                color: parent.color

                Image{
                    width: parent.width
                    height: parent.height - 6
                    anchors.centerIn: parent
                    source: "qrc:/icon/icon/vertical.png"
                }
            }


            //3D
            Rectangle{
                id: threeDimension
                anchors.right: setting.left
                anchors.top: parent.top
                height: parent.height
                color: parent.color
                width: 25

                Text {
                    id:text3D
                    anchors.centerIn: parent
                    text: qsTr("3D")
                    font{
                        family: "Arial"
                        pixelSize: 8
                    }
                    color: "#158dbc"
                }

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true

                    onEntered:
                        parent.color = parent.color = "#586773"

                    onExited:
                        parent.color = theamColor

                    onClicked:{
                        text2D.color = "white";
                        text3D.color = "#158dbc";
                        opengl.setDisplayState(1);
                        opengl.displayState = true;
                    }
                }
            }

            //设置
            Rectangle{
                id: setting
                anchors.right: parent.right
                anchors.top: parent.top
                height: parent.height
                width: 25
                color: parent.color

                Image {
                    width: parent.width
                    height: parent.height
                    source: "qrc:/icon/icon/setting.png"
                    anchors.centerIn: parent
                    scale: 0.5
                }

                ToolTip{
                    id:moreOptionsTip
                    implicitWidth: 50
                    implicitHeight: 20
                    clip: true
                    font{
                        pixelSize: 6
                        family: "Arial"
                    }

                    ColorAnimation {
                        from: "black"
                        to: "white"
                        duration: 200
                    }
                    delay: 200
                    text: "More Options"
                }

                MouseArea{
                    id:mouseAreaSetting
                    anchors.fill: parent
                    hoverEnabled: true

                    onEntered:{
                        moreOptionsTip.show(moreOptionsTip.text);
                        parent.color = parent.color = "#586773";
                    }

                    onExited:{
                        moreOptionsTip.hide();
                        parent.color = theamColor;
                    }

                    onPressed: {
                        displayAnimation.start();
                    }
                }
            }
        }
    }

    RowLayout{
        anchors.left: parent.left
        anchors.top:  topLayout.bottom
        width: topLayout.width
        height: parent.height - topLayout.height
        spacing: 0.5
        Rectangle{
            implicitWidth: addSourceRec.width
            Layout.fillHeight: true
            border.width: 0
            color: "black"
            Text {
                id: tipConnect
                anchors.fill: parent
                anchors.top: parent.top
                horizontalAlignment: Text.AlignHCenter
                anchors.topMargin: 30
                font{
                    family: "Arial"
                    pixelSize: 8
                }
                color: "#546b7e"
                text: qsTr("Connect to Structed Light Camera \n to Add Source")
                clip: true
                wrapMode: Text.WordWrap
                z:1
            }

            Controls14.TreeView{
                id: deviceView
                implicitWidth: addSourceRec.width - 5
                implicitHeight: parent.height
                style: treeViewStyle
                frameVisible: false
                backgroundVisible: true
                Component{
                    id:treeViewStyle

                    ControlsStyle14.TreeViewStyle{
                        //indentation: 20
                        backgroundColor: "black"
                    }
                }
            }

            Rectangle{
                id:realTimeCamera

                implicitHeight: 60
                anchors.left: deviceView.left
                anchors.top: deviceView.top
                anchors.right: deviceView.right
                anchors.topMargin: 5
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                color: "#004555"
                radius: 8
                visible: false
                z: 2

                NumberAnimation{
                    id:cameraShowAnimation
                    target: realTimeCamera
                    properties: "visible"
                    from: 0
                    to: 1
                    duration: 1000
                    easing.type: Easing.OutCubic
                }

                Image{
                    source: "qrc:/icon/icon/camera.png"
                    anchors.left:parent.left
                    anchors.top:parent.top
                    anchors.leftMargin: 20
                    anchors.topMargin: 5
                    width: 30
                    height: 30

                    Text {
                        anchors.top: parent.bottom
                        anchors.left: parent.left
                        anchors.leftMargin: -15
                        text: qsTr("Real time structed \n light camera")
                        font{
                            family: "Arial"
                            pixelSize: 8
                        }
                        color:"white"
                    }
                }

                Rectangle{
                    id:connectState
                    implicitWidth: 10
                    implicitHeight: 10
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: connectButton.left
                    anchors.rightMargin: 2
                    radius: 5
                    color: "red"

                    ColorAnimation {
                        id:colorAnimation
                        target: connectState
                        property: "color"
                        to: "green"
                        duration: 200
                    }
                }

                Rectangle{
                    id:connectButton
                    implicitWidth: 50
                    implicitHeight: 25
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 2
                    radius: 10
                    Text {
                        id:connectButtonTip
                        anchors.centerIn: parent
                        text: qsTr("connect")
                        font{
                            pixelSize: 10
                            family: "Arial"
                        }
                        color: "white"
                    }
                    color: theamColor

                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true

                        onEntered:
                            parent.color = "#586773"

                        onExited:
                            parent.color = theamColor

                        onPressed: {
                           colorAnimation.start();
                           if(!opengl.connectCamera()){
                                //连接失败警告，更完善的软件设计方式
                           }
                           connectButtonTip.text = "disconnect"
                        }
                    }
                }
            }

            Rectangle{
                implicitWidth: 5
                implicitHeight: parent.height
                anchors.left: deviceView.right
                anchors.top: deviceView.top
                anchors.bottom: deviceView.bottom
                anchors.topMargin: 2
                anchors.bottomMargin: 2
                color: "#364243"

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true

                    onEntered:
                        parent.color = "#505c5d"
                    onExited:
                        parent.color = "#364243"
                }
            }
        }
        ColumnLayout{
            spacing: 0
            implicitWidth: parent.width - deviceView.width - 5
            implicitHeight: parent.height

            Rectangle{
                id:displayScence
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 1
                color: "black"

                property int widthButton: 32

                Rectangle{
                    id:displaySetting
                    implicitWidth: parent.width
                    implicitHeight: 30
                    anchors.left: parent.left
                    anchors.top: parent.top
                    color: "#242c33"

                    //开始暂停按钮
                    Button{
                        id: pauseButton
                        anchors.left: parent.left
                        anchors.top: parent.top
                        implicitWidth: displayScence.widthButton
                        //enabled: true

                        property bool isStart: false

                        Image {
                            id:iconPause
                            source: pauseButton.isStart ? (pauseButton.enabled ? "qrc:/icon/icon/pauseEnable.png" : "qrc:/icon/icon/pauseDisable.png") : "qrc:/icon/icon/start.png"
                            width: parent.width
                            height: parent.height
                            horizontalAlignment: Image.AlignHCenter
                            anchors.fill: parent
                            scale: 0.4
                        }

                        background: Rectangle{
                            color: displaySetting.color
                        }

                        contentItem: Text{
                            text: pauseButton.isStart ? "Pause" : "Start"
                            font{
                                family: "Arial"
                                pixelSize: 6
                            }
                            horizontalAlignment: Text.AlignHCenter
                            anchors.top: iconPause.bottom
                            color: pauseButton.enabled ? "white" : "#3e4d59"
                        }

                        ToolTip{
                            id:pasueStreamingTip
                            implicitWidth: 60
                            implicitHeight: 30
                            clip: true
                            font{
                                pixelSize: 6
                                family: "Arial"
                            }

                            ColorAnimation {
                                from: "black"
                                to: "white"
                                duration: 200
                            }
                            delay: 200
                            text: pauseButton.isStart ? "Pause streaming" : "Start streaming"
                        }

                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true

                            onEntered:
                                pasueStreamingTip.show(pasueStreamingTip.text)
                            onExited:
                                pasueStreamingTip.close()

                            onPressed: {
                                pauseButton.isStart = !pauseButton.isStart;
                                if(pauseButton.isStart)
                                    opengl.start();
                                else
                                    opengl.pause();
                            }
                        }
                    }

                    //重置视角按钮
                    Button{
                        id: resetButton
                        anchors.left: pauseButton.right
                        anchors.top: parent.top
                        implicitWidth: displayScence.widthButton

                        Image {
                            id:iconReset
                            source: "qrc:/icon/icon/reset.png"
                            width: parent.width
                            height: parent.height
                            horizontalAlignment: Image.AlignHCenter
                            anchors.fill: parent
                            scale: 0.4
                        }

                        background: Rectangle{
                            color: displaySetting.color
                        }

                        contentItem: Text{
                            text: "Reset"
                            font{
                                family: "Arial"
                                pixelSize: 6
                            }
                            horizontalAlignment: Text.AlignHCenter
                            anchors.top: iconReset.bottom
                            color: "white"
                        }

                        ToolTip{
                            id:resetTip
                            implicitWidth: 60
                            implicitHeight: 30
                            clip: true
                            font{
                                pixelSize: 6
                                family: "Arial"
                            }

                            ColorAnimation {
                                from: "black"
                                to: "white"
                                duration: 200
                            }
                            delay: 200
                            text:"Reset 3D viewport to initial state"
                        }

                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true

                            onEntered:
                                resetTip.show(resetTip.text);
                            onExited:
                                resetTip.close();
                            onPressed:{
                                if(opengl.displayState){
                                    opengl.resetCamera();
                                    opengl.fovy = 80;
                                }
                                else{
                                    opengl.magCofficient = 1;
                                    opengl.updateMagCofficient(opengl.magCofficient);
                                }
                            }

                        }
                    }

                    //锁定解锁按钮
                    Button{
                        id: unlockButton
                        anchors.left: resetButton.right
                        anchors.top: parent.top
                        implicitWidth: displayScence.widthButton
                        //enabled: true

                        property bool isStart: false

                        Image {
                            id:iconUnlock
                            source: unlockButton.isStart ? (unlockButton.enabled ? "qrc:/icon/icon/lockEnable.png" : "qrc:/icon/icon/lockDisable.png") : "qrc:/icon/icon/unlock.png"
                            width: parent.width
                            height: parent.height
                            horizontalAlignment: Image.AlignHCenter
                            anchors.fill: parent
                            scale: 0.5
                        }

                        background: Rectangle{
                            color: displaySetting.color
                        }

                        contentItem: Text{
                            text: unlockButton.isStart ? "Lock" : "UnLock"
                            font{
                                family: "Arial"
                                pixelSize: 6
                            }
                            horizontalAlignment: Text.AlignHCenter
                            anchors.top: iconUnlock.bottom
                            color: iconUnlock.enabled ? "white" : "#3e4d59"
                        }

                        ToolTip{
                            id:unlockTip
                            implicitWidth: 60
                            implicitHeight: 30
                            clip: true
                            font{
                                pixelSize: 6
                                family: "Arial"
                            }

                            ColorAnimation {
                                from: "black"
                                to: "white"
                                duration: 200
                            }
                            delay: 200
                            text:unlockButton.isStart ? "Lock texture data from pointcloud" : "Unlock texture data from pointcloud"
                        }

                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true

                            onEntered:
                                unlockTip.show(unlockTip.text)
                            onExited:
                                unlockTip.close()
                            onPressed:
                                unlockButton.isStart = !unlockButton.isStart;
                        }
                    }

                    //分隔符
                    Rectangle{
                        id: spaceLockSource
                        anchors.left: unlockButton.right
                        anchors.top: parent.top
                        width: 7
                        height: parent.height
                        color: parent.color

                        Image{
                            width: parent.width
                            height: parent.height - 6
                            anchors.centerIn: parent
                            source: "qrc:/icon/icon/vertical.png"
                        }
                    }

                    //Source按钮
                    Button{
                        id: sourceButton
                        anchors.left: spaceLockSource.right
                        anchors.top: parent.top
                        implicitWidth: displayScence.widthButton

                        Image {
                            id:iconSource
                            source: sourceButton.enabled ? "qrc:/icon/icon/sourceEnable.png" : "qrc:/icon/icon/sourceDisable.png"
                            width: parent.width
                            height: parent.height
                            horizontalAlignment: Image.AlignHCenter
                            anchors.fill: parent
                            scale: 0.7
                        }

                        background: Rectangle{
                            color: displaySetting.color
                        }

                        contentItem: Text{
                            id:sourceText
                            text: "Source"
                            font{
                                family: "Arial"
                                pixelSize: 6
                            }
                            horizontalAlignment: Text.AlignHCenter - 5
                            anchors.top: iconSource.bottom
                            color: sourceButton.enabled ? "white" : "#3e4d59"
                        }

                        ToolTip{
                            id:sourceTip
                            implicitWidth: 60
                            implicitHeight: 30
                            clip: true
                            font{
                                pixelSize: 6
                                family: "Arial"
                            }

                            ColorAnimation {
                                from: "black"
                                to: "white"
                                duration: 200
                            }
                            delay: 200
                            text:"List of avaliable source data source"
                        }

                        Menu{
                            id: changeViewPortMenu

                            Action{
                                text: "left"

                                onTriggered: {
                                    opengl.setViewPort(false,false,true);
                                    iconSource.source = "qrc:/icon/icon/sourceEnable.png"
                                    sourceText.color = "white"
                                }
                            }

                            Action{
                                text: "upper"

                                onTriggered: {
                                    opengl.setViewPort(true,false,false);
                                    iconSource.source = "qrc:/icon/icon/sourceEnable.png"
                                    sourceText.color = "white"
                                }
                            }

                            Action{
                                text: "front"

                                onTriggered: {
                                    opengl.setViewPort(false,true,false);
                                    iconSource.source = "qrc:/icon/icon/sourceEnable.png"
                                    sourceText.color = "white"
                                }
                            }
                        }

                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true

                            onEntered:
                                sourceTip.show(sourceTip.text)
                            onExited:
                                sourceTip.close()
                            onPressed:{
                                changeViewPortMenu.popup();
                                sourceText.color = "#158dbc";
                                iconSource.source = "qrc:/icon/icon/sourceClick.png";
                            }

                            /*
                            onReleased: {
                            }
                            */
                        }
                    }

                    //Texture按钮
                    Button{
                        id: textureButton
                        anchors.left: sourceButton.right
                        anchors.top: parent.top
                        implicitWidth: displayScence.widthButton

                        Image {
                            id:iconTexture
                            source: sourceButton.enabled ? "qrc:/icon/icon/textureEnable.png" : "qrc:/icon/icon/textureDisable.png"
                            width: parent.width
                            height: parent.height
                            horizontalAlignment: Image.AlignHCenter
                            anchors.fill: parent
                            scale: 0.7
                        }

                        background: Rectangle{
                            color: displaySetting.color
                        }

                        contentItem: Text{
                            id:textureText
                            text: "Texture"
                            font{
                                family: "Arial"
                                pixelSize: 6
                            }
                            horizontalAlignment: Text.AlignHCenter - 5
                            anchors.top: iconTexture.bottom
                            color: sourceButton.enabled ? "white" : "#3e4d59"
                        }

                        ToolTip{
                            id:trxtureTip
                            implicitWidth: 60
                            implicitHeight: 30
                            clip: true
                            font{
                                pixelSize: 6
                                family: "Arial"
                            }

                            ColorAnimation {
                                from: "black"
                                to: "white"
                                duration: 200
                            }
                            delay: 200
                            text:"List of avaliable texture source"
                        }

                        Menu{
                            id:textureMenu

                            Action{
                                id: lightNight
                                text: qsTr("light night");
                                checkable: true
                                checked: !darkNight.checked

                                onTriggered:{
                                    opengl.changeScene(0);
                                    iconTexture.source = "qrc:/icon/icon/textureEnable.png"
                                    textureText.color = "white"
                                }

                            }

                            Action{
                                id: darkNight
                                text: qsTr("dark night");
                                checkable: true
                                checked: !lightNight.checked

                                onTriggered: {
                                    opengl.changeScene(1);
                                    iconTexture.source = "qrc:/icon/icon/textureEnable.png"
                                    textureText.color = "white"
                                }
                            }
                        }

                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true

                            onEntered:
                                trxtureTip.show(trxtureTip.text)
                            onExited:
                                trxtureTip.close()
                            onPressed:{
                                textureText.color = "#158dbc";
                                iconTexture.source = "qrc:/icon/icon/textureClick.png";
                            }

                            onClicked: {
                                textureMenu.popup();
                            }

                            /*
                            onReleased: {
                                iconTexture.source = "qrc:/icon/icon/textureEnable.png"
                                textureText.color = "white"
                            }
                            */
                        }
                    }

                    //Shading按钮
                    Button{
                        id: shadingButton
                        anchors.left: textureButton.right
                        anchors.top: parent.top
                        implicitWidth: displayScence.widthButton

                        Image {
                            id:iconShading
                            source: shadingButton.enabled ? "qrc:/icon/icon/ShadingEnable.png" : "qrc:/icon/icon/ShadingDisable.png"
                            width: parent.width
                            height: parent.height
                            horizontalAlignment: Image.AlignHCenter
                            anchors.fill: parent
                            scale: 0.7
                        }

                        background: Rectangle{
                            color: displaySetting.color
                        }

                        contentItem: Text{
                            id:shadingText
                            text: "Shading"
                            font{
                                family: "Arial"
                                pixelSize: 6
                            }
                            horizontalAlignment: Text.AlignHCenter - 5
                            anchors.top: iconShading.bottom
                            color: shadingButton.enabled ? "white" : "#3e4d59"
                        }

                        ToolTip{
                            id:shadingTip
                            implicitWidth: 60
                            implicitHeight: 30
                            clip: true
                            font{
                                pixelSize: 6
                                family: "Arial"
                            }

                            ColorAnimation {
                                from: "black"
                                to: "white"
                                duration: 200
                            }
                            delay: 200
                            text:"List of avaliable shading modes"
                        }

                        Menu{
                            id:shadingMenu

                            Action{
                                id: point
                                text: qsTr("point mode");
                                checkable: true
                                checked: !mesh.checked

                                onTriggered:{
                                    iconShading.source = "qrc:/icon/icon/ShadingEnable.png";
                                    shadingText.color = "white";
                                    opengl.setDisplayMode(0);
                                }

                            }

                            Action{
                                id: mesh
                                text: qsTr("mesh mode");
                                checkable: true
                                checked: !point.checked

                                onTriggered: {
                                    iconShading.source = "qrc:/icon/icon/ShadingEnable.png";
                                    shadingText.color = "white";
                                    opengl.setDisplayMode(2);
                                }
                            }
                        }

                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true

                            onEntered:
                                shadingTip.show(shadingTip.text)
                            onExited:
                                shadingTip.close()
                            onPressed:{
                                shadingMenu.popup();
                                shadingText.color = "#158dbc"
                                iconShading.source = "qrc:/icon/icon/ShadingClick.png"
                            }
                            /*
                            onReleased: {
                                iconShading.source = "qrc:/icon/icon/ShadingEnable.png"
                                shadingText.color = "white"
                            }
                            */
                        }
                    }

                    //分隔符
                    Rectangle{
                        id: spaceMeasure
                        anchors.left: shadingButton.right
                        anchors.top: parent.top
                        width: 7
                        height: parent.height
                        color: parent.color

                        Image{
                            width: parent.width
                            height: parent.height - 6
                            anchors.centerIn: parent
                            source: "qrc:/icon/icon/vertical.png"
                        }
                    }

                    //测量按钮
                    Button{
                        id: measureButton
                        anchors.left: spaceMeasure.right
                        anchors.top: parent.top
                        implicitWidth: displayScence.widthButton

                        Image {
                            id:iconMeasure
                            source: "qrc:/icon/icon/Measure.png"
                            width: parent.width
                            height: parent.height
                            horizontalAlignment: Image.AlignHCenter
                            anchors.fill: parent
                            scale: 0.5
                        }

                        background: Rectangle{
                            color: displaySetting.color
                        }

                        contentItem: Text{
                            id:textMeasure
                            text: "Measure"
                            font{
                                family: "Arial"
                                pixelSize: 6
                            }
                            horizontalAlignment: Text.AlignHCenter
                            anchors.top: iconMeasure.bottom
                            color: "white"
                        }

                        ToolTip{
                            id:measureTip
                            implicitWidth: 60
                            implicitHeight: 30
                            clip: true
                            font{
                                pixelSize: 6
                                family: "Arial"
                            }

                            ColorAnimation {
                                from: "black"
                                to: "white"
                                duration: 200
                            }
                            delay: 200
                            text:"Measure distance between two points\n
                                  Hold shift to connect more than 2 points and measure area"
                        }

                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true

                            onEntered:
                                measureTip.show(measureTip.text)
                            onExited:
                                measureTip.close()
                            onPressed:{
                                textMeasure.color = "#158dbc"
                                iconMeasure.source = "qrc:/icon/icon/MeasureClick.png"
                            }
                            /*
                            onReleased: {
                                iconMeasure.source = "qrc:/icon/icon/Measure.png"
                                textMeasure.color = "white"
                            }
                            */
                        }
                    }

                    //导出模型按钮
                    Button{
                        id: exportButton
                        anchors.left: measureButton.right
                        anchors.top: parent.top
                        implicitWidth: displayScence.widthButton

                        Image {
                            id:iconExport
                            source: exportButton.enabled ? "qrc:/icon/icon/exportEnable.png" : "qrc:/icon/icon/exportDisable.png"
                            width: parent.width
                            height: parent.height
                            horizontalAlignment: Image.AlignHCenter
                            anchors.fill: parent
                            scale: 0.5
                        }

                        background: Rectangle{
                            color: displaySetting.color
                        }

                        contentItem: Text{
                            id:textExport
                            text: "Export"
                            font{
                                family: "Arial"
                                pixelSize: 6
                            }
                            horizontalAlignment: Text.AlignHCenter
                            anchors.top: iconExport.bottom
                            color: exportButton.enabled ? "white" : "#3e4d59"
                        }

                        ToolTip{
                            id: exportTip
                            implicitWidth: 60
                            implicitHeight: 30
                            clip: true
                            font{
                                pixelSize: 6
                                family: "Arial"
                            }

                            ColorAnimation {
                                from: "black"
                                to: "white"
                                duration: 200
                            }
                            delay: 200
                            text:"Export 3D model to 3rd-party application"
                        }

                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true

                            onEntered:
                                exportTip.show(exportTip.text)
                            onExited:
                                exportTip.close()
                            onPressed:{
                                textExport.color = "#158dbc"
                                iconExport.source = "qrc:/icon/icon/exportClick.png"
                            }
                            /*
                            onReleased: {
                                iconExport.source = "qrc:/icon/icon/exportEnable.png"
                                textExport.color = "white"
                            }
                            */
                        }
                    }
                }
            }

            OpenGLItem{
                id:opengl
                implicitWidth: displaySetting.width
                implicitHeight: parent.height - displaySetting.height

                property point lastPoint: "0,0"
                property int fovy: 80
                property bool displayState: true
                property real magCofficient: 1

                Rectangle{
                    id:fpsRec
                    anchors.right: opengl.right
                    anchors.top: opengl.top
                    anchors.rightMargin: 10
                    anchors.topMargin: 10
                    implicitWidth: 40
                    implicitHeight: 20
                    opacity: 0
                }

                Text {
                    id: fpsLabel
                    text: qsTr("fps: " + opengl.fps.toString())
                    anchors.centerIn: fpsRec
                    font{
                        family: "Helvetica"
                        bold: true
                        pointSize: 10
                    }
                    color: "white"
                }

                MouseArea{
                    anchors.fill: parent
                    focus: true
                    acceptedButtons: Qt.AllButtons

                    property bool hasChoosen: false

                    onPressed: {
                        if(opengl.displayState){
                            if(mouse.button == Qt.LeftButton || mouse.button == Qt.MiddleButton){
                                parent.lastPoint.x = mouse.x;
                                parent.lastPoint.y = mouse.y;
                            }
                        }
                    }

                    onPositionChanged: {
                        if(opengl.displayState){
                            if(mouse.buttons == Qt.LeftButton){
                                var xMove = mouse.x - parent.lastPoint.x;
                                var yMove = mouse.y - parent.lastPoint.y;
                                parent.lastPoint.x = mouse.x;
                                parent.lastPoint.y = mouse.y;
                                opengl.updateQuaternion(xMove,yMove);
                            }
                            if(mouse.buttons == Qt.MiddleButton){
                                var xMove = mouse.x - parent.lastPoint.x;
                                var yMove = mouse.y - parent.lastPoint.y;
                                parent.lastPoint.x = mouse.x;
                                parent.lastPoint.y = mouse.y;
                                if(xMove < 0)
                                    opengl.updateCameraPos(false,true,false,false);
                                if(xMove > 0)
                                    opengl.updateCameraPos(true,false,false,false);
                                if(yMove > 0)
                                    opengl.updateCameraPos(false,false,true,false);
                                if(yMove < 0)
                                    opengl.updateCameraPos(false,false,false,true);
                            }
                        }
                    }

                    onWheel: {
                        if(opengl.displayState){
                            if(wheel.angleDelta.y > 0)
                                parent.fovy += 1;
                            else
                                parent.fovy -= 1;
                            if(parent.fovy > 90)
                                parent.fovy = 90;
                            if(parent.fovy < 1)
                                parent.fovy = 1;
                            opengl.updateFovy(parent.fovy);
                        }
                        else{
                            if(wheel.angleDelta.y > 0)
                                parent.magCofficient += 0.1;
                            else
                                parent.magCofficient -= 0.1;
                            if(parent.magCofficient > 10)
                                parent.magCofficient = 10;
                            if(parent.magCofficient < 0.1)
                                parent.magCofficient = 0.1;
                            opengl.updateMagCofficient(parent.magCofficient);
                        }
                    }

                    Keys.onPressed: {
                        if(opengl.displayState){
                            if(event.key == Qt.Key_A)
                                opengl.updateCameraPos(true,false,false,false);
                            if(event.key == Qt.Key_D)
                                opengl.updateCameraPos(false,true,false,false);
                            if(event.key == Qt.Key_W)
                                opengl.dragNearst();
                            if(event.key == Qt.Key_S)
                                opengl.dragFar();
                        }
                    }
                }
            }
        }
    }

    Window{
        id:settingFrame
        width: 640
        height: 480
        visible: false
        color: "#242C33"

        flags: Qt.FramelessWindowHint

        property int labelWidth: 20
        property int spinBoxWidth: 50
        property int controlHeight: 15

        NumberAnimation {
            id:displayAnimation
            target: settingFrame
            property: "visible"
            from: 0
            to: 1
            duration: 300
            easing.type: Easing.InOutQuad
        }

        Rectangle{
            id:settingsTip

            implicitWidth: parent.width
            implicitHeight: 15
            anchors.left: parent.left
            anchors.top: parent.top
            color: "#3E4D59"
            Text {
                anchors.fill: parent
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Settings")
                color: "white"
            }
        }

        Rectangle{
            id:labelMinDepth
            width: parent.labelWidth
            height: parent.controlHeight
            anchors.left: settingsTip.left
            anchors.top: settingsTip.bottom
            Text {
                anchors.fill: parent
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("minDepth")
                color: "white"
            }
            color: "blue"
            z:3
        }

        SpinBox{
            id:minDepthSb
            width: parent.spinBoxWidth
            height: parent.controlHeight
            anchors.left: labelMinDepth.right
            anchors.top: labelMinDepth.top
        }

    }
}
