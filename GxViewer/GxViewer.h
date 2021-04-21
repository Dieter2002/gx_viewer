//--------------------------------------------------------------------------------
/**
\file     GxViewer.h
\brief    CGxViewer Class declaration file

\version  v1.0.1807.9271
\date     2018-07-27

<p>Copyright (c) 2017-2018</p>
*/
//----------------------------------------------------------------------------------
#ifndef GXVIEWER_H
#define GXVIEWER_H

#include "Common.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QPixmap>
#include <deque>

#include "Roi.h"
#include "FrameRateControl.h"
#include "ExposureGain.h"
#include "WhiteBalance.h"
#include "ImageImprovement.h"
#include "UserSetControl.h"
#include "AcquisitionThread.h"

#define ENUMRATE_TIME_OUT       200

namespace Ui {
    class CGxViewer;
}

class CGxViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit CGxViewer(QWidget *parent = 0);
    ~CGxViewer();

private:
    /// Set the icon for the sample program
    void __SetSystemIcon();

    /// Set Keyboard ShortCut
    void __SetKeyboardShortCut();

    /// Clear Mainwindow items
    void ClearUI();

    /// Enable all UI Groups
    void EnableUI();

    /// Disable all UI Groups
    void DisableUI();

    /// Update all items status on MainWindow
    void UpdateUI();

    /// Update device list
    void UpdateDeviceList();

    /// Setup acquisition thread
    void SetUpAcquisitionThread();
    void SetUpAcquisitionThread2();

    /// Setup all dialogs
    void SetUpDialogs();

    /// Open dialog
    void OpenDialog(QDialog*);

    /// Close all Dialogs
    void CloseDialogs();

    /// Destroy all dialogs
    void DestroyDialogs();

    /// Open device selected
    void OpenDevice();
    void OpenDevice2();

    /// Close device opened
    void CloseDevice();
    void CloseDevice2();

    /// Get device info and show it on text label
    void ShowDeviceInfo();
    void ShowDeviceInfo2();

    /// Set device acquisition buffer number.
    bool SetAcquisitionBufferNum();
    bool SetAcquisitionBufferNum2();

    /// Get parameters from opened device
    void GetDeviceInitParam();
    void GetDeviceInitParam2();

    /// Check if MultiROI is on
    bool CheckMultiROIOn();
    bool CheckMultiROIOn2();

    /// Device start acquisition and start acquisition thread
    void StartAcquisition();
    void StartAcquisition2();

    /// Device stop acquisition and stop acquisition thread
    void StopAcquisition();
    void StopAcquisition2();

    /// Get image show frame rate
    double GetImageShowFps();

    Ui::CGxViewer       *ui;                        ///< User Interface

    CAcquisitionThread  *m_pobjAcqThread;           ///< Child image acquisition and process thread

    CRoi                *m_pobjROISettings;         ///< ROI Setting dialog
    CFrameRateControl   *m_pobjFrameRateControl;    ///< Frame rate control dialog
    CExposureGain       *m_pobjExposureGain;        ///< ExposureGain param adjust dialog
    CWhiteBalance       *m_pobjWhiteBalance;        ///< WhiteBalance param adjust dialog
    CImageImprovement   *m_pobjImgProc;             ///< ImageImprovement plugin dialog
    CUserSetControl     *m_pobjUserSetCtl;          ///< UserSetControl save and load dialog

    GX_DEV_HANDLE        m_hDevice;                 ///< Device Handle
    GX_DEVICE_BASE_INFO *m_pstBaseInfo;             ///< Pointer struct of Device info
    uint32_t             m_ui32DeviceNum;           ///< Device number enumerated

    bool                 m_bOpen;                   ///< Flag : camera is opened or not
    bool                 m_bAcquisitionStart;       ///< Flag : camera is acquiring or not
    bool                 m_bTriggerModeOn;          ///< Flag : Trigger mode is on or not
    bool                 m_bSoftTriggerOn;          ///< Flag : Trigger software is on or not
    bool                 m_bColorFilter;            ///< Flag : Support color pixel format or not
    bool                 m_bSaveImage;              ///< Flag : Save one image when it is true

    QImage               m_objImageForSave;         ///< For image saving
    CFps                 m_objFps;                  ///< Calculated image display fps
    uint32_t             m_ui32ShowCount;           ///< Frame count of image show
    double               m_dImgShowFrameRate;       ///< Framerate of image show

    QTimer              *m_pobjShowImgTimer;        ///< Timer of Show Image
    QTimer              *m_pobjShowFrameRateTimer;  ///< Timer of show Framerate

    //New camera (2)

    CAcquisitionThread  *m_pobjAcqThread2;           ///< Child image acquisition and process thread

    CRoi                *m_pobjROISettings2;         ///< ROI Setting dialog
    CFrameRateControl   *m_pobjFrameRateControl2;    ///< Frame rate control dialog
    CExposureGain       *m_pobjExposureGain2;        ///< ExposureGain param adjust dialog
    CWhiteBalance       *m_pobjWhiteBalance2;        ///< WhiteBalance param adjust dialog
    CImageImprovement   *m_pobjImgProc2;             ///< ImageImprovement plugin dialog
    CUserSetControl     *m_pobjUserSetCtl2;          ///< UserSetControl save and load dialog

    GX_DEV_HANDLE        m_hDevice2;                 ///< Device Handle
    GX_DEVICE_BASE_INFO *m_pstBaseInfo2;             ///< Pointer struct of Device info
    uint32_t             m_ui32DeviceNum2;           ///< Device number enumerated

    bool                 m_bOpen2;                   ///< Flag : camera is opened or not
    bool                 m_bAcquisitionStart2;       ///< Flag : camera is acquiring or not
    bool                 m_bTriggerModeOn2;          ///< Flag : Trigger mode is on or not
    bool                 m_bSoftTriggerOn2;          ///< Flag : Trigger software is on or not
    bool                 m_bColorFilter2;            ///< Flag : Support color pixel format or not
    bool                 m_bSaveImage2;              ///< Flag : Save one image when it is true

    QImage               m_objImageForSave2;         ///< For image saving
    CFps                 m_objFps2;                  ///< Calculated image display fps
    uint32_t             m_ui32ShowCount2;           ///< Frame count of image show
    double               m_dImgShowFrameRate2;       ///< Framerate of image show

    QTimer              *m_pobjShowImgTimer2;        ///< Timer of Show Image
    QTimer              *m_pobjShowFrameRateTimer2;  ///< Timer of show Framerate

private slots:
    /// Open SaveImage dialog
    void on_actionSaveImage_triggered();

    /// Save image to customize dir
    void slotSaveImageFile();

    /// Update device list
    void on_UpdateDeviceList_clicked();

    /// Open device selected
    void on_OpenDevice_clicked();
    void on_OpenDevice2_clicked();

    /// Close device opened
    void on_CloseDevice_clicked();
    void on_CloseDevice2_clicked();

    /// Start Acqusition
    void on_StartAcquisition_clicked();
    void on_StartAcquisition2_clicked();

    /// Stop Acquisition
    void on_StopAcquisition_clicked();
    void on_StopAcquisition2_clicked();

    /// Show images acquired and processed
    void slotShowImage();
    void slotShowImage2();


    /// Set TriggerMode
    void on_TriggerMode_activated(int);

    /// Send a software trigger
    void on_TriggerSoftWare_clicked();

    /// Set TriggerSource
    void on_TriggerSource_activated(int);

    /// Get acquisition frame count and frame rate from acquisition-thread
    void slotShowFrameRate();

    /// Show library version and demo version
    void on_actionAbout_triggered();

    /// Get error from acquisition thread and show error message
    void slotAcquisitionErrorHandler(QString);

    /// Get error from image processing and show error message
    void slotImageProcErrorHandler(VxInt32);

    /// Refresh Main window when execute usersetload
    void slotRefreshMainWindow();

signals:
    /// Emit save image signal
    void SigSaveImage();
};

#endif // GXVIEWERDEMO_H
