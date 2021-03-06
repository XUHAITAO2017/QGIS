/***************************************************************************
                         qgspaperitem.h
                       -------------------
    begin                : September 2008
    copyright            : (C) 2008 by Marco Hugentobler
    email                : marco dot hugentobler at karto dot baug dot ethz dot ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSPAPERITEM_H
#define QGSPAPERITEM_H

#include "qgis_core.h"
#include "qgscomposeritem.h"
#include <QGraphicsRectItem>

/** \ingroup core
 * Item representing a grid. This is drawn separately to the underlying paper item since the grid needs to be
 * drawn above all other composer items, while the paper item is drawn below all others.*/
class CORE_EXPORT QgsPaperGrid: public QGraphicsRectItem
{
  public:
    QgsPaperGrid( double x, double y, double width, double height, QgsComposition *composition );

    //! \brief Reimplementation of QCanvasItem::paint
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *itemStyle, QWidget *pWidget ) override;

  private:
    QgsComposition *mComposition = nullptr;
};

/** \ingroup core
 * Item representing the paper.*/
class CORE_EXPORT QgsPaperItem : public QgsComposerItem
{
    Q_OBJECT

  public:
    QgsPaperItem( QgsComposition *c );
    QgsPaperItem( qreal x, qreal y, qreal width, qreal height, QgsComposition *composition );
    ~QgsPaperItem();

    //! Return correct graphics item type.
    virtual int type() const override { return ComposerPaper; }

    //! \brief Reimplementation of QCanvasItem::paint
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *itemStyle, QWidget *pWidget ) override;

    /** Stores state in Dom element
       * @param elem is Dom element corresponding to 'Composer' tag
       * @param doc Dom document
       */
    bool writeXml( QDomElement &elem, QDomDocument &doc ) const override;

    /** Sets state from Dom document
     * @param itemElem is Dom node corresponding to item tag
     * @param doc is the Dom document
     */
    bool readXml( const QDomElement &itemElem, const QDomDocument &doc ) override;

    virtual void setSceneRect( const QRectF &rectangle ) override;

  private:
    QgsPaperItem();
    //! Set flags and z-value
    void initialize();

    void calculatePageMargin();

    QgsPaperGrid *mPageGrid = nullptr;
    double mPageMargin;
};

#endif
