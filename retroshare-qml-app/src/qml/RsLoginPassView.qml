/*
 * RetroShare Android QML App
 * Copyright (C) 2016  Gioacchino Mazzurco <gio@eigenlab.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQml 2.2
import org.retroshare.qml_components.LibresapiLocalClient 1.0

Item
{
	id: loginView
	property string buttonText: "Login"
	property string login
	property string password
	signal submit(string login, string password)

	ColumnLayout
	{
		id: inputView
		width: parent.width
		anchors.top: parent.top
		anchors.bottom: bottomButton.top

		Row { Text {text: "Name:" } TextField { id: nameField; text: loginView.login } }
		Row { Text {text: "Password:" } TextField { id: passwordField; text: loginView.password } }
	}

	Button
	{
		id: bottomButton
		text: loginView.buttonText
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		onClicked: loginView.submit(nameField.text, passwordField.text)
	}
}
