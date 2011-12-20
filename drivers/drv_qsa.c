/*	MikMod sound library
 (c) 1998, 1999, 2000 Miodrag Vallat and others - see file AUTHORS for
 complete list.

 This library is free software; you can redistribute it and/or modify
 it under the terms of the GNU Library General Public License as
 published by the Free Software Foundation; either version 2 of
 the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Library General Public License for more details.

 You should have received a copy of the GNU Library General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 02111-1307, USA.
 */

/*==============================================================================

 Driver for QNX Sound Architecture (QSA)

 ==============================================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mikmod_internals.h"

#ifdef DRV_QSA

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_SYS_ASOUNDLIB_H
#include <sys/asoundlib.h>
#endif

#define DEFAULT_NUMFRAGS 4

static snd_pcm_t *playback_handle = NULL;
static int fragmentsize, numfrags = DEFAULT_NUMFRAGS;
static SBYTE *audiobuffer = NULL;

static int num_devices;

static void qsa_command_line(CHAR *cmdline) {
	CHAR *ptr;

	if ((ptr = MD_GetAtom("buffer", cmdline, 0))) {
		numfrags = atoi(ptr);
		if ((numfrags < 2) || (numfrags > 16)) {
			numfrags = DEFAULT_NUMFRAGS;
		}
		MikMod_free(ptr);
	} else {
		numfrags = DEFAULT_NUMFRAGS;
	}
}

static BOOL qsa_is_there(void) {
	int ret_val;
	int *cards = NULL, *devices  = NULL;
	ret_val = num_devices = 8; /* start with eight */
	cards = (int*)MikMod_malloc(sizeof(int) * num_devices);
	devices = (int*)MikMod_malloc(sizeof(int) * num_devices);
	memset(cards, 0, sizeof(sizeof(int) * num_devices));
	memset(devices, 0, sizeof(sizeof(int) * num_devices));
	ret_val = snd_pcm_find(SND_PCM_FMT_S16_LE,
							&num_devices,
							cards,
							devices,
							SND_PCM_OPEN_PLAYBACK);
#if MIKMOD_DEBUG
	fprintf(stderr, "Number of devices: %d\n", num_devices);
#endif
	MikMod_free(cards);
	MikMod_free(devices);
	return ret_val > 0 && num_devices > 0;
}

static BOOL qsa_init_impl(void) {
	int err;
	int bps;
	int size;
	int card, device;
	snd_pcm_format_t 			pcm_format;
	snd_pcm_channel_info_t		channel_info;
	snd_pcm_channel_params_t	channel_params;
	snd_pcm_channel_setup_t 	channel_setup;

	_mm_errno = MMERR_OPENING_AUDIO;
	if(num_devices == 0) {
		return 1;
	}

	memset(&pcm_format, 0, sizeof(pcm_format));
#ifdef SND_LITTLE_ENDIAN
	pcm_format.format = (md_mode & DMODE_16BITS) ? SND_PCM_SFMT_S16_LE : SND_PCM_SFMT_U8;
#else
	pcm_format.format = (md_mode & DMODE_16BITS) ? SND_PCM_SFMT_S16_BE : SND_PCM_SFMT_U8;
#endif
	pcm_format.voices = (md_mode & DMODE_STEREO) ? 2 : 1;
	pcm_format.rate = md_mixfreq = 48000;

#if MIKMOD_DEBUG
	fprintf(stderr, "PCM format:\n");
	fprintf(stderr, "voices: %d, rate: %d, format: %d\n",
			pcm_format.voices,
			pcm_format.rate,
			pcm_format.format);
#endif

	if((err = snd_pcm_open_preferred(&playback_handle,
									&card,
									&device,
									SND_PCM_OPEN_PLAYBACK)) < 0) {
#if MIKMOD_DEBUG
		fprintf(stderr, "snd_pcm_open_preferred: %s\n", snd_strerror(err));
#endif
	} else {
#if MIKMOD_DEBUG
		fprintf(stderr, "snd_pcm_open_preferred: success [card:%d, device:%d]\n", card, device);
#endif
	}

	memset(&channel_info, 0, sizeof(channel_info));
	channel_info.channel = SND_PCM_CHANNEL_PLAYBACK;
	/* Get information about a PCM channel's capabilities from a control handle */
	if ((err = snd_pcm_channel_info(playback_handle, &channel_info)) < 0) {
#if MIKMOD_DEBUG
		fprintf(stderr, "snd_ctl_pcm_channel_info: %s\n", snd_strerror(err));
#endif
		return 1;
	}
#if MIKMOD_DEBUG
	fprintf(stderr, "PCM channel info for device %d:\n", device);
	fprintf(stderr, "subdevice:  %d\n", channel_info.subdevice);
	fprintf(stderr, "subname:    %s\n", channel_info.subname);
	fprintf(stderr, "channel:    %d\n", channel_info.channel);
	fprintf(stderr, "flags:      %d\n", channel_info.flags);
	fprintf(stderr, "formats:    %d\n", channel_info.formats);
	fprintf(stderr, "rates:      %d\n", channel_info.rates);
	fprintf(stderr, "min_rate:   %d\n", channel_info.min_rate);
	fprintf(stderr, "max_rate:   %d\n", channel_info.max_rate);
	fprintf(stderr, "min_voices: %d\n", channel_info.min_voices);
	fprintf(stderr, "max_voices: %d\n", channel_info.max_voices);
	fprintf(stderr, "max_buffer_size: %d\n", channel_info.max_buffer_size);
	fprintf(stderr, "min_fragment_size: %d\n", channel_info.min_fragment_size);
	fprintf(stderr, "max_fragment_size: %d\n", channel_info.max_fragment_size);
	fprintf(stderr, "fragment_align: %d\n", channel_info.fragment_align);
	fprintf(stderr, "fifo_size:  %d\n", channel_info.fifo_size);
#endif
	if (channel_info.min_rate > pcm_format.rate ||
		channel_info.max_rate < pcm_format.rate ||
		channel_info.max_voices < pcm_format.voices ||
		!(channel_info.formats & (1 << pcm_format.format))) {
		return 1;
	}

	fragmentsize = channel_info.max_fragment_size / numfrags;

	if ((err = snd_pcm_plugin_set_disable(playback_handle,
											PLUGIN_DISABLE_BUFFER_PARTIAL_BLOCKS)) < 0) {
#if MIKMOD_DEBUG
		fprintf(stderr, "snd_pcm_plugin_set_disable: %s\n", snd_strerror(err));
#endif
		snd_pcm_close(playback_handle);
		playback_handle = NULL;
		return 1;
	}

	if ((err = snd_pcm_plugin_set_disable(playback_handle,
											PLUGIN_DISABLE_MMAP)) < 0) {
#if MIKMOD_DEBUG
		fprintf(stderr, "snd_pcm_plugin_set_disable: %s\n", snd_strerror(err));
#endif
		snd_pcm_close(playback_handle);
		playback_handle = NULL;
		return 1;
	}

	/* Set the configurable parameters for a PCM channel (plugin-aware) */
	memset(&channel_params, 0, sizeof(channel_params));
	channel_params.channel = SND_PCM_CHANNEL_PLAYBACK;
	channel_params.mode = SND_PCM_MODE_BLOCK;
	channel_params.format.interleave = 1;
	channel_params.format.format = pcm_format.format;
	channel_params.format.rate = pcm_format.rate;
	channel_params.format.voices = pcm_format.voices;
	channel_params.start_mode = SND_PCM_START_FULL;
	channel_params.stop_mode = SND_PCM_STOP_STOP;
	channel_params.buf.block.frag_size = fragmentsize;
	channel_params.buf.block.frags_max = numfrags - 1;
	channel_params.buf.block.frags_min = 1;

	strcpy(channel_params.sw_mixer_subchn_name, "MikMod Player");

	/* Set the configurable parameters for a PCM channel (plugin-aware) */
	if ((err = snd_pcm_plugin_params(playback_handle, &channel_params)) < 0) {
#if MIKMOD_DEBUG
		fprintf(stderr, "snd_pcm_plugin_params: %s\n", snd_strerror(err));
#endif
		snd_pcm_close(playback_handle);
		playback_handle = NULL;
		return 1;
	}

	/* Signal the driver to ready the specified channel (plugin-aware) */
	if ((err = snd_pcm_plugin_prepare(playback_handle, SND_PCM_CHANNEL_PLAYBACK)) < 0) {
#if MIKMOD_DEBUG
		fprintf(stderr, "snd_pcm_plugin_prepare: %s\n", snd_strerror(err));
#endif
		snd_pcm_close(playback_handle);
		playback_handle = NULL;
		return 1;
	}

	/* Get the current configuration for the specified PCM channel (plugin aware) */
	memset(&channel_setup, 0, sizeof(channel_setup));
	channel_setup.mode = SND_PCM_MODE_BLOCK;
	channel_setup.channel = SND_PCM_CHANNEL_PLAYBACK;

	if ((err = snd_pcm_plugin_setup(playback_handle, &channel_setup)) < 0) {
#if MIKMOD_DEBUG
		fprintf(stderr, "snd_pcm_plugin_setup: %s\n", snd_strerror(err));
#endif
		snd_pcm_close(playback_handle);
		playback_handle = NULL;
		return 1;
	}

	if (!(audiobuffer = (SBYTE*)MikMod_malloc(4 * fragmentsize))) {
		snd_pcm_close(playback_handle);
		playback_handle = NULL;
		return 1;
	}

	return VC_Init();
}

static BOOL qsa_init(void) {
	return qsa_init_impl();
}

static void qsa_exit_impl(void) {
	VC_Exit();
	if (playback_handle) {
		snd_pcm_playback_drain(playback_handle);
		snd_pcm_close(playback_handle);
		playback_handle = NULL;
	}
	MikMod_free(audiobuffer);
	audiobuffer = NULL;
	num_devices = 0;
}

static void qsa_exit(void) {
	qsa_exit_impl();
}

static void qsa_update(void) {
	int err;
	snd_pcm_channel_status_t status;
	err = snd_pcm_plugin_write(playback_handle,
								audiobuffer,
								VC_WriteBytes(audiobuffer, fragmentsize));
	if(err < fragmentsize) {
		memset(&status, 0, sizeof(status));
		status.channel = SND_PCM_CHANNEL_PLAYBACK;
		if ((err = snd_pcm_plugin_status(playback_handle, &status)) < 0) {
#ifdef MIKMOD_DEBUG
			fprintf(stderr,"snd_pcm_plugin_status: %s\n", snd_strerror(err));
#endif
		} else {
			if (status.status == SND_PCM_STATUS_READY ||
				status.status == SND_PCM_STATUS_UNDERRUN) {
				if (snd_pcm_plugin_prepare (playback_handle, SND_PCM_CHANNEL_PLAYBACK) < 0) {
#ifdef MIKMOD_DEBUG
					fprintf(stderr,"snd_pcm_plugin_prepare: %s\n", snd_strerror(err));
#endif
				}
				err = snd_pcm_plugin_write(playback_handle,
											audiobuffer,
											VC_WriteBytes(audiobuffer, fragmentsize));
#ifdef MIKMOD_DEBUG
				if(err!=fragmentsize) {
					fprintf(stderr,"snd_pcm_plugin_write: %d (%s)\n", err, snd_strerror(err));
				}
#endif
			} else {
				fprintf(stderr,"snd_pcm_plugin_write: status %d)\n", status.status);
			}
		}
	}
}

static void qsa_play_stop(void) {
	VC_PlayStop();
	snd_pcm_playback_flush(playback_handle);
}

static BOOL qsa_reset(void) {
	qsa_exit_impl();
	return qsa_init_impl();
}

MIKMODAPI MDRIVER drv_qsa = {
	NULL,
	"QSA",
	"QNX Sound Architecture (QSA) driver v1.0",
	0,
	255,
	"qsa",
	"buffer:r:2,16,4:Number of buffer fragments\n",
	qsa_command_line,
	qsa_is_there,
	VC_SampleLoad,
	VC_SampleUnload,
	VC_SampleSpace,
	VC_SampleLength,
	qsa_init,
	qsa_exit,
	qsa_reset,
	VC_SetNumVoices,
	VC_PlayStart,
	qsa_play_stop,
	qsa_update,
	NULL,
	VC_VoiceSetVolume,
	VC_VoiceGetVolume,
	VC_VoiceSetFrequency,
	VC_VoiceGetFrequency,
	VC_VoiceSetPanning,
	VC_VoiceGetPanning,
	VC_VoicePlay,
	VC_VoiceStop,
	VC_VoiceStopped,
	VC_VoiceGetPosition,
	VC_VoiceRealVolume
};

#else
#if 0
MISSING(drv_qsa);
#endif
#endif

