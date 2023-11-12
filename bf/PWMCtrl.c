/*
 * PWMCtrl.c
 *
 * Created: 2023.08.11
 *  Author: WU555
 */

#include "PWMCtrl.h"

uint16_t _AC_Cycle = 1000 / AC_Freq;
uint16_t _PWM_Cycle = 1000;       //默认的周期为1s
uint16_t _PWM_duty = 50;          //默认占空比为50%
uint32_t lastTS = 0;

/*pwm的周期必须是交流电频率的倍数， 周期单位为ms， 例如：100 即 100ms*/
uint8_t setPWM_Cycle(uint8_t cycle) {
    if (cycle % _AC_Cycle != 0) return 0;
    _PWM_Cycle = cycle;
    return 1;
}

/*设置pwm的占空比，范囥[0,100]，单位为%，例如：50 即 50%占空比*/
uint8_t setPWM_duty(uint8_t duty) {
    if (duty < 0 || duty > 100) return 0;
    _PWM_duty = duty;
    return 1;
}



