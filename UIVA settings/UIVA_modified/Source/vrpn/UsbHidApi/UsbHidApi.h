/*******************************************************************/
/*                                                                 */
/* File Name:   UsbHidApi.h                                        */
/*                                                                 */
/* Description:                                                    */
/*                                                                 */
/*    This DLL provides a relatively simple method for accessing   */
/*    a USB HID device.  The device must specify HID reports for   */
/*    transferring data.                                           */
/*                                                                 */
/*    Client applications use the exported methods from the        */
/*    CUsbHidApi class to identify and access external             */
/*    HID device(s).  A new instance of the class must             */
/*    be created for each device being accessed.                   */
/*                                                                 */
/*                                                                 */
/* Revision History:                                               */
/*                                                                 */
/*  Name  Ver  Date      Description                               */
/*  ----  ---  ----      -----------                               */
/*  KAD  1.10  05/30/03  Initial version                           */
/*  KAD  1.12  06/19/03  Corrected first-chance exception          */
/*  KAD  1.13  12/09/03  Corrected nag-screen bug                  */
/*  KAD  1.14  02/19/05  Modified calling convention to stdcall    */
/*                       instead of cdecl.                         */
/*                       (Project -> Settings -> C/C++ tab ->      */
/*                        Category : Code Generation)              */
/*  KAD  1.15  03/05/05  Corrected a problem with the module       */
/*                       definition file.                          */
/*  KAD  1.16  03/06/05  Added structure & wrapper functions for   */
/*                       Visual Basic access. Also added library   */
/*                       deactivation after 30 days for shareware  */
/*                       version.                                  */
/*  KAD  1.17  03/12/07  Added interface and collection capability.*/
/*  KAD  1.18  08/28/07  Added pre-compile switch for compatibility*/
/*                       with ANSI C compilers like LabWindows/CVI.*/
/*  KAD  1.18a 09/06/07  Removed ANSI C macro.  Decided to use a   */
/*                       separate header file for ANSI C use.      */
/*  KAD  1.18b 09/19/07  Changed period of registration reminder   */
/*                       pop-ups from 10 minutes to 1 minute.      */
/*******************************************************************/
//
#ifndef __USBHID_API_H__
#define __USBHID_API_H__
//
// Notes:
// -----
// 
// This DLL provides the client application an easy method for accessing
// an HID device via USB link.  The acts of reading and writing 
// a USB device under Windows are significantly different and more
// involved than for other comm devices such as serial comm ports.
// For this reason, it seemed necessary to encapsulate the complexity
// of the interface within a DLL.  This DLL provides all the required
// functions for accessing the device, while hiding the details
// of the implementation.
//
// The USB link is implemented as a Human Interface Device (HID) class
// function.  As such, the DLL is dependent on the following Windows
// drivers:
//
//   hidclass.sys   hidparse.sys    hidusb.sys
//
// In addition to being USB-compliant, the host PC should have the 
// latest, versions of these drivers.  (Initial development was done
// using drivers from Windows 98, 2nd edition.)
//
// The module was built using Microsoft Visual C++, 6.0 as a Win32, 
// non-MFC DLL.  Since there is no dependence on the Microsoft 
// Foundation Class (MFC), there is no need to copy additional MFC 
// DLLs to your Windows system folder.  Also, no dependence on MFC
// means the size of the DLL itself is minimal.  (Although the DLL
// does not internally use the MFC, the client application is not
// restricted.  Exported functions in the DLL may be called by either
// MFC or non-MFC applications.)
//
// The client application may link implicitly or explicitly with the
// DLL.  In explicit linking, the client application makes function 
// calls to explicitly load and unload the DLL's exported functions.
// In implicit linking, the application links to an import library
// (UsbHidApi.lib) and makes function calls just as if the functions 
// were contained within the executable.
//
#define USBHIDAPI_DLL_NAME	"UsbHidApi.dll"

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the USBHIDAPI_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// USBHIDAPI_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef USBHIDAPI_EXPORTS
#define USBHIDAPI_API __declspec(dllexport)
#else
#define USBHIDAPI_API __declspec(dllimport)
#endif

   // This structure defines an entry in a device list
   // The list describes the parameters associated with
   // a device.  It is mainly used with the GetList()
   // function. 
   // modified 3/12/07
   typedef struct {
    char         DeviceName[50];   // Device name
    char         Manufacturer[50]; // Manufacturer
    char         SerialNumber[20]; // Serial number
    unsigned int VendorID;         // Vendor ID
    unsigned int ProductID;        // Product ID
    int          InputReportLen;   // Length of HID input report (bytes)
    int          OutputReportLen;  // Length of HID output report (bytes)
    int          Interface;        // Interface  
    int          Collection;       // Collection 
   } mdeviceList;

   // This structure was created to ease VB access.  It alleviates
   // some of the issues associated with inter-compiler data handling
   // such as alignment.
   // added 3/8/05
#pragma pack (push, 4)
   typedef struct {
    char         *DeviceName;   // Device name
    char         *Manufacturer; // Manufacturer
    char         *SerialNumber; // Serial number
    unsigned int VendorID;         // Vendor ID
    unsigned int ProductID;        // Product ID
    int          InputReportLen;   // Length of HID input report (bytes)
    int          OutputReportLen;  // Length of HID output report (bytes)
    int          Interface;        // Interface  
    int          Collection;       // Collection 
   } mdeviceList2;
#pragma pack (pop,4)

   //////////////////////////////////////////////////////////////////////////////////////////
   // These declarations define special non-member functions for VB access.  
   //////////////////////////////////////////////////////////////////////////////////////////

   extern "C" int _stdcall SetInstance(int instance);          // Set object instance

   extern "C" int _stdcall GetLibVersion(char *buf);           // Get DLL version string

   extern "C" void _stdcall ShowVersion(void);                 // Show a message box containing version

   extern "C" int _stdcall Read(void *pBuf);                   // Read the from the HID device.
   
   extern "C" void  _stdcall CloseRead(void);                  // Close the read pipe

   extern "C" void  _stdcall CloseWrite(void);                 // Close the write pipe

   extern "C" int _stdcall Write(void *pBuf);                  // Write to the HID device

   extern "C" void _stdcall GetReportLengths (int *input_len,   // Pointer for storing input length
                                                int *output_len); // Pointer for storing output length

   extern "C" void _stdcall SetCollection(int);                // Specifies a collection (call prior to Open())  (0xffff = unspecified)

   extern "C" int _stdcall GetCollection();                    // Retrieves collection setting (0xffff = unspecified)

   extern "C" void _stdcall SetInterface(int);                 // Specifies an interface (call prior to Open())  (0xffff = unspecified)

   extern "C" int _stdcall GetInterface();                     // Rerieves interface setting   (0xffff = unspecified)

   extern "C" int _stdcall Open (unsigned int VendorID,	   	// Vendor ID to search     (0xffff if unused)
                                   unsigned int ProductID,	   // Product ID to search    (0xffff if unused)
                                   char         *Manufacturer, // Manufacturer            (NULL if unused)
                                   char         *SerialNum,	   // Serial number to search (0xffff if unused)
                                   char         *DeviceName,	// Device name to search   (NULL if unused)
                                   int          bAsync);		   // Set TRUE for non-blocking read requests.



   extern "C" int _stdcall GetList(unsigned int VendorID,      // Vendor ID to search  (0xffff if unused)
                                     unsigned int ProductID,	   // Product ID to search (0xffff if unused)
                                     char         *Manufacturer, // Manufacturer            (NULL if unused)
                                     char         *SerialNum,    // Serial number to search (NULL if unused)
                                     char         *DeviceName,   // Device name to search   (NULL if unused)
                                     mdeviceList2 *pList,        // Caller's array for storing matching device(s)             
                                     int          nMaxDevices);  // Size of the caller's array list (no.entries)

//////////////////////////////////////////////////////////////////////////////////////////
// This class is exported from the UsbHidApi.dll
class USBHIDAPI_API CUsbHidApi {
public:

   // The serial number for the open device
   char  m_SerialNumber[20];

   // These variables define the required lengths for reading and writing
   // the device.
   unsigned short m_ReadSize;
   unsigned short m_WriteSize;

   // These variables define optional interface and/or collection values search purposes
   // added 3/12/07
   int m_Interface;
   int m_Collection;

   // Constructor
	CUsbHidApi(void);

   // Destructor
   ~CUsbHidApi(void);
	
   // Get list of devices and their availability.  Caller must supply a
   // pointer to a buffer that will hold the list of structure entries.
   // Must also supply an integer representing maximum no. of entries
   // his buffer can hold. Returns total number stored.
   int GetList(unsigned int VendorID,      // Vendor ID to search  (0xffff if unused)
               unsigned int ProductID,	    // Product ID to search (0xffff if unused)
               char         *Manufacturer, // Manufacturer            (NULL if unused)
               char         *SerialNum,    // Serial number to search (NULL if unused)
               char         *DeviceName,   // Device name to search   (NULL if unused)
               mdeviceList  *pList,        // Caller's array for storing matching device(s)             
               int          nMaxDevices);  // Size of the caller's array list (no.entries)

   // Opens a USB comm link to a HID device. Returns non-zero
   // on success or 0 on failure.  (Individual read and write handles
   // are maintained internally.)  If the caller desires to open 
   // a specific HID  device, he must provide one or more
   // specifiers (i.e., vendor ID, product ID, serial number,
   // or device name.  If a successful open occurs, the function
   // returns TRUE.  It returns FALSE otherwise.
   int Open (unsigned int VendorID,	  	  // Vendor ID to search     (0xffff if unused)
             unsigned int ProductID,	  // Product ID to search    (0xffff if unused)
             char         *Manufacturer, // Manufacturer            (NULL if unused)
             char         *SerialNum,	  // Serial number to search (0xffff if unused)
             char         *DeviceName,	  // Device name to search   (NULL if unused)
             int          bAsync);		  // Set TRUE for non-blocking read requests.

   // Sets an optional device interface ID (e.g., 0) for search purposes.
   // This method is used to pin-point a device to be opened.
   // Use this method when a USB device has multiple interfaces.
   // Must be called prior to calling the Open() method.
   // added 3/12/07
   void SetInterface (int iface);        // Interface (-1 if unused)

   // Returns the device interface ID that was set using SetInterface().
   // added 3/12/07
   int GetInterface (void);

   // Sets an optional collection ID (e.g., 0) for search purposes.
   // This method is used to pin-point a device to be opened.
   // Use this method when a USB device has multiple collections.
   // Must be called prior to calling the Open() method.
   // added 3/12/07
   void SetCollection (int col);        // Collection (-1 if unused)

   // Returns the collection ID that was set using SetCollection().
   // added 3/12/07
   int GetCollection (void);

   // Close the read pipe
   void CloseRead(void);

   // Close the write pipe
   void CloseWrite(void);

   // Read the from the HID device.  The number of bytes read is 
   // determined by the input report length specified by the device.
   // Depending on how the device was opened, the call may perform
   // blocking or non-blocking reads.  Refer to Open() for details.
   // On success, the call returns the number of bytes actually read.
   // A negative return value indicates an error (most likely means
   // the USB cable has been disconnected or device was powered off).
   // (Note:  The first byte location is usually a report ID
   // [typically = 0].  The caller must account for this value in 
   // the read buffer.)
   int Read(void *pBuf);                 // Buffer for storing bytes

   // Write a report to the HID device.  The number of bytes to 
   // write depend on the output report length specified by the
   // device.  Returns number of bytes actually written.  A negative
   // return value indicates an error (most likely means the
   // USB cable has been disconnected or device was powered off).
   // (Note:  The first byte location is usually a report ID
   // [typically = 0].  The caller must ensure this value is 
   // prepended to the buffer.)
    int Write(void *pBuf);              // Buffer containing bytes to write

   // This function retrieves the lengths of input- and output-reports
   // for the device.  These values determine the I/O lengths for the
   // Read() and Write() functions.  The device must be opened prior
   // to calling this routine.  Alternatively, you can just use m_ReadSize 
   // and m_WriteSize.
   void GetReportLengths (int *input_len,    // Pointer for storing input length
                          int *output_len);  // Pointer for storing output length

   // This function retrieves the current library version and populates a string
   int GetLibVersion(LPSTR buf);

   // This function displays the current library version in a standard message box
   void ShowVersion(void);


// Private (internal) declarations
private:

   // This function reads an input report from an open device.  
   // The call will block until any number of bytes is retrieved,
   // up to the maximum of nBytesToRead.  On successful completion,
   // the function returns an integer representing the
   // number of bytes read (usually the input report length
   // defined by the device). (Note:  The read buffer must
   // be large enough to accommodate the report length.  This
   // number is located in m_ReadSize.)
   int ReadSync(void *pBuf);              // Buffer for storing bytes

   // Read input report from an open device.  If no data
   // is currently available, the call will not block,
   // but will return 0.    On successful completion,
   // the function returns an integer representing the
   // number of bytes read (usually the input report length
   // defined by the device).  The function returns -1 if 
   // disconnect is detected.  (Note:  The read buffer must
   // be large enough to accommodate the report length.  This
   // number is located in m_ReadSize.)
   int ReadAsync(void *pBuf);              // Buffer for storing bytes

   // Read/write handles.  While a single handle could suffice for
   // both operations, 2 handles have been created to allow the client
   // application to utilize separate threads for reading and writing.
   HANDLE ReadHandle;
   HANDLE WriteHandle;
};

#endif // #ifndef __USBHID_API_H__




