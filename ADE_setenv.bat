@ECHO OFF
REM PROJECT TEAMS MAY WISH TO CONFIGURE THIS FOR THEIR ENV SPECIFICALLY

set PLATFORM=ADE

REM Remove Cygwin from path and add compiler + make
set PATH=%~dp0;C:\ADE\gcc-ade\;C:\ADE\gcc-ade\bin;C:\Program Files (x86)\GE Aviation\ADE_06-01\bin;%PATH:cygwin=niwgyc%
set SCOE_BASE=C:\SCOE_ADE\06-01\
set ADE_SCOE_BASE=C:\SCOE_ADE\06-01\