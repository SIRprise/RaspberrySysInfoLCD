tty has to be configured with:
stty -F /dev/ttyUSB0 cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts

one way to output ip:
ip -4 addr show eth0 | grep -oP '(?<=inet\s)\d+(\.\d+){3}' --color=never > /dev/ttyUSB0

another way:
ifconfig eth0 | grep 'inet ' | sed "s/^ \+//" > /dev/ttyUSB0

clear display (usually per sending 0xff, but if it doesn't work send):
echo -ne '\n\r' > /dev/ttyUSB0

init.d-script in /etc/init.d 
(see https://tutorials-raspberrypi.de/raspberry-pi-autostart-programm-skript/)

#! /bin/sh
### BEGIN INIT INFO
# Provides: showIP
# Required-Start: $syslog
# Required-Stop: $syslog
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: showIP script
# Description:
### END INIT INFO
 
case "$1" in
    start)
        echo "showIP will be started"
        # start program
        stty -F /dev/ttyUSB0 cs8 115200 min 0 -hupcl -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke -ixon -crtscts
	ip -4 addr show eth0 | grep -oP '(?<=inet\s)\d+(\.\d+){3}'	
	sleep 10s && ip -4 addr show eth0 | grep -oP '(?<=inet\s)\d+(\.\d+){3}' --color=never > /dev/ttyUSB0
        sleep 1s
	;;
    stop)
        echo "showIP will be closed"
        # exit program
        
        ;;
    *)
        # echo "usage: /etc/init.d/noip {start|stop}"
        exit 1
        ;;
esac
 
exit 0