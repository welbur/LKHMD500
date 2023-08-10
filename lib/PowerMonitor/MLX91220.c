/*
 * MXL91220.c
 *
 * Created: 2023.06.21
 *  Author: WU555
 */

#include "MLX91220.h"


float Default_AC_Freq       = 50.0;
float windowTimes          = 40.0;     // how long to average the signal, for statistist

/* 关于斜率(slope)和截距(intercept)
 * 首先你需要知道所有的 TRMS 计算都是由库中的函数完成的，它是“inputStats.sigma()”值
 * 然后，您可以显示“inputStats.sigma()”作为您的 TRMS 值，然后尝试在输入为 0.00A 时使用它进行测量
 * 如果测量值像我得到的那样为 0，您可以将截距保持为 0，否则您需要添加或减去以使该值等于 0
 * 换句话说“删除偏移量”
 * 然后将电源打开到已知值，例如使用知道其功率并且已经知道电压的灯泡或 LED，因此进行一点数学计算即可获得理论安培数
 * 将理论值除以测量值，得到斜率，现在放置或修改它们
 */
float intercept = 0;                    // to be adjusted based on calibration testing
float slope = 0.0752;                   // to be adjusted based on calibration testing

float sensitivity = 0.080;              //MLX91220KDC-ABF-025-RE的灵敏度为80mV/A

void MLX91220_Init(void)
{

}

void FiltersInit(void)
{
    LOG("fileters init \r\n");
    float windowLen = windowTimes / Default_AC_Freq;
    void *rStats = RunningStatistics_C_New();
    RunningStatistics_C_setWindowSecs(rStats, windowLen);
}
#if 0
float getAC_TRMS(float windowT, float )
{
    float windowLen = windowT / Default_AC_Freq;
    float *AmpsTrms;
    float Amps_TRMS;
    void *rStats = RunningStatistics_C_New();
	RunningStatistics_C_getACtrms(rStats, acValue, AmpsTrms);
    Amps_TRMS = intercept + slope * AmpsTrms[0];
}
#endif

#if 0
uint64_t GetusTime(void)
{
  // 我们需要记录老的时间值，因为不保证这个函数被调用的期间SysTick的中断不会被触发。
  static uint64_t OldTimeVal;
  uint64_t NewTimeVal;

  // 新的时间值以Tick计数为毫秒部分，以SysTick的计数器值换算为微秒部分，获得精确的时间。
  NewTimeVal = (HAL_GetTick() * 1000) + (SysTick->VAL * 1000 / SysTick->LOAD);

  // 当计算出来的时间值小于上一个时间值的时候，说明在函数计算的期间发生了SysTick中断，此时应该补正时间值。
  if (NewTimeVal < OldTimeVal) NewTimeVal += 1000;
  OldTimeVal = NewTimeVal;

  // 返回正确的时间值
  return NewTimeVal;
}
#endif
