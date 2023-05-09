#!/usr/bin/env python

from md380_tool import selfgps, init_dfu
import time
import socket

def nmea_checksum(s):
	c = 0;
	for i in s[1:-1]: #ignore $ and * characters
		c ^= ord(i)
	c= "%02x"%(c)
	return c.upper()

def selfgps_to_nmea(gpso):
	n = "$GPGLL,{0:02d}{1:07.4f},{2:s},{3:03d}{4:07.4f},{5:s},{6:s},A*".format(
			gpso["latdeg"],
			gpso["latmin"],
			"N",
			gpso["londeg"],
			gpso["lonmin"],
			"W",
			time.strftime("%H%M%S")
			)
	n += nmea_checksum(n)
	return n


class feed_gpsd_with_md380():
	def __init__(self,udptarget):
		self.dfu = init_dfu()
		self.udphost = udptarget[0]
		self.udpport = udptarget[1]
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

	def run(self):
		while 1:
			gpso = selfgps(self.dfu)
			nmea = selfgps_to_nmea(gpso)
			print(nmea)
			nmea += "\n"
			self.sock.sendto( nmea, (self.udphost, self.udpport))
			time.sleep(1)
	

def main():
	print("Make sure your GPSD is running to listen on udp://127.0.0.1:12947")
	print("Example for testing: \n\tgpsd -N -D 9 udp://127.0.0.1:12947")
	md380gps = feed_gpsd_with_md380(("127.0.0.1",12947))
	md380gps.run()

if __name__ == '__main__':
	main()
