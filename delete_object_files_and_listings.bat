REM clear OBJECT FILES but not MAP-, BIN-, and IMG files
del applet\*.o
del applet\*.lst
del applet\lib\*.o
del applet\lib\*.lst
del annotations\d13.020\listing.txt
REM on this occasion, also delete 'compiled' python modules
del *.pyc
pause