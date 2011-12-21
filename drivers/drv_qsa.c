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
#ifdef HAVE_PTHREAD
#include <pthread.h>
#endif

#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_SYS_ASOUNDLIB_H
#include <sys/asoundlib.h>
#endif

#define DEFAULT_NUMFRAGS 8

#ifdef MIKMOD_DEBUG
//#define QNX_MIKMOD_PROFILING
#endif

#ifdef QNX_MIKMOD_PROFILING
#include <time.h>
#include <float.h>
#endif

static snd_pcm_t *playback_handle = NULL;
static int fragmentsize, numfrags = DEFAULT_NUMFRAGS;
static SBYTE *audiobuffer = NULL;
#if QNX_MIKMOD_PLAY_THREAD
static int pcm_fd = 0;
static pthread_t player_thread;
#endif
static int num_devices;

#ifdef QNX_MIKMOD_PROFILING
#define STATS_COUNT 1000
typedef double stat_type;
static unsigned int dbg_num_writes = 0;
static unsigned int dbg_num_prepares = 0;
static unsigned int dbg_num_rewrites = 0;
static stat_type dbg_time_mixing[STATS_COUNT];
static stat_type dbg_time_select[STATS_COUNT];
static stat_type dbg_time_write[STATS_COUNT];
static stat_type dbg_time_status[STATS_COUNT];
static stat_type dbg_time_prepare[STATS_COUNT];

static void dbg_stats(stat_type data[],
						unsigned int size,
						stat_type *pmini,
						stat_type *pmaxi,
						stat_type *pavg,
						stat_type *total) {
	unsigned int i;
	stat_type avg = 0, mini = DBL_MAX, maxi = DBL_MIN;
	for(i = 0; i < size; i++){
		mini = min(data[i], mini);
		maxi = max(data[i], maxi);
		avg += data[i];
	}
	*pmini = mini;
	*pmaxi = maxi;
	*total = avg;
	*pavg = avg / (stat_type)size;
}

static void prn_stats() {
	if((dbg_num_writes % STATS_COUNT) == STATS_COUNT-1){
		dbg_stats(dbg_time_mixing,
				STATS_COUNT,
				&dbg_time_mixing[0],
				&dbg_time_mixing[1],
				&dbg_time_mixing[2],
				&dbg_time_mixing[3]);
		dbg_stats(dbg_time_select,
				STATS_COUNT,
				&dbg_time_select[0],
				&dbg_time_select[1],
				&dbg_time_select[2],
				&dbg_time_select[3]);
		dbg_stats(dbg_time_write,
				STATS_COUNT,
				&dbg_time_write[0],
				&dbg_time_write[1],
				&dbg_time_write[2],
				&dbg_time_write[3]);
		dbg_stats(dbg_time_status,
				STATS_COUNT,
				&dbg_time_status[0],
				&dbg_time_status[1],
				&dbg_time_status[2],
				&dbg_time_status[3]);
		dbg_stats(dbg_time_status,
				STATS_COUNT,
				&dbg_time_status[0],
				&dbg_time_status[1],
				&dbg_time_status[2],
				&dbg_time_status[3]);
		dbg_stats(dbg_time_prepare,
				STATS_COUNT,
				&dbg_time_prepare[0],
				&dbg_time_prepare[1],
				&dbg_time_prepare[2],
				&dbg_time_prepare[3]);

		fprintf(stderr, "\nWrite count=%d, prepares=%d, rewrites=%d\n",
				dbg_num_writes+1,
				dbg_num_prepares,
				dbg_num_rewrites);
		fprintf(stderr, "Select:  min=%9.6f, max=%9.6f, avg=%9.6f, total=%9.6f\n",
						dbg_time_select[0],
						dbg_time_select[1],
						dbg_time_select[2],
						dbg_time_select[3]);
		fprintf(stderr, "Mixing:  min=%9.6f, max=%9.6f, avg=%9.6f, total=%9.6f\n",
						dbg_time_mixing[0],
						dbg_time_mixing[1],
						dbg_time_mixing[2],
						dbg_time_mixing[3]);
		fprintf(stderr, "Write:   min=%9.6f, max=%9.6f, avg=%9.6f, total=%9.6f\n",
						dbg_time_write[0],
						dbg_time_write[1],
						dbg_time_write[2],
						dbg_time_write[3]);
		fprintf(stderr, "Status:  min=%9.6f, max=%9.6f, avg=%9.6f, total=%9.6f\n",
						dbg_time_status[0],
						dbg_time_status[1],
						dbg_time_status[2],
						dbg_time_status[3]);
		fprintf(stderr, "Prepare: min=%9.6f, max=%9.6f, avg=%9.6f, total=%9.6f\n",
						dbg_time_prepare[0],
						dbg_time_prepare[1],
						dbg_time_prepare[2],
						dbg_time_prepare[3]);

		dbg_num_prepares = 0;
		dbg_num_rewrites = 0;
	}
	dbg_num_writes++;
}

static stat_type diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return (stat_type)temp.tv_sec + (stat_type)temp.tv_nsec / 1000000000.;
}
#endif

static void qsa_update(void);
#if QNX_MIKMOD_PLAY_THREAD
static void* player_thread_function(void* data);
#endif

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

#if QNX_MIKMOD_PLAY_THREAD
	pcm_fd = snd_pcm_file_descriptor(playback_handle, SND_PCM_CHANNEL_PLAYBACK);
#endif

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

	fragmentsize = channel_info.max_fragment_size;

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

	fragmentsize = channel_setup.buf.block.frag_size;
	if (!(audiobuffer = (SBYTE*)MikMod_malloc(fragmentsize))) {
		snd_pcm_close(playback_handle);
		playback_handle = NULL;
		return 1;
	}

	err = VC_Init();
	return err;
}

static BOOL qsa_init(void) {
	return qsa_init_impl();
}

#ifdef QNX_MIKMOD_PLAY_THREAD
static void* player_thread_function(void* data) {
	int err;
	fd_set wset;
#ifdef QNX_MIKMOD_PROFILING
	struct timespec select_start, select_end;
#endif
	while(1){
		FD_ZERO(&wset);
		FD_SET(pcm_fd, &wset);
#ifdef QNX_MIKMOD_PROFILING
		clock_gettime(CLOCK_MONOTONIC, &select_start);
#endif
		err = select(1 + pcm_fd, NULL, &wset, NULL, NULL);
#ifdef QNX_MIKMOD_PROFILING
		clock_gettime(CLOCK_MONOTONIC, &select_end);
		dbg_time_select[dbg_num_writes % STATS_COUNT] = diff(select_start, select_end);
#endif
		if(err == -1){
#if MIKMOD_DEBUG
			fprintf(stderr, "select: %s\n", strerror(err));
#endif
		} else if FD_ISSET(pcm_fd, &wset) {
			qsa_update();
		} else {
#if MIKMOD_DEBUG
			fprintf(stderr, "select: unexpected return %d\n", err);
#endif
		}
#ifdef QNX_MIKMOD_PROFILING
		prn_stats();
#endif
	}
	return 0;
}
#endif

static void qsa_exit_impl(void) {
	VC_Exit();
	if (playback_handle) {
		snd_pcm_playback_drain(playback_handle);
		snd_pcm_close(playback_handle);
		playback_handle = NULL;
	}
#if QNX_MIKMOD_PLAY_THREAD
	pthread_join(player_thread, NULL);
	pcm_fd = 0;
#endif
	MikMod_free(audiobuffer);
	audiobuffer = NULL;
	num_devices = 0;
}

static void qsa_exit(void) {
	qsa_exit_impl();
}

#ifdef QNX_MIKMOD_PLAY_THREAD
static void qsa_update_dummy(void){}
#endif

static void qsa_update(void) {
	int err;
	snd_pcm_channel_status_t status;
#ifdef QNX_MIKMOD_PROFILING
	struct timespec write_start, write_end,
					mix_start, mix_end,
					status_start, status_end,
					prepare_start, prepare_end;
	clock_gettime(CLOCK_MONOTONIC, &mix_start);
#endif
	int count = VC_WriteBytes(audiobuffer, fragmentsize);
#ifdef QNX_MIKMOD_PROFILING
	clock_gettime(CLOCK_MONOTONIC, &mix_end);
	dbg_time_mixing[dbg_num_writes % STATS_COUNT] = diff(mix_start, mix_end);
	clock_gettime(CLOCK_MONOTONIC, &write_start);
#endif
	err = snd_pcm_plugin_write(playback_handle,
								audiobuffer,
								count);
#ifdef QNX_MIKMOD_PROFILING
	clock_gettime(CLOCK_MONOTONIC, &write_end);
	dbg_time_write[dbg_num_writes % STATS_COUNT] = diff(write_start, write_end);
#endif
	if(err < fragmentsize) {
		memset(&status, 0, sizeof(status));
		status.channel = SND_PCM_CHANNEL_PLAYBACK;
#ifdef QNX_MIKMOD_PROFILING
		clock_gettime(CLOCK_MONOTONIC, &status_start);
#endif
		if ((err = snd_pcm_plugin_status(playback_handle, &status)) < 0) {
#ifdef MIKMOD_DEBUG
			fprintf(stderr,"snd_pcm_plugin_status: %s\n", snd_strerror(err));
#endif
		} else {
#ifdef QNX_MIKMOD_PROFILING
			clock_gettime(CLOCK_MONOTONIC, &status_end);
			dbg_time_status[dbg_num_writes % STATS_COUNT] = diff(status_start, status_end);
#endif
			if (status.status == SND_PCM_STATUS_READY ||
				status.status == SND_PCM_STATUS_UNDERRUN) {
#ifdef QNX_MIKMOD_PROFILING
				dbg_num_prepares++;
				clock_gettime(CLOCK_MONOTONIC, &prepare_start);
#endif
				if (snd_pcm_plugin_prepare(playback_handle, SND_PCM_CHANNEL_PLAYBACK) < 0) {
#ifdef MIKMOD_DEBUG
					fprintf(stderr,"snd_pcm_plugin_prepare: %s\n", snd_strerror(err));
#endif
				}
#ifdef QNX_MIKMOD_PROFILING
				clock_gettime(CLOCK_MONOTONIC, &prepare_end);
				dbg_time_prepare[dbg_num_writes % STATS_COUNT] = diff(prepare_start, prepare_end);
				dbg_num_rewrites++;
				clock_gettime(CLOCK_MONOTONIC, &write_start);
#endif
				err = snd_pcm_plugin_write(playback_handle,
											audiobuffer,
											count);
#ifdef MIKMOD_DEBUG
				if(err!=fragmentsize) {
					fprintf(stderr,"snd_pcm_plugin_write: %d (%s)\n", err, snd_strerror(err));
				}
#endif
#ifdef QNX_MIKMOD_PROFILING
				clock_gettime(CLOCK_MONOTONIC, &write_end);
				dbg_time_write[dbg_num_writes % STATS_COUNT] = diff(write_start, write_end);
#endif
			} else {
				fprintf(stderr,"snd_pcm_plugin_write: status %d)\n", status.status);
#ifdef QNX_MIKMOD_PROFILING
				dbg_time_prepare[dbg_num_writes % STATS_COUNT] = 0;
#endif
			}
		}
	}
}

static void qsa_play_stop(void) {
	snd_pcm_playback_flush(playback_handle);
	VC_PlayStop();
}

static BOOL qsa_reset(void) {
	qsa_exit_impl();
	return qsa_init_impl();
}

static BOOL qsa_play_start(void){
#ifdef QNX_MIKMOD_PLAY_THREAD
	int err;
	if((err = pthread_create(&player_thread, NULL, player_thread_function, NULL)) != EOK) {
#if MIKMOD_DEBUG
		fprintf(stderr, "pthread_create: %s\n", strerror(err));
#endif
		return 1;
	}
#endif
	return VC_PlayStart();
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
	qsa_play_start,
	qsa_play_stop,
#if QNX_MIKMOD_PLAY_THREAD
	qsa_update_dummy,
#else
	qsa_update,
#endif
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

