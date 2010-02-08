///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#include "AIMImageGraphicsDelegate.h"

//-- STL includes
#include <iostream>

//-- Qt Includes
#include <QtCore/QPoint>
#include <QtGui/QMainWindow>
#include <QtGui/QTableWidget>
#include <QtGui/QPixmap>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QHeaderView>
#include <QtGui/QGraphicsPixmapItem>



#define ZOOM_INDEX_MAX 9
#define ZOOM_INDEX_MIN 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImageGraphicsDelegate::AIMImageGraphicsDelegate(QObject* parent) :
  QObject(parent),
  m_MainWindow(NULL),
  m_GraphicsView(NULL),
  m_GraphicsScene(NULL),
  m_CompositeImages(false),
  m_CurrentGraphicsItem(NULL),
  _zoomFactor(1.0),
  _shouldFitToWindow(false)
{


  _zoomFactors[0] = 0.05;
  _zoomFactors[1] = 0.1;
  _zoomFactors[2] = 0.25;
  _zoomFactors[3] = 0.50;
  _zoomFactors[4] = 1.0;
  _zoomFactors[5] = 1.5;
  _zoomFactors[6] = 2.0;
  _zoomFactors[7] = 4.0;
  _zoomFactors[8] = 6.0;
  _zoomFactors[9] = -1.0;
  _zoomIndex = 4;

  m_composition_mode = QPainter::CompositionMode_Exclusion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImageGraphicsDelegate::~AIMImageGraphicsDelegate()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::resetCaches()
{
  this->m_CachedImage = QImage();
  this->m_OverlayImage = QImage();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::displayTextMessage(QString message)
{
  _displayTextMessage( message );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::increaseZoom()
{
  _shouldFitToWindow = false;
  if (m_CachedImage.isNull() == true) { return; }
  // Find the next scaling factor up from where we are currently
  QSize imageSize = this->m_CachedImage.size();
  int gvWidth = m_GraphicsView->size().width();
  int gvHeight = m_GraphicsView->size().height();
  gvWidth -= 4;
  gvHeight -= 4;
  if (imageSize.width() > imageSize.height() )
  {
    for (int i = 0; i < ZOOM_INDEX_MAX - 1; ++i )
    {
      if (_zoomFactor < this->_zoomFactors[i] && i > 0)
      {
        this->_zoomIndex = i;
        this->_zoomFactor = this->_zoomFactors[this->_zoomIndex ];
        break;
      }
    }
  }

  for (int i = 0; i < ZOOM_INDEX_MAX - 1; ++i )
  {
    if (_zoomFactor < this->_zoomFactors[i] && i > 0)
    {
      this->_zoomIndex = i;
      this->_zoomFactor = this->_zoomFactors[this->_zoomIndex ];
      break;
    }
  }

  updateGraphicsScene();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::decreaseZoom()
{
  _shouldFitToWindow = false;
  // Find the next scaling factor down
  if (m_CachedImage.isNull() == true) { return; }
  QSize imageSize = this->m_CachedImage.size();
  int gvWidth = m_GraphicsView->size().width();
  int gvHeight = m_GraphicsView->size().height();
  gvWidth -= 4;
  gvHeight -= 4;
  if (imageSize.width() > imageSize.height() )
  {
    for (int i = 0; i < ZOOM_INDEX_MAX - 1; ++i )
    {
      if (_zoomFactor < this->_zoomFactors[i] && i > 0)
      {
        this->_zoomIndex = i - 1;
        this->_zoomFactor = this->_zoomFactors[this->_zoomIndex ];
        break;
      }
    }
  }

  for (int i = ZOOM_INDEX_MAX - 1; i >= 0; --i )
  {
    if (_zoomFactor > this->_zoomFactors[i] && i > 0)
    {
      this->_zoomIndex = i;
      this->_zoomFactor = this->_zoomFactors[this->_zoomIndex ];
      break;
    }
  }
  updateGraphicsScene();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::fitToWindow()
{
  if (m_CachedImage.isNull() == true) { return; }
  _shouldFitToWindow = true;
  _zoomIndex = ZOOM_INDEX_MAX;
  this->setZoomFactor(_zoomFactors[_zoomIndex]);

  QSize imageSize = this->m_CachedImage.size();
  int gvWidth = m_GraphicsView->size().width();
  int gvHeight = m_GraphicsView->size().height();
  gvWidth -= 4;
  gvHeight -= 4;
  if (imageSize.width() > imageSize.height() )
  {
    double zf = (double)(gvWidth)/(double)(imageSize.width());
    this->setZoomFactor(zf);
  }
  else
  {
    double zf = (double)(gvHeight)/(double)(imageSize.height());
    this->setZoomFactor(zf);
  }
  updateGraphicsScene();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::setZoomFactor(double zoomFactor)
{
  this->_zoomFactor = zoomFactor;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage AIMImageGraphicsDelegate::_scaleImage()
{
  //std::cout << "  AIMImageGraphicsDelegate::_scaleImage()" << std::endl;
  QSize imageSize = this->m_CachedImage.size();
 // std::cout << "    imageSize (W x H) :" << imageSize.width() << " x " << imageSize.height() << std::endl;
//  int gvWidth = m_GraphicsView->size().width();
//  int gvHeight = m_GraphicsView->size().height();
 // std::cout << "    GV Size (W X H) :" << gvWidth << " x " << gvHeight << std::endl;
  if (_zoomFactor > -1.0)
  {
   // std::cout << "  _zoomFactor: " << _zoomFactor << std::endl;
    imageSize *= _zoomFactor;
  }

  return this->m_CachedImage.scaled(imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage AIMImageGraphicsDelegate::_scaleImage(QImage image)
{
  //std::cout << "  AIMImageGraphicsDelegate::_scaleImage()" << std::endl;
  QSize imageSize = image.size();
 // std::cout << "    imageSize (W x H) :" << imageSize.width() << " x " << imageSize.height() << std::endl;
//  int gvWidth = m_GraphicsView->size().width();
//  int gvHeight = m_GraphicsView->size().height();
 // std::cout << "    GV Size (W X H) :" << gvWidth << " x " << gvHeight << std::endl;
  if (_zoomFactor > -1.0)
  {
   // std::cout << "  _zoomFactor: " << _zoomFactor << std::endl;
    imageSize *= _zoomFactor;
  }

  return image.scaled(imageSize, Qt::KeepAspectRatio );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::on_parentResized()
{
  //std::cout << "  AIMImageGraphicsDelegate::on_parentResized" << std::endl;
//  int gvWidth = m_GraphicsView->size().width();
//  int gvHeight = m_GraphicsView->size().height();
 // std::cout << "    GV Size (W X H) :" << gvWidth << " x " << gvHeight << std::endl;
  if (_shouldFitToWindow == true)
  {
    fitToWindow();
  }
  else
  {
    updateGraphicsScene();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::updateGraphicsScene(bool update)
{

  if (this->m_CachedImage.isNull() == true)
  {
    return;
  }
  if (NULL != m_CurrentGraphicsItem)
  {
    m_GraphicsScene->removeItem(m_CurrentGraphicsItem); //Remove the image that is displaying
    m_CurrentGraphicsItem->setParentItem(NULL); // Set the parent to NULL
    delete m_CurrentGraphicsItem; // Delete the object
  }

  QImage dataImage = _scaleImage();
  QPixmap imagePixmap;
  if (m_CompositeImages == true && m_OverlayImage.isNull() == false)
  {
    QImage topImage = _scaleImage(m_OverlayImage);
    QPainter painter;
    QImage paintImage(dataImage.size(), QImage::Format_ARGB32_Premultiplied);
    QPoint point(0, 0);
    painter.begin(&paintImage);
    // Draw the fixed Image first
    painter.setPen(Qt::NoPen);
    painter.drawImage(point, topImage );
    // Draw the moving image next
    painter.setCompositionMode(m_composition_mode);
    painter.drawImage(point, dataImage);
    painter.end();
    imagePixmap = QPixmap::fromImage(paintImage);
  }
  else
  {
    imagePixmap = QPixmap::fromImage(dataImage);
  }

  m_CurrentGraphicsItem = m_GraphicsScene->addPixmap(imagePixmap); // Add the new image into the display
  QRectF rect = m_CurrentGraphicsItem->boundingRect();
  m_GraphicsScene->setSceneRect(rect);
  m_GraphicsView->setScene(m_GraphicsScene);
  m_GraphicsView->centerOn(m_CurrentGraphicsItem);
  if (update) {
    m_GraphicsScene->update(rect);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::_displayTextMessage(QString message)
{
  if ( NULL != m_CurrentGraphicsItem )
  {
    m_GraphicsScene->removeItem(m_CurrentGraphicsItem); //Remove the image that is displaying
    m_CurrentGraphicsItem->setParentItem(NULL); // Set the parent to NULL
    delete m_CurrentGraphicsItem; // Delete the object
  }
  m_CurrentGraphicsItem = NULL;
  QGraphicsTextItem* tItem = m_GraphicsScene->addText(message); // Add the new image into the display

  QRectF rect = tItem->boundingRect();
  m_GraphicsScene->setSceneRect(rect);
  m_GraphicsView->setScene(m_GraphicsScene);
  m_GraphicsView->centerOn(tItem);
  m_GraphicsScene->update(rect);
}

