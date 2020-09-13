#ifndef _SPL_UTILS_H_
#define _SPL_UTILS_H_

void reset_timer16_counter(void);

void send_command(void);
void send_ack_command(void);
void send_btn_command(void);

void calculate_command_checksum(void);
void update_tx_checksum(void);

void update_leds(void);
void disable_rgb_led(void);

#endif // _SPL_UTILS_H_