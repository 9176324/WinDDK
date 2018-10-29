/*++ BUILD Version 0000

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    CircularQueue.h

Abstract:
    Definition of the CircularQueue and SYNC_LOCK classes

--*/

#pragma once
#define MAX_SIZE_OF_QUEUE 10240

class SYNC_LOCK
{
public:
     
    SYNC_LOCK() 
    /*++

    Routine Description:
        SYNC_LOCK constructor. Initializes member variables.

    Arguments:
        None

    Synchronization:
        Synchronized by language definition and convention.

    Return value:
        None
    --*/        
    { 
        m_lOwner = 0;
        m_fInitialized = FALSE;
    }
    
    ~SYNC_LOCK() 
    /*++
    
    Routine Description:
        SYNC_LOCK destructor. In user mode attempts to acquire m_lOwner and
        if successful and m_fInitialized is set, then deletes the critical
        section and resets m_fInitialized. 
    
    Arguments:
        None
    
    Synchronization:
        Synchronized as described above in routine description.
    
    Return value:
        None
    --*/
    { 
        LONG lCurOwner = ::InterlockedExchange(&m_lOwner, 1L);

        if (0L == lCurOwner)
        {
            // We acquired m_lOwner. A subsequent simultaneous thread will
            // get a 1 back from InterlockedExchange and skip this section.
            
            if (m_fInitialized)
            {
                ::DeleteCriticalSection(&_cs);
                m_fInitialized = FALSE;
            }
            m_lOwner = 0L;
        }
    }
    
    HRESULT Init()
    /*++
    
    Routine Description:
        Initializes underlying OS synchronization object and sets
        m_fInitialized. Must be called by object owner before using it for
        synchronization. It is internally synchronized and can be called
        multiple times without any adverse side effects.
    
    Arguments:
        None
    
    Synchronization:
        Synchronized using InterlockedExchange and m_lOwner.
    
    Return value:
        S_OK - initialization succeeded
        E_OUTOFMEMORY - initialization failed
    --*/
    {
        HRESULT hr = S_OK;
        LONG    lCurOwner = ::InterlockedExchange(&m_lOwner, 1L);
    
        if (0L == lCurOwner)
        {
            if (!m_fInitialized)
            {
                ::InitializeCriticalSection(&_cs);
                m_fInitialized = TRUE;
            }
            m_lOwner = 0;
        }
        return hr;
    }
         
    void lock () 
    /*++

    Routine Description:
        Called at the start of a code section to be synchronized. Calls the
        underlying API depending on execution mode.  If another thread has
        already entered the code section and called this function then the
        caller will block until the other thread calls EndSection. 

        NOTE: If SYNC_LOCK::Init has not been successfully called prior to
        calling this function then it does nothing.

    Arguments:
        None
        
    Synchronization:
        None.

    Return value:
        None
    --*/        
    { 
        EnterCriticalSection( &_cs ); 
    }
    
    void unlock () 
    /*++

    Routine Description:
        Called at the end of a code section that was synchronized with a
        prior call to SYNC_LOCK::lock. Calls the underlying API
        depending on execution mode.  If another thread has is blocked in
        StartSection then calling this function will release it.

        NOTE: If SYNC_LOCK::Init has not been successfully called prior to
        calling this function then it does nothing.

        NOTE: If this function is called without a prior call to
        SYNC_LOCK::lock then its behavior is undefined and could
        permanently corrupt the synchronization object.

    Arguments:
        None
        
    Synchronization:
        None.

    Return value:
        None
    --*/        
    { 
        LeaveCriticalSection( &_cs ); 
    }

private:
    CRITICAL_SECTION    _cs;            // used as lock/unlock object
    __volatile LONG    m_lOwner;
    BOOLEAN m_fInitialized;
};

template< typename T >
class CircularQueue
{
public:
    void InitMemberVariables()
    /*++
    Routine Description:
       Initialize member variables for the class CircularQueue
       
    Arguments:
        None
    
    Return value:
        None
    --*/
    {
        _queuePtr = NULL;
        _head = 0;
        _tail = 0;
        _queueSize = 0;
        _isEmpty = TRUE;
    }

    CircularQueue()
    /*++
    Routine Description:
        Constructor for CircularQueue will allocate memory for the queue
        and initialize synchronization object
    Arguments:
        None

    Return value:
        None
    --*/        
    {
        InitMemberVariables();
        _syncObj.Init();
        _queuePtr = (T*)CoTaskMemAlloc( sizeof(T) * MAX_SIZE_OF_QUEUE );
        _queueSize = MAX_SIZE_OF_QUEUE;
        ZeroMemory( _queuePtr, sizeof(T) * _queueSize );
    }

    ~CircularQueue()
    /*++
    Routine Description:
        Destructor for CircularQueue will zero out contents of the queue,
        so it will be up to the caller to track the contents of the queue
    
    Arguments:
        None
    
    Return value:
        None
    --*/        
    {
        if( _queuePtr )
        {
            ZeroMemory( _queuePtr, sizeof(T) * size());
            CoTaskMemFree( _queuePtr );
        }
        InitMemberVariables();
    }

    BOOLEAN resize( UINT cbSize )
    /*++
    Routine Description:
        resize for CircularQueue will zero out contents of the queue
        in the process of resizing the queue.  It will be up to the 
        caller to save the contents of the queue prior to resizing.
    
    Arguments:
        UINT size of the new queue
    
    Return value:
        TRUE  - resize succeeded
        FALSE - resize failed
    --*/        
    {
        if( cbSize == 0 )
            cbSize = 1;

        _syncObj.lock();
        if ( _queuePtr )
        {
            CoTaskMemFree(_queuePtr);
            _queueSize = 0;
        }
        
        _queuePtr  = (T*)CoTaskMemAlloc( sizeof(T) * cbSize );

        if( _queuePtr )
        {
            ZeroMemory( _queuePtr, sizeof(T) * cbSize );
            _queueSize = cbSize;
        }

        // Reset queue properties
        _head = 0;
        _tail = 0;
        _isEmpty = TRUE;

        _syncObj.unlock();
        
        // If the memory Allocation failed, return false
        if( _queuePtr == NULL )
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }

    BOOLEAN insert( __drv_when(return, __drv_aliasesMem) T value )
    /*++
    Routine Description:
        insert for CircularQueue will fail to insert entry if the queue is full.
        Make sure to check the size of the queue and resize before inserting.
    
    Arguments:
        item to insert into the queue
    
    Return value:
        TRUE  - item inserted successfully
        FALSE - item failed to insert and queue is full
    --*/        
    {
        BOOLEAN retVal = TRUE;
        
        _syncObj.lock();
        if( (_head == _tail) && (!_isEmpty) )
        {
            // If we do get to a point that we are inserting into a full queue
            //  we skip over adding the BYTE.  
            OutputDebugString(L"[CircularQueue][Error] Attempted to add to a queue that is already full...\n");

            // I think that if we hit a point where the queue is full, just drop the current value.
            // If having a full queue occurs often, we have a debug statement to track it and we
            // can increase the size or fix the timing to ensure the queue isn't full
            retVal = FALSE;
        }
        else
        {
            OutputDebugString(L"[CircularQueue]Adding to queue...\n");        
            _queuePtr[ _tail ] = value;
            NextIndex( _tail );
            _isEmpty = FALSE;
            if (_tail >=  _queueSize)
            {
                // Used up too much of the queue
                retVal = FALSE;
            }
        }
        _syncObj.unlock();
        
        return retVal;
    }

    T remove()
    /*++
    Routine Description:
        remove for CircularQueue will remove the head from the queue and return it 
        to the caller, then set the next item as the head.  

        This will fail to insert a new entry if the queue is empty.

        Make sure to check that the queue is populated before attempting to 
        clear items.
    
    Arguments:
        Head to be removed from queue and returned to caller
    
    Return value:
        T  - return head of queue 
    --*/        
    {
        // Set the value to a default value
        T queueHead;
        ZeroMemory( &queueHead, sizeof(T) );
        
        _syncObj.lock();
        if( !_isEmpty )
        {
            OutputDebugString(L"[CircularQueue]Remove head from queue...\n");
            queueHead = _queuePtr[_head];
            // Ensure no garbage is left in the queue by zero'ing out as we remove items
            ZeroMemory( &(_queuePtr[_head]), sizeof(T) );
            NextIndex( _head );
            if ( _head >= _queueSize )
            {
                // Removing more than is available on the queue
                OutputDebugString(L"[CircularQueue] Error: removing more than available on queue...\n");
            }
            
            if( _head == _tail )
            {
                _isEmpty = TRUE;
            }
        }
        _syncObj.unlock();

        return queueHead;
    }

    BOOLEAN isEmpty()
    /*++
    Routine Description:
        isEmpty returns the value of _isEmpty
    
    Arguments:
        None
    
    Return value:
        BOOLEAN
    --*/        
    {
        return _isEmpty;
    }

    UINT size()
    /*++
    Routine Description:
        size returns the number of items in the queue
        
    Arguments:
        None
    
    Return value:
        UINT
    --*/                
    {
        return _queueSize;
    }

private:
    INT       _queueSize;
    T*        _queuePtr;
    LONG      _head;
    LONG      _tail;
    BOOLEAN   _isEmpty;
    SYNC_LOCK _syncObj;
    
    void NextIndex( LONG & value )
    {
        if( value == ( _queueSize - 1 ) )
        {
            value = 0;
        }
        else
        {
            ++value;
        } 
    }

    CircularQueue( CircularQueue const& copy)
    {
        // Do nothing, this copy constructor is private so we don't attempt to use it
    }

    CircularQueue& operator=(const CircularQueue&)
    {
        // Do nothing, this is private as it is not useful for this program
    }
};






