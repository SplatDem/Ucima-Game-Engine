REM Engine build script for windows
@ECHO OFF
SetLocal EnableDelayedExpansion

SET cFilenames=
FOR /R %%f in (*c) do (
  SET cFilenames=!cFilenames! %%f
)

REM echo "Files:" %cFilenames%

SET bin=engine
SET compileFlags=-g -shared -Wwarargs -Wall -Werror
REM -Wall -Werror

REM TODO
