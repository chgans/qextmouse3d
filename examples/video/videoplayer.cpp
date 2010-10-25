/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
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

