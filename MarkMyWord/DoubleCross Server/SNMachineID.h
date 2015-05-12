/////////////////////////////////////////////////////////////////////////////////////////
//
// SNMachineID.h
//
// Copyright (c) World Opponent Network 1998, All rights reserved
//
// Initial coding by Pete Isensee, 09/16/98
//
// References:
//
// Description:
//
// Wrapper for structure containing machine-specific information used for banning.
// SNMachineID is used in two different ways.  1) Two IDs may be compared to determine
// if they are the same.  2) SNMachineIDs are passed to the server (binary block)
//
// This class is a cross-platform wrapper that hides the specific implementation
// of a machine key.

#if !defined(SN_MACHINEID_H)
#define SN_MACHINEID_H

/////////////////////////////////////////////////////////////////////////////////////////

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID  
	{ 
    unsigned long	Data1; 
    unsigned short  Data2; 
    unsigned short  Data3; 
    unsigned char   Data4[8]; 
	}
	GUID;
#endif

// need to move this
#ifndef DWORD 
typedef unsigned long DWORD;
#endif
#ifndef BOOL
typedef int BOOL;
#endif

typedef unsigned long ULONG;
typedef unsigned char BYTE;
#define TRUE 1
#define FALSE 0

// Maximum expected machine ID length
// added for server compatibility
#ifndef MACHINE_ID_LEN
#define MACHINE_ID_LEN 60
#endif

const ULONG SizeOfPC1 = 52;

const DWORD dwPC1 = 0x50432031; // 'PC 1'
const DWORD dwMAC1 = 0x4D616331; // 'Mac1'

/////////////////////////////////////////////////////////////////////////////////////////

// Initial implementation of a machine ID for Windows.
//
// Must be a simple class (no complex data, no constructors or virtual functions) 
// because included as a member of a union

class MachineIDPCRev1
	{
private:

	GUID  mVisibleGuid;		// 16 bytes; stored at HKLM\SOFTWARE\WON\MachineID
	GUID  mHiddenGuid;		// 16 bytes; stored at HKLM\SOFTWARE\Microsoft\Windows\ ...
							//			 CurrentVersion\Internet Settings\OwnerProxy
	DWORD mVolumeID;		// returned from GetVolumeInformation()
	DWORD mCPUID;			// hash of asm CPUID signature and feature flags
	DWORD mIPAddress;		// IP address (only high bits are significant)

public:

	BOOL operator == (const MachineIDPCRev1&) const;

	// Accessors
	GUID  GetVisibleGuid()	const;
	GUID  GetHiddenGuid()	const;
	DWORD GetVolumeID()		const;
	DWORD GetCPUID()		const;
	DWORD GetIPAddress()	const;

	void SetVisibleGuid	(const GUID& theVisibleGuid);
	void SetHiddenGuid	(const GUID& theHiddenGuid);
	void SetVolumeID	(DWORD theVolumeID);
	void SetCPUID		(DWORD theCPUID);
	void SetIPAddress	(DWORD theIPAddress);

	};

/////////////////////////////////////////////////////////////////////////////////////////

// Initial implementation of a machine ID for Macintosh.
//
// *** not currently used ***
// *** designed to test functionality of anonymous union ***
//
// Must be a simple struct (no complex data, no constructors or virtual functions) 
// because included as a member of a union

struct MachineIDMacRev1
	{
private:

	GUID  mVisibleGuid;		// 16 bytes; stored in WON preferences file
	GUID  mHiddenGuid;		// 16 bytes; stored at system preferences file
	DWORD mGestaltInfo;		// hash of Gestalt information on system
	DWORD mIPAddress;		// IP address (only high bits are significant)

public:

	BOOL operator == (const MachineIDMacRev1& theMacRev1) const;

	// Accessors
	GUID  GetVisibleGuid()	const;
	GUID  GetHiddenGuid()	const;
	DWORD GetGestaltInfo()	const;
	DWORD GetIPAddress()	const;

	void SetVisibleGuid	(const GUID& theVisibleGuid);
	void SetHiddenGuid	(const GUID& theHiddenGuid);
	void SetGestaltInfo (DWORD theGestaltInfo);
	void SetIPAddress	(DWORD theIPAddress);

	};

/////////////////////////////////////////////////////////////////////////////////////////

class SNMachineID
	{

private:

	ULONG mSize;	// size of SNMachineID structure in bytes, e.g. sizeof(*this);
					// size may include vptr
	char mType[4];	// FOURCC unique identifier for struct;
					// first 3 chars identify platform,
					// last char identifies revision number, e.g. 'PC 1', 'Mac3', 'Unx5'
					// Not a DWORD to insure works on different endian machines

	union
		{
		MachineIDPCRev1	mPCRev1;		// 'PC 1'
		MachineIDMacRev1 mMacRev1;		// 'Mac1' (not currently used; included for
										//		   evaluating feasibility of union)
		};

public :

	SNMachineID(const SNMachineID& theMachineID); // copy constructor
	SNMachineID(DWORD theType);
	SNMachineID(const BYTE *pMachineID);
	~SNMachineID();

	// Assignement operators
	SNMachineID& operator = (const SNMachineID& theMachineID);
	SNMachineID& operator = (const BYTE *pMachineID);
	
	// Compare two machine IDs
	BOOL operator == (const SNMachineID& theMachineID) const;
	BOOL operator != (const SNMachineID& theMachineID) const;

	// Get a pointer to the SNMachineID bytes, not including the vptr (if any)
	const BYTE * GetMachineIDPtr() const;

	// The size of the MachineID struct in bytes
	ULONG GetBytes() const;

	const char * GetType() const;

	// Accessors
	void GetPCRev1(MachineIDPCRev1& thePCRev1) const;
	void GetMacRev1(MachineIDMacRev1& theMacRev1) const;
	void SetPCRev1(const MachineIDPCRev1& thePCRev1);
	void SetMacRev1(const MachineIDMacRev1& theMacRev1);

private :

	// Disabled
	SNMachineID();

	};

#endif // SN_MACHINEID_H

/////////////////////////////////////////////////////////////////////////////////////////
