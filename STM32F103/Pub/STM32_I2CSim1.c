#include <Main.h>


//-----------------------------------------------------------------------------
static void SetSCLtoLow(void)
{
    GPIOD->BRR = GPIO_Pin_1;
}

static void SetSCLtoHigh(void)
{
    GPIOD->BSRR = GPIO_Pin_1;
}

static void SetSDAtoLow(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIOD->BRR = GPIO_Pin_2;
}

static void SetSDAtoHigh(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIOD->BSRR = GPIO_Pin_2;
}

static void SetSDAtoInput(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

static BYTE GetSDAStatus(void)
{
    return (GPIOD->IDR&GPIO_Pin_2)?1:0;
}


//-----------------------------------------------------------------------------
//Write Start Bit:
//    ������������        ��
//SDA:��        ������������
//    ��  ��������������  ��
//SCL:������          ������
static void WriteStartBit(void)
{
	SetSCLtoLow();
	SetSDAtoHigh();
	DelayOnce(5);
	SetSCLtoHigh();
	DelayOnce(10);
	SetSDAtoLow();
	DelayOnce(10);
	SetSCLtoLow();
}

//Write Stop Bit:
//    ��        ������������
//SDA:������������        ��
//    ��  ��������������  ��
//SCL:������          ������
static void WriteStopBit(void)
{
	SetSCLtoLow();
	SetSDAtoLow();
	DelayOnce(5);
	SetSCLtoHigh();
	DelayOnce(10);
	SetSDAtoHigh();
	DelayOnce(10);
	SetSCLtoLow();
}

//Write Answer Bit:
//    ��                  ��
//SDA:����������������������
//    ��  ��������������  ��
//SCL:������          ������
static void WriteAnswerBit(void)
{
	SetSCLtoLow();
	SetSDAtoLow();
	DelayOnce(5);
	SetSCLtoHigh();
	DelayOnce(20);
	SetSCLtoLow();
}

//Write No Answer Bit:
//    ����������������������
//SDA:��                  ��
//    ��  ��������������  ��
//SCL:������          ������
static void WriteNoAnswerBit(void)
{
	SetSCLtoLow();
	SetSDAtoHigh();
	DelayOnce(5);
	SetSCLtoHigh();
	DelayOnce(20);
	SetSCLtoLow();
}

//Write Answer Bit:
//    ��  i               ��
//SDA:������������xxxx������
//    ��  ��������������  ��
//SCL:������          ������
static BYTE ReadAnswerBit(void)
{
	WORD wCount = 100;
	SetSCLtoLow();
	DelayOnce(5);
	SetSDAtoInput();
	SetSCLtoHigh();
	DelayOnce(10);
	while(GetSDAStatus() && wCount--);
	DelayOnce(5);
	SetSCLtoLow();
	return (wCount==0)?0:1;
}

static BYTE ReadByte(BOOL bLastByte)
{
    WORD i;
	BYTE Data = 0;
	SetSCLtoLow();
	DelayOnce(10);
	SetSDAtoInput();
	for(i=0;i<8;i++)
	{
		SetSCLtoLow();
		DelayOnce(5);
		SetSCLtoHigh();
		DelayOnce(10);
		if(GetSDAStatus()) Data |= (0x80>>i);
		else Data &= (0xff^(0x80>>i));
		DelayOnce(5);
	}
	if(bLastByte) WriteNoAnswerBit();
	else WriteAnswerBit();
	return (Data&0xff);
}

static BYTE SendByte(BYTE Data)
{
    WORD i;
	DelayOnce(10);
	for(i=0;i<8;i++)
	{
		SetSCLtoLow();
		DelayOnce(5);
		if(Data&(0x80>>i)) SetSDAtoHigh();
		else SetSDAtoLow();
		DelayOnce(5);
		SetSCLtoHigh();
		DelayOnce(20);
	}
	return ReadAnswerBit();
}


//-----------------------------------------------------------------------------
BYTE I2C1GetStatus(WORD wDevAddr)
{
    BYTE CheckOK;
    WriteStartBit();
    CheckOK = SendByte(wDevAddr&0xff);
    WriteStopBit();
    return CheckOK;
}

void I2C1ReadDataByte(WORD wDevAddr,WORD wDataAddr,BYTE* pData,WORD wCountByte)
{
    if(wCountByte==0) return;
    WriteStartBit();
    SendByte(wDevAddr&0xff);
    if(wDevAddr&I2C_WDATAADDR) SendByte((BYTE)(wDataAddr>>8));
    SendByte(wDataAddr&0xff);
    WriteStartBit();
    SendByte((wDevAddr&0xff)+1);
    while(wCountByte)
    {
        *pData = ReadByte((wCountByte==1)?TRUE:FALSE);
        pData++;
        wCountByte--;
    }
    WriteStopBit();
}

void I2C1ReadDataWord(WORD wDevAddr,WORD wDataAddr,WORD* pData,WORD wCountWord)
{
	if(wCountWord==0) return;
	WriteStartBit();
	SendByte(wDevAddr&0xff);
	if(wDevAddr&I2C_WDATAADDR) SendByte((BYTE)(wDataAddr>>8));
	SendByte(wDataAddr&0xff);
	WriteStartBit();
	SendByte((wDevAddr&0xff)+1);
	while(wCountWord)
	{
		*pData = ReadByte(0);
		*pData |= ReadByte((wCountWord==1)?TRUE:FALSE)<<8;
		pData++;
		wCountWord--;
	}
	WriteStopBit();
}

void I2C1WriteDataByte(WORD wDevAddr,WORD wDataAddr,BYTE* pData,WORD wCountByte)
{ 
	if(wCountByte==0) return;
	WriteStartBit();
	SendByte(wDevAddr&0xff);
	if(wDevAddr&I2C_WDATAADDR) SendByte((BYTE)(wDataAddr>>8));
	SendByte(wDataAddr&0xff);
	while(wCountByte)
	{
		SendByte((*pData)&0xff);
		pData++;
		wCountByte--;
	}
	WriteStopBit();
}

void I2C1WriteDataWord(WORD wDevAddr,WORD wDataAddr,WORD* pData,WORD wCountWord)
{
	if(wCountWord==0) return;
	WriteStartBit();
	SendByte(wDevAddr&0xff);
	if(wDevAddr&I2C_WDATAADDR) SendByte((BYTE)(wDataAddr>>8));
	SendByte(wDataAddr&0xff);
	while(wCountWord)
	{
		SendByte((*pData)&0xff);
		SendByte((*pData)>>8);
		pData++;
		wCountWord--;
	}
	WriteStopBit();
}


