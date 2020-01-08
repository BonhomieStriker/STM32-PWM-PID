#include "sys.h"
#include "usart.h"	  
#include "math.h" 
void uart_init(u32 bound){
    //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟以及复用功能时钟
	 //USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

 //Usart1 NVIC 配置

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART1, ENABLE);                    //使能串口 

}


void USART1_PrintI(long int data,unsigned char newline)
{
	unsigned char temp[12], i = 0;
	if(data < 0)
	{
	   data = - data;
	   while((USART1->SR & 0X40) == 0);
	   USART1->DR = '-';                   //发送负号
	}
	if(data == 0)
	{
	   while((USART1->SR &0X40) == 0);  
	   USART1->DR = 48;
	} 
	else
	{
		while(data != 0)  
		{	
			i++;
			temp[i] = data % 10 + 48; //将待发送数据逐位转化为ASCII码发送
			data = data / 10;         
		}
		for( ; i > 0 ; i--)
		{
		   while((USART1->SR & 0X40) == 0);        //等待发送完成
		   USART1->DR = temp[i];
		}
	}
	
	if(newline == 1)     //依次发送 \r \n
	{
		while((USART1->SR & 0X40) == 0){};    USART1->DR = 13;    
		while((USART1->SR & 0X40) == 0){};    USART1->DR = 10	;   
	}
	
	 
}

void USART1_PrintF(double data,unsigned char precision,unsigned char newline)
{
	unsigned char temp[12], i = 0, j ;
	unsigned char Add_Flag = 0;

	if(data < 0)
	{
	   data = - data;
	   while((USART1->SR & 0X40) == 0);
	   USART1->DR = '-';                   //发送负号
		
	}
	if(data == 0)
	{
	   while((USART1->SR &0X40) == 0); 
	   USART1->DR = 48;
	}
	else
	{		
			for(j = 0; j < precision; j ++)
					data = data * 10;  
			if(data<pow(10,precision))
			{
				data = data + pow(10,precision);    // 处理有效数字在小数点后若干位的情况 -> 个位 + 1   显示时再 - 1
				Add_Flag = 1;
			}				
			
			while((int)data != 0)  
			{	
				i++;
				temp[i] = (int)data % 10 + 48;               //将待发送数据逐位转化为ASCII码发送
				if( i == precision )    i ++ , temp[i] = '.';
				if( i == precision + 2 && Add_Flag == 1)   temp[i] = temp[i] - 1; 
				data = data / 10;              
			}
			
			for( ; i > 0 ; i--)
			{
				 while((USART1->SR & 0X40) == 0);        //等待发送完成
				 USART1->DR = temp[i];
			}
		
	}
	if(newline == 1)     //依次发送 \r \n
	{
		while((USART1->SR & 0X40) == 0){};    USART1->DR = 13;    
		while((USART1->SR & 0X40) == 0){};    USART1->DR = 10	;   
	}
		
}
unsigned char Res;
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
			  		 
  } 
}
