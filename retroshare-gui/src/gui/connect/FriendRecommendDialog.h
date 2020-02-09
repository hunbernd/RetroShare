/*******************************************************************************
 * gui/connect/FriendRecommendDialog.h                                         *
 *                                                                             *
 * Copyright 2018      by Retroshare Team <retroshare.project@gmail.com>       *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify        *
 * it under the terms of the GNU Affero General Public License as              *
 * published by the Free Software Foundation, either version 3 of the          *
 * License, or (at your option) any later version.                             *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                *
 * GNU Affero General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Affero General Public License    *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.       *
 *                                                                             *
 *******************************************************************************/

#pragma once

#include <QDialog>
#include "ui_FriendRecommendDialog.h"

class FriendRecommendDialog: public QDialog
{
public:
	FriendRecommendDialog(QWidget *parent) ;
	virtual ~FriendRecommendDialog() ;

	static void showIt();

private:
	static FriendRecommendDialog *instance();

    virtual void accept() ;

	void load();

	Ui::FriendRecommendDialog *ui;
};
