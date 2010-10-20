/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qglslsyntaxhighlighter.h"
#include <QObject>
#include <QTextDocument>
#include <QTextEdit>

QGLSLSyntaxHighlighter::QGLSLSyntaxHighlighter ( QObject * parent ) : QSyntaxHighlighter(parent)
{
    setupHighlightingRules();
}

QGLSLSyntaxHighlighter::QGLSLSyntaxHighlighter ( QTextDocument * parent ) : QSyntaxHighlighter(parent)
{
    setupHighlightingRules();
}

QGLSLSyntaxHighlighter::QGLSLSyntaxHighlighter ( QTextEdit * parent ) : QSyntaxHighlighter(parent)
{
    setupHighlightingRules();
}

QGLSLSyntaxHighlighter::~QGLSLSyntaxHighlighter ()
{
}

void QGLSLSyntaxHighlighter::setupHighlightingRules()
{
    HighlightingRule rule;

    //    variableFormat.setFontItalic(true);
    rule.pattern = QRegExp(QLatin1String("[_a-zA-Z][_a-zA-Z0-9]*"));
    rule.format = variableFormat;
    highlightingRules.append(rule);

    keywordFormat.setForeground(QColor(0x80,0x80,0x00));
    QStringList keywordPatterns;
    QStringList attributePatterns;
    // Keywords from GLSL ES Specification 1.0.17
    keywordPatterns << QLatin1String("\\battribute\\b")
            << QLatin1String("\\bconst\\b")
            << QLatin1String("\\buniform\\b")
            << QLatin1String("\\bvarying\\b")
            << QLatin1String("\\bbreak\\b")
            << QLatin1String("\\bcontinue\\b")
            << QLatin1String("\\bdo\\b")
            << QLatin1String("\\bfor\\b")
            << QLatin1String("\\bwhile\\b")
            << QLatin1String("\\bif\\b")
            << QLatin1String("\\belse\\b")
            << QLatin1String("\\bin\\b")
            << QLatin1String("\\bout\\b")
            << QLatin1String("\\binout\\b")
            << QLatin1String("\\bstruct\\b")
            << QLatin1String("\\btrue\\b")
            << QLatin1String("\\bfalse\\b")
            << QLatin1String("\\bdiscard\\b")
            << QLatin1String("\\breturn\\b")
            << QLatin1String("\\blowp\\b")
            << QLatin1String("\\bmediump\\b")
            << QLatin1String("\\bhighp\\b")
            << QLatin1String("\\bprecision\\b")
            << QLatin1String("\\binvariant\\b");


    // Reserved words from GLSL ES Specification 1.0.17
    keywordPatterns << QLatin1String("\\basm\\b")
            << QLatin1String("\bclass\\b")
            << QLatin1String("\\bunion\\b")
            << QLatin1String("\\benum\\b")
            << QLatin1String("\\btypedef\\b")
            << QLatin1String("\\btemplate\\b")
            << QLatin1String("\\bthis\\b")
            << QLatin1String("\\bpacked\\b")
            << QLatin1String("\\bgoto\\b")
            << QLatin1String("\\bswitch\\b")
            << QLatin1String("\\bdefault\\b")
            << QLatin1String("\\binline\\b")
            << QLatin1String("\\bnoinline\\b")
            << QLatin1String("\\bvolatile\\b")
            << QLatin1String("\\bpublic\\b")
            << QLatin1String("\\bstatic\\b")
            << QLatin1String("\\bextern\\b")
            << QLatin1String("\\bexternal\\b")
            << QLatin1String("\\binterface\\b")
            << QLatin1String("\\bflat\\b")
            << QLatin1String("\\blong\\b")
            << QLatin1String("\\bshort\\b")
            << QLatin1String("\\bdouble\\b")
            << QLatin1String("\\bhalf\\b")
            << QLatin1String("\\bfixed\\b")
            << QLatin1String("\\bunsigned\\b")
            << QLatin1String("\\bsuperp\\b")
            << QLatin1String("\\binput\\b")
            << QLatin1String("\\boutput\\b")
            << QLatin1String("\\bhvec2\\b")
            << QLatin1String("\\bhvec3\\b")
            << QLatin1String("\\bhvec4\\b")
            << QLatin1String("\\bdvec2\\b")
            << QLatin1String("\\bdvec3\\b")
            << QLatin1String("\\bdvec4\\b")
            << QLatin1String("\\bfvec2\\b")
            << QLatin1String("\\bfvec3\\b")
            << QLatin1String("\\bfvec4\\b")
            << QLatin1String("\\bsampler1D\\b")
            << QLatin1String("\\bsampler3D\\b")
            << QLatin1String("\\bsampler1DShadow\\b")
            << QLatin1String("\\bsampler2DShadow\\b")
            << QLatin1String("\\bsampler2DRect\\b")
            << QLatin1String("\\bsampler3DRect\\b")
            << QLatin1String("\\bsampler2DRectShadow\\b")
            << QLatin1String("\\bsizeof\\b")
            << QLatin1String("\\bcast\\b")
            << QLatin1String("\\bnamespace\\b")
            << QLatin1String("\\busing\\b");

    attributePatterns
            << QLatin1String("\\bfloat\\b")
            << QLatin1String("\\bint\\b")
            << QLatin1String("\\bvoid\\b")
            << QLatin1String("\\bbool\\b")
            << QLatin1String("\\bmat2\\b")
            << QLatin1String("\\bmat3\\b")
            << QLatin1String("\\bmat4\\b")
            << QLatin1String("\\bvec2\\b")
            << QLatin1String("\\bvec3\\b")
            << QLatin1String("\\bvec4\\b")
            << QLatin1String("\\bivec2\\b")
            << QLatin1String("\\bivec3\\b")
            << QLatin1String("\\bivec4\\b")
            << QLatin1String("\\bbvec2\\b")
            << QLatin1String("\\bbvec3\\b")
            << QLatin1String("\\bbvec4\\b")
            << QLatin1String("\\bsampler2D\\b")
            << QLatin1String("\\bsamplerCube\\b");

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    glvariableFormat.setFontWeight(QFont::Bold);
    glvariableFormat.setForeground(QColor(0x80,0x00,0x80));
    rule.pattern = QRegExp(QLatin1String("gl_[_a-zA-Z0-9]*"));
    rule.format = glvariableFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);

    attributeFormat.setForeground(Qt::blue);
    foreach (const QString &pattern, attributePatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = attributeFormat;
        highlightingRules.append(rule);
    }


    functionFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp(QLatin1String("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp(QLatin1String("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp(QLatin1String("/\\*"));
    commentEndExpression = QRegExp(QLatin1String("\\*/"));

}

void QGLSLSyntaxHighlighter::highlightBlock ( const QString & text )
{

    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }

}
