#include "my_lib.h"

/**
  * @brief  比较两个数组前面的n个数组是否一致
  * @note   None
  * @param  p1_buf：要比较的数组1
  *			p2_buf：要比较的数组2
  *         len：   要比较的长度(前面的len个字符)
  * @retval 返回0： 代表两个数组前面len个字符，是一致的
  *			返回-1：代表两个数组前面len个字符，是不一致的 
  */
int8_t MY_LIB_CmpArray(int8_t* p1_buf, int8_t* p2_buf, uint16_t len)
{
	int16_t i = 0;
	
	for(i=0; i<len; i++)
	{
		if(p1_buf[i] != p2_buf[i])
			return -1;
	}
	return 0;
}


/**
  * @brief  比较两个数组前面的n个数组是否一致
  * @note   None
  * @param  p1_buf：要清空的数组
  *         len：   要清空的数组的长度
  * @retval None
  */
void MY_LIB_ClearArray(int8_t* p_buf, uint16_t len)
{
	int16_t i = 0;
	
	for(i=0; i<len; i++)
	{
		p_buf[i] = 0;
	}
}
