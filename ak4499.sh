#!/bin/sh
#
#	Copyright (C) 2017-2019 blue-7 (http://qiita.com/blue-7)

#NanoPi-NEO2
PDN="199"
MUTE="200"
I2CBUS="0"

#RaspberryPi Zero
#PDN="4"
#MUTE="23"
#I2CBUS="1"



# DAC I2C ADDR
CH0=${CH0:=0x10}



# 255=0db, 254=-0.5dB,,, 231=-12dB
VOLUME=${VOLUME:=255}

# Digital Filter
#   0 : Sharp roll-off filter
#   1 : Slow roll-off filter
#   2 : Short delay sharp roll off filter (AK449x default)
#   3 : Short delay slow roll off filter
#   4 : Super Slow roll off filter (Non Over Sampling mode) 
DF=${DF:=0}

# Operation Mode
#   0 : Single Stereo
#   1 : Dual Stereo
#   2 : Dual Monaural Unbalance (0x10:Lch, 0x11:Rch)
#   3 : Dual Monaural Balance (0x10:Lch, 0x11:Rch, Lch:Positive, Rch:Negative)
MODE=${MODE:=0}


# Gain Cotnrol
#		PCM		DSD		DSD bypass
#	0 : 72.8mA	72.8mA	45.5mA
#	1 : 72.8mA	45.5mA	45.5mA
#	2 : 45.5mA	45.5mA	45.5mA
#	3 : 45.5mA	45.5mA	45.5mA


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
		SD=0
		SLOW=0
		SSLOW=0
	;;
	1) echo "DF  : Slow roll-off filter" 
		SD=0
		SLOW=1
		SSLOW=0
	;;
	2) echo "DF  : Short delay sharp roll off filter (AK449x default)" 
		SD=0x20
		SLOW=0
		SSLOW=0
	;;
	3) echo "DF  : Short delay slow roll off filter" 
		SD=0x20
		SLOW=1
		SSLOW=0
	;;
	4) echo "DF  : Super Slow roll off filter (Non Over Sampling mode)" 
		SD=0
		SLOW=0
		SSLOW=1
	;;
esac

case $GC in
	0) REG07=0x00
	;;
	1) REG07=0x02
	;;
	2) REG07=0x04
	;;
	3) REG07=0x06
	;;
esac

REG00=$((0x9F))
REG01=`expr $((0x02)) + $SD`
REG02=`expr $((0x00)) + $SLOW`
REG05=`expr $((0x00)) + $SSLOW`

#REG08=$((0x08))
REG0B=$((0x00))


i2cset -y ${I2CBUS} ${CH0} 0x00 0


if [ ${MODE} -eq 2 ]
then
	echo "MODE: Dual Monaural - Unbalance output"
	i2cset -y ${I2CBUS} ${CH0} 0x01 $REG01
	i2cset -y ${I2CBUS} ${CH0} 0x02 `expr $((0x0A)) + $REG02`
	i2cset -y ${I2CBUS} ${CH0} 0x05 `expr $((0x00)) + $REG05`
	i2cset -y ${I2CBUS} ${CH0} 0x0B `expr $((0x20)) + $REG0B`
elif [ ${MODE} -eq 1 ]
then
	echo "MODE: Dual Stereo"
	i2cset -y ${I2CBUS} ${CH0} 0x01 $REG01
	i2cset -y ${I2CBUS} ${CH0} 0x02 $REG02
	i2cset -y ${I2CBUS} ${CH0} 0x05 $REG05
else
	echo "MODE: Dual Monaural - Balance output"
	i2cset -y ${I2CBUS} ${CH0} 0x01 $REG01
	i2cset -y ${I2CBUS} ${CH0} 0x02 `expr $((0x0A)) + $REG02`
	i2cset -y ${I2CBUS} ${CH0} 0x05 `expr $((0x50)) + $REG05`
	i2cset -y ${I2CBUS} ${CH0} 0x0B `expr $((0x20)) + $REG0B`
fi

i2cset -y ${I2CBUS} ${CH0} 0x07 $REG07
i2cset -y ${I2CBUS} ${CH0} 0x08 $REG08

i2cset -y ${I2CBUS} ${CH0} 0x03 ${VOLUME}
i2cset -y ${I2CBUS} ${CH0} 0x04 ${VOLUME}
i2cset -y ${I2CBUS} ${CH0} 0x0C ${VOLUME}
i2cset -y ${I2CBUS} ${CH0} 0x0D ${VOLUME}

i2cset -y ${I2CBUS} ${CH0} 0x00 $REG00




echo ""
echo "AK4499 register dump"

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


echo 1 > /sys/class/gpio/gpio${MUTE}/value

