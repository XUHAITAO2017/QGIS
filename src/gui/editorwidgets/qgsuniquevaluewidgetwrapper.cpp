/***************************************************************************
    qgsuniquevaluewidgetwrapper.cpp
     --------------------------------------
    Date                 : 5.1.2014
    Copyright            : (C) 2014 Matthias Kuhn
    Email                : matthias at opengis dot ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsuniquevaluewidgetwrapper.h"

#include "qgsvectorlayer.h"
#include "qgsfilterlineedit.h"

#include <QCompleter>
#include <QSettings>

QgsUniqueValuesWidgetWrapper::QgsUniqueValuesWidgetWrapper( QgsVectorLayer *vl, int fieldIdx, QWidget *editor, QWidget *parent )
  : QgsEditorWidgetWrapper( vl, fieldIdx, editor, parent )
  , mComboBox( nullptr )
  , mLineEdit( nullptr )
{
}

QVariant QgsUniqueValuesWidgetWrapper::value() const
{
  QVariant value;

  if ( mComboBox )
    value = mComboBox->currentData();

  if ( mLineEdit )
  {
    if ( mLineEdit->text() == QgsApplication::nullRepresentation() )
      value = QVariant( field().type() );
    else
      value = mLineEdit->text();
  }

  return value;
}

QWidget *QgsUniqueValuesWidgetWrapper::createWidget( QWidget *parent )
{
  if ( config( QStringLiteral( "Editable" ) ).toBool() )
    return new QgsFilterLineEdit( parent );
  else
    return new QComboBox( parent );
}

void QgsUniqueValuesWidgetWrapper::initWidget( QWidget *editor )
{
  mComboBox = qobject_cast<QComboBox *>( editor );
  mLineEdit = qobject_cast<QLineEdit *>( editor );

  QStringList sValues;

  QList<QVariant> values;

  layer()->uniqueValues( fieldIdx(), values );

  Q_FOREACH ( const QVariant &v, values )
  {
    if ( mComboBox )
    {
      mComboBox->addItem( v.toString(), v );
    }

    if ( mLineEdit )
    {
      sValues << v.toString();
    }
  }

  if ( mLineEdit )
  {
    QgsFilterLineEdit *fle = qobject_cast<QgsFilterLineEdit *>( editor );
    if ( fle && !( field().type() == QVariant::Int || field().type() == QVariant::Double || field().type() == QVariant::LongLong || field().type() == QVariant::Date ) )
    {
      fle->setNullValue( QgsApplication::nullRepresentation() );
    }

    QCompleter *c = new QCompleter( sValues );
    c->setCaseSensitivity( Qt::CaseInsensitive );
    c->setCompletionMode( QCompleter::PopupCompletion );
    mLineEdit->setCompleter( c );

    connect( mLineEdit, SIGNAL( textChanged( QString ) ), this, SLOT( valueChanged( QString ) ) );
  }

  if ( mComboBox )
  {
    connect( mComboBox, SIGNAL( currentIndexChanged( int ) ), this, SLOT( valueChanged() ) );
  }
}

bool QgsUniqueValuesWidgetWrapper::valid() const
{
  return mComboBox || mLineEdit;
}

void QgsUniqueValuesWidgetWrapper::showIndeterminateState()
{
  if ( mComboBox )
  {
    whileBlocking( mComboBox )->setCurrentIndex( -1 );
  }
  if ( mLineEdit )
  {
    whileBlocking( mLineEdit )->setText( QString() );
  }
}

void QgsUniqueValuesWidgetWrapper::setValue( const QVariant &value )
{
  if ( mComboBox )
  {
    mComboBox->setCurrentIndex( mComboBox->findData( value ) );
  }

  if ( mLineEdit )
  {
    if ( value.isNull() )
      mLineEdit->setText( QgsApplication::nullRepresentation() );
    else
      mLineEdit->setText( value.toString() );
  }
}
