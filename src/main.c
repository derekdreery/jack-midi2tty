/*
    Copyright (C) 2004 Ian Esten

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <jack/jack.h>
#include <jack/midiport.h>

static jack_port_t *input_port;

int process(jack_nframes_t nframes, void *arg)
{
    int i, j;
    void* port_buf = jack_port_get_buffer(input_port, nframes);
    jack_midi_event_t in_event;
    jack_nframes_t event_count = jack_midi_get_event_count(port_buf);
    if(event_count > 1)
    {
        printf(" midi2tty: have %d events\n", event_count);
        for(i=0; i<event_count; i++)
        {
            jack_midi_event_get(&in_event, port_buf, i);
            printf("    event %d time is %d. data is hex( ", i, in_event.time);
			for(j=0; j<in_event.size; j++) {
				printf("%x ", in_event.buffer[j]);
			}
			printf(")\n");
        }
    }
    return 0;      
}

void jack_shutdown(void *arg)
{
    exit(1);
}

int main(int narg, char **args)
{
    jack_client_t *client;

    if ((client = jack_client_open ("midi2tty", JackNullOption, NULL)) == 0)
    {
        fprintf(stderr, "jack server not running?\n");
        return 1;
    }

    jack_set_process_callback (client, process, 0);
    jack_on_shutdown (client, jack_shutdown, 0);

    input_port = jack_port_register (client, "midi_in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);

    if (jack_activate (client))
    {
        fprintf(stderr, "cannot activate client");
        return 1;
    }

    /* run until interrupted */
    while(1)
    {
        sleep(1);
    }
    jack_client_close(client);
    exit (0);
}
