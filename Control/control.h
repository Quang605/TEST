#include "main.h"

//UART
#define		byte_start		0xFE
#define		byte_adress		1
#define		buf_max				5

#define		motor_off		0
#define		motor_up		1
#define		motor_dw		2

//dong co 44 xung 1 vòng nho voi bo chia i488
#define		pluse_max_1vong		21472		//1 vong lon qua hop so 44 x 488 = 21472
#define		pluse_max					64500		//1 chieu

void init(void);
void run(void);
void send_uart_data(void);
void control_motor_by_position(int32_t target_position);
void control_motor(uint16_t _data_pwm, uint8_t _direct);
