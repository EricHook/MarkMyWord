////////////////////////////////////////////////////////////////////////////////////
//
// SNMachineID.cpp
//
// Copyright (c) World Opponent Network 1998, All rights reserved
//
// Initial coding by Pete Isensee, 09/16/98
//
// Implementation of cross-platform machine IDs.

#include "SNMachineID.h"
#include <stdio.h>
#include <assert.h>
#include <memory.h>

#if defined(sun) || defined(__LINUX__)
#	include <sys/socket.h>
#	include <arpa/inet.h>
#endif

#ifdef __LINUX__
#	define CopyMemory memcpy
#endif

#ifdef _WIN32
#	include <winsock.h>
#endif

// static BOOL DoesIPNetworkMatch(DWORD IPAddrA,DWORD IPAddrB);
static DWORD GetIPNetwork(DWORD IPAddr);
static int GetIPAddrClass(DWORD IPAddr);

////////////////////////////////////////////////////////////////////////////////////

const DWORD ClassMask[] =
	{
	0x80000000,	// class A: examine high bit
	0xC0000000,	// class B: examine high two bits
	0xE0000000,	// class C: examine high three bits
	0xF0000000,	// class D: examine high four bits (multicast)
	0xF0000000	// class E: examine high four bits (reserved)
	};

const DWORD ClassBits[] =
	{
	0x00000000,	// class A: high bit is zero
	0x80000000, // class B: high two bits are "10" binary
	0xC0000000,	// class C: high three bits are "110" binary
	0xE0000000,	// class D: high four bits are "1110" binary (multicast)
	0xF0000000	// class E: high four bits are "1111" binary (reserved)
	};

const DWORD NetworkMask[] =
	{
	0xFF000000,	// class A: high 8 bits
	0xFFFF0000, // class B: high 16 bits
	0xFFFFFF00,	// class C: high 24 bits
	0xFFFFFFFF,	// class D: all bits (multicast)
	0xFFFFFFFF	// class E: all bits (reserved)
	};

const int ClassMax = 5;

// FOURCC codes
static const char PC1[]  = "PC 1";
static const char Mac1[] = "Mac1";

////////////////////////////////////////////////////////////////////////////////////

// Compare two IP addresses for equivalency.
// Returns TRUE if they match, FALSE otherwise.
// Two addresses match if the network portion of the address matches.
// The host portion is ignored.

#if 0
static BOOL DoesIPNetworkMatch(DWORD IPAddrA,DWORD IPAddrB)
	{
	// Quick check for an exact match
	if (IPAddrA == IPAddrB)
		return (TRUE);

	// Determine class of each address
	int IPAddrAClass = GetIPAddrClass(IPAddrA);
	int IPAddrBClass = GetIPAddrClass(IPAddrB);

	// Different classes do not match
	if (IPAddrAClass != IPAddrBClass)
		return (FALSE);

	// Compare the network bits
	return ((IPAddrA & NetworkMask[IPAddrAClass]) ==
			(IPAddrB & NetworkMask[IPAddrBClass]));
	}
#endif // 0

////////////////////////////////////////////////////////////////////////////////////

// Strips the host portion of the IP address and returns the network portion.
// The resulting address has its host portion set to zero.

static DWORD GetIPNetwork(DWORD IPAddr)
	{
	// Convert from network byte order to host byte order
	IPAddr = ntohl(IPAddr);

	// Determine class of the address
	int IPClass = GetIPAddrClass(IPAddr);

	return (IPAddr & NetworkMask[IPClass]);
	}

////////////////////////////////////////////////////////////////////////////////////

// Returns the class of the address
// An IP address is one of five classes: A, B, C, D, E
// The class determines which portion of the address is the network
// portion and which is the host portion.
// Class A = 0, B = 1, etc.

static int GetIPAddrClass(DWORD IPAddr)
	{
	for (int i = 0; i < ClassMax; i++)
		{
		if ((IPAddr & ClassMask[i]) == ClassBits[i])
			return (i);
		}

	// should never happen
	assert(FALSE);
	return (0);
	}

////////////////////////////////////////////////////////////////////////////////////

// We have a "match" if either of the GUIDs matches, or the volume ID, CPUID and IP
// all match.

BOOL MachineIDPCRev1::operator == (const MachineIDPCRev1& thePCRev1) const
	{
	return ( ::memcmp(&mVisibleGuid,&thePCRev1.mVisibleGuid,sizeof(GUID)) == 0 ||
			 ::memcmp(&mHiddenGuid,&thePCRev1.mHiddenGuid,sizeof(GUID)) == 0 ||
			(mVolumeID == thePCRev1.mVolumeID &&
			 mCPUID == thePCRev1.mCPUID &&

			 // Used to store the entire IP address, but now we just store the 
			 // network portion
			 /* DoesIPNetworkMatch(mIPAddress,thePCRev1.mIPAddress))*/

			 mIPAddress == thePCRev1.mIPAddress));
	}

// Accessors
GUID MachineIDPCRev1::GetVisibleGuid() const
	{
	return (mVisibleGuid);
	}

GUID MachineIDPCRev1::GetHiddenGuid() const
	{
	return (mHiddenGuid);
	}

DWORD MachineIDPCRev1::GetVolumeID() const
	{
	return (mVolumeID);
	}

DWORD MachineIDPCRev1::GetCPUID() const
	{
	return (mCPUID);
	}

DWORD MachineIDPCRev1::GetIPAddress() const
	{
	return (mIPAddress);
	}

void MachineIDPCRev1::SetVisibleGuid(const GUID& theVisibleGuid)
	{ 
	::CopyMemory(&mVisibleGuid, &theVisibleGuid, sizeof(GUID));
	}

void MachineIDPCRev1::SetHiddenGuid(const GUID& theHiddenGuid)
	{
	::CopyMemory(&mHiddenGuid, &theHiddenGuid, sizeof(GUID));
	}

void MachineIDPCRev1::SetVolumeID(DWORD theVolumeID)
	{
	mVolumeID = theVolumeID;
	}

void MachineIDPCRev1::SetCPUID(DWORD theCPUID)
	{
	mCPUID = theCPUID;
	}

void MachineIDPCRev1::SetIPAddress(DWORD theIPAddress)
	{
	// We only store the network portion of the IP address,
	// because that's all we want to compare
	mIPAddress = GetIPNetwork(theIPAddress);
	}

////////////////////////////////////////////////////////////////////////////////////

// We have a "match" if either of the GUIDs matches, or the volume ID, CPUID and IP
// all match.

BOOL MachineIDMacRev1::operator == (const MachineIDMacRev1& theMacRev1) const
	{
	return ( ::memcmp(&mVisibleGuid,&theMacRev1.mVisibleGuid,sizeof(GUID)) == 0 ||
			 ::memcmp(&mHiddenGuid,&theMacRev1.mHiddenGuid,sizeof(GUID)) == 0 ||
			(mGestaltInfo == theMacRev1.mGestaltInfo &&

			 // Used to store the entire IP address, but now we just store the 
			 // network portion
			 /* DoesIPNetworkMatch(mIPAddress,theMacRev1.mIPAddress))*/

			 mIPAddress == theMacRev1.mIPAddress));
	}

GUID MachineIDMacRev1::GetVisibleGuid() const
	{
	return (mVisibleGuid);
	}

GUID MachineIDMacRev1::GetHiddenGuid() const
	{
	return (mHiddenGuid);
	}

DWORD MachineIDMacRev1::GetGestaltInfo() const
	{
	return (mGestaltInfo);
	}

DWORD MachineIDMacRev1::GetIPAddress() const
	{
	return (mIPAddress);
	}

void MachineIDMacRev1::SetVisibleGuid(const GUID& theVisibleGuid)
	{
	mVisibleGuid = theVisibleGuid;
	}

void MachineIDMacRev1::SetHiddenGuid(const GUID& theHiddenGuid)
	{
	mHiddenGuid = theHiddenGuid;
	}

void MachineIDMacRev1::SetGestaltInfo(DWORD theGestaltInfo)
	{
	mGestaltInfo = theGestaltInfo;
	}

void MachineIDMacRev1::SetIPAddress(DWORD theIPAddress)
	{
	// We only store the network portion of the IP address,
	// because that's all we want to compare
	mIPAddress = GetIPNetwork(theIPAddress);
	}

////////////////////////////////////////////////////////////////////////////////////

// copy constructor
SNMachineID::SNMachineID(const SNMachineID& theMachineID)
	{
	::CopyMemory(&mSize, &theMachineID, sizeof(*this));
	assert(mSize == sizeof(*this));

	// validate
	assert( ::memcmp(mType, PC1,  4) == 0 ||
			::memcmp(mType, Mac1, 4) == 0);
	}

SNMachineID::SNMachineID(DWORD theType)
	{
	mSize = sizeof(*this);	// may include vptr (if any); will include the largest
							// class in the anonymous union

	switch (theType)
		{
	case (dwPC1) : ::CopyMemory(mType, PC1,  4); break;
	case (dwMAC1) : ::CopyMemory(mType, Mac1, 4); break;
	default : assert(FALSE); break;
		}
	}

SNMachineID::SNMachineID(const BYTE *pMachineID)
	{
	assert(pMachineID != NULL);

	// This works because the first 4 bytes of pMachineID 
	// contain the size of the structure
	::CopyMemory(&mSize, pMachineID, sizeof(*this));
	assert(mSize == sizeof(*this));
	mSize = sizeof(*this);	// may include vptr (if any); will include the largest
							// class in the anonymous union

	// validate
	assert( ::memcmp(mType, PC1,  4) == 0 ||
			::memcmp(mType, Mac1, 4) == 0);
	}

SNMachineID::~SNMachineID()
	{
	}

// assignment operator
SNMachineID& SNMachineID::operator = (const SNMachineID& theMachineID)
	{
	if (this == &theMachineID)
	    return (*this);

	::CopyMemory(&mSize, &theMachineID, sizeof(*this));
	assert(mSize == sizeof(*this));
	mSize = sizeof(*this);

	// validate
	assert( ::memcmp(mType, PC1,  4) == 0 ||
			::memcmp(mType, Mac1, 4) == 0);

	return (*this);
	}

SNMachineID& SNMachineID::operator = (const BYTE *pMachineID)
	{
	assert(pMachineID != NULL);

	// This works because the first 4 bytes of pMachineID 
	// contain the size of the structure
	::CopyMemory(&mSize, pMachineID, sizeof(*this));
	assert(mSize == sizeof(*this));
	mSize = sizeof(*this);	// may include vptr (if any); will include the largest
							// class in the anonymous union

	// validate
	assert( ::memcmp(mType, PC1,  4) == 0 ||
			::memcmp(mType, Mac1, 4) == 0);

	return (*this);
	}

BOOL SNMachineID::operator == (const SNMachineID& theMachineID) const
	{
	if (mSize != theMachineID.mSize)
		return (FALSE);
	if ( ::memcmp(mType, theMachineID.mType, 4) != 0)
		return (FALSE);

	if ( ::memcmp(mType, PC1,  4) == 0)
		return (mPCRev1 == theMachineID.mPCRev1);
	if ( ::memcmp(mType, Mac1, 4) == 0)
		return (mMacRev1 == theMachineID.mMacRev1);

	// If we made it this far, mType is invalid
	assert(FALSE);
	return (FALSE);
	}

BOOL SNMachineID::operator != (const SNMachineID& theMachineID) const
	{
	return (!(operator == (theMachineID)));
	}

// Get a pointer to the MachineID bytes, not including the vptr (if any)
const BYTE * SNMachineID::GetMachineIDPtr() const
	{
	return ((BYTE *)&mSize);
	}

// The size of the SNMachineID struct in bytes
ULONG SNMachineID::GetBytes() const
	{
	return (mSize);
	}

const char * SNMachineID::GetType() const
	{
	return (mType);
	}

void SNMachineID::GetPCRev1(MachineIDPCRev1& thePCRev1) const
	{
	assert( ::memcmp(mType, PC1, 4) == 0);
	::CopyMemory(&thePCRev1, &mPCRev1, sizeof(MachineIDPCRev1));
	}

void SNMachineID::GetMacRev1(MachineIDMacRev1& theMacRev1) const
	{
	assert( ::memcmp(mType, Mac1, 4) == 0);
	::CopyMemory(&theMacRev1, &mMacRev1, sizeof(MachineIDMacRev1));
	}

void SNMachineID::SetPCRev1(const MachineIDPCRev1& thePCRev1)
	{
	assert( ::memcmp(mType, PC1, 4) == 0);
	::CopyMemory(&mPCRev1, &thePCRev1, sizeof(MachineIDPCRev1));
	}

void SNMachineID::SetMacRev1(const MachineIDMacRev1& theMacRev1)
	{
	assert( ::memcmp(mType, Mac1, 4) == 0);
	::CopyMemory(&mMacRev1, &theMacRev1, sizeof(MachineIDMacRev1));
	}

////////////////////////////////////////////////////////////////////////////////////
