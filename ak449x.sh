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
CH1=${CH1:=0x11}


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


REG00=$((0x8F))
REG01=`expr $((0x02)) + $SD`
REG02=`expr $((0x00)) + $SLOW`
REG05=`expr $((0x00)) + $SSLOW`

REG08=$((0xC0))



if [ ${MODE} -gt 0 ]
then
        i2cset -y ${I2CBUS} ${CH0} 0x01 $REG01
        i2cset -y ${I2CBUS} ${CH1} 0x01 $REG01
else
        i2cset -y ${I2CBUS} ${CH0} 0x01 $REG01
fi



if [ ${MODE} -eq 3 ]
then
	echo "MODE: Dual Monaural - Balance output"
	i2cset -y ${I2CBUS} ${CH0} 0x01 $REG01
	i2cset -y ${I2CBUS} ${CH1} 0x01 $REG01
	i2cset -y ${I2CBUS} ${CH0} 0x02 `expr $((0x08)) + $REG02`
	i2cset -y ${I2CBUS} ${CH1} 0x02 `expr $((0x0A)) + $REG02`
	i2cset -y ${I2CBUS} ${CH0} 0x05 `expr $((0x40)) + $REG05`
	i2cset -y ${I2CBUS} ${CH1} 0x05 `expr $((0x40)) + $REG05`
elif [ ${MODE} -eq 2 ]
then
	echo "MODE: Dual Monaural - Unbalance output"
	i2cset -y ${I2CBUS} ${CH0} 0x01 $REG01
	i2cset -y ${I2CBUS} ${CH1} 0x01 $REG01
	i2cset -y ${I2CBUS} ${CH0} 0x02 `expr $((0x08)) + $REG02`
	i2cset -y ${I2CBUS} ${CH1} 0x02 `expr $((0x0A)) + $REG02`
	i2cset -y ${I2CBUS} ${CH0} 0x05 `expr $((0x00)) + $REG05`
	i2cset -y ${I2CBUS} ${CH1} 0x05 `expr $((0x00)) + $REG05`
elif [ ${MODE} -eq 1 ]
then
	echo "MODE: Dual Stereo"
	i2cset -y ${I2CBUS} ${CH0} 0x01 $REG01
	i2cset -y ${I2CBUS} ${CH0} 0x02 $REG02
	i2cset -y ${I2CBUS} ${CH0} 0x05 $REG05

	i2cset -y ${I2CBUS} ${CH1} 0x01 $REG01
	i2cset -y ${I2CBUS} ${CH1} 0x02 $REG02
	i2cset -y ${I2CBUS} ${CH1} 0x05 $REG05
else
	echo "MODE: Single Stereo"
	i2cset -y ${I2CBUS} ${CH0} 0x01 $REG01
	i2cset -y ${I2CBUS} ${CH0} 0x02 $REG02
	i2cset -y ${I2CBUS} ${CH0} 0x05 $REG05
fi

if [ ${MODE} -gt 0 ]
then
	i2cset -y ${I2CBUS} ${CH0} 0x08 $REG08
	i2cset -y ${I2CBUS} ${CH1} 0x08 $REG08

	i2cset -y ${I2CBUS} ${CH0} 0x03 ${VOLUME}
	i2cset -y ${I2CBUS} ${CH0} 0x04 ${VOLUME}
	i2cset -y ${I2CBUS} ${CH1} 0x03 ${VOLUME}
	i2cset -y ${I2CBUS} ${CH1} 0x04 ${VOLUME}
	i2cset -y ${I2CBUS} ${CH0} 0x00 $REG00
	i2cset -y ${I2CBUS} ${CH1} 0x00 $REG00
else
	i2cset -y ${I2CBUS} ${CH0} 0x08 $REG08

	i2cset -y ${I2CBUS} ${CH0} 0x03 ${VOLUME}
	i2cset -y ${I2CBUS} ${CH0} 0x04 ${VOLUME}
	i2cset -y ${I2CBUS} ${CH0} 0x00 $REG00
fi



echo ""
echo "AK449x register dump"

if [ ${MODE} -gt 0 ]
then
	i2cget -y ${I2CBUS} ${CH0} 0x00
	i2cget -y ${I2CBUS} ${CH1} 0x00
	i2cget -y ${I2CBUS} ${CH0} 0x01
	i2cget -y ${I2CBUS} ${CH1} 0x01
	i2cget -y ${I2CBUS} ${CH0} 0x02
	i2cget -y ${I2CBUS} ${CH1} 0x02
	i2cget -y ${I2CBUS} ${CH0} 0x03
	i2cget -y ${I2CBUS} ${CH1} 0x03
	i2cget -y ${I2CBUS} ${CH0} 0x04
	i2cget -y ${I2CBUS} ${CH1} 0x04
	i2cget -y ${I2CBUS} ${CH0} 0x05
	i2cget -y ${I2CBUS} ${CH1} 0x05
else
	i2cget -y ${I2CBUS} ${CH0} 0x00
	i2cget -y ${I2CBUS} ${CH0} 0x01
	i2cget -y ${I2CBUS} ${CH0} 0x02
	i2cget -y ${I2CBUS} ${CH0} 0x03
	i2cget -y ${I2CBUS} ${CH0} 0x04
	i2cget -y ${I2CBUS} ${CH0} 0x05
fi



echo 1 > /sys/class/gpio/gpio${MUTE}/value

