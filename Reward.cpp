/*********************************************************************
*
* ANSI C Example program:
*    WriteDigPort.c
*
* Example Category:
*    DO
*
* Description:
*    This example demonstrates how to write values to a digital
*    output port.
*
* Instructions for Running:
*    1. Select the digital port on the DAQ device to be written.
*    2. Select a value to write.
*    Note: The Data to Write control is in hexadecimal.
*
* Steps:
*    1. Create a task.
*    2. Create a Digital Output channel. Use one channel for all
*       lines. In this case, the port itself acts as an individual
*       channel.
*    3. Call the Start function to start the task.
*    4. Write digital port data. This write function writes a single
*       sample of digital data on demand, so no timeout is necessary.
*    5. Call the Clear Task function to clear the Task.
*    6. Display an error if any.
*
* I/O Connections Overview:
*    Make sure your signal output terminals match the Port I/O
*    Control. In this case wire the item to receive the signal to the
*    first N digital lines on your DAQ Device.
*
*********************************************************************/
#include <stdio.h>
#include "NIDAQmx.h"

#if _MSC_VER // this is defined when compiling with Visual Studio
#define EXPORT_API __declspec(dllexport) // Visual Studio needs annotating exported functions with this
#else
#define EXPORT_API // XCode does not need annotating exported functions, so define is empty
#endif

// ------------------------------------------------------------------------
// Plugin itself

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

// Link following functions C-style (required for plugins)


extern "C"
{
	int EXPORT_API reward(bool on)
	{
		int         error = 0;
		TaskHandle	taskHandle = 0;
		uInt32      data = 0xffffffff;  // large number, not zero!!!
		char        errBuff[2048] = { '\0' };
		int32		written;
		
		if (!on) {
			data = 0;
		}

		/*********************************************/
		// DAQmx Configure Code
		/*********************************************/
		DAQmxErrChk(DAQmxCreateTask("", &taskHandle));
		DAQmxErrChk(DAQmxCreateDOChan(taskHandle, "Dev1/port0", "", DAQmx_Val_ChanForAllLines));

		/*********************************************/
		// DAQmx Start Code
		/*********************************************/
		DAQmxErrChk(DAQmxStartTask(taskHandle));

		/*********************************************/
		// DAQmx Write Code
		/*********************************************/
		DAQmxErrChk(DAQmxWriteDigitalU32(taskHandle, 1, 1, 10.0, DAQmx_Val_GroupByChannel, &data, &written, NULL));

		return 1;
	Error:
		if (DAQmxFailed(error))
			DAQmxGetExtendedErrorInfo(errBuff, 2048);
		if (taskHandle != 0) {
			/*********************************************/
			// DAQmx Stop Code
			/*********************************************/
			DAQmxStopTask(taskHandle);
			DAQmxClearTask(taskHandle);
		}
		if (DAQmxFailed(error))
			printf("DAQmx Error: %s\n", errBuff);
		printf("End of program, press Enter key to quit\n");
		getchar();
		return 0;
	}

// The functions we will call from Unity.
//
const EXPORT_API char*  PrintHello(){
	return "Hello";
}

int EXPORT_API AddTwoIntegers(int a, int b) {
	return a + b;
}

float EXPORT_API AddTwoFloats(float a, float b) {
	return a + b;
}

} // end of export C block
