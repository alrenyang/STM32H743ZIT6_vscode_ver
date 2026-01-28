/*
 * ax_mcu_if.c
 *
 *  Created on: Nov 22, 2025
 *      Author: User
 */


#include "ax_mcu_if.h"


void mcu_set_dout(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /*Configure GPIO pins : PE2 PE3 PE4 PE5 PE6 */
	  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	  /*Configure GPIO pins : PC13 PC14 PC15  */
	  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	  /*Configure GPIO pins : PF6 PF7 PF8 PF9 */
	  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	  /*Configure GPIO pins : PB12 PB14 PB15 */
	  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_15;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	  /*Configure GPIO pin : PD11 */
	  GPIO_InitStruct.Pin = GPIO_PIN_11;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}


void mcu_set_din(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /*Configure GPIO pins : PE2 PE3 PE4 PE5 PE6 */
	  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	  /*Configure GPIO pins : PC13 PC14 PC15 */
	  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	  /*Configure GPIO pins : PF6 PF7 PF8 PF9 */
	  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	  /*Configure GPIO pins : PB12 PB14 PB15 */
	  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_15;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	  /*Configure GPIO pin : PD11 */
	  GPIO_InitStruct.Pin = GPIO_PIN_11;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void mcu_set_data16(uint16_t data16)
{

	if(data16 & 0x0001)		HAL_GPIO_WritePin(F_DATA0_GPIO_Port, F_DATA0_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA0_GPIO_Port, F_DATA0_Pin, GPIO_PIN_RESET);

	if(data16 & 0x0002)		HAL_GPIO_WritePin(F_DATA1_GPIO_Port, F_DATA1_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA1_GPIO_Port, F_DATA1_Pin, GPIO_PIN_RESET);

	if(data16 & 0x0004)		HAL_GPIO_WritePin(F_DATA2_GPIO_Port, F_DATA2_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA2_GPIO_Port, F_DATA2_Pin, GPIO_PIN_RESET);

	if(data16 & 0x0008)		HAL_GPIO_WritePin(F_DATA3_GPIO_Port, F_DATA3_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA3_GPIO_Port, F_DATA3_Pin, GPIO_PIN_RESET);

	if(data16 & 0x0010)		HAL_GPIO_WritePin(F_DATA4_GPIO_Port, F_DATA4_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA4_GPIO_Port, F_DATA4_Pin, GPIO_PIN_RESET);

	if(data16 & 0x0020)		HAL_GPIO_WritePin(F_DATA5_GPIO_Port, F_DATA5_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA5_GPIO_Port, F_DATA5_Pin, GPIO_PIN_RESET);

	if(data16 & 0x0040)		HAL_GPIO_WritePin(F_DATA6_GPIO_Port, F_DATA6_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA6_GPIO_Port, F_DATA6_Pin, GPIO_PIN_RESET);

	if(data16 & 0x0080)		HAL_GPIO_WritePin(F_DATA7_GPIO_Port, F_DATA7_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA7_GPIO_Port, F_DATA7_Pin, GPIO_PIN_RESET);

	if(data16 & 0x0100)		HAL_GPIO_WritePin(F_DATA8_GPIO_Port, F_DATA8_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA8_GPIO_Port, F_DATA8_Pin, GPIO_PIN_RESET);

	if(data16 & 0x0200)		HAL_GPIO_WritePin(F_DATA9_GPIO_Port, F_DATA9_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA9_GPIO_Port, F_DATA9_Pin, GPIO_PIN_RESET);

	if(data16 & 0x0400)		HAL_GPIO_WritePin(F_DATA10_GPIO_Port, F_DATA10_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA10_GPIO_Port, F_DATA10_Pin, GPIO_PIN_RESET);

	if(data16 & 0x0800)		HAL_GPIO_WritePin(F_DATA11_GPIO_Port, F_DATA11_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA11_GPIO_Port, F_DATA11_Pin, GPIO_PIN_RESET);

	if(data16 & 0x1000)		HAL_GPIO_WritePin(F_DATA12_GPIO_Port, F_DATA12_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA12_GPIO_Port, F_DATA12_Pin, GPIO_PIN_RESET);

	if(data16 & 0x2000)		HAL_GPIO_WritePin(F_DATA13_GPIO_Port, F_DATA13_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA13_GPIO_Port, F_DATA13_Pin, GPIO_PIN_RESET);

	if(data16 & 0x4000)		HAL_GPIO_WritePin(F_DATA14_GPIO_Port, F_DATA14_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA14_GPIO_Port, F_DATA14_Pin, GPIO_PIN_RESET);

	if(data16 & 0x8000)		HAL_GPIO_WritePin(F_DATA15_GPIO_Port, F_DATA15_Pin, GPIO_PIN_SET);
	else					HAL_GPIO_WritePin(F_DATA15_GPIO_Port, F_DATA15_Pin, GPIO_PIN_RESET);

}


uint16_t mcu_get_data16(void)
{
	uint16_t ret16 = 0;

	if(HAL_GPIO_ReadPin(F_DATA0_GPIO_Port, F_DATA0_Pin) == GPIO_PIN_SET)	ret16 |= 0x0001;
	if(HAL_GPIO_ReadPin(F_DATA1_GPIO_Port, F_DATA1_Pin) == GPIO_PIN_SET)	ret16 |= 0x0002;
	if(HAL_GPIO_ReadPin(F_DATA2_GPIO_Port, F_DATA2_Pin) == GPIO_PIN_SET)	ret16 |= 0x0004;
	if(HAL_GPIO_ReadPin(F_DATA3_GPIO_Port, F_DATA3_Pin) == GPIO_PIN_SET)	ret16 |= 0x0008;
	if(HAL_GPIO_ReadPin(F_DATA4_GPIO_Port, F_DATA4_Pin) == GPIO_PIN_SET)	ret16 |= 0x0010;
	if(HAL_GPIO_ReadPin(F_DATA5_GPIO_Port, F_DATA5_Pin) == GPIO_PIN_SET)	ret16 |= 0x0020;
	if(HAL_GPIO_ReadPin(F_DATA6_GPIO_Port, F_DATA6_Pin) == GPIO_PIN_SET)	ret16 |= 0x0040;
	if(HAL_GPIO_ReadPin(F_DATA7_GPIO_Port, F_DATA7_Pin) == GPIO_PIN_SET)	ret16 |= 0x0080;
	if(HAL_GPIO_ReadPin(F_DATA8_GPIO_Port, F_DATA8_Pin) == GPIO_PIN_SET)	ret16 |= 0x0100;
	if(HAL_GPIO_ReadPin(F_DATA9_GPIO_Port, F_DATA9_Pin) == GPIO_PIN_SET)	ret16 |= 0x0200;
	if(HAL_GPIO_ReadPin(F_DATA10_GPIO_Port, F_DATA10_Pin) == GPIO_PIN_SET)	ret16 |= 0x0400;
	if(HAL_GPIO_ReadPin(F_DATA11_GPIO_Port, F_DATA11_Pin) == GPIO_PIN_SET)	ret16 |= 0x0800;
	if(HAL_GPIO_ReadPin(F_DATA12_GPIO_Port, F_DATA12_Pin) == GPIO_PIN_SET)	ret16 |= 0x1000;
	if(HAL_GPIO_ReadPin(F_DATA13_GPIO_Port, F_DATA13_Pin) == GPIO_PIN_SET)	ret16 |= 0x2000;
	if(HAL_GPIO_ReadPin(F_DATA14_GPIO_Port, F_DATA14_Pin) == GPIO_PIN_SET)	ret16 |= 0x4000;
	if(HAL_GPIO_ReadPin(F_DATA15_GPIO_Port, F_DATA15_Pin) == GPIO_PIN_SET)	ret16 |= 0x8000;

	return (ret16);
}

void mcu_set_fcs(uint8_t flag)
{
	if(flag==1)		HAL_GPIO_WritePin(F_CS_GPIO_Port, F_CS_Pin, GPIO_PIN_SET);
	else			HAL_GPIO_WritePin(F_CS_GPIO_Port, F_CS_Pin, GPIO_PIN_RESET);
}

void mcu_set_wen(uint8_t flag)
{
	if(flag==1)		HAL_GPIO_WritePin(F_WR_GPIO_Port, F_WR_Pin, GPIO_PIN_SET);
	else			HAL_GPIO_WritePin(F_WR_GPIO_Port, F_WR_Pin, GPIO_PIN_RESET);
}

void mcu_set_ren(uint8_t flag)
{
	if(flag==1)		HAL_GPIO_WritePin(F_RD_GPIO_Port, F_RD_Pin, GPIO_PIN_SET);
	else			HAL_GPIO_WritePin(F_RD_GPIO_Port, F_RD_Pin, GPIO_PIN_RESET);
}

void mcu_set_ale(uint8_t flag)
{
	if(flag==1)		HAL_GPIO_WritePin(F_ALE_GPIO_Port, F_ALE_Pin, GPIO_PIN_SET);
	else			HAL_GPIO_WritePin(F_ALE_GPIO_Port, F_ALE_Pin, GPIO_PIN_RESET);
}


void mcu_write16(uint16_t addr, uint16_t wdata16)
{
	// 1) w_addr
	mcu_set_dout();

	mcu_set_wen(1);
	mcu_set_ren(0);
	mcu_set_ale(0);
	mcu_set_data16(addr);
	mcu_set_fcs(1);
	mcu_set_fcs(0);

	// 2) w_data
	mcu_set_ale(1);
	mcu_set_data16(wdata16);
	mcu_set_fcs(1);
	mcu_set_fcs(0);
}


uint16_t mcu_read16(uint16_t addr)
{
	uint16_t rdata16 = 0;

	// 1) r_addr
	mcu_set_dout();

	mcu_set_wen(0);
	mcu_set_ren(1);
	mcu_set_ale(0);
	mcu_set_data16(addr);
	mcu_set_fcs(1);
	mcu_set_fcs(0);


	// 2) r_data
	mcu_set_din();

	mcu_set_ale(1);
	mcu_set_fcs(1);
	rdata16 = mcu_get_data16();
	mcu_set_fcs(0);

	return (rdata16);
}

void mcu_write_channel_con(uint16_t addr, pst_channel_con p_para)
{
	mcu_write16(addr, (uint16_t)p_para->trig_mode);
	mcu_write16(addr+1, p_para->delay);
	mcu_write16(addr+2, p_para->on);
	mcu_write16(addr+3, p_para->block);
}

void mcu_read_channel_con(uint16_t addr, pst_channel_con p_para)
{
	p_para->trig_mode = (uint8_t)mcu_read16(addr);
	p_para->delay     = mcu_read16(addr+1);
	p_para->on        = mcu_read16(addr+2);
	p_para->block     = mcu_read16(addr+3);
}


/*
void mcu_write16(uint16_t addr, uint16_t wdata16)
{

	mcu_set_fcs(1);		HAL_Delay(1);

	// 1) w_addr
	mcu_set_dout();		HAL_Delay(1);

	mcu_set_wen(1);		HAL_Delay(1);
	mcu_set_ren(0);		HAL_Delay(1);
	mcu_set_ale(0);		HAL_Delay(1);

	mcu_set_data16(addr);		HAL_Delay(1);

	// 2) w_data
	mcu_set_ale(1);		HAL_Delay(1);
	mcu_set_data16(wdata16);		HAL_Delay(1);

	mcu_set_fcs(0);		HAL_Delay(1);
}


uint16_t mcu_read16(uint16_t addr)
{
	uint16_t rdata16 = 0;

	mcu_set_fcs(1);		HAL_Delay(1);

	// 1) r_addr
	mcu_set_dout();		HAL_Delay(1);

	mcu_set_wen(0);		HAL_Delay(1);
	mcu_set_ren(1);		HAL_Delay(1);
	mcu_set_ale(0);		HAL_Delay(1);

	mcu_set_data16(addr);		HAL_Delay(1);


	// 2) r_data
	mcu_set_fcs(0);		HAL_Delay(1);

	mcu_set_din();		HAL_Delay(1);

	mcu_set_ale(1);		HAL_Delay(1);
	mcu_set_fcs(1);		HAL_Delay(1);
	rdata16 = mcu_get_data16();		HAL_Delay(1);

	mcu_set_fcs(0);		HAL_Delay(1);

	return (rdata16);
}
*/


/*
void mcu_write16(uint16_t addr, uint16_t wdata16)
{
	// 1) w_addr

	mcu_set_fcs(0);		HAL_Delay(1);

	mcu_set_dout();		HAL_Delay(1);

	mcu_set_wen(1);		HAL_Delay(1);
	mcu_set_ren(0);		HAL_Delay(1);
	mcu_set_ale(0);		HAL_Delay(1);

	mcu_set_data16(addr);		HAL_Delay(1);
	mcu_set_fcs(1);		HAL_Delay(1);


	// 2) w_data
	mcu_set_fcs(0);		HAL_Delay(1);

	mcu_set_ale(1);		HAL_Delay(1);
	mcu_set_data16(wdata16);		HAL_Delay(1);
	mcu_set_fcs(1);		HAL_Delay(1);

	mcu_set_fcs(0);		HAL_Delay(1);
}


uint16_t mcu_read16(uint16_t addr)
{
	uint16_t rdata16 = 0;

	// 1) r_addr

	mcu_set_fcs(0);		HAL_Delay(1);

	mcu_set_dout();		HAL_Delay(1);

	mcu_set_wen(0);		HAL_Delay(1);
	mcu_set_ren(1);		HAL_Delay(1);
	mcu_set_ale(0);		HAL_Delay(1);

	mcu_set_data16(addr);		HAL_Delay(1);
	mcu_set_fcs(1);		HAL_Delay(1);


	// 2) r_data
	mcu_set_fcs(0);		HAL_Delay(1);

	mcu_set_din();		HAL_Delay(1);

	mcu_set_ale(1);		HAL_Delay(1);
	mcu_set_fcs(1);		HAL_Delay(1);
	rdata16 = mcu_get_data16();		HAL_Delay(1);


	mcu_set_fcs(0);		HAL_Delay(1);

	return (rdata16);
}
*/
