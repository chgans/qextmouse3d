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

#include <QtTest/QtTest>
#include <QImage>

int qt_next_power_of_two(int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    ++v;
    return v;
}

static uint PREMUL(uint x) {
    uint a = x >> 24;
    uint t = (x & 0xff00ff) * a;
    t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
    t &= 0xff00ff;

    x = ((x >> 8) & 0xff) * a;
    x = (x + ((x >> 8) & 0xff) + 0x80);
    x &= 0xff00;
    x |= t | (a << 24);
    return x;
}

#define INV_PREMUL(p)                                   \
    (qAlpha(p) == 0 ? 0 :                               \
    ((qAlpha(p) << 24)                                  \
     | (((255*qRed(p))/ qAlpha(p)) << 16)               \
     | (((255*qGreen(p)) / qAlpha(p))  << 8)            \
     | ((255*qBlue(p)) / qAlpha(p))))

enum QGLImageFormat
{
    QGLI_Source_ARGB32          = 0x0001,
    QGLI_Source_ARGB32_Premul   = 0x0002,
    QGLI_Source_RGB16           = 0x0003,
    QGLI_Source_RGB888          = 0x0004,
    QGLI_Source_Format          = 0x000F,
    QGLI_Dest_RGBA8             = 0x0100,
    QGLI_Dest_BGRA8             = 0x0200,
    QGLI_Dest_RGB565            = 0x0300,
    QGLI_Dest_RGB8              = 0x0400,
    QGLI_Dest_Format            = 0x0F00,
    QGLI_Premul                 = 0x2000,
    QGLI_Scale                  = 0x4000,
    QGLI_Flip                   = 0x8000,

    QGLI_RGBA8                  = QGLI_Source_ARGB32 |
                                  QGLI_Dest_RGBA8,
    QGLI_RGBA8_Premul           = QGLI_Source_ARGB32_Premul |
                                  QGLI_Dest_RGBA8 |
                                  QGLI_Premul,

    QGLI_BGRA8                  = QGLI_Source_ARGB32 |
                                  QGLI_Dest_BGRA8,
    QGLI_BGRA8_Premul           = QGLI_Source_ARGB32_Premul |
                                  QGLI_Dest_BGRA8 |
                                  QGLI_Premul,

    QGLI_RGB565                 = QGLI_Source_RGB16 |
                                  QGLI_Dest_RGB565,

    QGLI_RGB8                   = QGLI_Source_RGB888 |
                                  QGLI_Dest_RGB8
};

static QImage qt_gl_convert_image_to_gl
    (const QImage& image, QGLImageFormat format)
{
    QImage img(image);

    // Scale the image to the next power of two in size.
    if ((format & QGLI_Scale) != 0) {
        int tx_w = qt_next_power_of_two(img.width());
        int tx_h = qt_next_power_of_two(img.height());
        img = img.scaled(tx_w, tx_h);
    }

    // Convert to the best source format for conversion.
    switch (format & QGLI_Source_Format) {
    case QGLI_Source_ARGB32:
        if (img.format() != QImage::Format_ARGB32)
            img = img.convertToFormat(QImage::Format_ARGB32);
        break;

    case QGLI_Source_ARGB32_Premul:
        if (img.format() != QImage::Format_ARGB32_Premultiplied)
            img = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        break;

    case QGLI_Source_RGB16:
        if (img.format() != QImage::Format_RGB16)
            img = img.convertToFormat(QImage::Format_RGB16);
        break;

    case QGLI_Source_RGB888:
        if (img.format() != QImage::Format_RGB888)
            img = img.convertToFormat(QImage::Format_RGB888);
        break;

    default: break;
    }

    // Convert the image into its destination pixel format.
    int width = img.width();
    int height = img.height();
    switch (format & QGLI_Dest_Format) {
    case QGLI_Dest_RGBA8:
        if (QSysInfo::ByteOrder == QSysInfo::LittleEndian) {
            for (int i=0; i < height; ++i) {
                uint *p = (uint *) img.scanLine(i);
                for (int x=0; x<width; ++x)
                    p[x] = ((p[x] << 16) & 0xff0000) | ((p[x] >> 16) & 0xff) | (p[x] & 0xff00ff00);
            }
        } else {
            for (int i=0; i < height; ++i) {
                uint *p = (uint *) img.scanLine(i);
                for (int x=0; x<width; ++x)
                    p[x] = (p[x] << 8) | ((p[x] >> 24) & 0xff);
            }
        }
        break;

    case QGLI_Dest_BGRA8:
        if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
            for (int i=0; i < height; ++i) {
                uint *p = (uint *) img.scanLine(i);
                for (int x=0; x<width; ++x)
                    p[x] = qbswap(p[x]);
            }
        }
        break;

    case QGLI_Dest_RGB565: break;
    case QGLI_Dest_RGB8: break;
    }

    // Flip the image upside down if necessary.
    if ((format & QGLI_Flip) != 0) {
        int ipl = img.bytesPerLine() / sizeof(int);
        int h = img.height();
        for (int y=0; y<h/2; ++y) {
            int *a = (int *) img.scanLine(y);
            int *b = (int *) img.scanLine(h - y - 1);
            for (int x=0; x<ipl; ++x)
                qSwap(a[x], b[x]);
        }
    }

    return img;
}

class tst_BindTexture : public QObject
{
    Q_OBJECT
public:
    tst_BindTexture();
    ~tst_BindTexture() {}

private slots:
    void convertToTexture_data();
    void convertToTexture();

private:
    QImage image1;
    QImage image2;
};

tst_BindTexture::tst_BindTexture()
{
    image1 = QImage(QLatin1String(":qtlogo.png"));
    image2 = QImage(QLatin1String(":deform-demo.png"));
}

void tst_BindTexture::convertToTexture_data()
{
    QTest::addColumn<int>("qimageFormat");
    QTest::addColumn<int>("glimageFormat");
    QTest::addColumn<QImage>("image");

    struct FormatPair
    {
        const char *name;
        int qimageFormat;
        int glimageFormat;
    };
    static FormatPair const pairs[] = {
        {"ARGB32 -> RGBA8", QImage::Format_ARGB32, QGLI_RGBA8},
        {"ARGB32 -> RGBA8_Premul", QImage::Format_ARGB32, QGLI_RGBA8_Premul},
        {"ARGB32 -> BGRA8", QImage::Format_ARGB32, QGLI_BGRA8},
        {"ARGB32 -> BGRA8_Premul", QImage::Format_ARGB32, QGLI_BGRA8_Premul},
        {"ARGB32 -> RGB565", QImage::Format_ARGB32, QGLI_RGB565},
        {"ARGB32 -> RGB8", QImage::Format_ARGB32, QGLI_RGB8},

        {"ARGB32_Premul -> RGBA8", QImage::Format_ARGB32_Premultiplied, QGLI_RGBA8},
        {"ARGB32_Premul -> RGBA8_Premul", QImage::Format_ARGB32_Premultiplied, QGLI_RGBA8_Premul},
        {"ARGB32_Premul -> BGRA8", QImage::Format_ARGB32_Premultiplied, QGLI_BGRA8},
        {"ARGB32_Premul -> BGRA8_Premul", QImage::Format_ARGB32_Premultiplied, QGLI_BGRA8_Premul},
        {"ARGB32_Premul -> RGB565", QImage::Format_ARGB32_Premultiplied, QGLI_RGB565},
        {"ARGB32_Premul -> RGB8", QImage::Format_ARGB32_Premultiplied, QGLI_RGB8},

        {"RGB16 -> RGBA8", QImage::Format_RGB16, QGLI_RGBA8},
        {"RGB16 -> RGBA8_Premul", QImage::Format_RGB16, QGLI_RGBA8_Premul},
        {"RGB16 -> BGRA8", QImage::Format_RGB16, QGLI_BGRA8},
        {"RGB16 -> BGRA8_Premul", QImage::Format_RGB16, QGLI_BGRA8_Premul},
        {"RGB16 -> RGB565", QImage::Format_RGB16, QGLI_RGB565},
        {"RGB16 -> RGB8", QImage::Format_RGB16, QGLI_RGB8},

        {"Indexed8 -> RGBA8", QImage::Format_Indexed8, QGLI_RGBA8},
        {"Indexed8 -> RGBA8_Premul", QImage::Format_Indexed8, QGLI_RGBA8_Premul},
        {"Indexed8 -> BGRA8", QImage::Format_Indexed8, QGLI_BGRA8},
        {"Indexed8 -> BGRA8_Premul", QImage::Format_Indexed8, QGLI_BGRA8_Premul},
        {"Indexed8 -> RGB565", QImage::Format_Indexed8, QGLI_RGB565},
        {"Indexed8 -> RGB8", QImage::Format_Indexed8, QGLI_RGB8},

        {"RGB32 -> RGBA8", QImage::Format_RGB32, QGLI_RGBA8},
        {"RGB32 -> RGBA8_Premul", QImage::Format_RGB32, QGLI_RGBA8_Premul},
        {"RGB32 -> BGRA8", QImage::Format_RGB32, QGLI_BGRA8},
        {"RGB32 -> BGRA8_Premul", QImage::Format_RGB32, QGLI_BGRA8_Premul},
        {"RGB32 -> RGB565", QImage::Format_RGB32, QGLI_RGB565},
        {"RGB32 -> RGB8", QImage::Format_RGB32, QGLI_RGB8},

        {"RGB888 -> RGBA8", QImage::Format_RGB888, QGLI_RGBA8},
        {"RGB888 -> RGBA8_Premul", QImage::Format_RGB888, QGLI_RGBA8_Premul},
        {"RGB888 -> BGRA8", QImage::Format_RGB888, QGLI_BGRA8},
        {"RGB888 -> BGRA8_Premul", QImage::Format_RGB888, QGLI_BGRA8_Premul},
        {"RGB888 -> RGB565", QImage::Format_RGB888, QGLI_RGB565},
        {"RGB888 -> RGB8", QImage::Format_RGB888, QGLI_RGB8},

        {"Mono -> RGBA8", QImage::Format_Mono, QGLI_RGBA8},
        {"Mono -> RGBA8_Premul", QImage::Format_Mono, QGLI_RGBA8_Premul},
        {"Mono -> BGRA8", QImage::Format_Mono, QGLI_BGRA8},
        {"Mono -> BGRA8_Premul", QImage::Format_Mono, QGLI_BGRA8_Premul},
        {"Mono -> RGB565", QImage::Format_Mono, QGLI_RGB565},
        {"Mono -> RGB8", QImage::Format_Mono, QGLI_RGB8},

        {0, -1, -1}
    };

    const FormatPair *p = pairs;
    while (p->name) {
        QByteArray name1("IMG1: ");
        QByteArray name2("IMG2: ");
        name1 += p->name;
        name2 += p->name;

        QTest::newRow(name1.constData())
            << p->qimageFormat
            << p->glimageFormat
            << image1;

        QTest::newRow(name2.constData())
            << p->qimageFormat
            << p->glimageFormat
            << image2;

        QTest::newRow((name1 + " [Flip]").constData())
            << p->qimageFormat
            << (p->glimageFormat | QGLI_Flip)
            << image1;

        QTest::newRow((name2 + " [Flip]").constData())
            << p->qimageFormat
            << (p->glimageFormat | QGLI_Flip)
            << image2;

        QTest::newRow((name1 + " [Scale]").constData())
            << p->qimageFormat
            << (p->glimageFormat | QGLI_Scale)
            << image1;

        QTest::newRow((name2 + " [Scale]").constData())
            << p->qimageFormat
            << (p->glimageFormat | QGLI_Scale)
            << image2;

        QTest::newRow((name1 + " [Flip, Scale]").constData())
            << p->qimageFormat
            << (p->glimageFormat | QGLI_Flip | QGLI_Scale)
            << image1;

        QTest::newRow((name2 + " [Flip, Scale]").constData())
            << p->qimageFormat
            << (p->glimageFormat | QGLI_Flip | QGLI_Scale)
            << image2;

        ++p;
    }
}

static const uchar *constBits(const QImage& img)
{
    return img.bits();
}

void tst_BindTexture::convertToTexture()
{
    QFETCH(int, qimageFormat);
    QFETCH(int, glimageFormat);
    QFETCH(QImage, image);

    // Print the name of each test case as we execute them.
    qWarning() << "";

    QImage img = image.convertToFormat(QImage::Format(qimageFormat));
    QImage result = qt_gl_convert_image_to_gl(img, QGLImageFormat(glimageFormat));

    // Verify that the correct output image was created.
    if ((glimageFormat & QGLI_Scale) != 0) {
        int tx_w = qt_next_power_of_two(img.width());
        int tx_h = qt_next_power_of_two(img.height());
        img = img.scaled(tx_w, tx_h);
    }
    const uchar *resbits = constBits(result);
    int resy;
    for (int y = 0; y < img.height(); ++y) {
        if ((glimageFormat & QGLI_Flip) != 0)
            resy = img.height() - y - 1;
        else
            resy = y;
        for (int x = 0; x < img.width(); ++x) {
            QRgb imgpix = img.pixel(x, y);
            QRgb respix = 0;
            int offset;
            switch (glimageFormat & QGLI_Dest_Format) {
            case QGLI_Dest_RGBA8:
                offset = (resy * img.width() + x) * 4;
                respix = qRgba(resbits[offset + 0], resbits[offset + 1],
                               resbits[offset + 2], resbits[offset + 3]);
                break;
            case QGLI_Dest_BGRA8:
                offset = (resy * img.width() + x) * 4;
                respix = qRgba(resbits[offset + 2], resbits[offset + 1],
                               resbits[offset + 0], resbits[offset + 3]);
                break;
            case QGLI_Dest_RGB565:
                respix = result.pixel(x, resy);
                break;
            case QGLI_Dest_RGB8:
                offset = resy * result.bytesPerLine() + x * 3;
                respix = qRgb(resbits[offset + 0], resbits[offset + 1],
                              resbits[offset + 2]);
                break;
            }
            if ((glimageFormat & QGLI_Premul) != 0 &&
                    img.format() != QImage::Format_ARGB32_Premultiplied)
                imgpix = PREMUL(imgpix);
            else if ((glimageFormat & QGLI_Premul) == 0 &&
                    img.format() == QImage::Format_ARGB32_Premultiplied)
                imgpix = INV_PREMUL(imgpix);
            if ((glimageFormat & QGLI_Dest_Format) == QGLI_Dest_RGB565) {
                respix &= 0x00F8FCF8;
                imgpix &= 0x00F8FCF8;
            } else if ((glimageFormat & QGLI_Dest_Format) == QGLI_Dest_RGB8) {
                respix &= 0x00FFFFFF;
                imgpix &= 0x00FFFFFF;
            }
            if (imgpix != respix) {
                qWarning("pixel mismatch at (%d, %d): 0x%lx -> 0x%lx",
                         x, y, long(imgpix), long(respix));
                QVERIFY(imgpix == respix);
            }
        }
    }
}

QTEST_APPLESS_MAIN(tst_BindTexture)

#include "tst_bindtexture.moc"
