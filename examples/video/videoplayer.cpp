/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "videoplayer.h"
#include "videoview.h"
#include "fbovideosurface.h"

#include <QtGui>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QGraphicsView>

#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>
#include <Phonon/SeekSlider>
#include <Phonon/VolumeSlider>
#include <Phonon/VideoSurfaceOutput>

VideoPlayer::VideoPlayer(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    mediaObject = new Phonon::MediaObject(this);
    Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
    Phonon::createPath(mediaObject, audioOutput);

    VideoView *glVideoView = new VideoView(this);
    Phonon::VideoSurfaceOutput *videoOutput = new Phonon::VideoSurfaceOutput(glVideoView->videoSurface(),this);
    Phonon::createPath(mediaObject, videoOutput);

    Phonon::SeekSlider *seekSlider = new Phonon::SeekSlider(mediaObject,this);

    connect(mediaObject,SIGNAL(stateChanged(Phonon::State,Phonon::State)),
             this, SLOT(stateChanged(Phonon::State,Phonon::State)));


    QMenu *menu = new QMenu(this);
    menu->addAction(tr("Open File..."), this, SLOT(openFile()));

    QPushButton *menuButton = new QPushButton(tr("Video"));
    menuButton->setMenu(menu);

    playButton = new QPushButton("Play", this);
    playButton->setEnabled(false);
    connect(mediaObject, SIGNAL(hasVideoChanged(bool)),
            playButton, SLOT(setEnabled(bool)));

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(menuButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(new QLabel("Seek:", this));
    controlLayout->addWidget(seekSlider);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(glVideoView);
    layout->addLayout(controlLayout);
    setLayout(layout);
}

VideoPlayer::~VideoPlayer()
{
    mediaObject->stop();
}

void VideoPlayer::openFile()
{
    openFile(QFileDialog::getOpenFileName(this));
}

void VideoPlayer::openFile(const QString &fileName)
{
    if (!fileName.isEmpty()) {
        mediaObject->setCurrentSource(fileName);
        mediaObject->pause();
    }
}

void VideoPlayer::stateChanged(Phonon::State newState, Phonon::State oldState)
{
    Q_UNUSED(oldState);
    switch (newState) {
    case Phonon::PlayingState:
    case Phonon::BufferingState:
        playButton->setText(tr("Pause"));
        disconnect(playButton, SIGNAL(clicked()), mediaObject, SLOT(play()));
        connect(playButton, SIGNAL(clicked()), mediaObject, SLOT(pause()));
        break;
    case Phonon::StoppedState:
    case Phonon::PausedState:
        playButton->setFocus();
        playButton->setText(tr("Play"));
        disconnect(playButton, SIGNAL(clicked()), mediaObject, SLOT(pause()));
        connect(playButton, SIGNAL(clicked()), mediaObject, SLOT(play()));
        break;
    case Phonon::ErrorState:
        QMessageBox::critical( this, "Phonon Error", mediaObject->errorString());
        break;
    default:
        break;
    }
}

