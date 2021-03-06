/***************************************************************************
                          qgsmapcanvas.h  -  description
                             -------------------
    begin                : Sun Jun 30 2002
    copyright            : (C) 2002 by Gary E.Sherman
    email                : sherman at mrcc.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSMAPCANVAS_H
#define QGSMAPCANVAS_H

#include "qgsconfig.h"

#include "qgsexpressioncontext.h"
#include "qgsfeature.h"
#include "qgsmessagebar.h"
#include "qgsrectangle.h"
#include "qgis.h"

#include <QDomDocument>
#include <QGraphicsView>
#include <QtCore>

#include "qgsmapsettings.h" // TEMPORARY
#include "qgsprevieweffect.h" //for QgsPreviewEffect::PreviewMode

#include <QGestureEvent>
#include "qgis_gui.h"

class QWheelEvent;
class QPixmap;
class QPaintEvent;
class QKeyEvent;
class ResizeEvent;

class QColor;
class QDomDocument;
class QPaintDevice;
class QMouseEvent;
class QRubberBand;
class QGraphicsScene;

class QgsMapToPixel;
class QgsMapLayer;
class QgsHighlight;
class QgsVectorLayer;

class QgsLabelingResults;
class QgsMapRendererCache;
class QgsMapRendererQImageJob;
class QgsMapSettings;
class QgsMapCanvasMap;
class QgsMapOverviewCanvas;
class QgsMapTool;
class QgsSnappingUtils;
class QgsRubberBand;


/** \ingroup gui
 * Map canvas is a class for displaying all GIS data types on a canvas.
 */

class GUI_EXPORT QgsMapCanvas : public QGraphicsView
{
    Q_OBJECT

  public:

    //! Constructor
    QgsMapCanvas( QWidget *parent = nullptr );

    ~QgsMapCanvas();

    //! Returns the magnification factor
    //! @note added in 2.16
    double magnificationFactor() const;

    //! Set list of layers that should be shown in the canvas
    //! @note added in 3.0
    void setLayers( const QList<QgsMapLayer *> &layers );

    void setCurrentLayer( QgsMapLayer *layer );

    //! Get access to properties used for map rendering
    //! @note added in 2.4
    const QgsMapSettings &mapSettings() const;

    //! sets destination coordinate reference system
    //! @note added in 2.4
    void setDestinationCrs( const QgsCoordinateReferenceSystem &crs );

    //! Get access to the labeling results (may be null)
    //! @note added in 2.4
    const QgsLabelingResults *labelingResults() const;

    //! Set whether to cache images of rendered layers
    //! @note added in 2.4
    void setCachingEnabled( bool enabled );

    //! Check whether images of rendered layers are curerently being cached
    //! @note added in 2.4
    bool isCachingEnabled() const;

    //! Make sure to remove any rendered images from cache (does nothing if cache is not enabled)
    //! @note added in 2.4
    void clearCache();

    //! Reload all layers, clear the cache and refresh the canvas
    //! @note added in 2.9
    void refreshAllLayers();

    //! Set whether the layers are rendered in parallel or sequentially
    //! @note added in 2.4
    void setParallelRenderingEnabled( bool enabled );

    //! Check whether the layers are rendered in parallel or sequentially
    //! @note added in 2.4
    bool isParallelRenderingEnabled() const;

    //! Set how often map preview should be updated while it is being rendered (in milliseconds)
    //! @note added in 2.4
    void setMapUpdateInterval( int timeMilliseconds );

    //! Find out how often map preview should be updated while it is being rendered (in milliseconds)
    //! @note added in 2.4
    int mapUpdateInterval() const;

    //! Get the last reported scale of the canvas
    double scale();

    //! Returns the mapUnitsPerPixel (map units per pixel) for the canvas
    double mapUnitsPerPixel() const;

    //! Returns the current zoom extent of the map canvas
    QgsRectangle extent() const;
    //! Returns the combined extent for all layers on the map canvas
    QgsRectangle fullExtent() const;

    //! Set the extent of the map canvas
    void setExtent( const QgsRectangle &r, bool magnified = false );

    //! Get the current map canvas rotation in clockwise degrees
    //! @note added in 2.8
    double rotation() const;

    //! Set the rotation of the map canvas in clockwise degrees
    //! @note added in 2.8
    void setRotation( double degrees );

    //! Set the center of the map canvas, in geographical coordinates
    //! @note added in 2.8
    void setCenter( const QgsPoint &center );

    //! Get map center, in geographical coordinates
    //! @note added in 2.8
    QgsPoint center() const;

    //! Zoom to the full extent of all layers
    void zoomToFullExtent();

    //! Zoom to the previous extent (view)
    void zoomToPreviousExtent();

    //! Zoom to the next extent (view)
    void zoomToNextExtent();

    // ! Clears the list of extents and sets current extent as first item
    void clearExtentHistory();

    /** Zoom to the extent of the selected features of current (vector) layer.
     * @param layer optionally specify different than current layer
     */
    void zoomToSelected( QgsVectorLayer *layer = nullptr );

    /** Set canvas extent to the bounding box of a set of features
        @param layer the vector layer
        @param ids the feature ids*/
    void zoomToFeatureIds( QgsVectorLayer *layer, const QgsFeatureIds &ids );

    /** Centers canvas extent to feature ids
        @param layer the vector layer
        @param ids the feature ids*/
    void panToFeatureIds( QgsVectorLayer *layer, const QgsFeatureIds &ids );

    //! Pan to the selected features of current (vector) layer keeping same extent.
    void panToSelected( QgsVectorLayer *layer = nullptr );

    //! \brief Sets the map tool currently being used on the canvas
    void setMapTool( QgsMapTool *mapTool );

    /** \brief Unset the current map tool or last non zoom tool
     *
     * This is called from destructor of map tools to make sure
     * that this map tool won't be used any more.
     * You don't have to call it manualy, QgsMapTool takes care of it.
     */
    void unsetMapTool( QgsMapTool *mapTool );

    //! Returns the currently active tool
    QgsMapTool *mapTool();

    //! Write property of QColor bgColor.
    void setCanvasColor( const QColor &_newVal );
    //! Read property of QColor bgColor.
    QColor canvasColor() const;

    //! Set color of selected vector features
    //! @note added in 2.4
    void setSelectionColor( const QColor &color );

    //! Emits signal scaleChanged to update scale in main window
    void updateScale();

    //! return the map layer at position index in the layer stack
    QgsMapLayer *layer( int index );

    //! return number of layers on the map
    int layerCount() const;

    //! return list of layers within map canvas.
    QList<QgsMapLayer *> layers() const;

    /**
     * Freeze/thaw the map canvas. This is used to prevent the canvas from
     * responding to events while layers are being added/removed etc.
     * @param frozen Boolean specifying if the canvas should be frozen (true) or
     * thawed (false). Default is true.
     * @see isFrozen()
     * @see setRenderFlag(). freeze() should be used to programmatically halt map updates,
     * while setRenderFlag() should only be used when users disable rendering via GUI.
     */
    void freeze( bool frozen = true );

    /**
     * Returns true if canvas is frozen.
     * @see renderFlag(). isFrozen() should be used to determine whether map updates
     * have been halted programmatically, while renderFlag() should be used to
     * determine whether a user has disabled rendering via GUI.
     * @see freeze()
     */
    bool isFrozen() const;

    /**
     * Returns true if canvas render is disabled as a result of user disabling
     * renders via the GUI.
     * @see setRenderFlag()
     * @see isFrozen(). isFrozen() should be used to determine whether map updates
     * have been halted programmatically, while renderFlag() should be used to
     * determine whether a user has disabled rendering via GUI.
     */
    bool renderFlag() const { return mRenderFlag; }

    /**
     * Convience function for returning the current canvas map units. The map units
     * are dictated by the canvas' destinationCrs() map units.
     */
    QgsUnitTypes::DistanceUnit mapUnits() const;

    //! Getter for stored overrides of styles for layers.
    //! @note added in 2.12
    QMap<QString, QString> layerStyleOverrides() const;

    //! Setter for stored overrides of styles for layers.
    //! @note added in 2.12
    void setLayerStyleOverrides( const QMap<QString, QString> &overrides );

    //! Get the current coordinate transform
    const QgsMapToPixel *getCoordinateTransform();

    //! Find out whether rendering is in progress
    bool isDrawing();

    //! returns current layer (set by legend widget)
    QgsMapLayer *currentLayer();

    //! set wheel zoom factor (should be greater than 1)
    void setWheelFactor( double factor );

    //! Zoom to a specific scale
    void zoomScale( double scale );

    //! Zoom with the factor supplied. Factor > 1 zooms out, interval (0,1) zooms in
    //! If point is given, re-center on it
    void zoomByFactor( double scaleFactor, const QgsPoint *center = nullptr );

    //! Zooms in/out with a given center
    void zoomWithCenter( int x, int y, bool zoomIn );

    //! Zooms to feature extent. Adds a small margin around the extent
    //! and does a pan if rect is empty (point extent)
    void zoomToFeatureExtent( QgsRectangle &rect );

    //! Returns whether the scale is locked, so zooming can be performed using magnication.
    //! @note added in 2.16
    //! @see setScaleLocked()
    bool scaleLocked() const { return mScaleLocked;}

    //! used to determine if anti-aliasing is enabled or not
    void enableAntiAliasing( bool flag );

    //! true if antialising is enabled
    bool antiAliasingEnabled() const { return mSettings.testFlag( QgsMapSettings::Antialiasing ); }

    //! sets map tile rendering flag
    void enableMapTileRendering( bool flag );

    // following 2 methods should be moved elsewhere or changed to private
    // currently used by pan map tool
    //! Ends pan action and redraws the canvas.
    void panActionEnd( QPoint releasePoint );

    //! Called when mouse is moving and pan is activated
    void panAction( QMouseEvent *event );

    //! returns last position of mouse cursor
    QPoint mouseLastXY();

    /** Enables a preview mode for the map canvas
     * @param previewEnabled set to true to enable a preview mode
     * @see setPreviewMode
     * @note added in 2.3 */
    void setPreviewModeEnabled( bool previewEnabled );

    /** Returns whether a preview mode is enabled for the map canvas
     * @returns true if a preview mode is currently enabled
     * @see setPreviewModeEnabled
     * @see previewMode
     * @note added in 2.3 */
    bool previewModeEnabled() const;

    /** Sets a preview mode for the map canvas. This setting only has an effect if
     * previewModeEnabled is true.
     * @param mode preview mode for the canvas
     * @see previewMode
     * @see setPreviewModeEnabled
     * @see previewModeEnabled
     * @note added in 2.3 */
    void setPreviewMode( QgsPreviewEffect::PreviewMode mode );

    /** Returns the current preview mode for the map canvas. This setting only has an effect if
     * previewModeEnabled is true.
     * @returns preview mode for map canvas
     * @see setPreviewMode
     * @see previewModeEnabled
     * @note added in 2.3 */
    QgsPreviewEffect::PreviewMode previewMode() const;

    /** Return snapping utility class that is associated with map canvas.
     *  If no snapping utils instance has been associated previously, an internal will be created for convenience
     *  (so map tools do not need to test for existence of the instance).
     *
     * Main canvas in QGIS returns an instance which is always up-to-date with the project's snapping configuration.
     *  @note added in 2.8
     */
    QgsSnappingUtils *snappingUtils() const;

    /** Assign an instance of snapping utils to the map canvas.
     * The instance is not owned by the canvas, so it is possible to use one instance in multiple canvases.
     *
     * For main canvas in QGIS, do not associate a different instance from the existing one (it is updated from
     * the project's snapping configuration).
     * @note added in 2.8
     */
    void setSnappingUtils( QgsSnappingUtils *utils );

    /** Sets an expression context scope for the map canvas. This scope is injected into the expression
     * context used for rendering the map, and can be used to apply specific variable overrides for
     * expression evaluation for the map canvas render. This method will overwrite the existing expression
     * context scope for the canvas.
     * @param scope new expression context scope
     * @note added in QGIS 2.12
     * @see expressionContextScope()
     */
    void setExpressionContextScope( const QgsExpressionContextScope &scope ) { mExpressionContextScope = scope; }

    /** Returns a reference to the expression context scope for the map canvas. This scope is injected
     * into the expression context used for rendering the map, and can be used to apply specific variable
     * overrides for expression evaluation for the map canvas render.
     * @note added in QGIS 2.12
     * @see setExpressionContextScope()
     */
    QgsExpressionContextScope &expressionContextScope() { return mExpressionContextScope; }

    /** Returns a const reference to the expression context scope for the map canvas.
     * @note added in QGIS 2.12
     * @see setExpressionContextScope()
     * @note not available in python bindings
     */
    const QgsExpressionContextScope &expressionContextScope() const { return mExpressionContextScope; }

    /** Sets the segmentation tolerance applied when rendering curved geometries
    @param tolerance the segmentation tolerance*/
    void setSegmentationTolerance( double tolerance );

    /** Sets segmentation tolerance type (maximum angle or maximum difference between curve and approximation)
    @param type the segmentation tolerance typename*/
    void setSegmentationToleranceType( QgsAbstractGeometry::SegmentationToleranceType type );

  public slots:

    //! Repaints the canvas map
    void refresh();

    //! Receives signal about selection change, and pass it on with layer info
    void selectionChangedSlot();

    //! Save the convtents of the map canvas to disk as an image
    void saveAsImage( const QString &fileName, QPixmap *QPixmap = nullptr, const QString & = "PNG" );

    //! This slot is connected to the visibility change of one or more layers
    void layerStateChange();

    //! This slot is connected to the layer's CRS change
    void layerCrsChange();

    /**
     * Sets whether a user has disabled canvas renders via the GUI.
     * @param flag set to false to indicate that user has disabled renders
     * @see renderFlag()
     * @see freeze(). freeze() should be used to programmatically halt map updates,
     * while setRenderFlag() should only be used when users disable rendering via GUI.
     */
    void setRenderFlag( bool flag );

    //! stop rendering (if there is any right now)
    //! @note added in 2.4
    void stopRendering();

    //! called to read map canvas settings from project
    void readProject( const QDomDocument & );

    //! called to write map canvas settings to project
    void writeProject( QDomDocument & );

    //! ask user about datum transformation
    void getDatumTransformInfo( const QgsMapLayer *ml, const QString &srcAuthId, const QString &destAuthId );

    //! Sets the factor of magnification to apply to the map canvas. Indeed, we
    //! increase/decrease the DPI of the map settings according to this factor
    //! in order to render marker point, labels, ... bigger.
    //! @note added in 2.16
    void setMagnificationFactor( double factor );

    //! Lock the scale, so zooming can be performed using magnication
    //! @note added in 2.16
    //! @see scaleLocked()
    void setScaleLocked( bool isLocked );

    //! Zoom in with fixed factor
    void zoomIn();

    //! Zoom out with fixed factor
    void zoomOut();

  private slots:
    //! called when current maptool is destroyed
    void mapToolDestroyed();

    //! called when a renderer job has finished successfully or when it was canceled
    void rendererJobFinished();

    void mapUpdateTimeout();

    void refreshMap();

  signals:

    /** Emits current mouse position
        \note changed in 1.3 */
    void xyCoordinates( const QgsPoint &p );

    //! Emitted when the scale of the map changes
    void scaleChanged( double );

    //! Emitted when the extents of the map change
    void extentsChanged();

    //! Emitted when the rotation of the map changes
    //! @note added in 2.8
    void rotationChanged( double );

    //! Emitted when the scale of the map changes
    //! @note added in 2.16
    void magnificationChanged( double );

    /** Emitted when the canvas has rendered.
     * Passes a pointer to the painter on which the map was drawn. This is
     * useful for plugins that wish to draw on the map after it has been
     * rendered.  Passing the painter allows plugins to work when the map is
     * being rendered onto a pixmap other than the mapCanvas own pixmap member.
     *
     */
    //! TODO: deprecate when decorations are reimplemented as map canvas items
    //! - anything related to rendering progress is not visible outside of map canvas
    //! - additional drawing shall be done directly within the renderer job or independently as a map canvas item
    void renderComplete( QPainter * );

    // ### QGIS 3: renamte to mapRefreshFinished()
    //! Emitted when canvas finished a refresh request.
    void mapCanvasRefreshed();

    // ### QGIS 3: rename to mapRefreshStarted()
    //! Emitted when the canvas is about to be rendered.
    void renderStarting();

    //! Emitted when a new set of layers has been received
    void layersChanged();

    //! Emit key press event
    void keyPressed( QKeyEvent *e );

    //! Emit key release event
    void keyReleased( QKeyEvent *e );

    /** Emit map tool changed with the old tool
     * @note added in 2.3
     */
    void mapToolSet( QgsMapTool *newTool, QgsMapTool *oldTool );

    // ### QGIS 3: remove the signal
    //! Emitted when selection in any layer gets changed
    void selectionChanged( QgsMapLayer *layer );

    //! Emitted when zoom last status changed
    void zoomLastStatusChanged( bool );

    //! Emitted when zoom next status changed
    void zoomNextStatusChanged( bool );

    //! Emitted when map CRS has changed
    //! @note added in 2.4
    void destinationCrsChanged();

    //! Emitted when the current layer is changed
    //! @note added in 2.8
    void currentLayerChanged( QgsMapLayer *layer );

    //! Emitted when the configuration of overridden layer styles changes
    //! @note added in 2.12
    void layerStyleOverridesChanged();

    //! emit a message (usually to be displayed in a message bar)
    void messageEmitted( const QString &title, const QString &message, QgsMessageBar::MessageLevel = QgsMessageBar::INFO );

  protected:

    //! Overridden standard event to be gestures aware
    bool event( QEvent *e ) override;

    //! Overridden key press event
    void keyPressEvent( QKeyEvent *e ) override;

    //! Overridden key release event
    void keyReleaseEvent( QKeyEvent *e ) override;

    //! Overridden mouse double click event
    void mouseDoubleClickEvent( QMouseEvent *e ) override;

    //! Overridden mouse move event
    void mouseMoveEvent( QMouseEvent *e ) override;

    //! Overridden mouse press event
    void mousePressEvent( QMouseEvent *e ) override;

    //! Overridden mouse release event
    void mouseReleaseEvent( QMouseEvent *e ) override;

    //! Overridden mouse wheel event
    void wheelEvent( QWheelEvent *e ) override;

    //! Overridden resize event
    void resizeEvent( QResizeEvent *e ) override;

    //! Overridden paint event
    void paintEvent( QPaintEvent *e ) override;

    //! Overridden drag enter event
    void dragEnterEvent( QDragEnterEvent *e ) override;

    //! called when panning is in action, reset indicates end of panning
    void moveCanvasContents( bool reset = false );

    //! called on resize or changed extent to notify canvas items to change their rectangle
    void updateCanvasItemPositions();

    /// implementation struct
    class CanvasProperties;

    /// Handle pattern for implementation object
    std::unique_ptr<CanvasProperties> mCanvasProperties;

#if 0

    /** Debugging member
     * invoked when a connect() is made to this object
     */
    void connectNotify( const char *signal ) override;
#endif
    //! Make sure the datum transform store is properly populated
    void updateDatumTransformEntries();

  private slots:

    void layerRepaintRequested( bool deferred );

    void autoRefreshTriggered();

    void updateAutoRefreshTimer();

  private:
    /// this class is non-copyable

    /**
       @note

       Otherwise QScopedPointer would pass the object responsibility on to the
       copy like a hot potato leaving the copyer in a weird state.
     */
    QgsMapCanvas( QgsMapCanvas const & );

    //! encompases all map settings necessary for map rendering
    QgsMapSettings mSettings;

    //! owns pixmap with rendered map and controls rendering
    QgsMapCanvasMap *mMap = nullptr;

    //! Flag indicating if the map canvas is frozen.
    bool mFrozen;

    //! Flag that allows squashing multiple refresh() calls into just one delayed rendering job
    bool mRefreshScheduled;

    //! determines whether user has requested to suppress rendering
    bool mRenderFlag;

    //! current layer in legend
    QgsMapLayer *mCurrentLayer = nullptr;

    //! graphics scene manages canvas items
    QGraphicsScene *mScene = nullptr;

    //! pointer to current map tool
    QgsMapTool *mMapTool = nullptr;

    //! previous tool if current is for zooming/panning
    QgsMapTool *mLastNonZoomMapTool = nullptr;

    //! recently used extent
    QList <QgsRectangle> mLastExtent;
    int mLastExtentIndex;

    //! Scale factor multiple for default zoom in/out
    double mWheelZoomFactor;

    //! Timer that periodically fires while map rendering is in progress to update the visible map
    QTimer mMapUpdateTimer;

    //! Job that takes care of map rendering in background
    QgsMapRendererQImageJob *mJob = nullptr;

    //! Flag determining whether the active job has been canceled
    bool mJobCanceled;

    //! Labeling results from the recently rendered map
    QgsLabelingResults *mLabelingResults = nullptr;

    //! Whether layers are rendered sequentially or in parallel
    bool mUseParallelRendering;

    //! Whether to add rendering stats to the rendered image
    bool mDrawRenderingStats;

    //! Optionally use cache with rendered map layers for the current map settings
    QgsMapRendererCache *mCache = nullptr;

    QTimer *mResizeTimer = nullptr;

    QgsPreviewEffect *mPreviewEffect = nullptr;

    QgsRectangle imageRect( const QImage &img, const QgsMapSettings &mapSettings );

    QgsSnappingUtils *mSnappingUtils = nullptr;

    //! lock the scale, so zooming can be performed using magnication
    bool mScaleLocked;

    QgsExpressionContextScope mExpressionContextScope;

    //! Stores zoom rect
    QRect mZoomRect;

    //! Flag to indicate a zoom by rectangle operation is taking place
    bool mZoomDragging;

    //! Zoom by rectangle rubber band
    std::unique_ptr< QgsRubberBand > mZoomRubberBand;

    QCursor mZoomCursor;

    QTimer mAutoRefreshTimer;

    //! Force a resize of the map canvas item
    //! @note added in 2.16
    void updateMapSize();

    /** Starts zooming via rectangle
     * @param pos start position for rectangle
     * @note added in QGIS 2.16
     */
    void beginZoomRect( QPoint pos );

    /** Ends zooming via rectangle
     * @param pos end position for rectangle
     * @note added in QGIS 2.16
     */
    void endZoomRect( QPoint pos );

    /** Returns bounding box of feature list (in canvas coordinates)
        @param ids feature id list
        @param layer the layer
        @param bbox out: bounding box
        @param errorMsg error message in case of error
        @return true in case of success*/
    bool boundingBoxOfFeatureIds( const QgsFeatureIds &ids, QgsVectorLayer *layer, QgsRectangle &bbox, QString &errorMsg ) const;

    friend class TestQgsMapCanvas;

}; // class QgsMapCanvas



#endif
