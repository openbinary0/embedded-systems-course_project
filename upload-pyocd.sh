#!/usr/bin/env sh

exec pyocd load \
	--target stm32l432kc \
	--frequency 4000khz \
	-O resume_on_disconnect=false \
	"$1"
