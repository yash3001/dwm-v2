#!/bin/sh


speed=$(/home/yash/.config/dwm/dwmblocks/dwmblock_scripts/netspeed.py -d "wlo1" -t "1" | awk 'OFS="   " {print $1, $2}')
icon=''
printf " %s %s \\n" "$icon" "$speed"
