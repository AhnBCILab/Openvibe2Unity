#ifndef VRPN_ANALOG_BPACK_H
#define VRPN_ANALOG_BPACK_H

#include "vrpn_Connection.h"
#include "vrpn_Analog.h"

class VRPN_API vrpn_BPack: public vrpn_Serial_Analog
{
public:
	vrpn_BPack(const char * name, vrpn_Connection * cxn,
		const char * port, int baud = 9600, 
		int period = 6, int avgCnt = 5);

	// Called once through each main loop iteration to handle updates
	virtual void mainloop();

protected:// methods
	/// Try to read reports from the device.
	/// Return 1 if msg received, or 0 if none received
	virtual int get_report();
	virtual void clear_values();	///< Clear all channels to 0

	/// Send command to BPack
	/// Return 0 on success, -1 on failure.
	int send_command(const unsigned char *cmd, int len);
	
	/// Send command to BPack to initiate sensing
	/// Return 0 on success, -1 on abnormal state
	int init_sensing();

	/// send report iff changed
	virtual void report_changes
		(vrpn_uint32 class_of_service = vrpn_CONNECTION_LOW_LATENCY);
	/// send report whether or not changed
	virtual void report
		(vrpn_uint32 class_of_service = vrpn_CONNECTION_LOW_LATENCY);

protected:// data
	int mNumChannels;	///< How many analog channels to open

	// Our test showed, by the fastest sample rate,
	// (period = 5ms and avgCnt = 4, a line of report sent to buffer every 20 ms)
	// and suppose the reader is in a very slow rate, 
	// the accumulated characters in the buffer can be no more than 7000.
	// Therefore, we set the length of this string to 10000.
	// However, this is not guaranteed to be a safe solution.
	char mBuffer[10000];	///< Buffer of characters available	
	int mBufCnt;		///< How many characters we actually have in the above buffer

	float oldVals[3];

	/// BPack accumulate sensed data every _period ms and get an average after _avgCnt times
	/// So, it will send a report every _period * _avgCnt milliseconds
	int mPeriod;		///< How often does BPack sense data: every _period millisecond
	int mAvgCnt;		///< BPack accumulate _avgCnt data report an average
	
	int mStatus;	///< If BPack is standby for command to initiate sensing

	struct timeval timestamp;	///< time of last report from device

private:	// disable unwanted default methods
	vrpn_BPack();
	vrpn_BPack(const vrpn_BPack&);
	const vrpn_BPack& operator=(const vrpn_BPack&);
};

#endif