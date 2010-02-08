///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _QEMMPM_GUI_H_
#define _QEMMPM_GUI_H_

//-- Qt Includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QResizeEvent>

//-- UIC generated Header
#include <ui_QEmMpm.h>

#include "EmMpm/Common/AIMImage.h"
#include "EmMpmThread.h"
#include "EmMpm/Common/Qt/AIMImageGraphicsDelegate.h"

/**
* @class QEmMpm QEmMpm.h SHP/GUI/QEmMpm.h
* @brief Main class that drives the GUI elements.
* @author Michael A. Jackson for BlueQuartz Software
* @date Dec 23, 2009
* @version 1.0
*/
class QEmMpm: public QMainWindow, private Ui::QEmMpm
{
  Q_OBJECT;

  public:
    QEmMpm(QWidget *parent = 0);
    virtual ~QEmMpm();

    void initWithFile(const QString imageFile, QString mountImage);

    AIMImage::Pointer readTiffFile(QString filename,
                                                  bool asGrayscale,
                                                  bool blackIsZero );

    MXA_INSTANCE_PROPERTY_m(AIMImage::Pointer, OriginalImage)
    MXA_INSTANCE_PROPERTY_m(AIMImage::Pointer, SegmentedImage)

    MXA_INSTANCE_PROPERTY_m(QString, CurrentImageFile)
    MXA_INSTANCE_PROPERTY_m(QString, CurrentSegmentedFile)

    qint32 saveSegmentedImage();

  signals:
    void cancelTask();
    void parentResized();

  protected slots:
  //Manual Hookup Menu Actions
    void on_actionOpen_triggered(); // Open a Data File
    void on_actionOpen_Segmented_Image_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionClose_triggered();
    void on_actionExit_triggered();

    /* slots for the buttons in the GUI */
    void on_m_SegmentBtn_clicked();
    void on_aboutBtn_clicked();

    void on_compositeWithOriginal_stateChanged(int state);
    void on_modeComboBox_currentIndexChanged();


    /* Slots to receive events from the Encoder task */
    void receiveTaskMessage(const QString &msg);
    void receiveTaskProgress(int p);
    void receiveTaskFinished();

    /**
     * @brief Updates the QMenu 'Recent Files' with the latest list of files. This
     * should be connected to the Signal QRecentFileList->fileListChanged
     * @param file The newly added file.
     */
    void updateRecentFileList(const QString &file);

    /**
     * @brief Qt Slot that fires in response to a click on a "Recent File' Menu entry.
     */
    void openRecentFile();

  protected:

    /**
     * @brief Implements the CloseEvent to Quit the application and write settings
     * to the preference file
     */
    void closeEvent(QCloseEvent *event);

    /**
     * @brief Drag and drop implementation
     */
    void dragEnterEvent(QDragEnterEvent*);

    /**
     * @brief Drag and drop implementation
     */
    void dropEvent(QDropEvent*);

    /**
     * @brief Enables or Disables all the widgets in a list
     * @param b
     */
    void setWidgetListEnabled(bool b);

    /**
     * @brief Verifies that a path exists on the file system.
     * @param outFilePath The file path to check
     * @param lineEdit The QLineEdit object to modify visuals of (Usually by placing a red line around the QLineEdit widget)
     */
    bool _verifyPathExists(QString outFilePath, QLineEdit* lineEdit);

    /**
     * @brief Verifies that a parent path exists on the file system.
     * @param outFilePath The parent file path to check
     * @param lineEdit The QLineEdit object to modify visuals of (Usually by placing a red line around the QLineEdit widget)
     */
    bool _verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit);

    /**
     * @brief Reads the Preferences from the users pref file
     */
    void readSettings();

    /**
     * @brief Writes the preferences to the users pref file
     */
    void writeSettings();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    void setupGui();

    /**
     * @brief Checks the currently open file for changes that need to be saved
     * @return
     */
    qint32 _checkDirtyDocument();

    void resizeEvent ( QResizeEvent * event );

     /**
      * @brief Opens an Image file
      * @param imageFile The path to the image file to open.
      */
     void _openFile(QString &imageFile);

     void _openSegmentedImage(QString mountImage);

     AIMImage::Pointer convertQImageToGrayScaleAIMImage(QImage image);

     qint32 initGraphicViews();


  private:

    QString m_OpenDialogLastDirectory;
    QList<QWidget*> m_WidgetList;

    EmMpmThread*      m_EmMpmThread;
    bool            m_OutputExistsCheck;

    QGraphicsScene*             m_OriginalImageGScene;
    QGraphicsScene*             m_SegmentedImageGScene;

    AIMImageGraphicsDelegate*      m_OriginalGDelegate;
    AIMImageGraphicsDelegate*      m_SegmentedGDelegate;

    QEmMpm(const QEmMpm&); // Copy Constructor Not Implemented
    void operator=(const QEmMpm&); // Operator '=' Not Implemented
};

#endif /* _QEMMPM_GUI_H_ */
