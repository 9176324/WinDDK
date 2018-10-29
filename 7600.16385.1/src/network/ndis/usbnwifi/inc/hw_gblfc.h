#ifndef __INC_GENERALFUNC_H
#define __INC_GENERALFUNC_H

#define eqMacAddr(a,b)                      ( ((a)[0]==(b)[0] && (a)[1]==(b)[1] && (a)[2]==(b)[2] && (a)[3]==(b)[3] && (a)[4]==(b)[4] && (a)[5]==(b)[5]) ? 1:0 )
#define cpMacAddr(des,src)                  ((des)[0]=(src)[0],(des)[1]=(src)[1],(des)[2]=(src)[2],(des)[3]=(src)[3],(des)[4]=(src)[4],(des)[5]=(src)[5])

#define MacAddr_isBcst(addr) \
( \
    ( (addr[0] == 0xff) && (addr[1] == 0xff) && \
        (addr[2] == 0xff) && (addr[3] == 0xff) && \
        (addr[4] == 0xff) && (addr[5] == 0xff) )  ? TRUE : FALSE \
)

#define MacAddr_isMulticast(addr) \
( \
    ( addr[0] & 0x01 )  ? TRUE : FALSE \
)


BOOLEAN 
eqNByte(
    PUCHAR str1,
    PUCHAR str2,
    ULONG  num
    );


//
// RT_PRINT_XXX MACROs: for debugging.
// Added by Annie, 2005-11-21.
//
#if DBG

// Ref: PRINT_DATA(), defined in [SecurityGen.c].
#define RT_PRINT_DATA(_Comp, _Level, Fmt, _Ptr, _Len)                           \
            if((_Comp & GlobalDebugComponents) && (_Level <= GlobalDebugLevel)) \
            {                                                                   \
                int i;                                                              \
                PUCHAR ptr = (PUCHAR)_Ptr;                                        \
                DbgPrint("Rtl818x: ");                                              \
                DbgPrint(Fmt);                                                  \
                for( i=0; i<(int)_Len; i++ )                                            \
                {                                                               \
                    if ((i & 15) == 0)  DbgPrint("\n");                             \
                    DbgPrint("%02X%s", ptr[i], ((i&3)==3)?"  ":" ");                        \
                }                                                               \
                DbgPrint("\n");                                                 \
            }

#define RT_PRINT_ADDR(_Comp, _Level, Fmt, _Ptr)                                 \
            if((_Comp & GlobalDebugComponents) && (_Level <= GlobalDebugLevel)) \
            {                                                                   \
                int i;                                                              \
                PUCHAR ptr = (PUCHAR)_Ptr;                                        \
                DbgPrint("Rtl818x: ");                                              \
                DbgPrint(Fmt);                                                  \
                DbgPrint(" ");                                                      \
                for( i=0; i<6; i++ )    DbgPrint("%02X%s", ptr[i], (i==5)?"":"-");          \
                DbgPrint("\n");                                                 \
            }

#define RT_PRINT_ADDRS(_Comp, _Level, Fmt, _Ptr, _AddNum)                       \
            if((_Comp & GlobalDebugComponents) && (_Level <= GlobalDebugLevel)) \
            {                                                                   \
                int i, j;                                                           \
                PUCHAR ptr = (PUCHAR)_Ptr;                                        \
                DbgPrint("Rtl818x: ");                                              \
                DbgPrint(Fmt);                                                  \
                DbgPrint("\n");                                                 \
                for( i=0; i<(int)_AddNum; i++ )                                     \
                {                                                               \
                    for( j=0; j<6; j++ )    DbgPrint("%02X%s", ptr[i*6+j], (j==5)?"":"-");  \
                    DbgPrint("\n");                                             \
                }                                                               \
            }

// Added by Annie, 2005-11-22.
#define MAX_STR_LEN 64
#define PRINTABLE(_ch)  (_ch>'!' &&_ch<'~' )    // I want to see ASCII 33 to 126 only. Otherwise, I print '?'. Annie, 2005-11-22.

#define RT_PRINT_STR(_Comp, _Level, Fmt, _Ptr, _Len)                                \
            if((_Comp & GlobalDebugComponents) && (_Level <= GlobalDebugLevel)) \
            {                                                                   \
                int     i;                                                      \
                UCHAR  buffer[MAX_STR_LEN];                                        \
                int     length = (_Len<MAX_STR_LEN)? _Len : (MAX_STR_LEN-1) ;       \
                NdisZeroMemory( buffer, MAX_STR_LEN );                      \
                NdisMoveMemory( buffer, (PUCHAR)_Ptr, length );                    \
                for( i=0; i<MAX_STR_LEN; i++ )                                      \
                {                                                               \
                    if( !PRINTABLE(buffer[i]) ) buffer[i] = '?';                            \
                }                                                               \
                buffer[length] = '\0';                                              \
                DbgPrint("Rtl818x: ");                                              \
                DbgPrint(Fmt);                                                  \
                DbgPrint(": %d, <%s>\n", _Len, buffer);                             \
            }
            
#else   // of #if DBG
#define RT_PRINT_DATA(_Comp, _Level, Fmt, _Ptr, _Len)
#define RT_PRINT_ADDR(_Comp, _Level, Fmt, _Ptr)
#define RT_PRINT_ADDRS(_Comp, _Level, Fmt, _Ptr, _AddNum)
#define RT_PRINT_STR(_Comp, _Level, Fmt, _Ptr, _Len)
#endif  // of #if DBG

#endif // #ifndef __INC_GENERALFUNC_H

