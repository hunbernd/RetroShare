/*******************************************************************************
 * gui/statusbar/hashingstatus.cpp                                             *
 *                                                                             *
 * Copyright (c) 2008 Retroshare Team <retroshare.project@gmail.com>           *
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

#include <QLayout>
#include <QLabel>
#include <QMovie>
#include <QToolButton>

#include "retroshare/rsfiles.h"
#include "hashingstatus.h"
#include "gui/common/ElidedLabel.h"
#include "gui/notifyqt.h"

HashingStatus::HashingStatus(QWidget *parent)
 : QWidget(parent)
{
    QHBoxLayout *hbox = new QHBoxLayout(this);
    hbox->setMargin(0);
    hbox->setSpacing(6);
        
    movie = new QMovie(":/images/loader/indicator-16.gif");
    movie->setSpeed(80); // 2x speed
    hashloader = new QLabel(this);
    hashloader->setMovie(movie);
    hbox->addWidget(hashloader);

    movie->jumpToNextFrame(); // to calculate the real width
    statusHashing = new ElidedLabel(this);
    hbox->addWidget(statusHashing);

    _compactMode = false;

    setLayout(hbox);

    hashloader->hide();
    statusHashing->hide();

    connect(NotifyQt::getInstance(), SIGNAL(hashingInfoChanged(const QString&)), SLOT(updateHashingInfo(const QString&)));
}

HashingStatus::~HashingStatus()
{
    delete(movie);
}

void HashingStatus::updateHashingInfo(const QString& s)
{
    if (s.isEmpty())
	{
        statusHashing->hide() ;
        hashloader->hide() ;
        setToolTip(QString());

        movie->stop() ;
    } else {
        setToolTip(s + "\n"+QObject::tr("Click to pause the hashing process"));

        if (_compactMode) {
            statusHashing->hide() ;
        } else {
            statusHashing->setText(s) ;
            statusHashing->show() ;
        }
        hashloader->show() ;

        movie->start() ;
    }
}

void HashingStatus::mousePressEvent(QMouseEvent *)
{
	rsFiles->togglePauseHashingProcess() ;

	if(rsFiles->hashingProcessPaused())
	{
		movie->stop() ;
		hashloader->setPixmap(QPixmap(":/images/resume.png")) ;

		mLastText = statusHashing->text();
		statusHashing->setText(QObject::tr("[Hashing is paused]"));
        setToolTip(QObject::tr("Click to resume the hashing process"));
	}
	else
	{
		hashloader->setMovie(movie) ;
		statusHashing->setText(mLastText);
		movie->start() ;
	}
}
