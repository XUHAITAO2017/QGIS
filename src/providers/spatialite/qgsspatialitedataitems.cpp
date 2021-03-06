/***************************************************************************
    qgsspatialitedataitems.cpp
    ---------------------
    begin                : October 2011
    copyright            : (C) 2011 by Martin Dobias
    email                : wonder dot sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qgsspatialitedataitems.h"

#include "qgsspatialiteprovider.h"
#include "qgsspatialiteconnection.h"
#include "qgsspatialitesourceselect.h"

#include "qgslogger.h"
#include "qgsmimedatautils.h"
#include "qgsvectorlayerimport.h"
#include "qgsmessageoutput.h"
#include "qgsvectorlayer.h"
#include "qgssettings.h"

#include <QAction>
#include <QMessageBox>
#include <QProgressDialog>

QGISEXTERN bool deleteLayer( const QString &dbPath, const QString &tableName, QString &errCause );

QgsSLLayerItem::QgsSLLayerItem( QgsDataItem *parent, QString name, QString path, QString uri, LayerType layerType )
  : QgsLayerItem( parent, name, path, uri, layerType, QStringLiteral( "spatialite" ) )
{
  setState( Populated ); // no children are expected
}

QList<QAction *> QgsSLLayerItem::actions()
{
  QList<QAction *> lst;

  QAction *actionDeleteLayer = new QAction( tr( "Delete Layer" ), this );
  connect( actionDeleteLayer, SIGNAL( triggered() ), this, SLOT( deleteLayer() ) );
  lst.append( actionDeleteLayer );

  return lst;
}

void QgsSLLayerItem::deleteLayer()
{
  if ( QMessageBox::question( nullptr, QObject::tr( "Delete Object" ),
                              QObject::tr( "Are you sure you want to delete %1?" ).arg( mName ),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) != QMessageBox::Yes )
    return;

  QgsDataSourceUri uri( mUri );
  QString errCause;
  bool res = ::deleteLayer( uri.database(), uri.table(), errCause );
  if ( !res )
  {
    QMessageBox::warning( nullptr, tr( "Delete Layer" ), errCause );
  }
  else
  {
    QMessageBox::information( nullptr, tr( "Delete Layer" ), tr( "Layer deleted successfully." ) );
    mParent->refresh();
  }
}

// ------

QgsSLConnectionItem::QgsSLConnectionItem( QgsDataItem *parent, QString name, QString path )
  : QgsDataCollectionItem( parent, name, path )
{
  mDbPath = QgsSpatiaLiteConnection::connectionPath( name );
  mToolTip = mDbPath;
}

static QgsLayerItem::LayerType _layerTypeFromDb( const QString &dbType )
{
  if ( dbType == QLatin1String( "POINT" ) || dbType == QLatin1String( "MULTIPOINT" ) )
  {
    return QgsLayerItem::Point;
  }
  else if ( dbType == QLatin1String( "LINESTRING" ) || dbType == QLatin1String( "MULTILINESTRING" ) )
  {
    return QgsLayerItem::Line;
  }
  else if ( dbType == QLatin1String( "POLYGON" ) || dbType == QLatin1String( "MULTIPOLYGON" ) )
  {
    return QgsLayerItem::Polygon;
  }
  else if ( dbType == QLatin1String( "qgis_table" ) )
  {
    return QgsLayerItem::Table;
  }
  else
  {
    return QgsLayerItem::NoType;
  }
}

QVector<QgsDataItem *> QgsSLConnectionItem::createChildren()
{
  QVector<QgsDataItem *> children;
  QgsSpatiaLiteConnection connection( mName );

  QgsSpatiaLiteConnection::Error err = connection.fetchTables( false ); // TODO: allow geometryless tables
  if ( err != QgsSpatiaLiteConnection::NoError )
  {
    QString msg;
    switch ( err )
    {
      case QgsSpatiaLiteConnection::NotExists:
        msg = tr( "Database does not exist" );
        break;
      case QgsSpatiaLiteConnection::FailedToOpen:
        msg = tr( "Failed to open database" );
        break;
      case QgsSpatiaLiteConnection::FailedToCheckMetadata:
        msg = tr( "Failed to check metadata" );
        break;
      case QgsSpatiaLiteConnection::FailedToGetTables:
        msg = tr( "Failed to get list of tables" );
        break;
      default:
        msg = tr( "Unknown error" );
        break;
    }
    QString msgDetails = connection.errorMessage();
    if ( !msgDetails.isEmpty() )
      msg = QStringLiteral( "%1 (%2)" ).arg( msg, msgDetails );
    children.append( new QgsErrorItem( this, msg, mPath + "/error" ) );
    return children;
  }

  QString connectionInfo = QStringLiteral( "dbname='%1'" ).arg( QString( connection.path() ).replace( '\'', QLatin1String( "\\'" ) ) );
  QgsDataSourceUri uri( connectionInfo );

  Q_FOREACH ( const QgsSpatiaLiteConnection::TableEntry &entry, connection.tables() )
  {
    uri.setDataSource( QString(), entry.tableName, entry.column, QString(), QString() );
    QgsSLLayerItem *layer = new QgsSLLayerItem( this, entry.tableName, mPath + '/' + entry.tableName, uri.uri(), _layerTypeFromDb( entry.type ) );
    children.append( layer );
  }
  return children;
}

bool QgsSLConnectionItem::equal( const QgsDataItem *other )
{
  if ( type() != other->type() )
  {
    return false;
  }
  const QgsSLConnectionItem *o = dynamic_cast<const QgsSLConnectionItem *>( other );
  return o && mPath == o->mPath && mName == o->mName;
}

QList<QAction *> QgsSLConnectionItem::actions()
{
  QList<QAction *> lst;

  //QAction* actionEdit = new QAction( tr( "Edit..." ), this );
  //connect( actionEdit, SIGNAL( triggered() ), this, SLOT( editConnection() ) );
  //lst.append( actionEdit );

  QAction *actionDelete = new QAction( tr( "Delete" ), this );
  connect( actionDelete, SIGNAL( triggered() ), this, SLOT( deleteConnection() ) );
  lst.append( actionDelete );

  return lst;
}

void QgsSLConnectionItem::editConnection()
{
}

void QgsSLConnectionItem::deleteConnection()
{
  if ( QMessageBox::question( nullptr, QObject::tr( "Delete Connection" ),
                              QObject::tr( "Are you sure you want to delete the connection to %1?" ).arg( mName ),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) != QMessageBox::Yes )
    return;

  QgsSpatiaLiteConnection::deleteConnection( mName );
  // the parent should be updated
  mParent->refresh();
}

bool QgsSLConnectionItem::handleDrop( const QMimeData *data, Qt::DropAction )
{
  if ( !QgsMimeDataUtils::isUriList( data ) )
    return false;

  // TODO: probably should show a GUI with settings etc

  QgsDataSourceUri destUri;
  destUri.setDatabase( mDbPath );

  qApp->setOverrideCursor( Qt::WaitCursor );

  QProgressDialog *progress = new QProgressDialog( tr( "Copying features..." ), tr( "Abort" ), 0, 0, nullptr );
  progress->setWindowTitle( tr( "Import layer" ) );
  progress->setWindowModality( Qt::WindowModal );
  progress->show();

  QStringList importResults;
  bool hasError = false;
  bool canceled = false;

  QgsMimeDataUtils::UriList lst = QgsMimeDataUtils::decodeUriList( data );
  Q_FOREACH ( const QgsMimeDataUtils::Uri &u, lst )
  {
    if ( u.layerType != QLatin1String( "vector" ) )
    {
      importResults.append( tr( "%1: Not a vector layer!" ).arg( u.name ) );
      hasError = true; // only vectors can be imported
      continue;
    }

    // open the source layer
    QgsVectorLayer *srcLayer = new QgsVectorLayer( u.uri, u.name, u.providerKey );

    if ( srcLayer->isValid() )
    {
      destUri.setDataSource( QString(), u.name, srcLayer->geometryType() != QgsWkbTypes::NullGeometry ? QStringLiteral( "geom" ) : QString() );
      QgsDebugMsg( "URI " + destUri.uri() );
      QgsVectorLayerImport::ImportError err;
      QString importError;
      err = QgsVectorLayerImport::importLayer( srcLayer, destUri.uri(), QStringLiteral( "spatialite" ), srcLayer->crs(), false, &importError, false, nullptr, progress );
      if ( err == QgsVectorLayerImport::NoError )
        importResults.append( tr( "%1: OK!" ).arg( u.name ) );
      else if ( err == QgsVectorLayerImport::ErrUserCanceled )
        canceled = true;
      else
      {
        importResults.append( QStringLiteral( "%1: %2" ).arg( u.name, importError ) );
        hasError = true;
      }
    }
    else
    {
      importResults.append( tr( "%1: OK!" ).arg( u.name ) );
      hasError = true;
    }

    delete srcLayer;
  }

  delete progress;

  qApp->restoreOverrideCursor();

  if ( canceled )
  {
    QMessageBox::information( nullptr, tr( "Import to SpatiaLite database" ), tr( "Import canceled." ) );
    refresh();
  }
  else if ( hasError )
  {
    QgsMessageOutput *output = QgsMessageOutput::createMessageOutput();
    output->setTitle( tr( "Import to SpatiaLite database" ) );
    output->setMessage( tr( "Failed to import some layers!\n\n" ) + importResults.join( QStringLiteral( "\n" ) ), QgsMessageOutput::MessageText );
    output->showMessage();
  }
  else
  {
    QMessageBox::information( nullptr, tr( "Import to SpatiaLite database" ), tr( "Import was successful." ) );
    refresh();
  }

  return true;
}


// ---------------------------------------------------------------------------

QgsSLRootItem::QgsSLRootItem( QgsDataItem *parent, QString name, QString path )
  : QgsDataCollectionItem( parent, name, path )
{
  mCapabilities |= Fast;
  mIconName = QStringLiteral( "mIconSpatialite.svg" );
  populate();
}

QVector<QgsDataItem *> QgsSLRootItem::createChildren()
{
  QVector<QgsDataItem *> connections;
  Q_FOREACH ( const QString &connName, QgsSpatiaLiteConnection::connectionList() )
  {
    QgsDataItem *conn = new QgsSLConnectionItem( this, connName, mPath + '/' + connName );
    connections.push_back( conn );
  }
  return connections;
}

QList<QAction *> QgsSLRootItem::actions()
{
  QList<QAction *> lst;

  QAction *actionNew = new QAction( tr( "New Connection..." ), this );
  connect( actionNew, SIGNAL( triggered() ), this, SLOT( newConnection() ) );
  lst.append( actionNew );

  QAction *actionCreateDatabase = new QAction( tr( "Create Database..." ), this );
  connect( actionCreateDatabase, SIGNAL( triggered() ), this, SLOT( createDatabase() ) );
  lst.append( actionCreateDatabase );

  return lst;
}

QWidget *QgsSLRootItem::paramWidget()
{
  QgsSpatiaLiteSourceSelect *select = new QgsSpatiaLiteSourceSelect( nullptr, 0, true );
  connect( select, SIGNAL( connectionsChanged() ), this, SLOT( connectionsChanged() ) );
  return select;
}

void QgsSLRootItem::connectionsChanged()
{
  refresh();
}

void QgsSLRootItem::newConnection()
{
  if ( QgsSpatiaLiteSourceSelect::newConnection( nullptr ) )
  {
    refresh();
  }
}

QGISEXTERN bool createDb( const QString &dbPath, QString &errCause );

void QgsSLRootItem::createDatabase()
{
  QgsSettings settings;
  QString lastUsedDir = settings.value( QStringLiteral( "/UI/lastSpatiaLiteDir" ), QDir::homePath() ).toString();

  QString filename = QFileDialog::getSaveFileName( nullptr, tr( "New SpatiaLite Database File" ),
                     lastUsedDir,
                     tr( "SpatiaLite" ) + " (*.sqlite *.db *.sqlite3 *.db3 *.s3db)" );
  if ( filename.isEmpty() )
    return;

  QString errCause;
  if ( ::createDb( filename, errCause ) )
  {
    // add connection
    settings.setValue( "/SpatiaLite/connections/" + QFileInfo( filename ).fileName() + "/sqlitepath", filename );

    refresh();
  }
  else
  {
    QMessageBox::critical( nullptr, tr( "Create SpatiaLite database" ), tr( "Failed to create the database:\n" ) + errCause );
  }
}

// ---------------------------------------------------------------------------

QGISEXTERN QgsSpatiaLiteSourceSelect *selectWidget( QWidget *parent, Qt::WindowFlags fl )
{
  // TODO: this should be somewhere else
  return new QgsSpatiaLiteSourceSelect( parent, fl, false );
}

QGISEXTERN int dataCapabilities()
{
  return  QgsDataProvider::Database;
}

QGISEXTERN QgsDataItem *dataItem( QString path, QgsDataItem *parentItem )
{
  Q_UNUSED( path );
  return new QgsSLRootItem( parentItem, QStringLiteral( "SpatiaLite" ), QStringLiteral( "spatialite:" ) );
}
