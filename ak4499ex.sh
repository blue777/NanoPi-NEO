#!/bin/sh
#
#	Copyright (C) 2017-2025 blue-7 (http://qiita.com/blue-7)

#NanoPi-NEO2
PDN="199"
MUTE="200"
I2CBUS="0"

#RaspberryPi Zero
#PDN="4"
#MUTE="23"
#I2CBUS="1"


# DAC I2C ADDR
CH0=0x10
CH1=0x11
CHEX=0x13

# 255=0db, 254=-0.5dB,,, 231=-12dB
VOLUME=${VOLUME:=255}

# Digital Filter
#   0 : Sharp roll-off filter
#   1 : Slow roll-off filter
#   2 : Short delay sharp roll off filter (AK449x default)
#   3 : Short delay slow roll off filter
#   4 : Super Slow roll off filter (Non Over Sampling mode) 
#   5 : Low Dispersion Short Delay filter
DF=${DF:=0}



if [ ! -e /sys/class/gpio/gpio${MUTE}/direction ]; then
        echo ${MUTE} > /sys/class/gpio/export
fi

echo out > /sys/class/gpio/gpio${MUTE}/direction
echo 0 > /sys/class/gpio/gpio${MUTE}/value


if [ ! -e /sys/class/gpio/gpio${PDN}/direction ]; then
	echo ${PDN} > /sys/class/gpio/export
fi

echo out > /sys/class/gpio/gpio${PDN}/direction
echo 0 > /sys/class/gpio/gpio${PDN}/value
echo 0 > /sys/class/gpio/gpio${PDN}/value
echo 0 > /sys/class/gpio/gpio${PDN}/value
echo 1 > /sys/class/gpio/gpio${PDN}/value
echo 1 > /sys/class/gpio/gpio${PDN}/value
echo 1 > /sys/class/gpio/gpio${PDN}/value


case $DF in
	0) echo "DF  : Sharp roll-off filter" 
		SSLOW=0
		SD=0
		SLOW=0
	;;
	1) echo "DF  : Slow roll-off filter" 
		SSLOW=0
		SD=0
		SLOW=1
	;;
	2) echo "DF  : Short delay sharp roll off filter (AK449x default)" 
		SSLOW=0
		SD=1
		SLOW=0
	;;
	3) echo "DF  : Short delay slow roll off filter" 
		SSLOW=0
		SD=1
		SLOW=1
	;;
	4) echo "DF  : Super Slow roll off filter (Non Over Sampling mode)" 
		SSLOW=1
		SD=0
		SLOW=0
	;;
	5) echo "DF : Low Dispersion Short Delay"
		SSLOW=1
		SD=1
		SLOW=0
	;;
esac



echo ""
echo "AK4191-Lch register dump"

i2cget -y ${I2CBUS} ${CH0} 0x00
i2cget -y ${I2CBUS} ${CH0} 0x01
i2cget -y ${I2CBUS} ${CH0} 0x02
i2cget -y ${I2CBUS} ${CH0} 0x03
i2cget -y ${I2CBUS} ${CH0} 0x04
i2cget -y ${I2CBUS} ${CH0} 0x05
i2cget -y ${I2CBUS} ${CH0} 0x06
i2cget -y ${I2CBUS} ${CH0} 0x07
i2cget -y ${I2CBUS} ${CH0} 0x08
i2cget -y ${I2CBUS} ${CH0} 0x09
i2cget -y ${I2CBUS} ${CH0} 0x0A
i2cget -y ${I2CBUS} ${CH0} 0x0B
i2cget -y ${I2CBUS} ${CH0} 0x0C
i2cget -y ${I2CBUS} ${CH0} 0x0D
i2cget -y ${I2CBUS} ${CH0} 0x0E
i2cget -y ${I2CBUS} ${CH0} 0x0F



echo ""
echo "AK4191-Rch register dump"

i2cget -y ${I2CBUS} ${CH1} 0x00
i2cget -y ${I2CBUS} ${CH1} 0x01
i2cget -y ${I2CBUS} ${CH1} 0x02
i2cget -y ${I2CBUS} ${CH1} 0x03
i2cget -y ${I2CBUS} ${CH1} 0x04
i2cget -y ${I2CBUS} ${CH1} 0x05
i2cget -y ${I2CBUS} ${CH1} 0x06
i2cget -y ${I2CBUS} ${CH1} 0x07
i2cget -y ${I2CBUS} ${CH1} 0x08
i2cget -y ${I2CBUS} ${CH1} 0x09
i2cget -y ${I2CBUS} ${CH1} 0x0A
i2cget -y ${I2CBUS} ${CH1} 0x0B
i2cget -y ${I2CBUS} ${CH1} 0x0C
i2cget -y ${I2CBUS} ${CH1} 0x0D
i2cget -y ${I2CBUS} ${CH1} 0x0E
i2cget -y ${I2CBUS} ${CH1} 0x0F



















REG01=2
CMD="expr ${REG01} + ${SSLOW} \"*\" 32 + ${SD} \"*\" 16 + ${SLOW} \"*\" 8"
echo ${CMD}
REG01=`$CMD`
echo ${REG01}




# AK4499EX
i2cset -y ${I2CBUS} ${CHEX} 0x00 0x91
i2cset -y ${I2CBUS} ${CHEX} 0x01 0x80
i2cset -y ${I2CBUS} ${CHEX} 0x02 0x40




#AK4191

i2cset -y ${I2CBUS} ${CH0} 0x00 0x00
i2cset -y ${I2CBUS} ${CH0} 0x01 ${REG01}
i2cset -y ${I2CBUS} ${CH0} 0x02 0x00
i2cset -y ${I2CBUS} ${CH0} 0x03 ${VOLUME}
i2cset -y ${I2CBUS} ${CH0} 0x04 ${VOLUME}
i2cset -y ${I2CBUS} ${CH0} 0x05 0x50
i2cset -y ${I2CBUS} ${CH0} 0x06 0x00
i2cset -y ${I2CBUS} ${CH0} 0x07 0x01
i2cset -y ${I2CBUS} ${CH0} 0x09 0x00
i2cset -y ${I2CBUS} ${CH0} 0x0A 0x03
i2cset -y ${I2CBUS} ${CH0} 0x0B 0x00
i2cset -y ${I2CBUS} ${CH0} 0x0C 0x00
i2cset -y ${I2CBUS} ${CH0} 0x10 0x21

i2cset -y ${I2CBUS} ${CH1} 0x00 0x00
i2cset -y ${I2CBUS} ${CH1} 0x01 ${REG01}
i2cset -y ${I2CBUS} ${CH1} 0x02 0x00
i2cset -y ${I2CBUS} ${CH1} 0x03 ${VOLUME}
i2cset -y ${I2CBUS} ${CH1} 0x04 ${VOLUME}
i2cset -y ${I2CBUS} ${CH1} 0x05 0x70
i2cset -y ${I2CBUS} ${CH1} 0x06 0x00
i2cset -y ${I2CBUS} ${CH1} 0x07 0x01
i2cset -y ${I2CBUS} ${CH1} 0x09 0x00
i2cset -y ${I2CBUS} ${CH1} 0x0A 0x03
i2cset -y ${I2CBUS} ${CH1} 0x0B 0x00
i2cset -y ${I2CBUS} ${CH1} 0x0C 0x00
i2cset -y ${I2CBUS} ${CH1} 0x10 0x21


i2cset -y ${I2CBUS} ${CH0} 0x08 0x65
i2cset -y ${I2CBUS} ${CH1} 0x08 0x65
sleep 0.1
i2cset -y ${I2CBUS} ${CH0} 0x00  0x3F
i2cset -y ${I2CBUS} ${CH1} 0x00  0x3F




echo ""
echo "AK4191-Lch register dump"

i2cget -y ${I2CBUS} ${CH0} 0x00
i2cget -y ${I2CBUS} ${CH0} 0x01
i2cget -y ${I2CBUS} ${CH0} 0x02
i2cget -y ${I2CBUS} ${CH0} 0x03
i2cget -y ${I2CBUS} ${CH0} 0x04
i2cget -y ${I2CBUS} ${CH0} 0x05
i2cget -y ${I2CBUS} ${CH0} 0x06
i2cget -y ${I2CBUS} ${CH0} 0x07
i2cget -y ${I2CBUS} ${CH0} 0x08
i2cget -y ${I2CBUS} ${CH0} 0x09
i2cget -y ${I2CBUS} ${CH0} 0x0A
i2cget -y ${I2CBUS} ${CH0} 0x0B
i2cget -y ${I2CBUS} ${CH0} 0x0C
i2cget -y ${I2CBUS} ${CH0} 0x0D
i2cget -y ${I2CBUS} ${CH0} 0x0E
i2cget -y ${I2CBUS} ${CH0} 0x0F


echo ""
echo "AK4191-Rch register dump"

i2cget -y ${I2CBUS} ${CH1} 0x00
i2cget -y ${I2CBUS} ${CH1} 0x01
i2cget -y ${I2CBUS} ${CH1} 0x02
i2cget -y ${I2CBUS} ${CH1} 0x03
i2cget -y ${I2CBUS} ${CH1} 0x04
i2cget -y ${I2CBUS} ${CH1} 0x05
i2cget -y ${I2CBUS} ${CH1} 0x06
i2cget -y ${I2CBUS} ${CH1} 0x07
i2cget -y ${I2CBUS} ${CH1} 0x08
i2cget -y ${I2CBUS} ${CH1} 0x09
i2cget -y ${I2CBUS} ${CH1} 0x0A
i2cget -y ${I2CBUS} ${CH1} 0x0B
i2cget -y ${I2CBUS} ${CH1} 0x0C
i2cget -y ${I2CBUS} ${CH1} 0x0D
i2cget -y ${I2CBUS} ${CH1} 0x0E
i2cget -y ${I2CBUS} ${CH1} 0x0F

echo ""
echo "AK4499EX register dump"

i2cget -y ${I2CBUS} ${CHEX} 0x00
i2cget -y ${I2CBUS} ${CHEX} 0x01
i2cget -y ${I2CBUS} ${CHEX} 0x02

echo 1 > /sys/class/gpio/gpio${MUTE}/value

