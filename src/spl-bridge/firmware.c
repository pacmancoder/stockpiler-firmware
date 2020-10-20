#include <pdk/device.h>
#include <pdk-utils/calibrate.h>
#include <pdk-utils/delay.h>

#define UART_BAUD_RATE 9600
// Add UART_BAUD_RATE / 2 to round up or down
#define UART_STEP_DELAY ((F_CPU + UART_BAUD_RATE / 2) / UART_BAUD_RATE)

void uart_set_low() {
    PA &= (~0x01);
}

void uart_set_high() {
    PA |= 0x01;
}

void uart_send_char(uint8_t ch) {
        // start bit
        uart_set_low();
        _delay_cycles(UART_STEP_DELAY);

        for (int i = 0; i < 8;  ++i) {
            if (ch & 0x01) {
                uart_set_high();
            } else {
                uart_set_low();
            }
            _delay_cycles(UART_STEP_DELAY);
            ch >>= 1;
        }

        // stop bit
        uart_set_high();
        _delay_cycles(UART_STEP_DELAY);
}

void main(void) {

    // Set PA0 as output
    PAPH = 0;
    PA = 0x01; // Initial state as high
    PAC = 0x01;

	MISC = 0b00100000;  // Enable fast wake up

	for(;;) {
        uart_send_char('H');
        uart_send_char('e');
        uart_send_char('l');
        uart_send_char('l');
        uart_send_char('o');
        uart_send_char('!');
        uart_send_char('\n');
        _delay_ms(100);
    }
}

unsigned char _sdcc_external_startup(void) {
	#if F_CPU == 8000000
    	PDK_USE_8MHZ_IHRC_SYSCLOCK();
		EASY_PDK_CALIBRATE_IHRC(8000000, 5000);
	#else
		#error "Invalid F_CPU"
	#endif

  	return 0; 
}