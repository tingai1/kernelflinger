/*
 * Copyright (c) 2014, Intel Corporation
 * All rights reserved.
 *
 * Author: Andrew Boie <andrew.p.boie@intel.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other materials provided with the
 *      distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <ui.h>

#include "lib.h"
#include "ux.h"
#include "vars.h"

#define TIMEOUT_SECS	60

#define RED_STATE_CODE		1
static const ui_textline_t red_state[] = {
	{ &COLOR_YELLOW,	"RECOVER",				TRUE },
	{ &COLOR_WHITE,		"Press Volume UP key",			FALSE },
	{ &COLOR_WHITE,		"",					FALSE },
	{ &COLOR_LIGHTRED,	"POWER OFF",				TRUE },
	{ &COLOR_WHITE,		"Press Volume DOWN key",		FALSE },
	{ &COLOR_WHITE,		"",					FALSE },
	{ &COLOR_LIGHTGRAY,	"Your device is unable to start",	FALSE },
	{ &COLOR_LIGHTGRAY,	"because the boot image has",		FALSE },
	{ &COLOR_LIGHTGRAY,	"failed to verify or is corrupted.",	FALSE },
	{ &COLOR_LIGHTGRAY,	"",					FALSE },
	{ &COLOR_LIGHTGRAY,	"You may attempt to recover",		FALSE },
	{ &COLOR_LIGHTGRAY,	"the device.",				FALSE },
	{ NULL, NULL, FALSE}
};

#define BAD_RECOVERY_CODE	2
static const ui_textline_t bad_recovery[] = {
	{ &COLOR_YELLOW,	"FASTBOOT",				TRUE },
	{ &COLOR_WHITE,		"Press Volume UP key",			FALSE },
	{ &COLOR_WHITE,		"",					FALSE },
	{ &COLOR_LIGHTRED,	"POWER OFF",				TRUE },
	{ &COLOR_WHITE,		"Press Volume DOWN key",		FALSE },
	{ &COLOR_WHITE,		"",					FALSE },
	{ &COLOR_LIGHTGRAY,	"Your device is unable to start",	FALSE },
	{ &COLOR_LIGHTGRAY,	"because the Recovery Console",		FALSE },
	{ &COLOR_LIGHTGRAY,	"image has failed to verify or is",	FALSE },
	{ &COLOR_LIGHTGRAY,	"corrupted.",				FALSE },
	{ &COLOR_LIGHTGRAY, 	"",					FALSE },
	{ &COLOR_LIGHTGRAY,	"You may repair your device with",	FALSE },
	{ &COLOR_LIGHTGRAY,	"Fastboot.",				FALSE },
	{ NULL, NULL, FALSE }
};

#define DEVICE_UNLOCKED_CODE	3
static const ui_textline_t device_altered_unlocked[] = {
	{ &COLOR_YELLOW, 	"START",				TRUE },
	{ &COLOR_WHITE, 	"Press Volume UP key",			FALSE },
	{ &COLOR_WHITE, 	"",					FALSE },
	{ &COLOR_LIGHTRED, 	"FASTBOOT",				TRUE },
	{ &COLOR_WHITE, 	"Press Volume DOWN key",		FALSE },
	{ &COLOR_WHITE, 	"",					FALSE },
	{ &COLOR_LIGHTRED, 	"WARNING:",				TRUE },
	{ &COLOR_LIGHTGRAY, 	"Your device has been altered",		FALSE },
	{ &COLOR_LIGHTGRAY, 	"from its factory configuration.",	FALSE },
	{ &COLOR_LIGHTGRAY, 	"and is no longer in a locked or",	FALSE },
	{ &COLOR_LIGHTGRAY, 	"verified state.",			FALSE },
	{ &COLOR_LIGHTGRAY, 	"",					FALSE },
	{ &COLOR_LIGHTGRAY, 	"If you were not responsible for",	FALSE },
	{ &COLOR_LIGHTGRAY, 	"these changes, the security of",	FALSE },
	{ &COLOR_LIGHTGRAY, 	"your device may be at risk.",		FALSE },
	{ &COLOR_LIGHTGRAY, 	"Choose \"FASTBOOT\" to change",	FALSE },
	{ &COLOR_LIGHTGRAY, 	"your device's state.",			FALSE },
	{ NULL, NULL, FALSE }
};

#define SECURE_BOOT_CODE	4
static const ui_textline_t secure_boot_off[] = {
	{ &COLOR_YELLOW,	"START",				TRUE },
	{ &COLOR_WHITE,		"Press Volume UP key",			FALSE },
	{ &COLOR_WHITE,		"",					FALSE },
	{ &COLOR_LIGHTRED,	"POWER OFF",				TRUE },
	{ &COLOR_WHITE,		"Press Volume DOWN key",		FALSE },
	{ &COLOR_WHITE,		"",					FALSE },
	{ &COLOR_LIGHTRED,	"WARNING:",				TRUE },
	{ &COLOR_LIGHTGRAY,	"Your device has been altered",		FALSE },
	{ &COLOR_LIGHTGRAY,	"from its factory configuration.",	FALSE },
	{ &COLOR_LIGHTGRAY,	"and is no longer in a locked or",	FALSE },
	{ &COLOR_LIGHTGRAY,	"verified state due to UEFI Secure",	FALSE },
	{ &COLOR_LIGHTGRAY,	"Boot being disabled.",			FALSE },
	{ &COLOR_LIGHTGRAY,	"",					FALSE },
	{ &COLOR_LIGHTGRAY,	"If you were not responsible for",	FALSE },
	{ &COLOR_LIGHTGRAY,	"these changes, the security of",	FALSE },
	{ &COLOR_LIGHTGRAY,	"your device may be at risk.",		FALSE },
	{ &COLOR_LIGHTGRAY,	"Please contact customer support",	FALSE },
	{ &COLOR_LIGHTGRAY,	"from your device's manufacturer.",	FALSE },
	{ NULL, NULL, FALSE }
};

#define KEYSTORE_ALTERED_CODE	5
static const ui_textline_t device_altered_keystore[] = {
	{ &COLOR_YELLOW,	"START",				TRUE },
	{ &COLOR_WHITE,		"Press Volume UP key",			FALSE },
	{ &COLOR_WHITE,		"",					FALSE },
	{ &COLOR_LIGHTRED,	"FASTBOOT",				TRUE },
	{ &COLOR_WHITE,		"Press Volume DOWN key",		FALSE },
	{ &COLOR_WHITE,		"",					FALSE },
	{ &COLOR_LIGHTRED,	"WARNING:",				TRUE },
	{ &COLOR_LIGHTGRAY,	"Your device has been altered",		FALSE },
	{ &COLOR_LIGHTGRAY,	"from its factory configuration.",	FALSE },
	{ &COLOR_LIGHTGRAY,	"",					FALSE },
	{ &COLOR_LIGHTGRAY,	"If you were not responsible for",	FALSE },
	{ &COLOR_LIGHTGRAY,	"these changes, the security of",	FALSE },
	{ &COLOR_LIGHTGRAY,	"your device may be at risk.",		FALSE },
	{ &COLOR_LIGHTGRAY,	"Choose \"FASTBOOT\" to clear",		FALSE },
	{ &COLOR_LIGHTGRAY,	"or upload a new user keystore.",	FALSE },
	{ &COLOR_LIGHTGRAY,	"",					FALSE },
	{ &COLOR_LIGHTGRAY,	"The device was unable to verify",	FALSE },
	{ &COLOR_LIGHTGRAY,	"the keystore with ID:",		FALSE },
	{ NULL, NULL, FALSE }
};

#define CRASH_EVENT_CODE	6
static const ui_textline_t crash_event_message[] = {
	{ &COLOR_LIGHTRED,	"WARNING:",				TRUE },
	{ &COLOR_LIGHTGRAY,	"Multiple crash events have been",	FALSE },
	{ &COLOR_LIGHTGRAY,	"reported.",				FALSE },
	{ &COLOR_LIGHTGRAY,	"",					FALSE },
	{ &COLOR_LIGHTGRAY,	"Use the above menu to select",		FALSE },
	{ &COLOR_LIGHTGRAY,	"the next boot option.",		FALSE },
	{ &COLOR_LIGHTGRAY,	"If the problem persists, please",	FALSE },
	{ &COLOR_LIGHTGRAY,	"contact the technical assistance.",	FALSE },
	{ NULL, NULL, FALSE }
};

static const char *VENDOR_IMG_NAME = "splash_intel";
static const char *LOW_BATTERY_IMG_NAME = "low_battery";

static UINTN swidth;
static UINTN sheight;
static UINTN wmargin;
static UINTN hmargin;

static EFI_STATUS ux_init_screen() {
	EFI_STATUS ret;

	ret = ui_init(&swidth, &sheight);
	if (EFI_ERROR(ret)) {
		efi_perror(ret, L"Failed to setup the graphical mode");
		return ret;
	}

	/* Use a 5 % screen margin. */
	wmargin = swidth / 20;
	hmargin = sheight / 20;

	return EFI_SUCCESS;
}

static ui_textline_t *build_error_code_text(UINT32 error_code)
{
	static char buf[26];
	static ui_textline_t code_text[] = {
		{ &COLOR_GREEN, buf, TRUE },
		{ &COLOR_WHITE, "", FALSE },
		{ NULL, NULL, FALSE }
	};

	snprintf((CHAR8 *)buf, sizeof(buf),
		 (CHAR8 *)"BOOTLOADER ERROR CODE %02x", error_code);

	return code_text;
}


static EFI_STATUS display_text(UINT32 error_code,
			       const ui_textline_t *text1,
			       const ui_textline_t *text2) {
	UINTN width, height, x, y, linesarea, colsarea;
	ui_image_t *vendor;
	EFI_STATUS ret;
	const ui_textline_t *texts[] =
		{ build_error_code_text(error_code),
		  text1, text2, NULL };

	ui_clear_screen();

	vendor = ui_image_get(VENDOR_IMG_NAME);
	if (!vendor) {
		efi_perror(EFI_UNSUPPORTED, L"Unable to load '%a' image",
			   VENDOR_IMG_NAME);
		return EFI_UNSUPPORTED;
	}

	if (swidth > sheight) {	/* Landscape orientation. */
		/* Display splash scaled on the left half of the screen,
		 * text area on the right */
		width = (swidth / 2) - (2 * wmargin);
		height = vendor->height * width / vendor->width;
		y = (sheight / 2) - (height / 2);
		ui_image_draw_scale(vendor, wmargin, y , width, height);
		x = swidth / 2 + wmargin;
	} else {		/* Portrait orientation. */
		/* Display splash on the top third of the screen,
		 * text area below it */
		height = sheight / 3;
		width = vendor->width * height / vendor->height;
		x = (swidth / 2) - (width / 2);
		y = hmargin;
		ui_image_draw_scale(vendor, x, y , width, height);
		y += height + hmargin;
	}

	colsarea = swidth - x - wmargin;
	linesarea = sheight - y - hmargin;

	ret = ui_display_texts(texts, x, y, linesarea, colsarea);
	if (EFI_ERROR(ret))
		return ret;

	return EFI_SUCCESS;
}

static EFI_STATUS clear_text() {
	if (swidth > sheight)	/* Landscape orientation. */
		return ui_clear_area(swidth / 2, hmargin,
				     swidth / 2, sheight - (2 * hmargin));
	/* Portrait orientation. */
	return ui_clear_area(0, sheight / 3 + hmargin,
			     swidth, sheight - (sheight / 3) - hmargin);
}

static BOOLEAN ux_display_splash() {
	UINT8 value;
	EFI_STATUS ret;

	ret = get_efi_variable_byte(&loader_guid, L"UIDisplaySplash", &value);
	if (EFI_ERROR(ret) || value != 1)
		return FALSE;

	return TRUE;
}

static BOOLEAN ux_prompt_user(UINT32 code, const ui_textline_t *text1,
			      const ui_textline_t *text2) {
	BOOLEAN answer;

	if (EFI_ERROR(ux_init_screen()))
		/* User won't be prompted.  Assume the answer is "yes".  */
		return TRUE;

	display_text(code, text1, text2);
	answer = ui_input_to_bool(TIMEOUT_SECS);
	clear_text();
	return answer;
}

BOOLEAN ux_prompt_user_keystore_unverified(UINT8 *hash) {
	char buf[15];
	const ui_textline_t hash_text[] = {
		{ &COLOR_WHITE, buf, FALSE },
		{ NULL, NULL, FALSE }
	};

	snprintf((CHAR8 *)buf, sizeof(buf),
		 (CHAR8 *)"%02x%02x-%02x%02x-%02x%02x",
		 hash[0], hash[1], hash[2], hash[3], hash[4], hash[5]);

	return ux_prompt_user(KEYSTORE_ALTERED_CODE, device_altered_keystore,
			      hash_text);
}

BOOLEAN ux_warn_user_unverified_recovery(VOID) {
	return ux_prompt_user(BAD_RECOVERY_CODE, bad_recovery, NULL);
}

BOOLEAN ux_prompt_user_bootimage_unverified(VOID) {
	return ux_prompt_user(RED_STATE_CODE, red_state, NULL);
}

BOOLEAN ux_prompt_user_secure_boot_off(VOID) {
	return ux_prompt_user(SECURE_BOOT_CODE, secure_boot_off, NULL);
}

BOOLEAN ux_prompt_user_device_unlocked(VOID) {
	return ux_prompt_user(DEVICE_UNLOCKED_CODE, device_altered_unlocked,
			      NULL);
}

static const char *CRASH_IMG_NAME = "crash_event";
static ui_boot_action_t BOOT_ACTIONS[] = {
	{ "start",		NULL,	NORMAL_BOOT },
	{ "bootloader",		NULL,	FASTBOOT },
	{ "recoverymode",	NULL,	RECOVERY },
	{ "reboot",		NULL,	REBOOT },
	{ "power_off",		NULL,	POWER_OFF },
	{ NULL,			NULL,	UNKNOWN_TARGET }
};

enum boot_target ux_crash_event_prompt_user_for_boot_target(VOID) {
	ui_image_t *img;
	ui_boot_menu_t *menu = NULL;
	UINTN width, height, img_x, img_y, area_x, area_y, colsarea, linesarea;
	EFI_STATUS ret = EFI_SUCCESS;
	enum boot_target target;
	const ui_textline_t *texts[] = { build_error_code_text(CRASH_EVENT_CODE),
					 crash_event_message, NULL };

	ret = ux_init_screen();
	if (EFI_ERROR(ret))
		/* User won't be able to make a choice.  Assume normal
		   boot flow.  */
		goto error;

	ui_clear_screen();

	ret = EFI_UNSUPPORTED;

	img = ui_image_get(CRASH_IMG_NAME);
	if (!img) {
		efi_perror(EFI_OUT_OF_RESOURCES,
			   L"Unable to load '%a' image",
			   CRASH_IMG_NAME);
		goto error;
	}

	if (swidth > sheight) {	/* Landscape orientation. */
		/* Display "failure" image scaled on the left half of
		 * the screen, boot menu on the right followed by
		 * the explanation text.  */
		width = (swidth / 2) - (2 * wmargin);
		height = img->height * width / img->width;
		img_x = wmargin;
		img_y = area_y = (sheight / 2) - (height / 2);
		area_x = img_x + swidth / 2;
	} else {		/* Portrait orientation. */
		/* Display "failure" image on the top third of the
		 * screen, boot menu below it followed by the
		 * explanation text.  */
		height = sheight / 3;
		width = img->width * height / img->height;
		img_x = (swidth / 2) - (width / 2);
		img_y = hmargin;
		area_x = wmargin;
		area_y = img_y + height + hmargin;
	}
	linesarea = sheight - area_y - hmargin;
	colsarea = swidth - area_x - wmargin;

	ret = ui_image_draw_scale(img, img_x, img_y, width, height);
	if (EFI_ERROR(ret))
		goto error;

	menu = ui_boot_menu_create(BOOT_ACTIONS);
	if (!menu) {
		error(L"Failed to build boot menu");
		goto error;
	}

	ret = ui_boot_menu_draw(menu, area_x, &area_y, colsarea);
	if (EFI_ERROR(ret))
		goto error;

	area_y += hmargin;

	ret = ui_display_texts(texts, area_x, area_y, linesarea, colsarea);
	if (EFI_ERROR(ret))
		goto error;

	uefi_call_wrapper(ST->ConIn->Reset, 2, ST->ConIn, FALSE);

	while (1) {
		target = ui_boot_menu_event_handler(menu,
						    ui_wait_for_input(TIMEOUT_SECS));
		if (target != UNKNOWN_TARGET) {
			ui_boot_menu_free(menu);
			ui_clear_screen();
			return target;
		}
	}

	halt_system();		/* Timer expired, turn-off the device. */

error:
	if (menu)
		ui_boot_menu_free(menu);

	return NORMAL_BOOT;
}


VOID ux_display_low_battery(UINTN delay) {
	ui_image_t *battery;
	EFI_STATUS ret;

	ret = ux_init_screen();
	if (EFI_ERROR(ret))
		return;

	ui_clear_screen();

	battery = ui_image_get(LOW_BATTERY_IMG_NAME);
	if (!battery) {
		efi_perror(EFI_NOT_FOUND, L"Failed to get '%a' image",
			   LOW_BATTERY_IMG_NAME);
		return;
	}

	ret = ui_image_draw(battery, (swidth / 2) - (battery->width / 2),
			    (sheight / 2) - (battery->height / 2));
	if (EFI_ERROR(ret))
		return;

	pause(delay);
}

VOID ux_init(VOID) {
	uefi_call_wrapper(ST->ConOut->Reset, 2, ST->ConOut, FALSE);
        uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut,
			  EFI_WHITE | EFI_BACKGROUND_BLACK);
	uefi_call_wrapper(ST->ConOut->EnableCursor, 2, ST->ConOut, FALSE);

	if (ux_display_splash()) {
		if (EFI_ERROR(ux_init_screen()))
			return;
		ui_display_vendor_splash();
	}
}
