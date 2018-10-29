//
//      Copyright (c) 1996-2000 Microsoft Corporation.  All rights reserved.
//      CControlLogic.cpp
//

#define STR_MODULENAME "DDKSynth.sys:Control: "

#include "common.h"

#pragma code_seg()
/*****************************************************************************
 * CControlLogic::CControlLogic()
 *****************************************************************************
 * Constructor for CControlLogic object.
 */
CControlLogic::CControlLogic()
{
    m_fCSInitialized = FALSE;
    InitializeCriticalSection(&m_CriticalSection);
    m_fCSInitialized = TRUE;

    m_pSynth = NULL;
	m_pInstruments = NULL;

    DWORD nIndex;
	GMReset();
	m_fGSActive = FALSE;
	m_fXGActive = FALSE;
    for (nIndex = 0;nIndex < 16;nIndex++)
    {
        m_fSustain[nIndex] = FALSE;
        m_dwProgram[nIndex] = 0;
    }
    m_fEmpty = TRUE;
}

/*****************************************************************************
 * CControlLogic::~CControlLogic()
 *****************************************************************************
 * Destructor for CControlLogic object.
 */
CControlLogic::~CControlLogic()
{
    if (m_fCSInitialized)
    {
        DeleteCriticalSection(&m_CriticalSection);
    }
}

/*****************************************************************************
 * CControlLogic::GMReset()
 *****************************************************************************
 * Performs a General MIDI reset, including resetting all the voice priorities,
 * GS, XG, master volume, etc.
 */
void CControlLogic::GMReset()
{
    static const int nPartToChannel[16] = { 
		9,0,1,2,3,4,5,6,7,8,10,11,12,13,14,15 
	};
	int nX;
	for (nX = 0; nX < 16; nX++)
	{
		int nY;
		m_nData[nX] = 0;
		m_prFineTune[nX] = 0;
		m_bDrums[nX] = 0;
		for (nY = 0; nY < 12; nY++)
		{
			m_prScaleTune[nX][nY] = 0;
		}
		m_nCurrentRPN[nX] = (short) 0x3FFF;
		m_prCoarseTune[nX] = 0;	
		m_bPartToChannel[nX] = (BYTE)nPartToChannel[nX];
		m_fMono[nX] = FALSE;
		m_dwPriority[nX] = DAUD_STANDARD_VOICE_PRIORITY + 
			((16 - nX) * DAUD_CHAN15_VOICE_PRIORITY_OFFSET);
        m_bBankH[nX] = 0;
        m_bBankL[nX] = 0;
	}
	m_bDrums[0] = 1;
	m_vrMasterVolume = 0;
	m_fGSActive = FALSE;
	m_fXGActive = FALSE;
}

/*****************************************************************************
 * CControlLogic::Init()
 *****************************************************************************
 * Initialize the CControlLogic object, and initialize all the child objects --
 * Note recorders, ModWheels, Volumes, Pan, Expression, PitchBend.
 */
HRESULT CControlLogic::Init(CInstManager *pInstruments, CSynth *pSynth)
{
	m_pSynth = pSynth;
	m_pInstruments = pInstruments;
    m_vrGainAdjust = 0;

    m_Notes.Init();

    int iRecIdx;
    for (iRecIdx = 0; iRecIdx < sizeof(m_ModWheel) / sizeof(*m_ModWheel); iRecIdx++)
    {
        m_ModWheel[iRecIdx].Init();
    }
    for (iRecIdx = 0; iRecIdx < sizeof(m_PitchBend) / sizeof(*m_PitchBend); iRecIdx++)
    {
        m_PitchBend[iRecIdx].Init();
    }
    for (iRecIdx = 0; iRecIdx < sizeof(m_Volume) / sizeof(*m_Volume); iRecIdx++)
    {
        m_Volume[iRecIdx].Init();
    }
    for (iRecIdx = 0; iRecIdx < sizeof(m_Expression) / sizeof(*m_Expression); iRecIdx++)
    {
        m_Expression[iRecIdx].Init();
    }
    for (iRecIdx = 0; iRecIdx < sizeof(m_Pan) / sizeof(*m_Pan); iRecIdx++)
    {
        m_Pan[iRecIdx].Init();
    }

	return S_OK;
}

/*****************************************************************************
 * CControlLogic::ClearMIDI()
 *****************************************************************************
 * Clears MIDI up to a certain time, and updates m_fEmpty.
 */
void CControlLogic::ClearMIDI(STIME stEndTime)
{
	DWORD dwIndex;
	::EnterCriticalSection(&m_CriticalSection);
    if (!m_fEmpty)
    {        
        m_fEmpty = TRUE;
        for (dwIndex = 0;dwIndex < 16; dwIndex++)
        {
            m_fEmpty = m_ModWheel[dwIndex].ClearMIDI(stEndTime)
                && m_PitchBend[dwIndex].ClearMIDI(stEndTime)
                && m_Volume[dwIndex].ClearMIDI(stEndTime)
                && m_Expression[dwIndex].ClearMIDI(stEndTime)
                && m_Pan[dwIndex].ClearMIDI(stEndTime);
        }
    }
	::LeaveCriticalSection(&m_CriticalSection);
}

/*****************************************************************************
 * CControlLogic::SetGainAdjust()
 *****************************************************************************
 * Sets the overall gain for this CControlLogic.
 */
void CControlLogic::SetGainAdjust(VREL vrGainAdjust)
{
    m_vrGainAdjust = vrGainAdjust;
}

/*****************************************************************************
 * CControlLogic::QueueNotes()
 *****************************************************************************
 * Retrieves notes from the note recorders, and dispatches them appropriately.
 * Called during the Mix.
 */
void CControlLogic::QueueNotes(STIME stEndTime)
{
    CNote note;
    
    while (m_Notes.GetNote(stEndTime,&note))
    {
		if (note.m_bKey > 0x7F)	// Special command events.
		{
			long lTemp;
			DWORD dwPart = note.m_bPart;
			DWORD dwCommand = note.m_bKey;
			BYTE bData = note.m_bVelocity;
			switch (dwCommand)
			{
            case NOTE_PROGRAMCHANGE:
                m_dwProgram[dwPart] = bData |
                    (m_bBankH[dwPart] << 16) | 
                    (m_bBankL[dwPart] << 8);
                break;
			case NOTE_CC_BANKSELECTH:
				m_bBankH[dwPart] = bData;
				break;
			case NOTE_CC_BANKSELECTL:
				m_bBankL[dwPart] = bData;
				break;
			case NOTE_CC_POLYMODE:
				m_fMono[dwPart] = FALSE;
				break;
			case NOTE_CC_MONOMODE:
				m_fMono[dwPart] = TRUE;
				break;
			case NOTE_CC_RPN_MSB:
				m_nCurrentRPN[dwPart] = (m_nCurrentRPN[dwPart] & 0x7f) + (bData << 7);
				break;
			case NOTE_CC_RPN_LSB:
				m_nCurrentRPN[dwPart] = (m_nCurrentRPN[dwPart] & 0x3f80) + bData;
				break;
			case NOTE_CC_NRPN:
				m_nCurrentRPN[dwPart] = 0x3FFF;
				break;
			case NOTE_CC_DATAENTRYLSB:
				m_nData[dwPart] &= ~0x7F;
				m_nData[dwPart] |= bData;
				switch (m_nCurrentRPN[dwPart])
				{
				case RPN_PITCHBEND: // Don't do anything, Roland ignores lsb
					break;
				case RPN_FINETUNE:
					lTemp = m_nData[dwPart];
					lTemp -= 8192;
					lTemp *= 100;
					lTemp /= 8192;
					m_prFineTune[dwPart] = lTemp;
					break;
				case RPN_COARSETUNE: // Ignore lsb
					break;            
				}
				break;
			case NOTE_CC_DATAENTRYMSB:
				m_nData[dwPart] &= ~(0x7F << 7);
				m_nData[dwPart] |= bData << 7;
				switch (m_nCurrentRPN[dwPart])
				{
				case RPN_PITCHBEND:
					m_PitchBend[dwPart].m_prRange = bData * 100;
					break;
				case RPN_FINETUNE:
					lTemp = m_nData[dwPart];
					lTemp -= 8192;
					lTemp *= 100;
					lTemp /= 8192;
					m_prFineTune[dwPart] = lTemp;
					break;
				case RPN_COARSETUNE:
					m_prCoarseTune[dwPart] = 100 * (bData - 64);
					break;        
				}
				break;
			case NOTE_SUSTAIN: // special sustain marker
	            m_fSustain[dwPart] = (BOOL) bData;
				if (bData == FALSE)
				{
					CVoice * pVoice = m_pSynth->m_VoicesInUse.GetHead();
					for (;pVoice != NULL;pVoice = pVoice->GetNext())
					{
						if (pVoice->m_fSustainOn &&
							(pVoice->m_nPart == dwPart) &&
							(pVoice->m_pControl == this))
						{
							pVoice->StopVoice(note.m_stTime);
						}
					}
				} 
				break;
			case NOTE_ALLOFF:
				{
					CVoice *pVoice = m_pSynth->m_VoicesInUse.GetHead();
					for (;pVoice != NULL; pVoice = pVoice->GetNext())
					{
						if (pVoice->m_fNoteOn && !pVoice->m_fSustainOn &&
							(pVoice->m_nPart == dwPart) &&
							(pVoice->m_pControl == this))
						{
							if (m_fSustain[dwPart])
							{
								pVoice->m_fSustainOn = TRUE;
							}
							else
							{
								pVoice->StopVoice(note.m_stTime);
							}
						}
					}
				}
				break;
			case NOTE_SOUNDSOFF:
				{
					CVoice *pVoice = m_pSynth->m_VoicesInUse.GetHead();
					for (;pVoice != NULL; pVoice = pVoice->GetNext())
					{
						if (pVoice->m_fNoteOn &&
							(pVoice->m_nPart == dwPart) &&
							(pVoice->m_pControl == this))
						{
							pVoice->StopVoice(note.m_stTime);
						}
					}
				}
				break;
			case NOTE_ASSIGNRECEIVE:
				m_bPartToChannel[dwPart] = (BYTE) bData;
				break;
			case NOTE_MASTERVOLUME:
				m_vrMasterVolume = CMIDIRecorder::VelocityToVolume(bData);
				break;
			}
		}
        else if (note.m_bVelocity == 0)  // Note Off.
        {
            CVoice * pVoice = m_pSynth->m_VoicesInUse.GetHead();
            WORD nPart = note.m_bPart;
            for (;pVoice != NULL;pVoice = pVoice->GetNext())
            {
                if (pVoice->m_fNoteOn && !pVoice->m_fSustainOn &&
                    (pVoice->m_nKey == (WORD) note.m_bKey) &&
                    (pVoice->m_nPart == nPart) &&
					(pVoice->m_pControl == this)) 
                {
                    if (m_fSustain[nPart])
                    {
                        pVoice->m_fSustainOn = TRUE;
                    }
                    else
                    {
                        pVoice->StopVoice(note.m_stTime);
                    }
                    break;
                }
            }
        }
        else   // Note On.
        {
            DWORD dwProgram = m_dwProgram[note.m_bPart];
            if (m_bDrums[note.m_bPart])
            {
                dwProgram |= F_INSTRUMENT_DRUMS;
            }
			if (m_fMono[note.m_bPart])
			{
				CVoice * pVoice = m_pSynth->m_VoicesInUse.GetHead();
				WORD nPart = note.m_bPart;
				for (;pVoice != NULL;pVoice = pVoice->GetNext())
				{
					if (pVoice->m_fNoteOn && (pVoice->m_nPart == nPart) &&
						(pVoice->m_pControl == this)) 
					{
						pVoice->StopVoice(note.m_stTime);
					}
				}
			}
			// While we are working with the instrument, including copying
			// the data over from the region, we have to make sure it
			// can not be removed from the instrument list.
			EnterCriticalSection(&m_pInstruments->m_CriticalSection);
            CInstrument * pInstrument = 
                m_pInstruments->GetInstrument(dwProgram,note.m_bKey);
            if (!pInstrument) 
			{
				if (dwProgram & F_INSTRUMENT_DRUMS)
				{
					dwProgram = F_INSTRUMENT_DRUMS;
					pInstrument = 
						m_pInstruments->GetInstrument(dwProgram,note.m_bKey);
				}
				else if (m_fXGActive)
				{
					if ((dwProgram & 0x7F0000) == 0x7F0000)	// Drum?
					{
						dwProgram &= 0x7F007F;				// Enforce 0 LSB
						pInstrument = 
							m_pInstruments->GetInstrument(dwProgram,note.m_bKey);
                        if (!pInstrument)
                        {
                            dwProgram = 0x7F0000;
						    pInstrument = 
							    m_pInstruments->GetInstrument(dwProgram,note.m_bKey);
                        }
					}
                    else
                    {
						dwProgram &= 0x7F;	// Fall back to GM set.
						pInstrument = 
							m_pInstruments->GetInstrument(dwProgram,note.m_bKey);
					}
				}
			}
            if (pInstrument != NULL)
            {
                CSourceRegion * pRegion = 
                    pInstrument->ScanForRegion(note.m_bKey);
                if (pRegion != NULL)
                {
                    WORD nPart = note.m_bPart;
                    CVoice * pVoice = m_pSynth->m_VoicesInUse.GetHead();
                    if (!pRegion->m_bAllowOverlap)
                    {
                        for (;pVoice != NULL; pVoice = pVoice->GetNext())
                        {
                            if ((pVoice->m_nPart == nPart) &&
                                (pVoice->m_nKey == note.m_bKey) &&
								(pVoice->m_pControl == this))
                            {
                                pVoice->QuickStopVoice(note.m_stTime);
                            }
                        }    
                    }

                    if (pRegion->m_bGroup != 0)
                    {
                        pVoice = m_pSynth->m_VoicesInUse.GetHead();
                        for (;pVoice != NULL;pVoice = pVoice->GetNext())
                        {
                            if ((pVoice->m_dwGroup == pRegion->m_bGroup) &&
                                (pVoice->m_nPart == nPart) &&
                                (pVoice->m_dwProgram == dwProgram) &&
								(pVoice->m_pControl == this))
                            {
                                pVoice->QuickStopVoice(note.m_stTime);
                            }
                        }
                    }
                    
                    pVoice = m_pSynth->m_VoicesFree.RemoveHead();
                    if (pVoice == NULL)
                    {
                        pVoice = m_pSynth->m_VoicesExtra.RemoveHead();
                    }

                    if (pVoice == NULL)
                    {
                        pVoice = m_pSynth->StealVoice(m_dwPriority[nPart]);
                    }

                    if (pVoice != NULL)
                    {
						PREL prPitch = m_prFineTune[nPart] + m_prScaleTune[nPart][note.m_bKey % 12];
						if (!m_bDrums[nPart]) 
						{
							if (m_fXGActive)
							{
								if ((dwProgram & 0x7F0000) != 0x7F0000)
								{
									prPitch += m_prCoarseTune[nPart];
								}
							}
							else
							{
								prPitch += m_prCoarseTune[nPart];
							}
                        }
						pVoice->m_nKey = note.m_bKey;
                        pVoice->m_nPart = nPart;
                        pVoice->m_dwProgram = dwProgram;
                        pVoice->m_dwPriority = m_dwPriority[nPart];
						pVoice->m_pControl = this;

                        if (pVoice->StartVoice( m_pSynth,
                                                pRegion, note.m_stTime, 
                                                &m_ModWheel[nPart],     
                                                &m_PitchBend[nPart],
                                                &m_Expression[nPart],   
                                                &m_Volume[nPart],
                                                &m_Pan[nPart],          
                                                (WORD)note.m_bKey,
							                    (WORD)note.m_bVelocity, 
                                                m_vrMasterVolume, 
							                    prPitch))
                        {
                            pVoice->m_fInUse = TRUE;

                            m_pSynth->QueueVoice(pVoice);
                        }
                        else
                        {
                            m_pSynth->m_VoicesFree.AddHead(pVoice);
                        }
                    } 
                    else 
                    {
                        m_pSynth->m_BuildStats.dwNotesLost++;
                    }
                } 
                else 
                {
                    m_pSynth->m_BuildStats.dwNotesLost++;
                }
            } 
            else 
            {
                Trace(1, "No instrument/region was found for patch # %lx, note %ld\n",
                          dwProgram, (long) note.m_bKey);
            }
			LeaveCriticalSection(&m_pInstruments->m_CriticalSection);
        }
    }
}

/*****************************************************************************
 * CControlLogic::Flush()
 *****************************************************************************
 * Clears events and notes up to a given time.
 */
void CControlLogic::Flush(STIME stTime)
{
    DWORD dwIndex;
    ::EnterCriticalSection(&m_CriticalSection);
    if (!m_fEmpty)
    {        
        m_fEmpty = TRUE;
        for (dwIndex = 0;dwIndex < 16; dwIndex++)
        {
            m_fEmpty = m_ModWheel[dwIndex].FlushMIDI(stTime)
                && m_PitchBend[dwIndex].FlushMIDI(stTime)
                && m_Volume[dwIndex].FlushMIDI(stTime)
                && m_Expression[dwIndex].FlushMIDI(stTime)
                && m_Pan[dwIndex].FlushMIDI(stTime);
        }
        m_Notes.FlushMIDI(stTime);
    }
    ::LeaveCriticalSection(&m_CriticalSection);
}

/*****************************************************************************
 * CControlLogic::RecordMIDI()
 *****************************************************************************
 * Record a MIDI short message into this channel group.  
 * This dispatches the different MIDI
 * messages to the different receptor objects.
 */
BOOL CControlLogic::RecordMIDI(STIME stTime,BYTE bStatus, BYTE bData1, BYTE bData2)
{
    WORD nPreChannel = bStatus & 0xF; 
    CNote note;
    bStatus = bStatus & 0xF0;
    BOOL bReturn = TRUE;
	WORD nPart;

    ::EnterCriticalSection(&m_CriticalSection);

    for (nPart = 0;nPart < 16; nPart++)
	{
		if (nPreChannel == m_bPartToChannel[nPart])
		{
			switch (bStatus)
			{
			case MIDI_NOTEOFF :
				bData2 = 0;
			case MIDI_NOTEON :
				note.m_bPart = (BYTE) nPart;
				note.m_bKey = bData1;
				note.m_bVelocity = bData2;
				bReturn = m_Notes.RecordNote(stTime,&note);
				break;
			case MIDI_CCHANGE :
				switch (bData1)
				{
				case CC_BANKSELECTH :
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_CC_BANKSELECTH, bData2);
					break;
				case CC_MODWHEEL :
                    m_fEmpty = FALSE;
					bReturn = m_ModWheel[nPart].RecordMIDI(stTime,(long) bData2);
					break;
				case CC_VOLUME :
                    m_fEmpty = FALSE;
					bReturn = m_Volume[nPart].RecordMIDI(stTime,(long) bData2);
					break;
				case CC_PAN :
                    m_fEmpty = FALSE;
					bReturn = m_Pan[nPart].RecordMIDI(stTime,(long) bData2);
					break;
				case CC_EXPRESSION :
                    m_fEmpty = FALSE;
					bReturn = m_Expression[nPart].RecordMIDI(stTime,(long)bData2);
					break;
				case CC_BANKSELECTL :
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_CC_BANKSELECTL, bData2);
					break;
				case CC_RESETALL:
                    m_fEmpty = FALSE;
					if (bData2)
					{
						bReturn = bReturn && m_Volume[nPart].RecordMIDI(stTime, 100);
						bReturn = bReturn && m_Pan[nPart].RecordMIDI(stTime, 64);
					}
					bReturn = bReturn && m_Expression[nPart].RecordMIDI(stTime, 127);
					bReturn = bReturn && m_PitchBend[nPart].RecordMIDI(stTime, 0x2000);
					bReturn = m_ModWheel[nPart].RecordMIDI(stTime, 0);
					bData2 = 0;
					// fall through into Sustain Off case....
        
				case CC_SUSTAIN :
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_SUSTAIN, bData2);
					break;
				case CC_ALLSOUNDSOFF:
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_SOUNDSOFF, 0);
					break;
				case CC_ALLNOTESOFF:
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_ALLOFF, 0);
					break;
				case CC_DATAENTRYMSB:
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_CC_DATAENTRYMSB, bData2);
					break;
				case CC_DATAENTRYLSB:
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_CC_DATAENTRYLSB, bData2);
					break;
				case CC_NRPN_LSB :
				case CC_NRPN_MSB :
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_CC_NRPN, bData2);
					break;
				case CC_RPN_LSB:
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_CC_RPN_LSB, bData2);
					break; 
				case CC_RPN_MSB:
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_CC_RPN_MSB, bData2);
					break;
				case CC_MONOMODE :
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_CC_MONOMODE, bData2);
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_SOUNDSOFF, 0);
					break;
				case CC_POLYMODE :
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_CC_POLYMODE, bData2);
					bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_SOUNDSOFF, 0);
					break;
				default:
					break;
				}
				break;

			case MIDI_PCHANGE :
				bReturn = m_Notes.RecordEvent(stTime, nPart, NOTE_PROGRAMCHANGE, bData1);
				break;
        
			case MIDI_PBEND :
                m_fEmpty = FALSE;
				{
					WORD nBend;
					nBend = bData2 << 7;
					nBend |= bData1;
					bReturn = m_PitchBend[nPart].RecordMIDI(stTime,(long)nBend);
				}
				break;
			}
		}
	}
    ::LeaveCriticalSection(&m_CriticalSection);

    return bReturn;
}

/*****************************************************************************
 * CControlLogic::RecordSysEx()
 *****************************************************************************
 * Record a MIDI SysEx message into this channel group.  
 * This dispatches the different MIDI
 * messages to the different receptor objects.
 */
HRESULT	CControlLogic::RecordSysEx(DWORD dwSysExLength,BYTE *pSysExData, STIME stTime)
{
	HRESULT hSuccess = S_OK;
	int nPart;
	int nTune;
	DWORD dwAddress;
	
    BOOL fClearAll = FALSE;
	BOOL fResetPatches = FALSE;
	if (dwSysExLength < 6)
    {
        return E_FAIL;
    }

	switch (pSysExData[1])	// ID number
	{
	case 0x7E :	// General purpose ID
		if (pSysExData[3] == 0x09) 
		{
			GMReset();
			fClearAll = TRUE;
			fResetPatches = TRUE;
		}
		break;
	case 0x7F : // Real time ID
		if (pSysExData[3] == 0x04)
		{
			if (pSysExData[4] == 1)	// Master Volume
			{
				m_Notes.RecordEvent(stTime, 0, NOTE_MASTERVOLUME, pSysExData[6]);
			}
		}
		break;
	case 0x41 : // Roland
		if (dwSysExLength < 11)
        {
            return E_FAIL;
        }
		if (pSysExData[3] != 0x42) break;
		if (pSysExData[4] != 0x12) break;
		nPart = pSysExData[6] & 0xF;
		dwAddress = (pSysExData[5] << 16) |
			((pSysExData[6] & 0xF0) << 8) | pSysExData[7];
		switch (dwAddress)
		{
		case 0x40007F :		// GS Reset.
			GMReset();
			m_fXGActive = FALSE;
			fClearAll = TRUE;
			m_fGSActive = TRUE;
			fResetPatches = TRUE;
			break;
		case 0x401002 :		// Set Receive Channel.
			if (m_fGSActive)
			{
				if (pSysExData[8])
				{
					m_Notes.RecordEvent(stTime, nPart, NOTE_ASSIGNRECEIVE, pSysExData[8] - 1);
				}
			}
			break;
		case 0x401015 :		// Use for Rhythm.
			if (m_fGSActive)
			{
				m_bDrums[nPart] = pSysExData[8];
				fClearAll = TRUE;
			}
			break;
		case 0x401040 :		// Scale Tuning.
			if (m_fGSActive)
			{
				for (nTune = 0;nTune < 12; nTune++)
				{
					if (pSysExData[9 + nTune] & 0x80) break;
					m_prScaleTune[nPart][nTune] = 
						(PREL) pSysExData[8 + nTune] - (PREL) 64;
				}
			}
			break;
		}
		break;
	case 0x43 : // Yamaha
		if ((pSysExData[3] == 0x4C) &&
			(pSysExData[4] == 0) &&
			(pSysExData[5] == 0) &&
			(pSysExData[6] == 0x7E) &&
			(pSysExData[7] == 0))
		{	// XG System On
			m_fXGActive = TRUE;
			m_fGSActive = FALSE;
			GMReset();
			m_fXGActive = TRUE;
			m_bDrums[0] = 0;
			m_bBankH[0] = 127;
			fClearAll = TRUE;
			fResetPatches = TRUE;
		}
		break;
	}
	if (fClearAll)
	{
		m_pSynth->AllNotesOff();
		Flush(0);
		for (nPart = 0;nPart < 16;nPart++)
		{        
			m_Notes.RecordEvent(stTime, nPart, NOTE_SUSTAIN, 0);
			m_Volume[nPart].RecordMIDI(stTime, 100);
			m_Pan[nPart].RecordMIDI(stTime, 64);
			m_Expression[nPart].RecordMIDI(stTime, 127);
			m_PitchBend[nPart].RecordMIDI(stTime, 0x2000);
			m_ModWheel[nPart].RecordMIDI(stTime, 0);
		}
	}
	if (fResetPatches)
	{
		for (nPart = 0;nPart < 16;nPart++)
		{     
			if ((nPart == 0) && (m_fXGActive))
			{
				m_Notes.RecordEvent(stTime-1, nPart, NOTE_CC_BANKSELECTH, 127);
			}
			else
			{
				m_Notes.RecordEvent(stTime-1, nPart, NOTE_CC_BANKSELECTH, 0);
			}
			m_Notes.RecordEvent(stTime-1, nPart, NOTE_CC_BANKSELECTL, 0);
			m_Notes.RecordEvent(stTime, nPart, NOTE_PROGRAMCHANGE, 0);
		}
	}

	return hSuccess;
}

/*****************************************************************************
 * CControlLogic::SetChannelPriority()
 *****************************************************************************
 * Set the priority for a given channel, within this channel group.
 */
HRESULT	CControlLogic::SetChannelPriority(DWORD dwChannel,DWORD dwPriority)
{
    DWORD dwPart;
	for (dwPart = 0;dwPart < 16; dwPart++)
	{
		if (m_bPartToChannel[dwPart] == dwChannel)
		{
        	m_dwPriority[dwPart] = dwPriority;
        }
	}
	return S_OK;
}

/*****************************************************************************
 * CControlLogic::GetChannelPriority()
 *****************************************************************************
 * Retrieve the priority for a given channel, within this channel group.
 */
HRESULT	CControlLogic::GetChannelPriority(DWORD dwChannel,LPDWORD pdwPriority)
{
    DWORD dwPart;
	for (dwPart = 0;dwPart < 16; dwPart++)
	{
		if (m_bPartToChannel[dwPart] == dwChannel)
		{
        	*pdwPriority = m_dwPriority[dwPart];
			break;
        }
	}
	return S_OK;
}

