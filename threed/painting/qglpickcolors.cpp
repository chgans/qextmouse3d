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

#if !defined(QGL_GENERATOR_PROGRAM)

#include "qglpickcolors_p.h"

QT_BEGIN_NAMESPACE

// The following tables are generated by the program listed at the
// end of this source file.

static int const pickColors[1024] = {
    0xffffff, 0xff0000, 0x00ff00, 0x0000ff, 0xffff00, 0xff00ff, 0x00ffff,
    0xff8000, 0x80ff00, 0x8000ff, 0xff0080, 0x0080ff, 0x00ff80, 0xff80ff,
    0x80ffff, 0xffff80, 0x80ff80, 0xff8080, 0x8080ff, 0x808080, 0x800000,
    0x008000, 0x000080, 0x808000, 0x800080, 0x008080, 0xff80c0, 0x80c0ff,
    0xc0ff80, 0xffc080, 0x80ffc0, 0xc080ff, 0xffc000, 0xc0ff00, 0xc000ff,
    0xff00c0, 0x00c0ff, 0x00ffc0, 0xffc0ff, 0xc0ffff, 0xffffc0, 0xc0ffc0,
    0xffc0c0, 0xc0c0ff, 0x80c000, 0xc08000, 0xc00080, 0x8000c0, 0x00c080,
    0x0080c0, 0x80c080, 0xc08080, 0x8080c0, 0xc080c0, 0x80c0c0, 0xc0c080,
    0xc0c0c0, 0xc00000, 0x00c000, 0x0000c0, 0xc0c000, 0xc000c0, 0x00c0c0,
    0xff8040, 0x8040ff, 0x40ff80, 0xff4080, 0x80ff40, 0x4080ff, 0xffc040,
    0xc040ff, 0x40ffc0, 0xff40c0, 0xc0ff40, 0x40c0ff, 0xff4000, 0x40ff00,
    0x4000ff, 0xff0040, 0x0040ff, 0x00ff40, 0xff40ff, 0x40ffff, 0xffff40,
    0x40ff40, 0xff4040, 0x4040ff, 0x80c040, 0xc04080, 0x4080c0, 0x8040c0,
    0xc08040, 0x40c080, 0x804000, 0x408000, 0x400080, 0x800040, 0x004080,
    0x008040, 0x804080, 0x408080, 0x808040, 0x408040, 0x804040, 0x404080,
    0xc04000, 0x40c000, 0x4000c0, 0xc00040, 0x0040c0, 0x00c040, 0xc040c0,
    0x40c0c0, 0xc0c040, 0x40c040, 0xc04040, 0x4040c0, 0x404040, 0x400000,
    0x004000, 0x000040, 0x404000, 0x400040, 0x004040, 0xff80e0, 0x80e0ff,
    0xe0ff80, 0xffe080, 0x80ffe0, 0xe080ff, 0xffc0e0, 0xc0e0ff, 0xe0ffc0,
    0xffe0c0, 0xc0ffe0, 0xe0c0ff, 0xff40e0, 0x40e0ff, 0xe0ff40, 0xffe040,
    0x40ffe0, 0xe040ff, 0xffe000, 0xe0ff00, 0xe000ff, 0xff00e0, 0x00e0ff,
    0x00ffe0, 0xffe0ff, 0xe0ffff, 0xffffe0, 0xe0ffe0, 0xffe0e0, 0xe0e0ff,
    0x80c0e0, 0xc0e080, 0xe080c0, 0x80e0c0, 0xc080e0, 0xe0c080, 0x8040e0,
    0x40e080, 0xe08040, 0x80e040, 0x4080e0, 0xe04080, 0x80e000, 0xe08000,
    0xe00080, 0x8000e0, 0x00e080, 0x0080e0, 0x80e080, 0xe08080, 0x8080e0,
    0xe080e0, 0x80e0e0, 0xe0e080, 0xc040e0, 0x40e0c0, 0xe0c040, 0xc0e040,
    0x40c0e0, 0xe040c0, 0xc0e000, 0xe0c000, 0xe000c0, 0xc000e0, 0x00e0c0,
    0x00c0e0, 0xc0e0c0, 0xe0c0c0, 0xc0c0e0, 0xe0c0e0, 0xc0e0e0, 0xe0e0c0,
    0x40e000, 0xe04000, 0xe00040, 0x4000e0, 0x00e040, 0x0040e0, 0x40e040,
    0xe04040, 0x4040e0, 0xe040e0, 0x40e0e0, 0xe0e040, 0xe0e0e0, 0xe00000,
    0x00e000, 0x0000e0, 0xe0e000, 0xe000e0, 0x00e0e0, 0xff8060, 0x8060ff,
    0x60ff80, 0xff6080, 0x80ff60, 0x6080ff, 0xffc060, 0xc060ff, 0x60ffc0,
    0xff60c0, 0xc0ff60, 0x60c0ff, 0xff4060, 0x4060ff, 0x60ff40, 0xff6040,
    0x40ff60, 0x6040ff, 0xffe060, 0xe060ff, 0x60ffe0, 0xff60e0, 0xe0ff60,
    0x60e0ff, 0xff6000, 0x60ff00, 0x6000ff, 0xff0060, 0x0060ff, 0x00ff60,
    0xff60ff, 0x60ffff, 0xffff60, 0x60ff60, 0xff6060, 0x6060ff, 0x80c060,
    0xc06080, 0x6080c0, 0x8060c0, 0xc08060, 0x60c080, 0x804060, 0x406080,
    0x608040, 0x806040, 0x408060, 0x604080, 0x80e060, 0xe06080, 0x6080e0,
    0x8060e0, 0xe08060, 0x60e080, 0x806000, 0x608000, 0x600080, 0x800060,
    0x006080, 0x008060, 0x806080, 0x608080, 0x808060, 0x608060, 0x806060,
    0x606080, 0xc04060, 0x4060c0, 0x60c040, 0xc06040, 0x40c060, 0x6040c0,
    0xc0e060, 0xe060c0, 0x60c0e0, 0xc060e0, 0xe0c060, 0x60e0c0, 0xc06000,
    0x60c000, 0x6000c0, 0xc00060, 0x0060c0, 0x00c060, 0xc060c0, 0x60c0c0,
    0xc0c060, 0x60c060, 0xc06060, 0x6060c0, 0x40e060, 0xe06040, 0x6040e0,
    0x4060e0, 0xe04060, 0x60e040, 0x406000, 0x604000, 0x600040, 0x400060,
    0x006040, 0x004060, 0x406040, 0x604040, 0x404060, 0x604060, 0x406060,
    0x606040, 0xe06000, 0x60e000, 0x6000e0, 0xe00060, 0x0060e0, 0x00e060,
    0xe060e0, 0x60e0e0, 0xe0e060, 0x60e060, 0xe06060, 0x6060e0, 0x606060,
    0x600000, 0x006000, 0x000060, 0x606000, 0x600060, 0x006060, 0xff80a0,
    0x80a0ff, 0xa0ff80, 0xffa080, 0x80ffa0, 0xa080ff, 0xffc0a0, 0xc0a0ff,
    0xa0ffc0, 0xffa0c0, 0xc0ffa0, 0xa0c0ff, 0xff40a0, 0x40a0ff, 0xa0ff40,
    0xffa040, 0x40ffa0, 0xa040ff, 0xffe0a0, 0xe0a0ff, 0xa0ffe0, 0xffa0e0,
    0xe0ffa0, 0xa0e0ff, 0xff60a0, 0x60a0ff, 0xa0ff60, 0xffa060, 0x60ffa0,
    0xa060ff, 0xffa000, 0xa0ff00, 0xa000ff, 0xff00a0, 0x00a0ff, 0x00ffa0,
    0xffa0ff, 0xa0ffff, 0xffffa0, 0xa0ffa0, 0xffa0a0, 0xa0a0ff, 0x80c0a0,
    0xc0a080, 0xa080c0, 0x80a0c0, 0xc080a0, 0xa0c080, 0x8040a0, 0x40a080,
    0xa08040, 0x80a040, 0x4080a0, 0xa04080, 0x80e0a0, 0xe0a080, 0xa080e0,
    0x80a0e0, 0xe080a0, 0xa0e080, 0x8060a0, 0x60a080, 0xa08060, 0x80a060,
    0x6080a0, 0xa06080, 0x80a000, 0xa08000, 0xa00080, 0x8000a0, 0x00a080,
    0x0080a0, 0x80a080, 0xa08080, 0x8080a0, 0xa080a0, 0x80a0a0, 0xa0a080,
    0xc040a0, 0x40a0c0, 0xa0c040, 0xc0a040, 0x40c0a0, 0xa040c0, 0xc0e0a0,
    0xe0a0c0, 0xa0c0e0, 0xc0a0e0, 0xe0c0a0, 0xa0e0c0, 0xc060a0, 0x60a0c0,
    0xa0c060, 0xc0a060, 0x60c0a0, 0xa060c0, 0xc0a000, 0xa0c000, 0xa000c0,
    0xc000a0, 0x00a0c0, 0x00c0a0, 0xc0a0c0, 0xa0c0c0, 0xc0c0a0, 0xa0c0a0,
    0xc0a0a0, 0xa0a0c0, 0x40e0a0, 0xe0a040, 0xa040e0, 0x40a0e0, 0xe040a0,
    0xa0e040, 0x4060a0, 0x60a040, 0xa04060, 0x40a060, 0x6040a0, 0xa06040,
    0x40a000, 0xa04000, 0xa00040, 0x4000a0, 0x00a040, 0x0040a0, 0x40a040,
    0xa04040, 0x4040a0, 0xa040a0, 0x40a0a0, 0xa0a040, 0xe060a0, 0x60a0e0,
    0xa0e060, 0xe0a060, 0x60e0a0, 0xa060e0, 0xe0a000, 0xa0e000, 0xa000e0,
    0xe000a0, 0x00a0e0, 0x00e0a0, 0xe0a0e0, 0xa0e0e0, 0xe0e0a0, 0xa0e0a0,
    0xe0a0a0, 0xa0a0e0, 0x60a000, 0xa06000, 0xa00060, 0x6000a0, 0x00a060,
    0x0060a0, 0x60a060, 0xa06060, 0x6060a0, 0xa060a0, 0x60a0a0, 0xa0a060,
    0xa0a0a0, 0xa00000, 0x00a000, 0x0000a0, 0xa0a000, 0xa000a0, 0x00a0a0,
    0xff8020, 0x8020ff, 0x20ff80, 0xff2080, 0x80ff20, 0x2080ff, 0xffc020,
    0xc020ff, 0x20ffc0, 0xff20c0, 0xc0ff20, 0x20c0ff, 0xff4020, 0x4020ff,
    0x20ff40, 0xff2040, 0x40ff20, 0x2040ff, 0xffe020, 0xe020ff, 0x20ffe0,
    0xff20e0, 0xe0ff20, 0x20e0ff, 0xff6020, 0x6020ff, 0x20ff60, 0xff2060,
    0x60ff20, 0x2060ff, 0xffa020, 0xa020ff, 0x20ffa0, 0xff20a0, 0xa0ff20,
    0x20a0ff, 0xff2000, 0x20ff00, 0x2000ff, 0xff0020, 0x0020ff, 0x00ff20,
    0xff20ff, 0x20ffff, 0xffff20, 0x20ff20, 0xff2020, 0x2020ff, 0x80c020,
    0xc02080, 0x2080c0, 0x8020c0, 0xc08020, 0x20c080, 0x804020, 0x402080,
    0x208040, 0x802040, 0x408020, 0x204080, 0x80e020, 0xe02080, 0x2080e0,
    0x8020e0, 0xe08020, 0x20e080, 0x806020, 0x602080, 0x208060, 0x802060,
    0x608020, 0x206080, 0x80a020, 0xa02080, 0x2080a0, 0x8020a0, 0xa08020,
    0x20a080, 0x802000, 0x208000, 0x200080, 0x800020, 0x002080, 0x008020,
    0x802080, 0x208080, 0x808020, 0x208020, 0x802020, 0x202080, 0xc04020,
    0x4020c0, 0x20c040, 0xc02040, 0x40c020, 0x2040c0, 0xc0e020, 0xe020c0,
    0x20c0e0, 0xc020e0, 0xe0c020, 0x20e0c0, 0xc06020, 0x6020c0, 0x20c060,
    0xc02060, 0x60c020, 0x2060c0, 0xc0a020, 0xa020c0, 0x20c0a0, 0xc020a0,
    0xa0c020, 0x20a0c0, 0xc02000, 0x20c000, 0x2000c0, 0xc00020, 0x0020c0,
    0x00c020, 0xc020c0, 0x20c0c0, 0xc0c020, 0x20c020, 0xc02020, 0x2020c0,
    0x40e020, 0xe02040, 0x2040e0, 0x4020e0, 0xe04020, 0x20e040, 0x406020,
    0x602040, 0x204060, 0x402060, 0x604020, 0x206040, 0x40a020, 0xa02040,
    0x2040a0, 0x4020a0, 0xa04020, 0x20a040, 0x402000, 0x204000, 0x200040,
    0x400020, 0x002040, 0x004020, 0x402040, 0x204040, 0x404020, 0x204020,
    0x402020, 0x202040, 0xe06020, 0x6020e0, 0x20e060, 0xe02060, 0x60e020,
    0x2060e0, 0xe0a020, 0xa020e0, 0x20e0a0, 0xe020a0, 0xa0e020, 0x20a0e0,
    0xe02000, 0x20e000, 0x2000e0, 0xe00020, 0x0020e0, 0x00e020, 0xe020e0,
    0x20e0e0, 0xe0e020, 0x20e020, 0xe02020, 0x2020e0, 0x60a020, 0xa02060,
    0x2060a0, 0x6020a0, 0xa06020, 0x20a060, 0x602000, 0x206000, 0x200060,
    0x600020, 0x002060, 0x006020, 0x602060, 0x206060, 0x606020, 0x206020,
    0x602020, 0x202060, 0xa02000, 0x20a000, 0x2000a0, 0xa00020, 0x0020a0,
    0x00a020, 0xa020a0, 0x20a0a0, 0xa0a020, 0x20a020, 0xa02020, 0x2020a0,
    0x202020, 0x200000, 0x002000, 0x000020, 0x202000, 0x200020, 0x002020,
    0xff80f0, 0x80f0ff, 0xf0ff80, 0xfff080, 0x80fff0, 0xf080ff, 0xffc0f0,
    0xc0f0ff, 0xf0ffc0, 0xfff0c0, 0xc0fff0, 0xf0c0ff, 0xff40f0, 0x40f0ff,
    0xf0ff40, 0xfff040, 0x40fff0, 0xf040ff, 0xffe0f0, 0xe0f0ff, 0xf0ffe0,
    0xfff0e0, 0xe0fff0, 0xf0e0ff, 0xff60f0, 0x60f0ff, 0xf0ff60, 0xfff060,
    0x60fff0, 0xf060ff, 0xffa0f0, 0xa0f0ff, 0xf0ffa0, 0xfff0a0, 0xa0fff0,
    0xf0a0ff, 0xff20f0, 0x20f0ff, 0xf0ff20, 0xfff020, 0x20fff0, 0xf020ff,
    0xfff000, 0xf0ff00, 0xf000ff, 0xff00f0, 0x00f0ff, 0x00fff0, 0xfff0ff,
    0xf0ffff, 0xfffff0, 0xf0fff0, 0xfff0f0, 0xf0f0ff, 0x80c0f0, 0xc0f080,
    0xf080c0, 0x80f0c0, 0xc080f0, 0xf0c080, 0x8040f0, 0x40f080, 0xf08040,
    0x80f040, 0x4080f0, 0xf04080, 0x80e0f0, 0xe0f080, 0xf080e0, 0x80f0e0,
    0xe080f0, 0xf0e080, 0x8060f0, 0x60f080, 0xf08060, 0x80f060, 0x6080f0,
    0xf06080, 0x80a0f0, 0xa0f080, 0xf080a0, 0x80f0a0, 0xa080f0, 0xf0a080,
    0x8020f0, 0x20f080, 0xf08020, 0x80f020, 0x2080f0, 0xf02080, 0x80f000,
    0xf08000, 0xf00080, 0x8000f0, 0x00f080, 0x0080f0, 0x80f080, 0xf08080,
    0x8080f0, 0xf080f0, 0x80f0f0, 0xf0f080, 0xc040f0, 0x40f0c0, 0xf0c040,
    0xc0f040, 0x40c0f0, 0xf040c0, 0xc0e0f0, 0xe0f0c0, 0xf0c0e0, 0xc0f0e0,
    0xe0c0f0, 0xf0e0c0, 0xc060f0, 0x60f0c0, 0xf0c060, 0xc0f060, 0x60c0f0,
    0xf060c0, 0xc0a0f0, 0xa0f0c0, 0xf0c0a0, 0xc0f0a0, 0xa0c0f0, 0xf0a0c0,
    0xc020f0, 0x20f0c0, 0xf0c020, 0xc0f020, 0x20c0f0, 0xf020c0, 0xc0f000,
    0xf0c000, 0xf000c0, 0xc000f0, 0x00f0c0, 0x00c0f0, 0xc0f0c0, 0xf0c0c0,
    0xc0c0f0, 0xf0c0f0, 0xc0f0f0, 0xf0f0c0, 0x40e0f0, 0xe0f040, 0xf040e0,
    0x40f0e0, 0xe040f0, 0xf0e040, 0x4060f0, 0x60f040, 0xf04060, 0x40f060,
    0x6040f0, 0xf06040, 0x40a0f0, 0xa0f040, 0xf040a0, 0x40f0a0, 0xa040f0,
    0xf0a040, 0x4020f0, 0x20f040, 0xf04020, 0x40f020, 0x2040f0, 0xf02040,
    0x40f000, 0xf04000, 0xf00040, 0x4000f0, 0x00f040, 0x0040f0, 0x40f040,
    0xf04040, 0x4040f0, 0xf040f0, 0x40f0f0, 0xf0f040, 0xe060f0, 0x60f0e0,
    0xf0e060, 0xe0f060, 0x60e0f0, 0xf060e0, 0xe0a0f0, 0xa0f0e0, 0xf0e0a0,
    0xe0f0a0, 0xa0e0f0, 0xf0a0e0, 0xe020f0, 0x20f0e0, 0xf0e020, 0xe0f020,
    0x20e0f0, 0xf020e0, 0xe0f000, 0xf0e000, 0xf000e0, 0xe000f0, 0x00f0e0,
    0x00e0f0, 0xe0f0e0, 0xf0e0e0, 0xe0e0f0, 0xf0e0f0, 0xe0f0f0, 0xf0f0e0,
    0x60a0f0, 0xa0f060, 0xf060a0, 0x60f0a0, 0xa060f0, 0xf0a060, 0x6020f0,
    0x20f060, 0xf06020, 0x60f020, 0x2060f0, 0xf02060, 0x60f000, 0xf06000,
    0xf00060, 0x6000f0, 0x00f060, 0x0060f0, 0x60f060, 0xf06060, 0x6060f0,
    0xf060f0, 0x60f0f0, 0xf0f060, 0xa020f0, 0x20f0a0, 0xf0a020, 0xa0f020,
    0x20a0f0, 0xf020a0, 0xa0f000, 0xf0a000, 0xf000a0, 0xa000f0, 0x00f0a0,
    0x00a0f0, 0xa0f0a0, 0xf0a0a0, 0xa0a0f0, 0xf0a0f0, 0xa0f0f0, 0xf0f0a0,
    0x20f000, 0xf02000, 0xf00020, 0x2000f0, 0x00f020, 0x0020f0, 0x20f020,
    0xf02020, 0x2020f0, 0xf020f0, 0x20f0f0, 0xf0f020, 0xf0f0f0, 0xf00000,
    0x00f000, 0x0000f0, 0xf0f000, 0xf000f0, 0x00f0f0, 0xff80b0, 0x80b0ff,
    0xb0ff80, 0xffb080, 0x80ffb0, 0xb080ff, 0xffc0b0, 0xc0b0ff, 0xb0ffc0,
    0xffb0c0, 0xc0ffb0, 0xb0c0ff, 0xff40b0, 0x40b0ff, 0xb0ff40, 0xffb040,
    0x40ffb0, 0xb040ff, 0xffe0b0, 0xe0b0ff, 0xb0ffe0, 0xffb0e0, 0xe0ffb0,
    0xb0e0ff, 0xff60b0
};

static unsigned char const pickMapComponent[256] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50,
    0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50,
    0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
    0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
    0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70,
    0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
    0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
    0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
    0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0,
    0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0,
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
    0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
    0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
    0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
    0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
    0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static unsigned char const pickMapComponent444[256] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x00, 0x00, 0x60, 0x60, 0x60,
    0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
    0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0x60, 0x60,
    0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
    0x60, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00,
    0x00, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
    0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
    0x00, 0x00, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0,
    0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0,
    0xb0, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0xe0,
    0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
    0xe0, 0xe0, 0xe0, 0xe0, 0x00, 0x00, 0xf0, 0xf0,
    0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
    0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x00, 0x00, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// Get the value at "index" to use for color picking.  Returns a zero
// QRgb when the table has been exhausted.
QRgb qt_qgl_pick_color(int index)
{
    if (index >= 0 && index < 1024)
        return QRgb(pickColors[index] | 0xff000000);
    else
        return 0;
}

// Normalize a color that was picked out of a screen color buffer
// so that it is a better match for something that was generated
// by qt_qgl_pick_color().  Rounding discrepancies in the
// low bits due to floating-point conversions are factored out.
QRgb qt_qgl_normalize_pick_color(QRgb color, bool is444)
{
    int red, green, blue;
    if (!is444) {
        // RGB565, RGB555, and RGB888 screens (alpha is ignored).
        red = pickMapComponent[qRed(color)];
        green = pickMapComponent[qGreen(color)];
        blue = pickMapComponent[qBlue(color)];
    } else {
        // RGB444 screens need a little more care when normalizing.
        red = pickMapComponent444[qRed(color)];
        green = pickMapComponent444[qGreen(color)];
        blue = pickMapComponent444[qBlue(color)];
    }
    return qRgb(red, green, blue);
}

QT_END_NAMESPACE

#else // QGL_GENERATOR_PROGRAM

#include <stdio.h>

static unsigned char singlePatterns[] = {
    1, 1, 1,

    1, 0, 0,
    0, 1, 0,
    0, 0, 1,
    1, 1, 0,
    1, 0, 1,
    0, 1, 1
};
#define	NUM_SINGLE_PATTERNS 7

static unsigned char doublePatterns[] = {
    1, 2, 0,
    2, 1, 0,
    2, 0, 1,
    1, 0, 2,
    0, 2, 1,
    0, 1, 2,

    1, 2, 1,
    2, 1, 1,
    1, 1, 2,
    2, 1, 2,
    1, 2, 2,
    2, 2, 1
};
#define	NUM_DOUBLE_PATTERNS 12

static unsigned char triplePatterns[] = {
    1, 2, 3,
    2, 3, 1,
    3, 1, 2,
    1, 3, 2,
    2, 1, 3,
    3, 2, 1
};
#define	NUM_TRIPLE_PATTERNS 6

static unsigned char values[] = {
    0x00,
    0xff, 0x80, 0xc0, 0x40, 0xe0, 0x60, 0xa0, 0x20,
    0xf0, 0xb0, 0x50, 0xd0, 0x70, 0x30, 0x90, 0x10
};
#define	NUM_VALUES 16
#define	NUM_VALUES_444 10

#define	MAX_GENERATE 1024

static unsigned char used[17][17][17];
static int generated = 0;

static void genPattern(int red, int green, int blue)
{
    ++red;
    ++green;
    ++blue;
    if (used[red][green][blue] || generated >= MAX_GENERATE)
	return;
    used[red][green][blue] = 1;
    if ((generated % 7) == 0)
	printf("\n    ");
    printf("0x%02x%02x%02x", values[red], values[green], values[blue]);
    ++generated;
    if (generated < MAX_GENERATE && (generated % 7) != 0)
	printf(", ");
    else if (generated < MAX_GENERATE)
	printf(",");
}

static void genSinglePatterns(int value)
{
    int index, red, green, blue;
    for (index = 0; index < NUM_SINGLE_PATTERNS; ++index) {
	if (singlePatterns[index * 3] == 0)
	    red = -1;
	else
	    red = value;
	if (singlePatterns[index * 3 + 1] == 0)
	    green = -1;
	else
	    green = value;
	if (singlePatterns[index * 3 + 2] == 0)
	    blue = -1;
	else
	    blue = value;
	genPattern(red, green, blue);
    }
}

static void genDoublePatterns(int value1, int value2)
{
    int index, red, green, blue;
    for (index = 0; index < NUM_DOUBLE_PATTERNS; ++index) {
	if (doublePatterns[index * 3] == 0)
	    red = -1;
	else if (doublePatterns[index * 3] == 1)
	    red = value1;
	else
	    red = value2;
	if (doublePatterns[index * 3 + 1] == 0)
	    green = -1;
	else if (doublePatterns[index * 3 + 1] == 1)
	    green = value1;
	else
	    green = value2;
	if (doublePatterns[index * 3 + 2] == 0)
	    blue = -1;
	else if (doublePatterns[index * 3 + 2] == 1)
	    blue = value1;
	else
	    blue = value2;
	genPattern(red, green, blue);
    }
}

static void genTriplePatterns(int value1, int value2, int value3)
{
    int index, red, green, blue;
    for (index = 0; index < NUM_TRIPLE_PATTERNS; ++index) {
	if (triplePatterns[index * 3] == 0)
	    red = -1;
	else if (triplePatterns[index * 3] == 1)
	    red = value1;
	else if (triplePatterns[index * 3] == 2)
	    red = value2;
	else
	    red = value3;
	if (triplePatterns[index * 3 + 1] == 0)
	    green = -1;
	else if (triplePatterns[index * 3 + 1] == 1)
	    green = value1;
	else if (triplePatterns[index * 3 + 1] == 2)
	    green = value2;
	else
	    green = value3;
	if (triplePatterns[index * 3 + 2] == 0)
	    blue = -1;
	else if (triplePatterns[index * 3 + 2] == 1)
	    blue = value1;
	else if (triplePatterns[index * 3 + 2] == 2)
	    blue = value2;
	else
	    blue = value3;
	genPattern(red, green, blue);
    }
}

static void genPatternRange(int limit)
{
    // This will generate 4912 unique color values which are
    // reasonably well-spaced in the RGB color cube.
    int first, second, third;
    for (first = 0; first < limit; ++first) {
	genSinglePatterns(first);
	for (second = first + 1; second < limit; ++second) {
	    genDoublePatterns(first, second);
	    for (third = second + 1; third < limit; ++third) {
		genTriplePatterns(first, second, third);
	    }
	}
    }
}

static void generateComponentMap(void)
{
    int map[256];
    int index, value, index2;

    for (index = 0; index < 256; ++index)
	map[index] = 0;

    for (index = 0; index < NUM_VALUES; ++index) {
	value = values[index + 1];
	for (index2 = value - 8; index2 < (value + 8); ++index2) {
	    if (index2 >= 0 && index2 < 256)
		map[index2] = value;
	}
    }

    for (index = 0; index < 256; ++index) {
	if ((index % 8) == 0)
	    printf("    ");
	printf("0x%02x", map[index]);
	if (index < 255)
	    printf(",");
	if ((index % 8) == 7)
	    printf("\n");
	else if (index < 255)
	    printf(" ");
    }

    // Validate the reversibility of RGB565 and RGB555 mappings.
    for (index = 0; index < 17; ++index) {
	// Integer truncation test - 5-bit for red and blue (and green RGB555).
	value = values[index] * 31 / 255;
	index2 = value * 255 / 31;
	if (values[index] != map[index2]) {
	    fprintf(stderr, "RGB565 (i5) failure: 0x%02X -> 0x%02X -> 0x%02X\n",
		    values[index], index2, map[index2]);
	}

	// Integer truncation test - 6-bit for green.
	value = values[index] * 63 / 255;
	index2 = value * 255 / 63;
	if (values[index] != map[index2]) {
	    fprintf(stderr, "RGB565 (i6) failure: 0x%02X -> 0x%02X -> 0x%02X\n",
		    values[index], index2, map[index2]);
	}

	// Floating point rounding test - 5-bit for red and blue.
	value = (int)((values[index] * 31.0 / 255.0) + 0.5);
	index2 = (int)((value * 255.0 / 31.0) + 0.5);
	if (values[index] != map[index2]) {
	    fprintf(stderr, "RGB565 (f5) failure: 0x%02X -> 0x%02X -> 0x%02X\n",
		    values[index], index2, map[index2]);
	}

	// Floating point rounding test - 6-bit for green.
	value = (int)((values[index] * 63.0 / 255.0) + 0.5);
	index2 = (int)((value * 255.0 / 63.0) + 0.5);
	if (values[index] != map[index2]) {
	    fprintf(stderr, "RGB565 (f6) failure: 0x%02X -> 0x%02X -> 0x%02X\n",
		    values[index], index2, map[index2]);
	}

	// Test 5-bit to 8-bit conversion using doubling (ABCDE -> ABCDEABC).
	value = values[index] * 31 / 255;
	index2 = (value << 3) | (value >> 2);
	if (values[index] != map[index2]) {
	    fprintf(stderr, "RGB565 (di5) failure: 0x%02X -> 0x%02X -> 0x%02X\n",
		    values[index], index2, map[index2]);
	}
	value = (int)((values[index] * 31.0 / 255.0) + 0.5);
	index2 = (value << 3) | (value >> 2);
	if (values[index] != map[index2]) {
	    fprintf(stderr, "RGB565 (df5) failure: 0x%02X -> 0x%02X -> 0x%02X\n",
		    values[index], index2, map[index2]);
	}

	// Test 6-bit to 8-bit conversion using doubling (ABCDEF -> ABCDEFAB).
	value = values[index] * 63 / 255;
	index2 = (value << 2) | (value >> 4);
	if (values[index] != map[index2]) {
	    fprintf(stderr, "RGB565 (di6) failure: 0x%02X -> 0x%02X -> 0x%02X\n",
		    values[index], index2, map[index2]);
	}
	value = (int)((values[index] * 63.0 / 255.0) + 0.5);
	index2 = (value << 2) | (value >> 4);
	if (values[index] != map[index2]) {
	    fprintf(stderr, "RGB565 (df6) failure: 0x%02X -> 0x%02X -> 0x%02X\n",
		    values[index], index2, map[index2]);
	}
    }
}

static void generateComponentMap444(void)
{
    int map[256];
    int index, value, index2;

    for (index = 0; index < 256; ++index)
	map[index] = 0;

    // Populate mappings for integer conversion with truncation.
    for (index = 0; index < NUM_VALUES_444; ++index) {
	value = values[index + 1] * 15 / 255;
	value = value * 255 / 15;
	if (value > 255)
	    value = 255;
	else if (value < 0)
	    value = 0;
	for (index2 = value - 8; index2 < (value + 7); ++index2) {
	    if (index2 >= 0 && index2 < 256)
		map[index2] = values[index + 1];
	}
    }

    // Add some extra mappings for floating-point conversion with rounding.
    for (index = 0; index < NUM_VALUES_444; ++index) {
	value = (int)((values[index + 1] * 15.0 / 255.0) + 0.5);
	value = (int)((value * 255.0 / 15.0) + 0.5);
	if (value > 255)
	    value = 255;
	else if (value < 0)
	    value = 0;
	for (index2 = value - 8; index2 < (value + 7); ++index2) {
	    if (index2 >= 0 && index2 < 256 && map[index2] == 0)
		map[index2] = values[index + 1];
	}
    }

    for (index = 0; index < 256; ++index) {
	if ((index % 8) == 0)
	    printf("    ");
	printf("0x%02x", map[index]);
	if (index < 255)
	    printf(",");
	if ((index % 8) == 7)
	    printf("\n");
	else if (index < 255)
	    printf(" ");
    }

    // Validate the reversibility of RGB444 mappings.
    for (index = 0; index <= NUM_VALUES_444; ++index) {
	// Integer truncation test.
	value = values[index] * 15 / 255;
	index2 = value * 255 / 15;
	if (values[index] != map[index2]) {
	    fprintf(stderr, "RGB444 (i) failure: 0x%02X -> 0x%02X -> 0x%02X\n",
		    values[index], index2, map[index2]);
	}

	// Floating point rounding test.
	value = (int)((values[index] * 15.0 / 255.0) + 0.5);
	index2 = (int)((value * 255.0 / 15.0) + 0.5);
	if (values[index] != map[index2]) {
	    fprintf(stderr, "RGB444 (f) failure: 0x%02X -> 0x%02X -> 0x%02X\n",
		    values[index], index2, map[index2]);
	}

	// Test 4-bit to 8-bit conversion using doubling (ABCD -> ABCDABCD).
	value = values[index] * 15 / 255;
	index2 = value | (value << 4);
	if (values[index] != map[index2]) {
	    fprintf(stderr, "RGB444 (di) failure: 0x%02X -> 0x%02X -> 0x%02X\n",
		    values[index], index2, map[index2]);
	}
	value = (int)((values[index] * 15.0 / 255.0) + 0.5);
	index2 = value | (value << 4);
	if (values[index] != map[index2]) {
	    fprintf(stderr, "RGB444 (df) failure: 0x%02X -> 0x%02X -> 0x%02X\n",
		    values[index], index2, map[index2]);
	}
    }
}

int main(int argc, char *argv[])
{
    int limit;

    // Run the generator multiple times using more and more of
    // the elements in the "values" table, and limit to a maximum
    // of 1024 colors.
    //
    // This will sort the output so that colors that involve elements
    // early in the table will be generated first.  All combinations
    // of early elements are exhausted before moving onto later values.
    //
    // The result of this sorting should be to maximize the spacing
    // between colors that appear early in the generated output.
    // This should produce better results for color picking on
    // low-end devices with RGB565, RGB555, and RGB444 displays.
    printf("static int const pickColors[%d] = {", MAX_GENERATE);
    for (limit = 1; limit <= NUM_VALUES; ++limit)
	genPatternRange(limit);
    printf("\n};\n\n");

    // Generate a component mapping table for mapping 8-bit RGB
    // components to the nearest normalized value.
    printf("static unsigned char const pickMapComponent[256] = {\n");
    generateComponentMap();
    printf("};\n\n");

    // Generate a separate mapping table for RGB444, which needs a
    // little more care to deal with truncation errors.
    printf("static unsigned char const pickMapComponent444[256] = {\n");
    generateComponentMap444();
    printf("};\n\n");

    return 0;
}

#endif // QGL_GENERATOR_PROGRAM
