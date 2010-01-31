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
    rule.pattern = QRegExp("[_a-zA-Z][_a-zA-Z0-9]*");
    rule.format = variableFormat;
    highlightingRules.append(rule);

    keywordFormat.setForeground(QColor(0x80,0x80,0x00));
    QStringList keywordPatterns;
    QStringList attributePatterns;
    // Keywords from GLSL ES Specification 1.0.17
    keywordPatterns << "\\battribute\\b"  << "\\bconst\\b"  << "\\buniform\\b"
            << "\\bvarying\\b" << "\\bbreak\\b" << "\\bcontinue\\b"
            << "\\bdo\\b" << "\\bfor\\b" << "\\bwhile\\b" << "\\bif\\b"
            << "\\belse\\b" << "\\bin\\b" << "\\bout\\b" << "\\binout\\b"
            << "\\bstruct\\b" << "\\btrue\\b" << "\\bfalse\\b"
            << "\\bdiscard\\b" << "\\breturn\\b" << "\\blowp\\b"
            << "\\bmediump\\b" << "\\bhighp\\b" << "\\bprecision\\b"
            << "\\binvariant\\b";


    // Reserved words from GLSL ES Specification 1.0.17
    keywordPatterns << "\\basm\\b" << "\bclass\\b" << "\\bunion\\b"
            << "\\benum\\b" << "\\btypedef\\b" << "\\btemplate\\b"
            << "\\bthis\\b" << "\\bpacked\\b" << "\\bgoto\\b"
            << "\\bswitch\\b" << "\\bdefault\\b" << "\\binline\\b"
            << "\\bnoinline\\b" << "\\bvolatile\\b" << "\\bpublic\\b"
            << "\\bstatic\\b" << "\\bextern\\b" << "\\bexternal\\b"
            << "\\binterface\\b" << "\\bflat\\b" << "\\blong\\b"
            << "\\bshort\\b" << "\\bdouble\\b" << "\\bhalf\\b" << "\\bfixed\\b"
            << "\\bunsigned\\b" << "\\bsuperp\\b" << "\\binput\\b"
            << "\\boutput\\b" << "\\bhvec2\\b" << "\\bhvec3\\b"
            << "\\bhvec4\\b" << "\\bdvec2\\b" << "\\bdvec3\\b" << "\\bdvec4\\b"
            << "\\bfvec2\\b" << "\\bfvec3\\b" << "\\bfvec4\\b"
            << "\\bsampler1D\\b" << "\\bsampler3D\\b"
            << "\\bsampler1DShadow\\b" << "\\bsampler2DShadow\\b"
            << "\\bsampler2DRect\\b" << "\\bsampler3DRect\\b"
            << "\\bsampler2DRectShadow\\b" << "\\bsizeof\\b" << "\\bcast\\b"
            << "\\bnamespace\\b" << "\\busing\\b";

    attributePatterns
            << "\\bfloat\\b"  << "\\bint\\b"  << "\\bvoid\\b"  << "\\bbool\\b"
            << "\\bmat2\\b"  << "\\bmat3\\b"  << "\\bmat4\\b"
            << "\\bvec2\\b"  << "\\bvec3\\b"  << "\\bvec4\\b"  << "\\bivec2\\b"  << "\\bivec3\\b"  << "\\bivec4\\b"  << "\\bbvec2\\b"  << "\\bbvec3\\b"  << "\\bbvec4\\b"
            << "\\bsampler2D\\b"  << "\\bsamplerCube\\b";

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    glvariableFormat.setFontWeight(QFont::Bold);
    glvariableFormat.setForeground(QColor(0x80,0x00,0x80));
    rule.pattern = QRegExp("gl_[_a-zA-Z0-9]*");
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
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");

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
