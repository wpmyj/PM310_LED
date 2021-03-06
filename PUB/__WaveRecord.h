#ifndef __WAVERECORD_H
#define __WAVERECORD_H

#define MODULE__WAVERECORD_H

#ifndef WAVERECORD_WAVENUM
#define WAVERECORD_WAVENUM      4               //最大录波器数;
#endif

#ifndef WAVERECORD_DOTNUM
#define WAVERECORD_DOTNUM       2400            //每个录波器最大录波点数;
#endif

#ifndef WAVERECORD_DATASIZE
#define WAVERECORD_DATASIZE     7               //每个录波点数据大小;
#endif

#define WAVERECORD_ALLWORDS     ((DWORD)WAVERECORD_WAVENUM*(18L+(DWORD)WAVERECORD_DOTNUM*(DWORD)WAVERECORD_DATASIZE))

#define WRS_NULL                0x0000          //空闲状态;
#define WRS_RECORDING           0xaaaa          //录波状态;
#define WRS_RECORDED            0xffff          //录波完成状态;

#define WAVEHEAD_SIZE           18              //录波列表信息WORD数目
#define WAVEDATA_SIZE           1680            //录波数据WORD数目
#define WAVEDCHANEL_SIZE        80              //单次传送录波数据WORD数目



//-----------------------------------------------------------------------------
typedef struct
{
    DEVICETIME timeFreeze;                              //录波器录波冻结时间;
    WORD wRecordStatus;                                 //录波器当前录波状态;
    WORD wFreezeDotNo;                                  //录波器录波冻结点号;
    WORD wCycleFlag;                                    //录波器是否循环标志;
    short nK[WAVERECORD_DATASIZE];                      // 校正系数
    WORD wOffset;                                       // 直流分量
    WORD wData[WAVERECORD_DOTNUM][WAVERECORD_DATASIZE]; //录波器录波数据;
}WAVERECORD;

//-----------------------------------------------------------------------------
extern void WaveRecord_OnInit(void);
extern void WaveRecord_OnTimer1000ms(void);
extern void WaveRecord_StartRecord(void);
extern void WaveRecord_FreezeRecord(void);
extern void WaveRecord_OnRecord(void);
extern WAVERECORD* WaveRecord_GetRecord(WORD wWaveRecordNo);
extern void WaveRecord_ReadRecord(void);
extern void WaveRecord_WriteRecord(void);

extern WORD g_wWRCountNew;                      // 故障录波计数
extern WORD g_wWRCountOld;                      // 故障录波计数(跟踪更新用);
extern WORD g_wWRData[WAVERECORD_DATASIZE];     //当前点数据;

#endif/*__WAVERECORD_H*/
