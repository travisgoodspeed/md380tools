

all: ambe
clean:

ambe:
	python dmesg2amb.py <dmesg.txt
	dsd -n -w foo.wav -r output.amb
	mplayer foo.wav
wav:
	python dmesg2wav.py <dmesg.txt
	mplayer output.wav
