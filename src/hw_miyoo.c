/* 
 * Gmu Music Player
 *
 * Copyright (c) 2006-2011 Johannes Heimansberg (wejp.k.vu)
 *
 * File: hw_miyoo.c  Created: 090629
 *
 * Description: Hardware specific header file for unknown devices (such as PCs)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 of
 * the License. See the file COPYING in the Gmu's main directory
 * for details.
 */
#include <stdio.h>
#include "oss_mixer.h"
#include "debug.h"
#include "hw_miyoo.h"

static int selected_mixer = -1;
static int display_on_value = 100;

static void set_display_state(const char *state) { //added because the image continues to be generated even with the screen off
    FILE *f;
    if ((f = fopen("/proc/mi_modules/fb/mi_fb0", "w"))) {
        fprintf(f, "GUI_SHOW %s\n", state);
        fclose(f);
    }
}

void hw_display_off(void) 
{
	char  tmp[5];
	FILE *f;
	int   display_on_value_tmp = 0;

	wdprintf(V_DEBUG, "hw_miyoo", "Display off requested.\n");
	if ((f = fopen("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "r"))) { /* Old kernel */
		if (fgets(tmp, 4, f)) display_on_value_tmp = atoi(tmp);
		fclose(f);
		if (display_on_value_tmp > 0) display_on_value = display_on_value_tmp;
		if ((f = fopen("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "w"))) {
			fprintf(f, "0\n");
			fclose(f);
		}
	}
	set_display_state("off");
}

void hw_display_on(void)
{
	FILE *f;

	wdprintf(V_DEBUG, "hw_miyoo", "Display on requested.\n");
	if (display_on_value > 0) {
		if ((f = fopen("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "w"))) {
			fprintf(f, "%d\n", display_on_value);
			fclose(f);
		}
	}
	set_display_state("on");
}

int hw_open_mixer(int mixer_channel)
{
#ifndef GMU_DISABLE_OSS_MIXER
	int res = oss_mixer_open();
	selected_mixer = mixer_channel;
	wdprintf(V_INFO, "hw_miyoo", "Selected mixer: %d\n", selected_mixer);
	return res;
#else
	return 0;
#endif
}

void hw_close_mixer(void)
{
#ifndef GMU_DISABLE_OSS_MIXER
	oss_mixer_close();
#endif
}

void hw_set_volume(int volume)
{
#ifndef GMU_DISABLE_OSS_MIXER
	if (selected_mixer >= 0) {
		if (volume >= 0) oss_mixer_set_volume(selected_mixer, volume);
	} else {
		wdprintf(V_INFO, "hw_miyoo", "No suitable mixer available.\n");
	}
#endif
}

void hw_detect_device_model(void)
{
}

const char *hw_get_device_model_name(void)
{
	return "Miyoo Mini";
}
