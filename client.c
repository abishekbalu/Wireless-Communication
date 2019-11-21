#include <stdio.h>
#include "contiki.h"
#include "net/rime.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/cc2420.h"
#include "clicker.h"
#include "dev/adxl345.h"

#define ACCM_READ_INTERVAL    CLOCK_SECOND
#define PROCESS_EVENT_SHAKEN 110
/* Declare our "main" process, the client process*/
PROCESS(client_process, "Clicker client");
/* The client process should be started automatically when
 * the node has booted. */
PROCESS(accel_process, "Test Accel process");
AUTOSTART_PROCESSES(&client_process, &accel_process);

/* Callback function for received packets.
 *
 * Whenever this node receives a packet for its broadcast handle,
 * this function will be called.
 *
 * As the client does not need to receive, the function does not do anything
 */
static void recv(struct broadcast_conn *c, const rimeaddr_t *from) {
}

/* Broadcast handle to receive and send (identified) broadcast
 * packets. */
static struct broadcast_conn bc;
/* A structure holding a pointer to our callback function. */
static struct broadcast_callbacks bc_callback = { recv };

/* Our main process. */
PROCESS_THREAD(client_process, ev, data) {
	PROCESS_BEGIN();


	broadcast_open(&bc, CLICKER_CHANNEL, &bc_callback);
	/* Set the radio's channel to IEEE802_15_4_CHANNEL */
	cc2420_set_channel(IEEE802_15_4_CHANNEL);
	
	/* Set the radio's transmission power. */
	cc2420_set_txpower(CC2420_TX_POWER);

	/* Loop forever. */
	while (1) {
		/* Wait until an event occurs. If the event has
		 * occured, ev will hold the type of event, and
		 * data will have additional information for the
		 * event. In the case of a sensors_event, data will
		 * point to the sensor that caused the event.
		 * Here we wait until the button was pressed. */
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_SHAKEN);

		leds_toggle(LEDS_RED);
		/* Copy the string "hej" into the packet buffer. */
		packetbuf_copyfrom("hej", 4);
		/* Send the content of the packet buffer using the
		 * broadcast handle. */
		broadcast_send(&bc);
	}

	/* This will never be reached, but we'll put it here for
	 * the sake of completeness: Close the broadcast handle. */
	broadcast_close(&bc);
	PROCESS_END();
}

/* Main process, setups  */

static struct etimer et;

PROCESS_THREAD(accel_process, ev, data) {
	PROCESS_BEGIN();
	{
		static int16_t x;
		static int16_t y=0;
		static int result;
		accm_init();
		while(1) {
			x = accm_read_axis(X_AXIS);
			printf(" \n x: %d \n", x);
			result = abs(x-y);
				
			if (result > 10) {
				printf("Send packet \n");
				process_post(&client_process, PROCESS_EVENT_SHAKEN, NULL);	
				}
			y = x;
			etimer_set(&et, ACCM_READ_INTERVAL);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		}
	}

PROCESS_END();
}

		
