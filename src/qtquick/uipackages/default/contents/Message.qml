/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2017 Eike Hein <hein@kde.org>
*/

import QtQuick 2.7

import org.kde.kirigami 2.1 as Kirigami

Item {
    width: textArea.width // HACK Coupling to parent components is bad
    height: nick.height + messageText.height + Kirigami.Units.gridUnit

    Rectangle {
        id: avatar

        x: Kirigami.Units.gridUnit / 2

        width: height
        height: (nick.height * 2)

        anchors.verticalCenter: parent.verticalCenter

        color: model.NickColor

        radius: width * 0.5

        Text {
            anchors.fill: parent

            color: "white"

            font.weight: Font.Bold
            font.pointSize: 100
            minimumPointSize: theme.defaultFont.pointSize
            fontSizeMode: Text.Fit

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            text: model.Nick.match(/[a-zA-Z]/).pop().toUpperCase() // HACK
        }
    }

    Text {
        id: nick

        y: Kirigami.Units.gridUnit / 2

        anchors.left: avatar.right
        anchors.leftMargin: Kirigami.Units.gridUnit / 2

        text: model.Nick
        color: model.NickColor
        font.weight: Font.Bold

        Component.onCompleted: {
            font.pixelSize = font.pixelSize * 1.1;
        }
    }

    Text {
        id: timeStamp

        y: Kirigami.Units.gridUnit / 2

        height: nick.height

        anchors.left: nick.right
        anchors.leftMargin: Kirigami.Units.gridUnit / 2

        text: model.TimeStamp
        color: "grey"

        verticalAlignment: Text.AlignVCenter
    }

    Text {
        id: messageText

        anchors.left: avatar.right
        anchors.leftMargin: Kirigami.Units.gridUnit / 2
        anchors.right: parent.right
        anchors.top: nick.bottom

        text: model.display
        textFormat: Text.StyledText

        wrapMode: Text.WordWrap

        onLinkActivated: Qt.openUrlExternally(link)

        Component.onCompleted: {
            font.pixelSize = font.pixelSize * 1.1;
        }
    }
}

