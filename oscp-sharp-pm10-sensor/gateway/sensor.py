import serial
import sqlite3
import datetime
import time
import re

meaning = 'SensorPM10'
controllerStrId = 'IoT_02'
idOnController = 'SLV_PM10'

ping_re = re.compile('([0-9]*\.[0-9]+|[0-9]+),([0-9]*\.[0-9]+|[0-9]+)')

ser = serial.Serial('/dev/cu.wchusbserial1410', 9600)

try:
	while 1:
		s = str(ser.readline())
		re_match = ping_re.findall(s)
		value = str(re_match[0][1])
		print('value: '+str(re_match[0][1]))

		now = datetime.datetime.now().strftime('%Y-%m-%dT%H:%M:%S.%fZ')  # get timestamp for 'now'

		print('Inserting value: '+value)
		print('Now: '+now)

		# prepare command to insert new row into `oscp_data` table
		command = """INSERT INTO `oscp_data`
			(`controllerStrId`,`idOnController`,`meaning`,`value`,`time`)
			VALUES
			('{controllerStrId}','{idOnController}','{meaning}',"{value}",'{time}');"""

		# open connection to sqlite database shared with the OSCP Data Logger
		conn = sqlite3.connect('../../oscp-gateway/python/data.db', timeout=60)

		conn.execute(command.format(
			controllerStrId=controllerStrId,
			idOnController=idOnController,
			meaning=meaning,
			value=value,
			time=now))  # execute command setting values
		conn.commit()  # commit the changes
		conn.close()  # close connection with the database

except KeyboardInterrupt:
	ser.close()
except FileNotFoundError:
	print('Couldn\'t find file!')