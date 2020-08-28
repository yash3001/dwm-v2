//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/*Icon*/	/*Command*/		/*Update Interval*/	/*Update Signal*/
	//{"", "cat ~/.pacupdate | sed /📦0/d",					0,		9},
	
	//{"🧠", "free -h | awk '/^Mem/ { print $3\"/\"$2 }' | sed s/i//g",	30,		0},

	//{"", "~/bin/statusbar/volume",						0,		10},

	//{"☀", "xbacklight | sed 's/\\..*//'",					0,		11},
	
	//{"", "~/bin/statusbar/battery",						5,		0},

	//{"🌡", "sensors | awk '/^temp1:/{print $2}'",				5,		0},

	//{"", "~/bin/statusbar/clock",						5,		0},

    {"", "/home/yash/.config/dwm/dwmblocks/dwmblock_scripts/netspeed.sh", 2, 0},

    {"", "/home/yash/.config/dwm/dwmblocks/dwmblock_scripts/battery.sh", 10, 0},

    {"", "/home/yash/.config/dwm/dwmblocks/dwmblock_scripts/sound.sh", 0, 11},

    {"", "/home/yash/.config/dwm/dwmblocks/dwmblock_scripts/pacman.sh", 1800, 0},

    {"", "/home/yash/.config/dwm/dwmblocks/dwmblock_scripts/time.sh", 1, 0}
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim = '|';
