#include "control.h"
#include "EEPROM.h"
#include "string.h"
#include "tim.h"
#include "usart.h"

volatile uint16_t counter;
volatile int32_t current_position;

int32_t target_position = 0;
uint16_t max_pwm = 50;  // Giá tr? PWM t?i da (b?n có th? di?u ch?nh)
uint16_t pwm_value;
uint8_t number = 0;
void init(void){
	//HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start_IT(&htim1, TIM_CHANNEL_ALL);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);	
}

void run(void){
	//target_position = number * pluse_max/99;
	//control_motor_by_position(target_position);
	//HAL_Delay(200);
	//send_uart_data();
	control_motor(50, motor_up);
	HAL_Delay(5000);
	control_motor(0, motor_off);
	HAL_Delay(1000);
	control_motor(50, motor_dw);
	HAL_Delay(5000);
	control_motor(0, motor_off);
	HAL_Delay(1000);
}
//void receive_uart_data()
void send_uart_data(void){
	uint8_t data_send[buf_max];
	static uint32_t data_crc;
	data_send[0] = byte_start;
	data_send[1] = byte_adress;
	data_send[2] = 10;
	data_send[3] = 15;
	data_crc = 0;
	for(uint8_t i = 0; i < buf_max - 1; i ++){
		data_crc += data_send[i];
		data_crc = data_crc % 250;
	}
	data_send[4] = data_crc;
	
	if(HAL_UART_Transmit_IT(&huart1, data_send, sizeof(data_send)) != HAL_OK){
			Error_Handler();
	}
}
void control_motor_by_position(int32_t target_position) {
	int32_t position_error = target_position - current_position;
	uint8_t direction;

	if(position_error > 0){
		pwm_value = (position_error > max_pwm) ? max_pwm : position_error;
		direction = motor_up;
	}
	else if(position_error < 0){
		pwm_value = (-position_error > max_pwm) ? max_pwm : -position_error;
		direction = motor_dw;
	}
	else{
		pwm_value = 0;
		direction = motor_off;
	}
	control_motor(pwm_value, direction);
}

void control_motor(uint16_t _data_pwm, uint8_t _direct){
	static uint16_t _pre_data_pwm = 0;
	//if(_direct != _pre_direct){
		//_pre_direct = _direct;
		if(_direct == motor_up){
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, _data_pwm);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
		}
		else if(_direct == motor_dw){
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, _data_pwm);
		}
		else{
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
		}
	//}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){	
	if(htim->Instance == TIM1){
		counter = __HAL_TIM_GET_COUNTER(&htim1);
		static uint16_t last_counter = 0;
		int16_t delta = counter - last_counter;		
		if(delta < 0){
			delta += 65536;
		}
		current_position += delta;
		last_counter = counter;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART1){
		uint8_t data_buf[buf_max + 1];
		static uint32_t data_crc;
		HAL_UART_Receive_IT(&huart1, (uint8_t*)data_buf, buf_max);
		for(uint8_t i = 0; i < buf_max; i ++){
			data_crc += data_buf[i];
			data_crc = data_crc % 250;
		}
		if(data_crc == data_buf[buf_max]){
			
		}
	}
}
