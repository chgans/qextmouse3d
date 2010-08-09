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

#include "qglrendersequencer.h"
#include "qglrenderorder.h"
#include "qglpainter.h"
#include "qglrenderorderrepository.h"
#include "qglrenderstate.h"

/*!
    \class QGLRenderSequencer
    \brief The QGLRenderSequencer class orders the rendering of QGLSceneNode instances.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::scene

    The QGLRenderSequencer class works with the QGLRenderOrderRepository and
    QGLRenderOrder classes to optimize the rendering order of scene nodes.

    In general instances of this class are managed by QGLPainter and it should
    not be necessary to explicitly create or manipulate them.

    The render sequencer works by tracking instances of QGLRenderOrder objects
    in a queue.  As the scene graph is traversed during a call to a top-level
    node's QGLSceneNode::draw() function, the sequencer adds one QGLRenderOrder
    to the queue for each unique combination of rendering attributes.

    The top level scene graph node loops once for each unique combination - it
    does this in QGLSceneNode::draw() by calling nextInSequence().  At
    each iteration, a current QGLRenderOrder is maintained, and only nodes
    matching that order - as determined by \l{QGLRenderOrder::operator==()} -
    are rendered in that pass.  Non-matching nodes are added to a queue in the
    order specified by \l{QGLRenderOrder::operator<()}.


    Once an iteration/pass of
    the scene graph is done, the next order is pulled from the front of the queue
    and the current QGLRenderOrder is set to it.

    Since the rendering attributes at a node are a function both of that node,
    and attributes inherited from its parents, and since a given node may appear
    multiple times at different places in the scene, it can thus have different
    attributes and orders in each place.  So there is no one-to-one mapping
    between nodes and attributes.

    To deal with this, QGLRenderOrder mappings are discovered during rendering.
    There is no discovery pass.  First, the initial QGLRenderOrder is lazily set
    when the first geometry is actually drawn to the GPU - latching in that order
    as the first current order.  From that point, orders discovered that are
    distinct from the current one are skipped in this rendering pass - by returning
    false from renderInSequence() - and are instead added to the queue for rendering
    on a subsequent pass.

    When the final pass has been made, renderInSequence() returns false to the
    top level QGLSceneNode, indicating that looping over passes is complete.

    \sa QGLRenderOrder
*/

class QGLRenderSequencerPrivate
{
public:
    QGLRenderSequencerPrivate(QGLPainter *painter);
    ~QGLRenderSequencerPrivate();
    QGLSceneNode *top;
    QLinkedList<RenderOrderKey> queue;
    QArray<QGLRenderState> stack;
    int stackTop;
    QSet<QGLRenderOrder> exclude;
    RenderOrderKey current;
    QGLPainter *painter;
    QGLRenderOrderRepository *renderOrderRepository;
    bool latched;
};

QGLRenderSequencerPrivate::QGLRenderSequencerPrivate(QGLPainter *painter)
    : top(0)
    , stackTop(-1)
    , current(RenderOrderKey())
    , painter(painter)
    , renderOrderRepository(new QGLRenderOrderRepository)
    , latched(false)
{
    stack.resize(8);
}

QGLRenderSequencerPrivate::~QGLRenderSequencerPrivate()
{
    delete renderOrderRepository;
}

/*!
    Construct a new QGLRenderSequencer for the \a painter.
*/
QGLRenderSequencer::QGLRenderSequencer(QGLPainter *painter)
    : d(new QGLRenderSequencerPrivate(painter))
{
}

/*!
    Sets the render sequencer to operate on \a painter.
*/
void QGLRenderSequencer::setPainter(QGLPainter *painter)
{
    d->painter = painter;
}

/*!
    Returns the current top node of the rendering tree, or NULL if the
    sequencer has been reset.
*/
QGLSceneNode *QGLRenderSequencer::top() const
{
    return d->top;
}

/*!
    Sets the current top node of the rendering tree to \a top.
*/
void QGLRenderSequencer::setTop(QGLSceneNode *top)
{
    d->top = top;
}

/*!
    Reset this sequencer to start from the top of the scene graph again.
    After this call the top() function will return NULL, and any scene
    node passed to the renderInSequence() function will be treated as the
    top of the scene graph.  Also effects and materials will be ignored
    until latched in - QGLPainter::draw() will call latch() to achieve this.

    \sa top()
*/
void QGLRenderSequencer::reset()
{
    d->top = 0;
    d->latched = false;
    //qDebug() << "QGLRenderSequencer::reset()";
}

/*!
    Returns true if there is a next rendering state in the queue; and if there
    is a new order will be pulled from the queue, and its rendering attributes
     - materials, effects and so on - will be applied to the painter for this
    sequencer.  Returns false when no more rendering states are queued and
    scene is completely rendered.
*/
bool QGLRenderSequencer::nextInSequence()
{
    bool nextAvailable = true;
    //qDebug() << "############## nextInSequence() ###############";
    if (d->queue.size() > 0)
    {
        // process thru next render order
        d->current = d->queue.takeFirst();
    }
    else
    {
        // end top level loop
        nextAvailable = false;
    }
    //qDebug() << "              ---> return is next:" << nextAvailable;
    return nextAvailable;
}

/*!
    Returns true, when this \a node should be rendered, in the order dictated
    by QGLRenderOrder objects generated by the current render order repository.
    The \a node must be non-NULL.

    When this function returns false, indicating that rendering should be
    skipped for the current pass, a check is made to ensure that a state object
    for this nodes rendering attributes is queued up for a later pass.

    To customize the ordering, reimplement QGLRenderOrder and
    QGLRenderOrderRepository; then set the custom repository onto the current
    painter using setRenderOrderRepository().

    \sa reset(), top(), nextInSequence()
*/
bool QGLRenderSequencer::renderInSequence(QGLSceneNode *node)
{
    Q_ASSERT(node);
    Q_ASSERT(d->top);
    bool doRender = true;
    QGLRenderState state;
    if (d->stackTop >= 0)
        state = d->stack.at(d->stackTop);
    RenderOrderKey key(node, state);
    QGLRenderOrder *o = d->renderOrderRepository->getOrder(key);
    //qDebug() << "QGLRenderSequencer::renderInSequence:" << *o;
    if (!d->current.valid())
        d->current = key;
    QGLRenderOrder *c = d->renderOrderRepository->getOrder(d->current);
    //qDebug() << "     comparing with current:" << *c;
    if (d->latched && *o != *c)
    {
        if (!d->exclude.contains(*o))
        {
            //qDebug() << "    no match - render on later pass";
            insertNew(*o);
        }
        else
        {
            //qDebug() << "    no match - already rendered";
        }
        doRender = false;
    }
    else
    {
        if (!d->latched)
        {
            //qDebug() << "    not latched -falling thru";
            d->exclude.insert(*o);
        }
        else
        {
            //qDebug() << "    matched!! - rendering...";
        }
    }
    return doRender;
}

/*!
    Marks the render state for the \a node as started for this rendering pass.  Call
    this before rendering \a node, or any child nodes of \a node.

    Once the rendering state is no longer valid, call endState().

    To actually apply the effective state, as inherited from previous calls to
    beginState() call the applyState() function.

    \sa endState(), applyState()
*/
void QGLRenderSequencer::beginState(QGLSceneNode *node)
{
    QGLRenderState state;
    if (d->stackTop >= 0)
        state = d->stack.at(d->stackTop);
    state.updateFrom(node);
    ++d->stackTop;
    if (d->stackTop >= d->stack.size())
        d->stack.extend(d->stack.size() * 2);
    d->stack[d->stackTop] = state;
    //qDebug() << "begin state for:" << node << "stack size now:" << (d->stackTop + 1);
}

/*!
    Marks the render state for the \a node as done for this rendering pass.

    If a node has called beginState(), then this function must be called to maintain
    the rendering stack.  If this function call is not matched by a previous
    beginState() call undefined behaviour may result.  In debug mode it may assert.

    \sa beginState(), applyState()
*/
void QGLRenderSequencer::endState(QGLSceneNode *node)
{
    //qDebug() << "end state for:" << node << "stack size now:" << (d->stackTop)
    //        << "popping stack top:" << d->stack.at(d->stackTop);
#ifndef QT_NO_DEBUG_STREAM
    const QGLSceneNode *n = d->stack.at(d->stackTop).node();
    Q_ASSERT(n == node);
#endif
    --d->stackTop;
}

/*!
    Applies the current rendering state to the painter for this sequencer.

    \sa beginState(), endState()
*/
void QGLRenderSequencer::applyState()
{
    d->latched = true;
    QGLRenderState s = d->stack.at(d->stackTop);
    // qDebug() << "Applying state:\n" << s;
    if (s.hasEffect() && !d->painter->isPicking())
    {
        if (s.userEffect())
        {
            if (d->painter->userEffect() != s.userEffect())
                d->painter->setUserEffect(s.userEffect());
        }
        else
        {
            if (d->painter->standardEffect() != s.standardEffect())
                d->painter->setStandardEffect(s.standardEffect());
        }
    }
    if (s.material() && !d->painter->isPicking())
    {
        QGLMaterial *mat = s.material();
        if (d->painter->faceMaterial(QGL::FrontFaces) != mat)
        {
            d->painter->setFaceMaterial(QGL::FrontFaces, mat);
            int texUnit = 0;
            for (int i = 0; i < mat->textureLayerCount(); ++i)
            {
                QGLTexture2D *tex = mat->texture(i);
                if (tex)
                {
                    d->painter->setTexture(texUnit, tex);
                    ++texUnit;
                }
            }
        }
    }
}

void QGLRenderSequencer::insertNew(const QGLRenderOrder &order)
{
    QLinkedList<RenderOrderKey>::iterator it = d->queue.begin();
    int j = 0; // debug = remove me
    for ( ; it != d->queue.end(); ++it)
    {
        const QGLRenderOrder *o = d->renderOrderRepository->getOrder(*it);
        if (order < *o)
            break;
        ++j;
    }
    //qDebug() << "inserting:" << order << "at" << j << "in queue of" << d->queue.size();
    d->queue.insert(it, order.key());
    d->exclude.insert(order);
}

/*!
    Returns the current render order repository.  By default this is an
    instance of the base class QGLRenderOrderRepository.

    \sa setRenderOrderRepository()
*/
QGLRenderOrderRepository *QGLRenderSequencer::renderOrderRepository() const
{
    return d->renderOrderRepository;
}

/*!
    Sets the current render order \a repository.  This is only needed if a
    custom rendering order is to be created.

    Any existing current repository is destroyed.

    \sa renderInSequence(), renderOrderRepository()
*/
void QGLRenderSequencer::setRenderOrderRepository(QGLRenderOrderRepository *repository)
{
    Q_ASSERT(repository);
    delete d->renderOrderRepository;
    d->renderOrderRepository = repository;
}
