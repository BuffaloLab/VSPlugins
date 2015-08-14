/*********************************************************************
*
* ANSI C Example program:
*    ContAcq-IntClk-EveryNSamplesEvent.c
*
* Example Category:
*    Events
*
* Description:
*    This example demonstrates how to use Every N Samples events to
*    acquire a continuous amount of data using the DAQ device's
*    internal clock. The Every N Samples events indicate when data is
*    available from DAQmx.
*
* Instructions for Running:
*    1. Select the physical channel to correspond to where your
*       signal is input on the DAQ device.
*    2. Enter the minimum and maximum voltage range.
*    Note: For better accuracy try to match the input range to the
*          expected voltage level of the measured signal.
*    3. Set the rate of the acquisition. Also set the Samples per
*       Channel control. This will determine how many samples are
*       read each time. This also determines how many points are
*       plotted on the graph each iteration.
*    Note: The rate should be at least twice as fast as the maximum
*          frequency component of the signal being acquired.
*
* Steps:
*    1. Create a task.
*    2. Create an analog input voltage channel.
*    3. Set the rate for the sample clock. Additionally, define the
*       sample mode to be continuous.
*    4. Register a callback to receive the Every N Samples event. An
*       Every N Samples event occurs each time the specified number
*       of samples are transferred from the device to the DAQmx
*       driver. The callback contains code to read the specified
*       number of samples from the DAQmx driver.
*    5. Call the Start function to start the acquistion.
*    6. Receive Every N Samples events until the stop button is
*       pressed or an error occurs.
*    7. Call the Clear Task function to clear the task.
*    8. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal input terminal matches the Physical
*    Channel I/O control. For further connection information, refer
*    to your hardware reference manual.
*
*********************************************************************/

#include <stdio.h>
#include <NIDAQmx.h>

#if _MSC_VER // this is defined when compiling with Visual Studio
#define EXPORT_API __declspec(dllexport) // Visual Studio needs annotating exported functions with this
#else
#define EXPORT_API // XCode does not need annotating exported functions, so define is empty
#endif

// ------------------------------------------------------------------------
// Plugin itself

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

extern "C"
{

	//typedef void(*UnityCallback)(float64);
	//UnityCallback Callback;
	typedef int32(*UnityCallback)(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
	//int32 CVICALLBACK EveryNSamplesCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
	int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);


	void Cleanup(void);
	//void set_eog_callback(void(*UnityCallback));
	static TaskHandle	taskHandle = 0;
	static float64		data[1];
	static int32		totalRead = 0;
	static int32		numSamples = 1;
	
	int EXPORT_API eog_start_task(void)
	{
		int32       error = 0;
		char        errBuff[2048] = { '\0' };

		/*********************************************/
		// DAQmx Configure Code
		/*********************************************/
		DAQmxErrChk(DAQmxCreateTask("", &taskHandle));
		DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, "Dev1/ai3", "", DAQmx_Val_Cfg_Default, -5.0, 5.0, DAQmx_Val_Volts, NULL));
		DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", 240.0, DAQmx_Val_Rising, DAQmx_Val_ContSamps, numSamples));
		return DAQmx_Val_GroupByChannel;
		//return 1;

	Error:
		if (DAQmxFailed(error))
		{
			DAQmxGetExtendedErrorInfo(errBuff, 2048);
			Cleanup();
			printf("DAQmx Error: %s\n", errBuff);
		}
		return 0;
	}

	int EXPORT_API eog_set_callback(UnityCallback uCallback)
	{
		int32       error = 0;
		char        errBuff[2048] = { '\0' };
		int32       read;
		float64		data[1];

		if (uCallback) 
		{
			//uCallback(5.0f);
			DAQmxErrChk(DAQmxRegisterEveryNSamplesEvent(taskHandle, DAQmx_Val_Acquired_Into_Buffer, 1, 0, uCallback, NULL));
			// DAQmxErrChk(DAQmxRegisterEveryNSamplesEvent(taskHandle, DAQmx_Val_Acquired_Into_Buffer, 1000, 0, EveryNSamplesCallback, NULL));
			//uCallback(6.0f);
			
			DAQmxErrChk(DAQmxRegisterDoneEvent(taskHandle, 0, DoneCallback, NULL));
			/*********************************************/
			// DAQmx Start Code
			/*********************************************/
			DAQmxErrChk(DAQmxStartTask(taskHandle));

			//DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, 1, 10.0, DAQmx_Val_GroupByChannel, data, 1, &read, NULL));
			//uCallback(7.0f);
			//uCallback(data[0]);
			return 1;
		}
	Error:
		if (DAQmxFailed(error))
		{
			DAQmxGetExtendedErrorInfo(errBuff, 2048);
			Cleanup();
			printf("DAQmx Error: %s\n", errBuff);
		}

		return 0;
	}

	int32 CVICALLBACK EveryNSamplesCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
	{
		int32       error = 0;
		int32       read;
		char        errBuff[2048] = { '\0' };

		/*********************************************/
		// DAQmx Read Code
		/*********************************************/

		//uCallback(9.0f);
		DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, numSamples, 10.0, DAQmx_Val_GroupByScanNumber, data, 1, &read, NULL));
		// DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, 1000, 10.0, DAQmx_Val_GroupByScanNumber, data, 1000, &read, NULL));

		// now call the callback we have set and send the data
		if (read > 0) {
			//UnityCallback(8.0f);
			//UnityCallback(read);
			//UnityCallback(data);
			return 1;
			//printf("Acquired %d samples. Total %d\r", (int)read, (int)(totalRead += read));
			//fflush(stdout);
		}
		else {
			//UnityCallback(9.0f);
			return 2;
		}

	Error:
		if (DAQmxFailed(error))
		{
			DAQmxGetExtendedErrorInfo(errBuff, 2048);
			Cleanup();
			printf("DAQmx Error: %s\n", errBuff);
		}
		return 0;
	}

	int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
	{
		int32   error = 0;
		char    errBuff[2048] = { '\0' };

		UnityCallback(2);
		// Check to see if an error stopped the task.
		DAQmxErrChk(status);

	Error:
		if (DAQmxFailed(error)) {
			DAQmxGetExtendedErrorInfo(errBuff, 2048);
			DAQmxClearTask(taskHandle);
			printf("DAQmx Error: %s\n", errBuff);
		}
		return 0;
	}

	int EXPORT_API eog_stop_task(void)
	{
		int32       error = 0;
		char        errBuff[2048] = { '\0' };

		DAQmxErrChk(DAQmxStopTask(taskHandle));
		DAQmxClearTask(taskHandle);
		return 1;

	Error:

		if (DAQmxFailed(error))
		{
			DAQmxGetExtendedErrorInfo(errBuff, 2048);
			Cleanup();
			printf("DAQmx Error: %s\n", errBuff);
		}

		return 0;
	}

	void Cleanup(void)
	{
		if (taskHandle != 0)
		{
			/*********************************************/
			// DAQmx Stop Code
			/*********************************************/
			DAQmxStopTask(taskHandle);
			DAQmxClearTask(taskHandle);
			taskHandle = 0;
		}
	}
}