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

#include "qgraphicslookattransform.h"
#include "qgraphicsrotation3d.h"
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGraphicsLookAtTransform
    \brief The QGraphicsLookAtTransform class implements a transformation
    that causes an Item3D to orient to face another Item3D.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::graphicsview

    Sometimes it can be useful to make an object face towards another object,
    wherever it might be located.  This is useful for objects like cameras,
    lights, arrows, faces etc.

    Another common use is called "billboarding", where a quad is always
    oriented to face the camera.
*/

/*!
    \qmlclass LookAtTransform QGraphicsLookAtTransform
    \brief The LookAtTransform item implements a transformation that causes objects to face the camera.
    \since 4.8
    \ingroup qt3d::qml3d

    Sometimes it can be useful to make an object face towards another object,
    wherever it might be located.  This is useful for objects like cameras,
    lights, arrows, faces etc.

    Another common use is called "billboarding", where a quad is always
    oriented to face the camera.  In QML, this can be achieved as follows:

    \code
    Camera3D camera
    Item3D {
        mesh: Mesh { source: "pane.obj" }
        position: Qt.vector3d(2, 0, -20)
        transform: LookAtTransform { worldPosition - camera.worldPosition }
        effect: Effect { texture: "picture.jpg" }
    }
    \endcode

    Because the lookAt transformation will override many other transformations
    ont the matrix, it will usually be the last element in the \c transform
    list (transformations are applied to the matrix in reverse order of their
    appearance in \c transform):

    \code
    Item3D {
        mesh: Mesh { source: "pane.obj" }
        position: Qt.vector3d(2, 0, -20)
        transform: [
            Scale3D { scale: 0.5 },
            Rotation3D { angle: 30 },
            LookAtTransform { worldPosition - camera.worldPosition }
        ]
        effect: Effect { texture: "picture.jpg" }
    }
    \endcode

    Typically, orientation as well as facing is important.  For example, a face
    that does not remain basically upright will look very odd indeed as it
    tracks a subject.  This control is achieved using the upVector property.
    The LookAt transform will first rotate around this axis to look towards a
    subject, and then rotate around an axis orthogonal to the upVector and the
    subject vector.  By default, the upVector is the objects local up
    Qt.vector3d(0, 1, 0).

    \code
    Item3D {
        mesh: Mesh { source: "pane.obj" }
        position: Qt.vector3d(2, 0, -20)
        transform: [
            Scale3D { scale: 0.5 },
            Rotation3D { angle: 30 },
            LookAtTransform { upVector: Qt.vector3d(0, 0, 1) }
        ]
        effect: Effect { texture: "picture.jpg" }
    }
    \endcode


    By default the lookAt transform will cause the object to
    face directly at the subject no matter how the world co-ordinate
    system is rotated.  Sometimes it is useful to limit the lookAt to only
    one axis of rotation - for example, a tank with a turret and barrel that
    each have only one degree of freedom.
    This is achieved by using the preserveUpVector property:

    \code
    Pane {
        position: Qt.vector3d(2, 0, -20)
        transform: LookAtTransform { preserveUpVector: true }
        effect: Effect { texture: "picture.jpg" }
    }
    \endcode

    \sa {Forest Example}
*/

/*!
    \internal
*/
static QMatrix4x4* cheatingSphericalBillboard(QMatrix4x4 *matrix)
{
    // Replace the top-left 3x3 of the matrix with the identity.
    // The technique is "Cheating Spherical Billboards", described here:
    // http://www.lighthouse3d.com/opengl/billboarding/index.php?billCheat
    (*matrix)(0, 0) = 1.0f;
    (*matrix)(0, 1) = 0.0f;
    (*matrix)(0, 2) = 0.0f;
    (*matrix)(1, 0) = 0.0f;
    (*matrix)(1, 1) = 1.0f;
    (*matrix)(1, 2) = 0.0f;
    (*matrix)(2, 0) = 0.0f;
    (*matrix)(2, 1) = 0.0f;
    (*matrix)(2, 2) = 1.0f;

    return matrix;
};

/*!
    \internal
*/
static QMatrix4x4* cheatingCylindricalBillboard(QMatrix4x4 *matrix)
{
    // Replace some of the top-left 3x3 of the matrix with the identity,
    // but leave the up vector component in the second column as-is.
    // The technique is "Cheating Cylindrical Billboards", described here:
    // http://www.lighthouse3d.com/opengl/billboarding/index.php?billCheat1
    (*matrix)(0, 0) = 1.0f;
    (*matrix)(0, 2) = 0.0f;
    (*matrix)(1, 0) = 0.0f;
    (*matrix)(1, 2) = 0.0f;
    (*matrix)(2, 0) = 0.0f;
    (*matrix)(2, 2) = 1.0f;

    return matrix;
};

#define RADS_TO_DEGREES (180.0 / 3.14159265358979323846264338327950288419717)

class QGraphicsLookAtTransformPrivate
{
public:
    QGraphicsLookAtTransformPrivate() :
        preserveUpVector(false), subject(0, 0, 1), upVector(0, 1, 0) {}

    void calculateRotationValues()
    {
        QVector3D forwards(0, 0, 1);
        QVector3D primarySubjectProjection = subject;
        // For a cylindrical lookat, we want no Y movement in the local frame:
        primarySubjectProjection.setY(0);
        primarySubjectProjection.normalize();

        primaryRotation.setAxis(QVector3D::crossProduct(forwards, primarySubjectProjection));
        secondaryRotation.setAxis(QVector3D::crossProduct(forwards, upVector));

        qreal angleCosine =
                QVector3D::dotProduct(forwards, primarySubjectProjection);
        qreal angle = qAcos(angleCosine);
        primaryRotation.setAngle(angle * RADS_TO_DEGREES );


        QVector3D secondarySubjectProjection = subject;
        // For the secondary rotation, we project into the x=0 plane:
        secondarySubjectProjection.setX(0);
        secondarySubjectProjection.normalize();

        qreal secondaryAngleCosine =
                QVector3D::dotProduct(forwards, secondarySubjectProjection);
        qreal secondaryAngle = qAcos(secondaryAngleCosine);
        secondaryRotation.setAngle(secondaryAngle * RADS_TO_DEGREES );
    }

    bool preserveUpVector;
    QVector3D subject;
    QVector3D upVector;

    QGraphicsRotation3D primaryRotation;
    QGraphicsRotation3D secondaryRotation;
};

/*!
    Construct a lookAt transform and attach it to \a parent.
*/
QGraphicsLookAtTransform::QGraphicsLookAtTransform(QObject *parent)
    : QGraphicsTransform3D(parent), d_ptr(new QGraphicsLookAtTransformPrivate)
{
}

/*!
    Destroy this lookAt transform.
*/
QGraphicsLookAtTransform::~QGraphicsLookAtTransform()
{
}

/*!
    \property QGraphicsLookAtTransform::preserveUpVector
    \brief true to preserve the up orientation.

    This property indicates whether the transform should rotate around a second
    axis to face directly at the subject (when the subject is the camera, this
    is known as "spherical billboarding").

    If the value for this property is true, then the object will have
    its upVector's orientation preserved, rotating around only this axis and
    not directly facing the subject unless it happens to fall on the correct
    plane (when the subject is the camera, this is known as known as
    "cylindrical billboarding").

    The default value for this property is false.
*/

/*!
    \qmlproperty bool LookatTransform::preserveUpVector

    This property indicates whether the transform should rotate around a second
    axis to face directly at the subject (known as a "spherical look-at").

    If the value for this property is true, then the object will have
    its upVector's orientation preserved, rotating around only this axis and
    not directly facing the subject unless it happens to fall on the correct
    plane (known as a "cylindrical look-at").

    The default value for this property is false.
*/

bool QGraphicsLookAtTransform::preserveUpVector() const
{
    Q_D(const QGraphicsLookAtTransform);
    return d->preserveUpVector;
}

void QGraphicsLookAtTransform::setPreserveUpVector(bool value)
{
    Q_D(QGraphicsLookAtTransform);
    if (d->preserveUpVector != value) {
        d->preserveUpVector = value;
        d->calculateRotationValues();
        emit preserveUpVectorChanged();
        emit transformChanged();
    }
}

/*!
    \property QGraphicsLookAtTransform::subject
    \brief The local-relative coordinates that are being looked at

    This property indicates what this transform is trying to look at.  After
    applying the transformation,
    Forwards is always considered to be in the direction of the positive Z axis
    in local space

ther the transform should rotate around a second
    axis to face directly at the subject (when the subject is the camera, this
    is known as "spherical billboarding").

    If the value for this property is true, then the object will have
    its upVector's orientation preserved, rotating around only this axis and
    not directly facing the subject unless it happens to fall on the correct
    plane (when the subject is the camera, this is known as known as
    "cylindrical billboarding").

    The default value for this property is false.
*/

/*!
    \qmlproperty bool LookatTransform::preserveUpVector

    This property indicates whether the transform should rotate around a second
    axis to face directly at the subject (known as a "spherical look-at").

    If the value for this property is true, then the object will have
    its upVector's orientation preserved, rotating around only this axis and
    not directly facing the subject unless it happens to fall on the correct
    plane (known as a "cylindrical look-at").

    The default value for this property is false.
*/

/*!
    \fn void QGraphicsLookAtTransform::subjectChanged()

    Signal that is emitted when subject() changes.
*/


QVector3D QGraphicsLookAtTransform::subject() const
{
    Q_D(const QGraphicsLookAtTransform);
    return d->subject;
}

void QGraphicsLookAtTransform::setSubject(QVector3D value)
{
    Q_D(QGraphicsLookAtTransform);
    if (d->subject != value)
    {
        d->subject = value;
        d->calculateRotationValues();
        emit subjectChanged();
        emit transformChanged();
    }
};

/*!
    \property QGraphicsLookAtTransform::upVector
    \brief The primary axis of rotation.

    This property indicates the first axis around which the object should
    rotate to face the subject.  If preserveUpVector is set true, will not
    change the object's orientation relative to the up vector at all.

    The default value for this property is Qt.vector3d(0, 1, 0).
*/

/*!
    \qmlproperty bool LookatTransform::upVector

    This property indicates the first axis around which the object should
    rotate to face the subject.  If preserveUpVector is set true, will not
    change the object's orientation relative to the up vector at all.

    The default value for this property is Qt.vector3d(0, 1, 0).
*/

/*!
    \fn void QGraphicsLookAtTransform::upVectorChanged()

    Signal that is emitted when upVector() changes.
*/

QVector3D QGraphicsLookAtTransform::upVector() const
{
    Q_D(const QGraphicsLookAtTransform);
    return d->upVector;
}

void QGraphicsLookAtTransform::setUpVector(QVector3D value)
{
    Q_D(QGraphicsLookAtTransform);
    if (d->upVector != value)
    {
        d->upVector = value;
        d->calculateRotationValues();
        emit upVectorChanged();
        emit transformChanged();
    }
};

/*!
    \property QGraphicsLookAtTransform::primaryAngle
    \brief true to preserve the up orientation.

    This property indicates the amount of rotation around the upVector that
    this transform applies.

    It is calculated based on subject and upVector, and cannot be set directly.

    \sa subject, upVector, secondaryAngle
*/

/*!
    \qmlproperty real LookatTransform::primaryAngle

    This property indicates the amount of rotation around the upVector that
    this transform applies.

    It is calculated based on subject and upVector, and cannot be set directly.

    \sa subject, upVector, secondaryAngle
*/

/*!
    \fn void QGraphicsLookAtTransform::primaryAngleChanged()

    Signal that is emitted when primaryAngle() changes.
*/

qreal QGraphicsLookAtTransform::primaryAngle() const
{
    Q_D(const QGraphicsLookAtTransform);
    return d->primaryRotation.angle();
}

/*!
    \property QGraphicsLookAtTransform::secondaryAngle
    \brief true to preserve the up orientation.

    This property indicates the amount of tilt this transformation applies.

    The secondary rotation is around a vector that is orthogonal to the up
    vector and the subject vector.  If we consider the analogy of turning a
    head to look at something, the up vector is up, the primary rotation
    control looking side to side, and the secondary rotation will control
    looking up and down.

    It is calculated based on subject and upVector, and cannot be set directly.

    If preserveUpVector is true, this value has no effect.

    \sa subject, upVector, primaryAngle, preserveUpVector
*/

/*!
    \qmlproperty real LookatTransform::secondaryAngle

    This property indicates the amount of tilt this transformation applies.

    The secondary rotation is around a vector that is orthogonal to the up
    vector and the subject vector.  If we consider the analogy of turning a
    head to look at something, the up vector is up, the primary rotation
    control looking side to side, and the secondary rotation will control
    looking up and down.

    It is calculated based on subject and upVector, and cannot be set directly.

    If preserveUpVector is true, this value has no effect.

    \sa subject, upVector, primaryAngle, preserveUpVector
*/

/*!
    \fn void QGraphicsLookAtTransform::secondaryAngleChanged()

    Signal that is emitted when secondaryAngle() changes.
*/

qreal QGraphicsLookAtTransform::secondaryAngle() const
{
    Q_D(const QGraphicsLookAtTransform);
    return d->secondaryRotation.angle();
}

/*!
    \internal
*/
void QGraphicsLookAtTransform::applyTo(QMatrix4x4 *matrix) const
{
    Q_D(const QGraphicsLookAtTransform);
    bool cameraTest = false;
    bool animationTest = false;
    // If we're looking at the camera, and not animating,
    // we can use a cheap cheat
    if (cameraTest && !animationTest)
    {
        if (!d->preserveUpVector)
            cheatingSphericalBillboard(matrix);
        else
            cheatingCylindricalBillboard(matrix);
        matrix->optimize();
        return;
    }

    d->primaryRotation.applyTo(matrix);
    // then, if preserveVector is not set, perform a second rotation
    // around the cross product of the lookAt and the upVector
    if (!preserveUpVector())
        d->secondaryRotation.applyTo(matrix);
}

/*!
    \internal
*/
QGraphicsTransform3D *QGraphicsLookAtTransform::clone(QObject *parent) const
{
    Q_D(const QGraphicsLookAtTransform);
    QGraphicsLookAtTransform *copy = new QGraphicsLookAtTransform(parent);
    copy->setPreserveUpVector(d->preserveUpVector);
    copy->setUpVector(d->upVector);
    copy->setSubject(d->subject);
    return copy;
}

/*!
    \fn void QGraphicsLookAtTransform::preserveUpVectorChanged()

    Signal that is emitted when preserveUpVector() changes.
*/

QT_END_NAMESPACE
