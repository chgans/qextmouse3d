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
    tracks a subject.  The lookAt transform will always rotate first around the
    local y axis, and secondarily around the local x axis, around an origin of
    (0,0,0).

    If rotation around a different axis or origin is desired, place the
    Item3D to be rotated inside a new parent Item3D.  Apply rotations the
    and translations to the original Item3D such that the desired "front"
    matches the new parent Item3d's positive z direction and the left to right
    matches the parent's x-axis, and then apply the LookAt transform to the
    new parent item.

    \code
    Item3D {
        position: Qt.vector3d(0,0,4)
        transform: LookAt { subject: subjectPenguin }

        Item3D {
            id: lookAwayMonkey
            mesh: Mesh { source: "meshes/monkey.3ds" }
            transform: Rotation3D { axis: Qt.vector3d(0,1,0); angle: 180 }
        }
    }
    \endcode

    By default the LookAt transform will cause the object to
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

#define RADS_TO_DEGREES (180.0 / M_PI)

class QGraphicsLookAtTransformPrivate
{
public:
    QGraphicsLookAtTransformPrivate(QGraphicsLookAtTransform* _lookAt);
    QGraphicsLookAtTransform* lookAt;
    void determineOriginItem();
    void calculateRotationValues();
    QVector3D relativePosition(QDeclarativeItem3D* originItem, QDeclarativeItem3D* subject);

    bool preserveUpVector;
    QDeclarativeItem3D* originItem;
    QDeclarativeItem3D* subject;

    QGraphicsRotation3D primaryRotation;
    QGraphicsRotation3D secondaryRotation;
};

QGraphicsLookAtTransformPrivate::QGraphicsLookAtTransformPrivate(QGraphicsLookAtTransform* _lookAt) :
    lookAt(_lookAt), preserveUpVector(false), originItem(0), subject(0)
{
}

void QGraphicsLookAtTransformPrivate::determineOriginItem()
{
    QObject* workingObject = lookAt->parent();
    while (qobject_cast<QDeclarativeItem3D*>(workingObject) == 0 &&
           workingObject != 0)
    {
        workingObject = workingObject->parent();
    }

    originItem = qobject_cast<QDeclarativeItem3D*>(workingObject);
    if (!originItem)
        qWarning() << "LookAt transform requires an Item3D ancestor";
}

QVector3D QGraphicsLookAtTransformPrivate::relativePosition(QDeclarativeItem3D* originItem, QDeclarativeItem3D* subject)
{
    QVector3D result =  originItem->worldToLocal(subject->localToWorld());
    return result;

}

void QGraphicsLookAtTransformPrivate::calculateRotationValues()
{
    QVector3D forwards(0, 0, 1);
    primaryRotation.setAngle(0);
    secondaryRotation.setAngle(0);
    determineOriginItem();

    if (subject == 0 || originItem == 0)
    {
        if (subject == 0)
            qWarning() << "LookAt transform got null subject";
        if (originItem == 0)
            qWarning() << "LookAt transform got null originItem";
        primaryRotation.setAxis(QVector3D(0,1,0));
        primaryRotation.setAngle(0);
        secondaryRotation.setAxis(QVector3D(1,0,0));
        secondaryRotation.setAngle(0);
        return;
    }

    // Calculate the lookat vector in the local frame:
    QVector3D relativePositionVector = relativePosition(originItem, subject);

    // Project the relative position into the xz plane:
    QVector3D subjectProjection = relativePositionVector;
    subjectProjection.setY(0);
    subjectProjection.normalize();
    QVector3D primaryRotationAxis;

    if (!subjectProjection.isNull())
    {
        primaryRotationAxis = QVector3D::crossProduct(forwards, subjectProjection);
        // Fix rotational axis for positions along z axis
        if (primaryRotationAxis.length() == 0)
        {
            primaryRotationAxis = QVector3D(0,1,0);
        }

        qreal angleCosine =
                QVector3D::dotProduct(forwards, subjectProjection);
        qreal angle = qAcos(angleCosine);

        primaryRotation.setAxis(primaryRotationAxis);
        primaryRotation.setAngle(angle * RADS_TO_DEGREES );
    } else {
        qWarning() << "QGraphicsLookAtTransform: unable to generate target projection for primary rotation";
        primaryRotation.setAxis(QVector3D(0,1,0));
        primaryRotation.setAngle(0);
    }

    relativePositionVector.normalize();

    qreal secondaryAngleCosine = QVector3D::dotProduct( subjectProjection,
                                                       relativePositionVector);
    // Sanity check in case of rounding errors
    if (secondaryAngleCosine <= 1.0 && secondaryAngleCosine >= -1.0)
    {
        if (relativePositionVector.y() < 0)
            secondaryRotation.setAxis(QVector3D(1,0,0));
        else
            secondaryRotation.setAxis(QVector3D(-1,0,0));
        secondaryRotation.setAngle(qAcos(secondaryAngleCosine)*RADS_TO_DEGREES);
    } else {
        secondaryRotation.setAxis(QVector3D(1,0,0));
        secondaryRotation.setAngle(0.0);
    }
}

/*!
    Construct a lookAt transform and attach it to \a parent.
*/
QGraphicsLookAtTransform::QGraphicsLookAtTransform(QObject *parent)
    : QGraphicsTransform3D(parent), d_ptr(new QGraphicsLookAtTransformPrivate(this))
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
    its up vector's orientation preserved, rotating around only the y-axis and
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
    its y-axis' orientation preserved, rotating around only this axis and
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
    applying the transformation.

    Forwards is always considered to be in the direction of the positive z axis
    in local space, and up is always considered to be the positive y axis, and
    rotation is always around the position (0,0,0) in local space (although
    any values can be emulated by applying the LookAt transformation to a
    parent Item3D, and then applying transformations relative to that parent).

    If no subject is set, no transformation is applied.
*/

/*!
    \qmlproperty bool LookatTransform::preserveUpVector

    This property indicates whether the transform should rotate around a second
    axis to face directly at the subject (known as a "spherical look-at").

    If the value for this property is true, then the object will have
    its y-axis' orientation preserved, rotating around only this axis and
    not directly facing the subject unless it happens to fall on the correct
    plane (when the subject is the camera, this is known as a
    "cylindrical look-at").

    The default value for this property is false.
*/

/*!
    \fn void QGraphicsLookAtTransform::subjectChanged()

    Signal that is emitted when subject() changes.
*/


QDeclarativeItem3D* QGraphicsLookAtTransform::subject() const
{
    Q_D(const QGraphicsLookAtTransform);
    return d->subject;
}

void QGraphicsLookAtTransform::setSubject(QDeclarativeItem3D* value)
{
    Q_D(QGraphicsLookAtTransform);
    if (d->subject != value)
    {
        disconnect(this, SLOT(subjectPositionChanged()));
        d->subject = value;
        QDeclarativeItem3D* ancestorItem = d->subject;
        while (ancestorItem != 0)
        {
            connect(ancestorItem, SIGNAL(positionChanged()), this, SLOT(subjectPositionChanged()));
            connect(ancestorItem, SIGNAL(rotationChanged()), this, SLOT(subjectPositionChanged()));
            connect(ancestorItem, SIGNAL(scaleChanged()), this, SLOT(subjectPositionChanged()));
            connect(ancestorItem, SIGNAL(parentChanged()), this, SLOT(subjectPositionChanged()));
            ancestorItem = qobject_cast<QDeclarativeItem3D*>(ancestorItem->parent());
        };

        d->calculateRotationValues();
        emit subjectChanged();
        emit transformChanged();
    }
};

void QGraphicsLookAtTransform::subjectPositionChanged()
{
    Q_D(QGraphicsLookAtTransform);
    d->calculateRotationValues();
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
    // around the x-axis
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
    copy->setSubject(d->subject);
    return copy;
}

/*!
    \fn void QGraphicsLookAtTransform::preserveUpVectorChanged()

    Signal that is emitted when preserveUpVector() changes.
*/

QT_END_NAMESPACE
