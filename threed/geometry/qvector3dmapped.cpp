#include "qvector3dmapped_p.h"

inline bool QVector3DMapped::findNormal(const QVector3D &norm,
                                       const QVector3DMapped::VList &list) const
{
    const QVector3DMapped *nx = this;
    for ( ; nx->normPtr != -1; nx = &(list[nx->normPtr]))
        if (qFuzzyCompare(norm, list[nx->normPtr]))
            return true;
    return false;
}

inline void QVector3DMapped::appendNormal(int vec, QGeometryData *normals,
                                          QVector3DMapped::VList &list)
{
    QVector3DMapped &nx = *this;
    for ( ; nx.normPtr != -1; nx = list[nx.normPtr]) {}
    nx.normPtr = list.count();
    list.append(QVector3DMapped(vec, normals));
}

