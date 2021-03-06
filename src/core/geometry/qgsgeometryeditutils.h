/***************************************************************************
                        qgsgeometryeditutils.h
  -------------------------------------------------------------------
Date                 : 21 Jan 2015
Copyright            : (C) 2015 by Marco Hugentobler
email                : marco.hugentobler at sourcepole dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSGEOMETRYEDITUTILS_H
#define QGSGEOMETRYEDITUTILS_H

class QgsAbstractGeometry;
class QgsCurve;
class QgsGeometryEngine;
class QgsVectorLayer;

#include "qgsfeature.h"
#include <QMap>

/** \ingroup core
 * \class QgsGeometryEditUtils
 * \brief Convenience functions for geometry editing
 * \note added in QGIS 2.10
 * \note this API is not considered stable and may change for 2.12
 * \note not available in Python bindings
 */
class QgsGeometryEditUtils
{
  public:

    /** Adds interior ring (taking ownership).
    @return 0 in case of success (ring added), 1 problem with geometry type, 2 ring not closed,
    3 ring is not valid geometry, 4 ring not disjoint with existing rings, 5 no polygon found which contained the ring*/
    // TODO QGIS 3.0 returns an enum instead of a magic constant
    static int addRing( QgsAbstractGeometry *geom, QgsCurve *ring );

    /** Adds part to multi type geometry (taking ownership)
    @return 0 in case of success, 1 if not a multigeometry, 2 if part is not a valid geometry, 3 if new polygon ring
    not disjoint with existing polygons of the feature*/
    // TODO QGIS 3.0 returns an enum instead of a magic constant
    static int addPart( QgsAbstractGeometry *geom, QgsAbstractGeometry *part );

    /** Deletes a ring from a geometry.
     * @returns true if delete was successful
     */
    static bool deleteRing( QgsAbstractGeometry *geom, int ringNum, int partNum = 0 );

    /** Deletes a part from a geometry.
     * @returns true if delete was successful
     */
    static bool deletePart( QgsAbstractGeometry *geom, int partNum );

    /** Alters a geometry so that it avoids intersections with features from all open vector layers.
     * @param geom geometry to alter
     * @param avoidIntersectionsLayers list of layers to check for intersections
     * @param ignoreFeatures map of layer to feature id of features to ignore
     */
    static QgsAbstractGeometry *avoidIntersections( const QgsAbstractGeometry &geom,
        const QList<QgsVectorLayer *> &avoidIntersectionsLayers,
        QHash<QgsVectorLayer *, QSet<QgsFeatureId> > ignoreFeatures = ( QHash<QgsVectorLayer *, QSet<QgsFeatureId> >() ) );
};

#endif // QGSGEOMETRYEDITUTILS_H
