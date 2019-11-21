#include <stdio.h>
#include "contiki.h"
#include "net/rime.h"
#include "dev/leds.h"
#include "dev/cc2420.h"

#include "clicker.h"

static struct etimer et;

/* Declare our "main" process, the basestation_process */
PROCESS(basestation_process, "Clicker basestation");
PROCESS(led_off_process, "led off");
/* The basestation process should be started automatically when
 * the node has booted. */
AUTOSTART_PROCESSES(&basestation_process, &led_off_process);

/* Holds the number of packets received. */


/* Callback function for received packets.
 *
 * Whenever this node receives a packet for its broadcast handle,
 * this function will be called.
 */
static void recv(struct broadcast_conn *c, const rimeaddr_t *from){ 	
	leds_on(LEDS_GREEN);
	printf("in rec \n");
	process_poll(&led_off_process);
}

/* Broadcast handle to receive and send (identified) broadcast
 * packets. */
static struct broadcast_conn bc;
/* A structure holding a pointer to our callback function. */
static struct broadcast_callbacks bc_callback = { recv };

/* Our main process. */
PROCESS_THREAD(basestation_process, ev, data) {
	PROCESS_BEGIN();
	//etimer_set(&et, CLOCK_SECOND);	
	leds_on(0b001);
	//etimer_set(&et, CLOCK_SECOND);
	/* Open the broadcast handle, use the rime channel
	 * defined by CLICKER_CHANNEL. */
	broadcast_open(&bc, CLICKER_CHANNEL, &bc_callback);
	/* Set the radio's channel to IEEE802_15_4_CHANNEL */
	cc2420_set_channel(IEEE802_15_4_CHANNEL);
	/* Set the radio's transmission power. */
	cc2420_set_txpower(CC2420_TX_POWER);
	/* That's all we need to do. Whenever a packet is received,
	 * our callback function will be called. */	
	PROCESS_END();
	
}


// led process
PROCESS_THREAD(led_off_process, ev, data){
	PROCESS_BEGIN();
	while(1){
		PROCESS_WAIT_EVENT();	
		if (ev == PROCESS_EVENT_POLL){
			etimer_set(&et, CLOCK_SECOND);
		}		
		else{
			leds_off(LEDS_ALL);
			printf("off \n");
	}
}	
	PROCESS_END();
}

