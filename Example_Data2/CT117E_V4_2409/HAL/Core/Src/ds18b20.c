#include "main.h"

void DSB_us(uint32_t delay)
{
  delay *= 42;
  while (delay--);
}

void DSB_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
//LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
}

void DSB_Input(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
//LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* ����PA7Ϊ�������� */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
/*GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);*/
}

void DSB_Output(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
//LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* ����PA7Ϊͨ�ÿ�©��� */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
/*GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);*/
}

void DSB_Reset(void)
{
  DSB_Output();
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
//LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_6);
  /* ��ʱ720��s��480~960��s�� */
  DSB_us(720);
  /* �л�Ϊ����ģʽ */
  DSB_Input();
  DSB_us(1);
  /* �ȴ�DSB��Ӧ */
  while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6));
  while(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6));
/*while(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_6));
  while(!LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_6));*/
}

uint8_t DSB_Wrbit(uint8_t bit)
{
  DSB_Output();
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
//LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_6);
  DSB_us(1);
  /* ���bitΪ1�л�Ϊ����ģʽ������������������Ϊ�ߵ�ƽ���1��ͬʱ׼������ */
  if (bit) DSB_Input();
  /* ��ʱ������1��s��С��15��s�� */
  DSB_us(8);
  bit = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);
//bit = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_6);
  DSB_us(80);
  DSB_Input();
  return bit;
}

uint8_t DSB_Wrbyte(uint8_t byte)
{
  uint8_t i, bit;

  for (i=0; i<8; i++)
  {
    bit = DSB_Wrbit(byte & 1);
    byte >>= 1;
    if (bit) byte |= 0x80;
    DSB_us(1);
  }
  return byte;
}

uint16_t DSB_Read(void)
{
  uint8_t dsb_val[2];

  DSB_Reset();
  DSB_Wrbyte(0xCC);
  DSB_Wrbyte(0x44);

  DSB_Reset();
  DSB_Wrbyte(0xCC);
  DSB_Wrbyte(0xBE);

  dsb_val[0] = DSB_Wrbyte(0xFF);
  dsb_val[1] = DSB_Wrbyte(0xFF);
  return (dsb_val[1]<<8) + dsb_val[0];
}
