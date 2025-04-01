/* usrAppInit.c - application main entry point */

extern "C" void ENTRY_POINT( void );
extern "C" void StartApplication( void* EntryPoint, int stackSize );

/* globals */


/* extern */

extern "C" void sslStubInit( void );
extern void sslCPlusPlusStubInit( void );

/******************************************************************************
*
* scoeMain - entry point for applications and shared libraries
*/ 

extern "C" void scoeMain
(
    void * sslRegisterOrLink __attribute__ ((unused)),
    int    sysStartType __attribute__ ((unused))
)
{
    /* get entry point table for the required SSL and check compatibility */
    sslStubInit();
    sslCPlusPlusStubInit();
    
    /* Start the application */
    StartApplication( ( void* ) ENTRY_POINT, 0x0000A000 );
}