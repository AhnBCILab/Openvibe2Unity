#include "vrpn_Analog_BPack.h"
#include "vrpn_Serial.h"
#include "vrpn_Shared.h"

#undef VERBOSE

//Uncomment this to debug
//#define __DEBUG

#define STATUS_STANDBY (0)
#define STATUS_SENSING (1)

#define	_BPACK_ERROR(msg)	    { send_text_message(msg, timestamp, vrpn_TEXT_ERROR) ; if (d_connection) d_connection->send_pending_reports(); }

//************************************
// Method:    vrpn_BPack
// FullName:  vrpn_BPack::vrpn_BPack
// Access:    private 
// Returns:   None
// Parameter: const char * name <-- name of the device
// Parameter: vrpn_Connection * cxn <-- the connection
// Parameter: const char * port <-- serial port name, e.g "COM5"
// Parameter: int baud <-- baud rate, default to be 9600
// Parameter: int period <-- How many ms does BPack wait to sense a sample
// Parameter: int avgCnt <-- How many samples does BPack accumulate to get 
//							 and average and report
//
// Description: Creates vrpn_Serial_Analog and initiate 3 channels
//************************************
vrpn_BPack::vrpn_BPack(const char *name, vrpn_Connection *cxn, 
					   const char* port, int baud, int period, int avgCnt):
	vrpn_Serial_Analog(name, cxn, port, baud, 8, vrpn_SER_PARITY_NONE), 
	mNumChannels(3), mPeriod(period), mAvgCnt(avgCnt)
{
	// Set the number of channels in the parent class.
	vrpn_Analog::num_channel = mNumChannels;

	if(mPeriod < 5 || mAvgCnt < 4)
	{
		_BPACK_ERROR("vrpn_BPack: constructor: BPack cannot fulfill the sensing task, the sampling is too fast\n");
	}

	// Clear all channels to 0
	clear_values();

	mStatus = STATUS_STANDBY;
}

//************************************
// Method:    clear_values
// FullName:  vrpn_BPack::clear_values
// Access:    virtual protected 
// Returns:   void
// Qualifier: Clear all channels to 0
//************************************
void vrpn_BPack::clear_values()
{
	for(int i = 0; i < mNumChannels; ++i)
	{
		vrpn_Analog::channel[i] = vrpn_Analog::last[i] = 0;
	}
}

//************************************
// Method:    send_command
// FullName:  vrpn_BPack::send_command
// Access:    protected 
// Returns:   int <-- 0 on success, -1 on failure
// Parameter: const unsigned char * cmd <-- command content
// Parameter: int len <-- length of command
//
// Description: Send command to BPack by writing to vrpn_Serial
//************************************
int vrpn_BPack::send_command(const unsigned char *cmd, int len)
{
	// Send the command
	if(vrpn_write_characters(serial_fd, cmd, len) == len)
	{
		return 0;
	}else
	{
		vrpn_flush_output_buffer(serial_fd);
		return -1;
	}
}


//************************************
// Method:    init_sensing
// FullName:  vrpn_BPack::init_sensing
// Access:    protected 
// Returns:   int <-- Return 0 on success, -1 on failure
//
// Description: Initiate sensing by sending a command to BPack
//************************************
int vrpn_BPack::init_sensing()
{
	char cmd_content[128];

	vrpn_flush_input_buffer(serial_fd);
	vrpn_flush_output_buffer(serial_fd);
	// Format the command based on _peroid and _avgCnt
	// BPack is supposed to send data every _peroid * _avgCnt milliseconds
	// "+000000000" means the start time begins with 0
	// The trailing 0 means sensing starts immediately (0 ms waiting)
	sprintf(cmd_content, "sens +000000000 %d %d 0\r\n", mPeriod, mAvgCnt);
	if(send_command((unsigned char *)cmd_content, strlen(cmd_content)) == -1)
	{
		_BPACK_ERROR("vprn_BPack: init_sensing: Unable to send command to BPack\n");
		return -1;
	}

	// Give BPack a short brake to respond
	vrpn_SleepMsecs(100);
	
	// Read 4 characters from the serial port, hope it is "OK\r\n"
	char line[5];
	vrpn_read_available_characters(serial_fd, (unsigned char*)line, 4, NULL);
	line[4] = '\0';		// Append a '\0' to make it a string

	if(strcmp((const char*)"OK\r\n", line) != 0)
	{
		_BPACK_ERROR("vrpn_BPack: init_sensing: BPack sensing failure, not returning OK.\n");
		return -1;
	}

	return 0;
}


//************************************
// Method:    get_report
// FullName:  vrpn_BPack::get_report
// Access:    virtual protected 
// Returns:   int - 0 on success, -1 on no report available
//
// Description: Get the latest report from BPack
//************************************
int vrpn_BPack::get_report()
{
	// Clear the buffer and the counter
	strcpy(mBuffer, "");
	mBufCnt = 0;

	//-----------------------------------------------------
	// Read all characters in the input buffer to mBuffer
	//-----------------------------------------------------
	char temp[1001];
	int cntRead = 0;
	do 
	{
		cntRead = vrpn_read_available_characters(serial_fd, (unsigned char*)temp, 1000);
		temp[cntRead] = '\0';		// Append a '\0' to make it a string
		mBufCnt += cntRead;
		if(mBufCnt >= 10000)
		{
			_BPACK_ERROR("vrpn_BPack: get_report: buffer overflow - 10000 is not enough .\n")
		}
		strcat(mBuffer, temp);
	} while (cntRead == 1000 || temp[cntRead - 1] != '\n');

#ifdef __DEBUG
	printf("-----------Characters available in the buffer----------------\n");
	printf("%s---------Characters available in the buffer---------------\n\n", mBuffer);
#endif

	//-----------------------------------------------------
	// Tokenize mBuffer to get the last line of report
	//-----------------------------------------------------
	char lastLine[100];	//The length of a report line won't exceed 100
	char *nextSeg;
	if(mBufCnt == 0)
	{
		return -1;
	}else	// There are at least one line of report
	{
		nextSeg = strtok(mBuffer, "\n");
		strcpy(lastLine, nextSeg);
		do
		{
			nextSeg = strtok(NULL, "\n");
			if(nextSeg != NULL)
			{
				strcpy(lastLine, nextSeg);
			}
		}while(nextSeg != NULL);
	}

#ifdef __DEBUG
	printf("******** The latest report ********:\n %s\n\n", lastLine);
#endif

	// The coordinate system of BPack is a left hand coordinate system
	// The X axis extends from the letter "w" to "T", when put top surface up
	// The Y axis extends from the LED indicator and down to the letter "w"
	// Please refer to the following sketch-up
	//
	//						^	 ^
	//					 Z	|   / Y
	//						|  /
	//						| /
	//						|/
	//				--------/- - - - - - -/-----------> X 
	//					   / *		     /
	//					  /			    /
	//					 / wireless-T  /
	//					/_ _ _ _ _ _ _/	
	//				   /    |
	//				  /     |
	//
	// The data we want is the acceleration value along these three axes
	
	//----------------------------------------------------------------------------
	// Tokenize the last line of report to get the acceleration data of three axes
	//-----------------------------------------------------------------------------
	// A typical report from BPack is like:
	// sens,,0000019876,12,13,988\r\n
	// 12, 13, and 988 is the acceleration value along the three axes
	
#ifdef __DEBUG
	printf("$$$$$$ Begin tokenization $$$$$$\n");
	printf("%s\n", strtok(lastLine, ",\r\n"));		//Return "sens", skip
	printf("%s\n", strtok(NULL, ",\r\n"));			// Return "0000019876", skip
	float x = atoi(strtok(NULL, ",\r\n"));		// Return "13", convert to float
	float y = atoi(strtok(NULL, ",\r\n"));		// Return "12", convert to float
	float z = atoi(strtok(NULL, ",\r\n"));		// Return "988", convert to float
	printf("x = %f\n", x);			
	printf("y = %f\n", y);			
	printf("z = %f\n", z);
	channel[0] = x;
	channel[1] = y;
	channel[2] = z;
	printf("$$$$$$ End tokenization $$$$$$\n\n");
	printf("Channel outputs: %f\t%f\t%f\n\n\n", channel[0], channel[1], channel[2]);
	printf("<<<<<<<<<<<<<<<<<< END OF THIS RUN >>>>>>>>>>>>>>>>\n\n");
#else
	strtok(lastLine, ",\r\n");	// Return "sens", skip
	strtok(NULL, ",\r\n");		// Return "0000019876", skip
	channel[0] = atoi(strtok(NULL, ",\r\n"));	// Return "12", convert to float 
	channel[1] = atoi(strtok(NULL, ",\r\n"));	// Return "13", convert to float 
	channel[2] = atoi(strtok(NULL, ",\r\n"));	// Return "988", convert to float 
#endif
	
	//Uncomment this line for simple debug, just report the channel values
	//printf("Channel outputs: %f\t%f\t%f\n", channel[0], channel[1], channel[2]);

	vrpn_gettimeofday(&timestamp, NULL);
	// Done with decoding, send the report if needed
	if(channel[0] != oldVals[0] || channel[1] != oldVals[1] || channel[2] != oldVals[2])
	{
		oldVals[0] = channel[0];
		oldVals[1] = channel[1];
		oldVals[2] = channel[2];
		report_changes();
	}
	
	return 0;
}

//************************************
// Method:    report_changes
// FullName:  vrpn_BPack::report_changes
// Access:    virtual protected 
// Returns:   void
// Parameter: vrpn_uint32 class_of_service
//
// Description: Report when changes
//************************************
void vrpn_BPack::report_changes(vrpn_uint32 class_of_service)
{
	vrpn_Analog::timestamp = timestamp;
	vrpn_Analog::report_changes(class_of_service);
}

//************************************
// Method:    report
// FullName:  vrpn_BPack::report
// Access:    virtual protected 
// Returns:   void
// Qualifier:
// Parameter: vrpn_uint32 class_of_service
//
// Description: Report whether changes or not
//************************************
void vrpn_BPack::report(vrpn_uint32 class_of_service)
{
	vrpn_Analog::timestamp = timestamp;
	vrpn_Analog::report(class_of_service);
}


void vrpn_BPack::mainloop()
{
	// Call the mainloop of parent classes
	server_mainloop();

	// Initiate sensing if it hasn't started
	switch(mStatus)
	{
	case STATUS_STANDBY:
		if(init_sensing() == 0)
		{
			mStatus = STATUS_SENSING;
		}
		break;
	case STATUS_SENSING:
		get_report();
		break;
	default:
		_BPACK_ERROR ("vrpn_BPack::mainloop: Unknown mode (internal error)");
		break;
	}
}