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

#include "qglsection_p.h"
#include "qgldisplaylist_p.h"
#include "qarray.h"
#include "qvector_utils_p.h"

#include <QtGui/qvector3d.h>
#include <QtCore/qdebug.h>
#include <QtCore/qpointer.h>
#include <QtCore/qhash.h>

#include <limits.h>

/*!
    \internal
    \class QGLSection
    \brief The QGLSection class clusters like geometry in a QGLDisplayList.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::geometry

    QGLSection instances partition a QGLDisplayList into related sections,
    while the display list is being initialized with geometry data.

    Once the display list is initialized, and geometry building is complete
    the QGLSection instances are destroyed and the data is uploaded to the
    graphics hardware.

    The QGLSection class is a work horse for the QGLDisplayList, and it
    takes care of automatically managing vertex data.  As such
    for usual use cases, its functionality will not need to be referenced
    directly.  For low-level access to geometry, QGLSection provides a
    range of accessors to reference geometry data during scene building.

    Within a section, incoming geometry data will be coalesced and
    indexes created to reference the fewest possible copies of the vertex
    data.  For example, in smooth geometry all copies of a vertex are
    coalesced into one, and referenced by indices - except in the case
    where texture data forms a \i seam and a copy must be created to carry
    the two texture coordinates of the seam.

    This is handled automatically by QGLSection, to pack data into the
    smallest space possible thus improving cache coherence and performance.

    All the vertices in a QGLSection are treated with the same
    \l{QGL::Smoothing}{smoothing}, and have the same
    \l{QLogicalVertex::Type}{data types}.

    Each QGLSection references a contiguous range of vertices in a
    QGLDisplayList.

    A QGLDisplayList instance has the \l{QGLDisplayList::newSection()}{newSection()}
    function which creates a new QGLSection to reference its data.  Use this
    to construct new QGLSection instances, or alternatively construct
    a new QGLSection() and pass a non-null QGLDisplayList pointer.

    These functions all return QVector values. QVector instances are
    implicitly shared, thus the copies are inexpensive unless a
    non-const function is called on them, triggering a copy-on-write.

    Generally for adding geometry, use append().  This function simply
    calls virtual protected functions appendSmooth() (for smoothed vertices)
    and appendFaceted() (for faceted vertices).  See QGLDisplayList for a
    discussion of smoothing.

    Note that after initialization of the display list, the QGLSection
    instances are destroyed.  To access the geometry data during the run-time
    of the application, if needed, call the geometry() method on the
    display list and its child nodes.
*/


/**** goop to QHash a QVector3d ************************************/

uint qHash(float data)
{
    union U {
        quint32 n;
        float f;
    };
    U u;
    u.f = data;
    return u.n;
}

uint qHash(double data)
{
    union U {
        quint64 n;
        double f;
    };
    U u;
    u.f = data;
    return u.n ^ (u.n << 32);
}

static inline uint ROTLY(uint x)
{
    return (x << 10) | (x >> 22);
}

static inline uint ROTLZ(uint x)
{
    return (x << 20) | (x >> 12);
 }

uint qHash(const QVector3D &v)
{
    return qHash(v.x()) ^ ROTLY(qHash(v.y())) ^ ROTLZ(qHash(v.z()));
}


/****  interface for QVector3D maps ****************************************/

class QVector3DMapperIterator;

class QVector3DMapper
{
public:
    QVector3DMapper() {}
    virtual ~QVector3DMapper() {}
    virtual void insert(const QVector3D &, int) {};
    virtual QVector3DMapperIterator *find(const QVector3D &) const { return 0; }
    virtual bool atEnd(QVector3DMapperIterator *) const { return true; }
    virtual void reserve(int) {};
};

class QVector3DMapperIterator
{
public:
    QVector3DMapperIterator() {}
    virtual ~QVector3DMapperIterator() {}
    virtual QVector3D key() const { return QVector3D(); }
    virtual int value() const { return -1; }
    virtual void next() {}
    operator int () { return value(); }
    QVector3DMapperIterator &operator++() { next(); return *this; }
};


/**** qhash based implementation ****************************************/

class QHashMapper;

class QHashMapperIterator : public QVector3DMapperIterator
{
public:
    QHashMapperIterator(QHash<QVector3D,int>::const_iterator i, const QHashMapper *m)
        : it(i), map(m) {}
    ~QHashMapperIterator() {}
    QVector3D key() const { return it.key(); }
    int value() const { return it.value(); }
    void next() { ++it; }
private:
    friend class QHashMapper;
    QHash<QVector3D,int>::const_iterator it;
    const QHashMapper *map;
};

class QHashMapper : public QVector3DMapper
{
public:
    QHashMapper() {}
    ~QHashMapper() {}
    inline void insert(const QVector3D &vec, int i);
    inline QVector3DMapperIterator *find(const QVector3D &vec) const;
    inline bool atEnd(QVector3DMapperIterator *it) const;
private:
    QHash<QVector3D, int> hash;
};

inline void QHashMapper::insert(const QVector3D &vec, int i)
{
    hash.insertMulti(vec, i);
}

inline QVector3DMapperIterator *QHashMapper::find(const QVector3D &vec) const
{
    return new QHashMapperIterator(hash.find(vec), this);
}

inline bool QHashMapper::atEnd(QVector3DMapperIterator *it) const
{
    if (it != NULL)
    {
        QHashMapperIterator *mit = static_cast<QHashMapperIterator*>(it);
        return mit->it == hash.constEnd();
    }
    return true;
}

/**** qmap based implementation *******************************************/

bool operator<(const QVector3D &a, const QVector3D &b)
{
    if (qFskCompare(a.x(), b.x()))
    {
        if (qFskCompare(a.y(), b.y()))
        {
            if (qFskCompare(a.z(), b.z()))
            {
                return false;  // equal so not less-than
            }
            else
            {
                return a.z() < b.z();
            }
        }
        else
        {
            return a.y() < b.y();
        }
    }
    else
    {
        return a.x() < b.x();
    }
}

class QMapMapper;

class QMapMapperIterator : public QVector3DMapperIterator
{
public:
    QMapMapperIterator(QMap<QVector3D,int>::const_iterator i, const QMapMapper *m)
        : it(i), map(m) {}
    ~QMapMapperIterator() {}
    QVector3D key() const { return it.key(); }
    int value() const { return it.value(); }
    void next() { ++it; }
private:
    friend class QMapMapper;
    QMap<QVector3D,int>::const_iterator it;
    const QMapMapper *map;
};

class QMapMapper : public QVector3DMapper
{
public:
    QMapMapper() {}
    ~QMapMapper() {}
    inline void insert(const QVector3D &vec, int i);
    inline QVector3DMapperIterator *find(const QVector3D &vec) const;
    inline bool atEnd(QVector3DMapperIterator *it) const;
private:
    QMap<QVector3D, int> map;
};

inline void QMapMapper::insert(const QVector3D &vec, int i)
{
    map.insertMulti(vec, i);
}

inline QVector3DMapperIterator *QMapMapper::find(const QVector3D &vec) const
{
    return new QMapMapperIterator(map.find(vec), this);
}

inline bool QMapMapper::atEnd(QVector3DMapperIterator *it) const
{
    if (it != NULL)
    {
        QMapMapperIterator *mit = static_cast<QMapMapperIterator*>(it);
        return mit->it == map.constEnd();
    }
    return true;
}

/************************************************************************/

class QGLSectionPrivate
{
public:
    QGLSectionPrivate(const QVector3DArray *, QGL::Strategy st)
        : finalized(false)
    {
        if (st == QGL::HashLookup)
        {
            //  QHash based implmentation
            map = new QHashMapper;
        }
        else if (st == QGL::MapLookup)
        {
            //  QMap based implmentation
            map = new QMapMapper;
        }
        else
        {
            //  "do nothing" implementation
            map = new QVector3DMapper;
        }
    }
    ~QGLSectionPrivate() {}

    bool normalAccumulated(int index, const QVector3D &norm) const
    {
        QHash<int, QVector3D>::const_iterator nit = norms.constFind(index);
        while (nit != norms.constEnd() && nit.key() == index)
            if (*nit++ == norm)
                return true;
        return false;
    }

    void accumulateNormal(int index, const QVector3D &norm)
    {
        norms.insertMulti(index, norm);
    }

    QVector3DMapper *map;
    QHash<int, QVector3D> norms;
    bool finalized;
    QList<QGLSceneNode*> nodes;
};

/*!
    \internal
    Construct a new QGLSection on the display list \a list, and with smoothing \a s.
    By default the smoothing is QGL::Smooth.

    See QGLDisplayList for a discussion of smoothing.

    The pointer \a list must be non-null, and in debug mode, unless QT_NO_DEBUG is
    defined, this function will assert if \a list is null.

    The following lines of code have identical effect:
    \code
    QGLSection *s = myDisplayList->newSection(QGL::Faceted);
    QGLSection *s2 = new QGLSection(myDisplayList, QGL::Faceted);
    \endcode
*/
QGLSection::QGLSection(QGLDisplayList *list, QGL::Smoothing s, QGL::Strategy st)
    : m_smoothing(s)
    , m_displayList(list)
    , d(0)
{
    Q_ASSERT(m_displayList);
    d = new QGLSectionPrivate(vertexData(), st);
    m_displayList->addSection(this);
}

/*!
    \internal
    Destroy this QGLSection, recovering any resources.
*/
QGLSection::~QGLSection()
{
    delete d;
}

/*!
    \internal
    Reserve capacity for \a amount items.  This may avoid realloc
    overhead when a large number of items will be appended.
*/
void QGLSection::reserve(int amount)
{
    QGeometryData::reserve(amount);
    d->map->reserve(amount);
    d->norms.reserve(amount);
}

/*!
    \internal
    Adds the logical vertices \a a, \a b and \c to this section.  All
    should have the same fields.  This function is exactly equivalent to
    \code
        append(a); append(b); append(c);
    \endcode

    \sa appendSmooth(), appendFaceted(), appendFlat()
*/
void QGLSection::append(const QLogicalVertex &a, const QLogicalVertex &b, const QLogicalVertex &c)
{
    Q_ASSERT(a.fields() == b.fields() && b.fields() == c.fields());
    if (!a.hasField(QGL::Normal))
    {
        appendFlat(a, b, c);
    }
    else
    {
        if (m_smoothing == QGL::Smooth)
            appendSmooth(a, b, c);
        else
            appendFaceted(a, b, c);
    }
}

/*!
    \internal
    Adds the logical vertex \a lv to this section.

    If the \a lv has no lighting normal component, then the append will
    be done by calling appendFlat().

    Otherwise, if the \a lv does have a lighting normal; then the
    vertex processing depends on the smoothing property of this section.
    If this section has smoothing QGL::Smooth, then the append will be done
    by calling appendSmooth(); or if this section has smoothing QGL::Faceted,
    then the append will be done by calling appendFaceted().

    \sa appendSmooth(), appendFaceted(), appendFlat()
*/
void QGLSection::append(const QLogicalVertex &lv)
{
    if (!lv.hasField(QGL::Normal))
    {
        appendFlat(lv);
    }
    else
    {
        if (m_smoothing == QGL::Smooth)
            appendSmooth(lv);
        else
            appendFaceted(lv);
    }
}

static bool qCompareByAttributes(const QLogicalVertex &a, const QLogicalVertex &b)
{
    static const quint32 ATTRS_AND_TEXTURES = (0xFFFFFFFF << QGL::TextureCoord0);
    quint32 af = a.fields() & ATTRS_AND_TEXTURES;
    quint32 bf = b.fields() & ATTRS_AND_TEXTURES;
    if (af != bf)
        return false;
    quint32 flds = af | bf;
    const quint32 mask = 0x01;
    flds >>= QGL::TextureCoord0;
    for (int i = QGL::TextureCoord0; flds; ++i, flds >>= 1)
    {
        if (flds & mask)
        {
            QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(i);
            if (attr < QGL::CustomVertex0)
            {
                if (!qFskCompare(a.texCoord(attr), b.texCoord(attr)))
                    return false;
            }
            else
            {
                QVariant v1 = a.attribute(attr);
                QVariant v2 = b.attribute(attr);
                if (v1.type() == (QVariant::Type)QMetaType::Float)
                    return qFskCompare(v1.toFloat(), v2.toFloat());
                else if (v1.type() == QVariant::Vector2D)
                    return qFskCompare(qVariantValue<QVector2D>(v1), qVariantValue<QVector2D>(v2));
                else if (v1.type() == QVariant::Vector3D)
                    return qFskCompare(qVariantValue<QVector3D>(v1), qVariantValue<QVector3D>(v2));
                else
                    return v1 == v2;
            }
        }
    }
    return true;
}

int QGLSection::appendOne(const QLogicalVertex &lv)
{
    int index = appendVertex(lv);
    d->map->insert(lv.vertex(), index);
    appendIndex(index);
    return index;
}

/*!
    \internal
    Adds the logical vertex \a lv to this section of a display list.

    Two QLogicalVertex instances a and b are treated as being duplicates for
    the purpose of smoothing, if \c{qFuzzyCompare(a.vertex(), b.vertex())} is
    true

    All duplicate occurrences of a vertex are coalesced, that is replaced
    by a GL index referencing the one copy.

    In order to draw \a lv as part of a smooth continuous surface, with
    no distinct edge, duplicates of vertex \a lv are coalesced into one
    (within this section) and the normal for that one set to the average of
    the incoming unique normals.

    The incoming vertex \a lv is not treated as a duplicate if \a lv has
    different texture coordinates or attributes.  This occurs for example
    in the case of a texture seam, where two different texture coordinates
    are required at the same point on the geometry.

    In that case a new duplicate vertex is added to carry the unique
    texture coordinates or attributes.  When new vertex copies are added in
    this way all copies receive the averaged normals.

    Call this function to add the vertices of a smooth face to the section
    of a display list, or use:

    \code
    myDisplayList->newSection(QGLDisplayList::Smooth);
    myDisplayList->addTriangle(a, b, c);
    \endcode

    In smooth surfaces, the vertex and its normal is only sent to the
    graphics hardware once (not once per face), thus smooth geometry may
    consume fewer resources.

    \sa appendFaceted(), updateTexCoord(), QGLDisplayList::newSection()
*/
void QGLSection::appendSmooth(const QLogicalVertex &lv)
{
    Q_ASSERT(lv.hasField(QGL::Position));
    Q_ASSERT(lv.hasField(QGL::Normal));

    QVector3DMapperIterator  *it = d->map->find(lv.vertex());
    bool coalesce = false;
    if (d->map->atEnd(it))
    {
        int newIndex = appendOne(lv);
        d->accumulateNormal(newIndex, lv.normal());
    }
    else
    {
        while (!coalesce && !d->map->atEnd(it) && it->key() == lv.vertex())
            if (qCompareByAttributes(lv, vertexAt(*it)))
                coalesce = true;
            else
                ++*it;
        if (!coalesce)  // texture or attributes prevented coalesce
        {
            // new vert to carry tex/attrib data
            d->accumulateNormal(appendOne(lv), lv.normal());
        }
        else
        {
            appendIndex(it->value());
            it = d->map->find(lv.vertex());
            while (!d->map->atEnd(it) && it->key() == lv.vertex())
            {
                if (!d->normalAccumulated(it->value(), lv.normal()))
                {
                    normalRef(it->value()) += lv.normal();
                    d->accumulateNormal(it->value(), lv.normal());
                }
                ++*it;
            }
        }
    }
    d->finalized = false;
    m_displayList->setDirty(true);
}

/*!
    \internal
    Add the logical vertex \a lv to this section of a display list.

    The vertex will be drawn as a distinct edge, instead of just part of a
    continuous smooth surface.  To acheive this the vertex value of \a lv
    is duplicated for each unique normal in the current section.

    Note that duplication is only for unique normals: if a vertex is already
    present with a given normal it is coalesced and simply referenced by index.
    As for appendSmooth() vertices are not coalesced in this way if \a lv
    has a different texture coordinate or attribute than its duplicate.

    This function is used to add the vertices of a faceted face to the list:

    \code
    myDisplayList->newSection(QGLDisplayList::Faceted);
    myDisplayList->addVertex(lv);
    \endcode

    In faceted surfaces, the vertex is sent to the graphics hardware once for
    each normal it has, and thus may consume more resources.

    \sa appendSmooth(), updateTexCoord(), QGLDisplayList::newSection()
*/
void QGLSection::appendFaceted(const QLogicalVertex &lv)
{
    Q_ASSERT(lv.hasField(QGL::Position));
    Q_ASSERT(lv.hasField(QGL::Normal));
    QVector3DMapperIterator *it = d->map->find(lv.vertex());
    bool coalesce = false;
    while (!coalesce && !d->map->atEnd(it) && it->key() == lv.vertex())
    {
        if (vertexAt(it->value()) == lv)
            coalesce = true;
        else
            ++*it;
    }
    if (coalesce) // found
    {
        appendIndex(it->value());
    }
    else
    {
        appendOne(lv);
    }
    d->finalized = false;
    m_displayList->setDirty(true);
}

/*!
    \internal
    Append the logical vertex \a lv to this section.

    The vertex will be treated as flat colored, and thus no management
    of lighting normals is done.
*/
void QGLSection::appendFlat(const QLogicalVertex &lv)
{
    appendVertex(lv);
    d->finalized = false;
    m_displayList->setDirty(true);
}

/*!
    \internal
    \fn QGLDisplayList *QGLSection::displayList() const

    Returns the display list associated with this section.
*/

/*!
    \internal
    Returns a list of the QGLSceneNode instances associated with this section.
*/
QList<QGLSceneNode*> QGLSection::nodes() const
{
    return d->nodes;
}

/*!
    \internal
    Adds the \a node to the list of QGLSceneNode instances associated with
    this section.
*/
void QGLSection::addNode(QGLSceneNode *node)
{
    d->nodes.append(node);
}

/*!
    \internal
    Deletes the \a node from the list of QGLSceneNode instances associated
    with this section.  Returns true if the \a node was found, false
    otherwise.
*/
bool QGLSection::deleteNode(QGLSceneNode *node)
{
    int ix = d->nodes.indexOf(node);
    if (ix != -1)
    {
        d->nodes.removeAt(ix);
        return true;
    }
    return false;
}

#ifndef QT_NO_DEBUG_STREAM
/*!
    \internal
    Output a string representation of \a section to a debug stream \a dbg.
    \relates QGLSection
*/
QDebug operator<<(QDebug dbg, const QGLSection &section)
{
    dbg.space()
            << "QGLSection(" << &section
            << "- count:" << section.count()
            << "- smoothing mode:" << (section.smoothing() == QGL::Smooth ?
                                       "QGL::Smooth" : "QGL::Faceted") << "\n";
    QGLIndexArray indices = section.indices();
    for (int i = 0; i < section.count(); ++i)
    {
        int ix = indices[i];
        dbg << section.vertexAt(ix) << "\n";
    }
    dbg << ")\n";
    return dbg.space();
}
#endif
