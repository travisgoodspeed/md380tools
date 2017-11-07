REM clear OBJECT FILES but not MAP-, BIN-, and IMG files
del applet\*.o
del applet\*.lst
del applet\lib\*.o
del applet\lib\*.lst
del annotations\d13.020\listing.txt
REM on this occasion, also delete 'compiled' python modules
del *.pyc
REM screenshot_*.png doesn't belong into the git repo.
REM These files were created by the screenshot-via-USB utility, so delete them.
REM For documentation purposes, screenshots should be renamed and moved somewhere else.
del screenshot_*.png
pause