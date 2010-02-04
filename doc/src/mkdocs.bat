@ECHO OFF

ECHO.
ECHO *************************************************
ECHO ***      DOCUMENTATION CREATION FOR QT3D      ***
ECHO *************************************************
ECHO.

:BEGIN
rem  display and check if there is an argument	
IF "%1"=="" (
	ECHO USAGE: mkdocs.bat [qdocconf script directory]
	GOTO END
) ELSE (
	SET SCRIPTDIR=%1
	ECHO Script directory is: %SCRIPTDIR%
	ECHO.
	IF EXIST %SCRIPTDIR%\qt3d.qdocconf (
		GOTO MAKEDOCS
	) ELSE (
		ECHO "cannot locate qt3d.qdocconf to generate documentation"
		GOTO END
	)
)

:MAKEDOCS

SET TOPSRC=%SCRIPTDIR%\..\..
ECHO Top level directory is: %TOPSRC%
ECHO.
SET DESTDIR=%SCRIPTDIR%\..\html
IF NOT EXIST %DESTDIR% (
	ECHO Creating destination directory %DESTDIR%
	MKDIR %DESTDIR%
) ELSE (
	ECHO Destination directory '%DESTDIR%' already exists - skipping creation step.
)
ECHO.
ECHO Running script '%SCRIPTDIR%\qt3d.qdocconf', please wait...
qdoc3 %SCRIPTDIR%\qt3d.qdocconf
ECHO Script complete, documents written to '%DESTDIR%'

:END