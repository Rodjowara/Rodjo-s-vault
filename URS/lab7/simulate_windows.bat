
@echo off
echo
echo Start testbench_movavg4.
echo

setlocal
set GHDL=ghdl
set FLAGS=--std=08 -fsynopsys
set FLAGS_WAVE="--wave=wave.ghw"
set FLAGS_RUN_TIME=--stop-time=500ns

echo    1) Analyse
%GHDL% -a %FLAGS% testbench_movavg4.vhd movavg4.vhd || exit /b

echo    2) Execute
%GHDL% -e %FLAGS% testbench_movavg4 || exit /b

echo    3) Simulate
%GHDL% -r %FLAGS% testbench_movavg4 %FLAGS_WAVE% %FLAGS_RUN_TIME% || exit /b

echo    4) Do You Want to start gtkwave?
choice /c YN
if %errorlevel%==1 goto yes
if %errorlevel%==2 goto no
:yes
echo STARTING GTKWAVE
gtkwave wave.ghw	
goto :EOF
:no

endlocal