/*
 * Create4Care - Erasmus MC, Rotterdam © 2020 <create4care@erasmusmc.nl>
 *
 * Copyright 2020 Bastiaan Teeuwen <bastiaan@mkcl.nl>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <videobackend/opencv.h>
#include <videobackend/deheng.h>

/* TODO implement threading to run the aquisition on a separate thread */

OpenCVVideoDevice::OpenCVVideoDevice(int n):
	m_pobjAcqThread(NULL),
    m_hDevice(NULL),
    m_pstBaseInfo(NULL),
    m_ui32DeviceNum(0),
    m_bOpen(false),
    m_bAcquisitionStart(false)
{
	name = tr("Device ") + QString::number(n);
	index = n;
	
	
	// Init GxiApi libary
    GX_STATUS emStatus = GX_STATUS_SUCCESS;
    emStatus = GXInitLib();
    if (emStatus != GX_STATUS_SUCCESS)
    {
        ShowErrorString(emStatus);
    }
}

bool CGxViewer::SetAcquisitionBufferNum()
{
    GX_STATUS emStatus = GX_STATUS_SUCCESS;
    uint64_t ui64BufferNum = 0;
    int64_t i64PayloadSize = 0;

    // Get device current payload size
    emStatus = GXGetInt(m_hDevice, GX_INT_PAYLOAD_SIZE, &i64PayloadSize);
    if (emStatus != GX_STATUS_SUCCESS)
    {
        ShowErrorString(emStatus);
        return false;
    }

    // Set buffer quantity of acquisition queue
    if (i64PayloadSize == 0)
    {
        QMessageBox::about(this, "Set Buffer Number", "Set acquisiton buffer number failed : Payload size is 0 !");
        return false;
    }

    // Calculate a reasonable number of Buffers for different payload size
    // Small ROI and high frame rate will requires more acquisition Buffer
    const size_t MAX_MEMORY_SIZE = 8 * 1024 * 1024; // The maximum number of memory bytes available for allocating frame Buffer
    const size_t MIN_BUFFER_NUM  = 5;               // Minimum frame Buffer number
    const size_t MAX_BUFFER_NUM  = 450;             // Maximum frame Buffer number
    ui64BufferNum = MAX_MEMORY_SIZE / i64PayloadSize;
    ui64BufferNum = (ui64BufferNum <= MIN_BUFFER_NUM) ? MIN_BUFFER_NUM : ui64BufferNum;
    ui64BufferNum = (ui64BufferNum >= MAX_BUFFER_NUM) ? MAX_BUFFER_NUM : ui64BufferNum;

    emStatus = GXSetAcqusitionBufferNumber(m_hDevice, ui64BufferNum);
    if (emStatus != GX_STATUS_SUCCESS)
    {
        ShowErrorString(emStatus);
        return false;
    }

    // Transfer buffer number to acquisition thread class for using GXDQAllBufs
    m_pobjAcqThread->m_ui64AcquisitionBufferNum = ui64BufferNum;

    return true;
}

int OpenCVVideoDevice::open()
{
    // Open Device
    GX_STATUS emStatus = GX_STATUS_SUCCESS;
    emStatus = GXOpenDeviceByIndex(ui->DeviceList->currentIndex() + 1, &m_hDevice);
    GX_VERIFY(emStatus);

    // isOpen flag set true
    m_bOpen = true;

    // Do not init device or get init params when open failed
    if (!m_bOpen)
    {
        return;
    }

    // Setup acquisition thread
        // if Acquisition thread is on Stop acquisition thread
    if (m_pobjAcqThread != NULL)
    {
        m_pobjAcqThread->m_bAcquisitionThreadFlag = false;
        m_pobjAcqThread->quit();
        m_pobjAcqThread->wait();

        // Release acquisition thread object
        RELEASE_ALLOC_MEM(m_pobjAcqThread);
    }

    // Instantiation acquisition thread
    try
    {
        m_pobjAcqThread = new CAcquisitionThread;
    }
    catch (std::bad_alloc &e)
    {
        QMessageBox::about(NULL, "Allocate memory error", "Cannot allocate memory, please exit this app!");
        RELEASE_ALLOC_MEM(m_pobjAcqThread);
        return;
    }

    // Connect error signal and error handler
    connect(m_pobjAcqThread, SIGNAL(SigAcquisitionError(QString)), this,
            SLOT(slotAcquisitionErrorHandler(QString)), Qt::QueuedConnection);
    connect(m_pobjAcqThread, SIGNAL(SigImageProcError(VxInt32)), this,
            SLOT(slotImageProcErrorHandler(VxInt32)), Qt::QueuedConnection);


    // Transfer Device handle to acquisition thread class
    m_pobjAcqThread->GetDeviceHandle(m_hDevice);

    // Get MainWindow param from device
    GetDeviceInitParam();

    // Update Mainwindow
    UpdateUI();

    bool bSetDone = false;
    // Set acquisition buffer number
    bSetDone = SetAcquisitionBufferNum();
    if (!bSetDone)
    {
        return;
    }

    bool bPrepareDone = false;
    // Alloc resource for image acquisition
    bPrepareDone = m_pobjAcqThread->PrepareForShowImg();
    if (!bPrepareDone)
    {
        return;
    }

    // Device start acquisition and start acquisition thread
    GX_STATUS emStatus = GX_STATUS_SUCCESS;

    emStatus = GXStreamOn(m_hDevice);
    GX_VERIFY(emStatus);

    // Set acquisition thread run flag
    m_pobjAcqThread->m_bAcquisitionThreadFlag = true;

    // Acquisition thread start
    m_pobjAcqThread->start();

    // isStart flag set true
    m_bAcquisitionStart = true;

    // Do not start timer when acquisition start failed
    if (!m_bAcquisitionStart)
    {
        return;
    }

    // Start image showing timer(Image show frame rate = 1000/nShowTimerInterval)
    // Refresh interval 33ms
    const int nShowTimerInterval = 33;
    m_pobjShowImgTimer->start(nShowTimerInterval);

    // Refresh interval 500ms
    const int nFrameRateTimerInterval = 500;
    m_pobjShowFrameRateTimer->start(nFrameRateTimerInterval);

    // Update Mainwindow
    UpdateUI();

	return 1;
}



void OpenCVVideoDevice::close()
{
    GX_STATUS emStatus = GX_STATUS_SUCCESS;

    // Stop Timer
    m_pobjShowImgTimer->stop();
    m_pobjShowFrameRateTimer->stop();

    // Stop acquisition thread before close device if acquisition did not stoped
    if (m_bAcquisitionStart)
    {
        m_pobjAcqThread->m_bAcquisitionThreadFlag = false;
        m_pobjAcqThread->quit();
        m_pobjAcqThread->wait();

        // isStart flag reset
        m_bAcquisitionStart = false;
    }

    // Release acquisition thread object
    RELEASE_ALLOC_MEM(m_pobjAcqThread);

    //Close Device
    emStatus = GXCloseDevice(m_hDevice);
    GX_VERIFY(emStatus);

    // isOpen flag reset
    m_bOpen = false;

    // release device handle
    m_hDevice = NULL;

    // Update Mainwindow
    UpdateUI();

    return;
}

bool OpenCVVideoDevice::isOpen()
{
	return m_bOpen;
}

void OpenCVVideoDevice::loadFrame()
{
	if (isOpen())
		videocap >> image;
}

OpenCVVideoDeviceManager::OpenCVVideoDeviceManager()
{
	//First analyse the library
	
	GX_STATUS emStatus = GX_STATUS_SUCCESS;
	
	    if (emStatus != GX_STATUS_SUCCESS)
    {
        ShowErrorString(emStatus);
    }
	
	RELEASE_ALLOC_ARR(m_pstBaseInfo);
	
	emStatus = GXUpdateDeviceList(&m_ui32DeviceNum, ENUMRATE_TIME_OUT);
    GX_VERIFY(emStatus);
	
	// If avalible devices enumerated, get base info of enumerate devices
    if(m_ui32DeviceNum > 0)
    {
        // Alloc resourses for device baseinfo
        try
        {
            m_pstBaseInfo = new GX_DEVICE_BASE_INFO[m_ui32DeviceNum];
        }
        catch (std::bad_alloc &e)
        {
            QMessageBox::about(NULL, "Allocate memory error", "Cannot allocate memory, please exit this app!");
            RELEASE_ALLOC_MEM(m_pstBaseInfo);
            return;
        }
        // Set size of function "GXGetAllDeviceBaseInfo"
        size_t nSize = m_ui32DeviceNum * sizeof(GX_DEVICE_BASE_INFO);

        // Get all device baseinfo
        emStatus = GXGetAllDeviceBaseInfo(m_pstBaseInfo, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            RELEASE_ALLOC_ARR(m_pstBaseInfo);
            ShowErrorString(emStatus);

            // Reset device number
            m_ui32DeviceNum = 0;

            return;
        }
    }
	
	else{
		//No devices
	}
	
	
	
	// VideoCapture videocap;

	// for (int i = 0;; i++) {
		// videocap.open(i);

		// if (!videocap.isOpened())
			// break;

		// devices.append((VideoDevice *) new OpenCVVideoDevice(i));

		// videocap.release();
	// }
}

QList<VideoDevice *> OpenCVVideoDeviceManager::enumerate()
{
	return devices;
}