/* usrAppInit.c - application main entry point */

extern void ENTRY_POINT ( void );
extern void StartApplication( void * EntryPoint, int stackSize );

/* globals */

/* extern */

extern void sslStubInit (void);

/******************************************************************************
*
* scoeMain - entry point for applications and shared libraries
*/ 

void scoeMain
    (
    void * sslRegisterOrLink,
    int    sysStartType
    )
{
    /* get entry point table for the required SSL and check compatibility */
    sslStubInit();

    /* Start the application */
    StartApplication(ENTRY_POINT, 0x0000A000);
}

