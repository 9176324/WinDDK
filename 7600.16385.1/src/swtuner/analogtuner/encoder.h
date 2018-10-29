//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;
/**************************************************************************
    File    : encoder.h
    Abstract: Contains the class definition of the encoder filter extension
**************************************************************************/

class CEncoderFilter {

private:

    //
    // The AVStream filter object associated with this CEncoderFilter.
    //
    PKSFILTER m_Filter;
    CEncoderDevice * m_pDevice;

    //
    // Cleanup():
    //
    // This is the bag cleanup callback for the CEncoderFilter.  Not providing
    // one would cause ExFreePool to be used.  This is not good for C++
    // constructed objects.  We simply delete the object here.
    //
    static
    void
    Cleanup (
        IN CEncoderFilter *EncFilter
        )
    {
        delete EncFilter;
    }

public:

    //
    // CEncoderFilter():
    //
    // The capture filter object constructor.  Since the new operator will
    // have zeroed the memory, do not bother initializing any NULL or 0
    // fields.  Only initialize non-NULL, non-0 fields.
    //
    CEncoderFilter (
        IN PKSFILTER Filter
        ) :
        m_Filter (Filter)
    {
    }

    //
    // ~CEncoderFilter():
    //
    // The capture filter destructor.
    //
    ~CEncoderFilter (
        )
    {
    }
	CEncoderDevice* GetDevice()
	{
		return m_pDevice;
	}

    //
    // DispatchCreate():
    //
    // This is the filter creation dispatch for the capture filter.  It
    // creates the CEncoderFilter object, associates it with the AVStream
    // object, and bags it for easy cleanup later.
    //
    static
    NTSTATUS
    DispatchCreate (
        IN PKSFILTER Filter,
        IN PIRP Irp
        );

};




