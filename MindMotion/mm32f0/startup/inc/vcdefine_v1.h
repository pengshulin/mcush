////////////////////////////////////////////////////////////////////////////////
/// @brief Version compatibility definition
////////////////////////////////////////////////////////////////////////////////
#define ADDATA_DATA                     ADC_DR_DATA

#define ADDATA_CHANNELSEL               ADC_DR_CH
#define ADDATA_CHANNELSEL_0             ADC_DR_CH0
#define ADDATA_CHANNELSEL_1             ADC_DR_CH1
#define ADDATA_CHANNELSEL_2             ADC_DR_CH2
#define ADDATA_CHANNELSEL_3             ADC_DR_CH3
#define ADDATA_CHANNELSEL_4             ADC_DR_CH4
#define ADDATA_CHANNELSEL_5             ADC_DR_CH5
#define ADDATA_CHANNELSEL_6             ADC_DR_CH6
#define ADDATA_CHANNELSEL_7             ADC_DR_CH7

#if defined(__MM3N1)
    #define ADDATA_CHANNELSEL_Sensor    ADC_DR_Sensor
#endif

#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    #define ADDATA_CHANNELSEL_CH8       ADC_DR_CH8
    #define ADDATA_CHANNELSEL_CH9       ADC_DR_CH9
    #define ADDATA_CHANNELSEL_TempSensor    ADC_DR_TempSensor
    #define ADDATA_CHANNELSEL_VolSensor ADC_DR_VoltRef
#endif

#if defined(__MM0P1) || defined(__MM0Q1)
    #define ADDATA_CHANNELSEL_CH10      ADC_DR_CH10
    #define ADDATA_CHANNELSEL_CH11      ADC_DR_CH11
    #define ADDATA_CHANNELSEL_CH13      ADC_DR_CH13
#endif

#define ADDATA_OVERRUN                  ADC_DR_OVERRUN
#define ADDATA_VALID                    ADC_DR_VALID

#define ADCFG_ADEN                      ADC_CFGR_ADEN
#define ADCFG_ADWEN                     ADC_CFGR_ADWEN

#define ADCFG_RSLTCTL                   ADC_CFGR_RSLTCTL
#define ADCFG_RSLTCTL_12                ADC_CFGR_RSLTCTL_12
#define ADCFG_RSLTCTL_11                ADC_CFGR_RSLTCTL_11
#define ADCFG_RSLTCTL_10                ADC_CFGR_RSLTCTL_10
#define ADCFG_RSLTCTL_9                 ADC_CFGR_RSLTCTL_9
#define ADCFG_RSLTCTL_8                 ADC_CFGR_RSLTCTL_8

#if defined(__MM3N1)
    #define ADCFG_TVSEN                 ADC_CFGR_TVEN
#endif

#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    #define ADCFG_TSEN                  ADC_CFGR_TEN
    #define ADCFG_VSEN                  ADC_CFGR_VEN
#endif

#define ADCFG_ADCPRE                    ADC_CFGR_PRE
#define ADCFG_ADCPRE_2                  ADC_CFGR_PRE_2
#define ADCFG_ADCPRE_4                  ADC_CFGR_PRE_4
#define ADCFG_ADCPRE_6                  ADC_CFGR_PRE_6
#define ADCFG_ADCPRE_8                  ADC_CFGR_PRE_8
#define ADCFG_ADCPRE_10                 ADC_CFGR_PRE_10
#define ADCFG_ADCPRE_12                 ADC_CFGR_PRE_12
#define ADCFG_ADCPRE_14                 ADC_CFGR_PRE_14
#define ADCFG_ADCPRE_16                 ADC_CFGR_PRE_16
#define ADCFG_SAMCTL                    ADC_CFGR_SAMCTL
#define ADCFG_SAMCTL_1_5                ADC_CFGR_SAMCTL_1_5
#define ADCFG_SAMCTL_7_5                ADC_CFGR_SAMCTL_7_5
#define ADCFG_SAMCTL_13_5               ADC_CFGR_SAMCTL_13_5
#define ADCFG_SAMCTL_28_5               ADC_CFGR_SAMCTL_28_5
#define ADCFG_SAMCTL_41_5               ADC_CFGR_SAMCTL_41_5
#define ADCFG_SAMCTL_55_5               ADC_CFGR_SAMCTL_55_5
#define ADCFG_SAMCTL_71_5               ADC_CFGR_SAMCTL_71_5
#define ADCFG_SAMCTL_239_5              ADC_CFGR_SAMCTL_239_5


#if defined(__MM0P1) || defined(__MM0Q1)
    #define ADCFG_ADCPRE_3              ADC_CFGR_PRE_3
    #define ADCFG_ADCPRE_5              ADC_CFGR_PRE_5
    #define ADCFG_ADCPRE_7              ADC_CFGR_PRE_7
    #define ADCFG_ADCPRE_9              ADC_CFGR_PRE_9
    #define ADCFG_ADCPRE_11             ADC_CFGR_PRE_11
    #define ADCFG_ADCPRE_13             ADC_CFGR_PRE_13
    #define ADCFG_ADCPRE_15             ADC_CFGR_PRE_15
    #define ADCFG_ADCPRE_17             ADC_CFGR_PRE_17
    #define ADCFG_SAMCTL_2_5            ADC_CFGR_SAMCTL_2_5
    #define ADCFG_SAMCTL_3_5            ADC_CFGR_SAMCTL_3_5
    #define ADCFG_SAMCTL_4_5            ADC_CFGR_SAMCTL_4_5
    #define ADCFG_SAMCTL_5_5            ADC_CFGR_SAMCTL_5_5
    #define ADCFG_SAMCTL_6_5            ADC_CFGR_SAMCTL_6_5
#endif

#define ADCR_ADIE                       ADC_CR_ADIE
#define ADCR_ADWIE                      ADC_CR_ADWIE
#define ADCR_TRGEN                      ADC_CR_TRGEN
#define ADCR_DMAEN                      ADC_CR_DMAEN
#define ADCR_ADST                       ADC_CR_ADST
#define ADCR_ADMD                       ADC_CR_MODE
#define ADCR_ADMD_SINGLE                ADC_CR_IMM
#define ADCR_ADMD_PERIOD                ADC_CR_SCAN
#define ADCR_ADMD_CONTINUE              ADC_CR_CONTINUE
#define ADCR_ALIGN                      ADC_CR_ALIGN
#define ADCR_ALIGN_LEFT                 ADC_CR_LEFT
#define ADCR_ALIGN_RIGHT                ADC_CR_RIGHT
#define ADCR_CMPCH_Pos                  ADC_CR_CMPCH_Pos
#define ADCR_CMPCH                      ADC_CR_CMPCH
#define ADCR_CMPCH_0                    ADC_CR_CMPCH_0
#define ADCR_CMPCH_1                    ADC_CR_CMPCH_1
#define ADCR_CMPCH_2                    ADC_CR_CMPCH_2
#define ADCR_CMPCH_4                    ADC_CR_CMPCH_4
#define ADCR_CMPCH_5                    ADC_CR_CMPCH_5
#define ADCR_CMPCH_6                    ADC_CR_CMPCH_6
#define ADCR_CMPCH_7                    ADC_CR_CMPCH_7
#define ADCR_CMPCH_8                    ADC_CR_CMPCH_8
#define ADCR_CMPCH_9                    ADC_CR_CMPCH_9
#define ADCR_CMPCH_10                   ADC_CR_CMPCH_10
#define ADCR_CMPCH_11                   ADC_CR_CMPCH_11
#define ADCR_CMPCH_13                   ADC_CR_CMPCH_13
#define ADCR_CMPCH_14                   ADC_CR_CMPCH_14
#define ADCR_CMPCH_ALL                  ADC_CR_CMPCH_ALL

#if defined(__MM3N1)
    #define ADCR_SYNCEN                 ADC_CR_SYNCEN
#endif

#define ADCR_TRGSEL                     ADC_CR_TRGSEL
#define ADCR_TRGSEL_T1_CC1              ADC_CR_T1_CC1
#define ADCR_TRGSEL_T1_CC2              ADC_CR_T1_CC2
#define ADCR_TRGSEL_T1_CC3              ADC_CR_T1_CC3
#define ADCR_TRGSEL_T2_CC2              ADC_CR_T2_CC2
#define ADCR_TRGSEL_T3_TRGO             ADC_CR_T3_TRIG
#define ADCR_TRGSEL_EXTI_11             ADC_CR_TRGSEL_EXTI_11

#if defined(__MM3N1) || defined(__MM0N1)
    #define ADCR_TRGSEL_Pos             ADC_CR_TRGSEL_Pos
    #define ADCR_TRGSEL_T3_CC1          ADC_CR_T3_CC1
#endif

#if defined(__MM3N1)
    #define ADCR_TRGSEL_T4_CC4          ADC_CR_T4_CC4
    #define ADCR_TRGSEL_T1_TRGO         ADC_CR_T1_TRIG
    #define ADCR_TRGSEL_T1_CC4          ADC_CR_T1_CC4
    #define ADCR_TRGSEL_T2_TRGO         ADC_CR_T2_TRIG
    #define ADCR_TRGSEL_T2_CC1          ADC_CR_T2_CC1
    #define ADCR_TRGSEL_T3_CC4          ADC_CR_T3_CC4
    #define ADCR_TRGSEL_T4_TRGO         ADC_CR_T4_TRIG
    #define ADCR_TRGSEL_EXTI_15         ADC_CR_EXTI_15
#endif

#if defined(__MM0P1) || defined(__MM0Q1)
    #define ADCR_TRGSEL_T1_CC4_CC5      ADC_CR_T1_CC4_CC5
    #define ADCR_TRGSEL_T3_CC1          ADC_CR_T3_CC1
    #define ADCR_TRGSEL_T1_TRGO         ADC_CR_T1_TRIG
    #if defined(__MM0P1)
    #define ADCR_TRGSEL_T8_CC4          ADC_CR_T8_CC4
    #define ADCR_TRGSEL_T8_CC4_CC5      ADC_CR_T8_CC4_CC5
    #endif
    #define ADCR_TRGSEL_T2_CC1          ADC_CR_T2_CC1
    #define ADCR_TRGSEL_T3_CC4          ADC_CR_T3_CC4
    #define ADCR_TRGSEL_T2_TRGO         ADC_CR_T2_TRIG
    #if defined(__MM0P1)
    #define ADCR_TRGSEL_T8_CC5          ADC_CR_T8_CC5
    #endif
    #define ADCR_TRGSEL_EXTI_15         ADC_CR_EXTI_15
    #define ADCR_TRGSEL_TIM1_CC4        ADC_CR_TIM1_CC4
    #define ADCR_TRGSEL_TIM1_CC5        ADC_CR_TIM1_CC5
    #define ADCR_SCANDIR                ADC_CR_SCANDIR
    #define ADCR_TRGSHIFT               ADC_CR_TRGSHIFT
    #define ADCR_TRGSHIFT_0             ADC_CR_TRGSHIFT_0
    #define ADCR_TRGSHIFT_4             ADC_CR_TRGSHIFT_4
    #define ADCR_TRGSHIFT_16            ADC_CR_TRGSHIFT_16
    #define ADCR_TRGSHIFT_32            ADC_CR_TRGSHIFT_32
    #define ADCR_TRGSHIFT_64            ADC_CR_TRGSHIFT_64
    #define ADCR_TRGSHIFT_128           ADC_CR_TRGSHIFT_128
    #define ADCR_TRGSHIFT_256           ADC_CR_TRGSHIFT_256
    #define ADCR_TRGSHIFT_512           ADC_CR_TRGSHIFT_512
    #define ADCR_CALIBEN                ADC_CR_CALIBEN
    #define ADCR_CALIBSEL               ADC_CR_CALIBSEL
#endif

#define ADCHS_CHEN0                     ADC_CHSR_CH0
#define ADCHS_CHEN1                     ADC_CHSR_CH1
#define ADCHS_CHEN2                     ADC_CHSR_CH2
#define ADCHS_CHEN3                     ADC_CHSR_CH3
#define ADCHS_CHEN4                     ADC_CHSR_CH4
#define ADCHS_CHEN5                     ADC_CHSR_CH5
#define ADCHS_CHEN6                     ADC_CHSR_CH6
#define ADCHS_CHEN7                     ADC_CHSR_CH7
#define ADCHS_ALL                       ADC_CHSR_CHALL

#if defined(__MM3N1)
    #define ADCHS_SENSOREN              ADC_CHSR_CHTV
#endif

#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
    #define ADCHS_CHEN8                 ADC_CHSR_CH8
    #define ADCHS_CHEN9                 ADC_CHSR_CH9
    #define ADCHS_CHENTS                ADC_CHSR_CHT
    #define ADCHS_CHENVS                ADC_CHSR_CHV
#endif

#if defined(__MM0P1) || defined(__MM0Q1)
    #define ADCHS_CHEN10                ADC_CHSR_CH10
    #define ADCHS_CHEN11                ADC_CHSR_CH11
    #define ADCHS_CHCALIB               ADC_CHSR_CHCALIB
#endif

#define ADCMPR_CMPLDATA                 ADC_CMPR_CMPLDATA
#define ADCMPR_CMPHDATA                 ADC_CMPR_CMPHDATA

#define ADSTA_ADIF                      ADC_SR_ADIF
#define ADSTA_ADWIF                     ADC_SR_ADWIF
#define ADSTA_BUSY                      ADC_SR_BUSY
#define ADSTA_CHANNEL                   ADC_SR_CH
#define ADSTA_CHANNEL_CH0               ADC_SR_CH0
#define ADSTA_CHANNEL_CH1               ADC_SR_CH1
#define ADSTA_CHANNEL_CH2               ADC_SR_CH2
#define ADSTA_CHANNEL_CH3               ADC_SR_CH3
#define ADSTA_CHANNEL_CH4               ADC_SR_CH4
#define ADSTA_CHANNEL_CH5               ADC_SR_CH5
#define ADSTA_CHANNEL_CH6               ADC_SR_CH6
#define ADSTA_CHANNEL_CH7               ADC_SR_CH7
#define ADSTA_CHANNEL_CH8               ADC_SR_CH8
#define ADSTA_CHANNEL_CH9               ADC_SR_CH9
#define ADSTA_CHANNEL_CH10              ADC_SR_CH10
#define ADSTA_CHANNEL_CH11              ADC_SR_CH11
#define ADSTA_CHANNEL_CH13              ADC_SR_CH13
#define ADSTA_CHANNEL_CH14              ADC_SR_CH14
#define ADSTA_CHANNEL_CH15              ADC_SR_CH15
#define ADSTA_VALID                     ADC_SR_VALID
#define ADSTA_OVERRUN                   ADC_SR_OVERRUN

#define ADDR_OVERRUN                    ADC_CHDR_OVERRUN
#define ADDR_VALID                      ADC_CHDR_VALID
#define ADDR_DATA                       ADC_CHDR_DATA

////////////////////////////////////////////////////////////////////////////////
/// @brief Version compatibility definition
////////////////////////////////////////////////////////////////////////////////
    #define AES_KEYR0                   AES_KEYRn                               ///< AES Key Register 0
    #define AES_KEYR1                   AES_KEYRn                               ///< AES Key Register 1
    #define AES_KEYR2                   AES_KEYRn                               ///< AES Key Register 2
    #define AES_KEYR3                   AES_KEYRn                               ///< AES Key Register 3
    #define AES_KEYR4                   AES_KEYRn                               ///< AES Key Register 4
    #define AES_KEYR5                   AES_KEYRn                               ///< AES Key Register 5
    #define AES_KEYR6                   AES_KEYRn                               ///< AES Key Register 6
    #define AES_KEYR7                   AES_KEYRn                               ///< AES Key Register 7

    #define AES_IVR0                    AES_IVRn                                ///< AES Initialization Vector Register 0
    #define AES_IVR1                    AES_IVRn                                ///< AES Initialization Vector Register 1
    #define AES_IVR2                    AES_IVRn                                ///< AES Initialization Vector Register 2
    #define AES_IVR3                    AES_IVRn                                ///< AES Initialization Vector Register 3


#if defined(__MM3N1) || defined(__MM3O1) || defined(__MM0P1) || defined(__MM0Q1)
#define CRC_DR_DR                       CRC_DR_DATA
#define CRC_IDR_IDR                     CRC_IDR_DATA
#endif
////////////////////////////////////////////////////////////////////////////////
/// @brief Version compatibility definition
////////////////////////////////////////////////////////////////////////////////
#define CRS_CR_SYNCOKIE                 CRS_CR_OKIE
#define CRS_CR_SYNCWARNIE               CRS_CR_WARNIE
#define CRS_CR_ESYNCIE                  CRS_CR_EXPTIE
#define CRS_CR_CEN                      CRS_CR_CNTEN
#define CRS_CFGR_SYNCDIV                CRS_CFGR_DIV
#define CRS_CFGR_SYNCSRC                CRS_CFGR_SRC
#define CRS_CFGR_SYNCPOL                CRS_CFGR_POL
#define CRS_ISR_SYNCOKF                 CRS_ISR_OKIF
#define CRS_ISR_SYNCWARNF               CRS_ISR_WARNIF
#define CRS_ISR_ERRF                    CRS_ISR_ERRIF
#define CRS_ISR_ESYNCF                  CRS_ISR_EXPTIF
#define CRS_ISR_SYNCERR                 CRS_ISR_ERR
#define CRS_ISR_SYNCMISS                CRS_ISR_MISS
#define CRS_ISR_TRIMOVF                 CRS_ISR_OVERFLOW
#define CRS_ICR_SYNCOKC                 CRS_ICR_OK
#define CRS_ICR_SYNCWARNC               CRS_ICR_WARN
#define CRS_ICR_ERRC                    CRS_ICR_ERR
#define CRS_ICR_ESYNCC                  CRS_ICR_EXPT

#define CRS_IT_SYNCOK                   CRS_ISR_SYNCOKF                         ///< SYNC event OK
#define CRS_IT_SYNCWARN                 CRS_ISR_SYNCWARNF                       ///< SYNC warning
#define CRS_IT_ERR                      CRS_CR_ERRIE                            ///< error
#define CRS_IT_ESYNC                    CRS_ISR_ESYNCF                          ///< Expected SYNC
#define CRS_IT_TRIMOVF                  CRS_ISR_TRIMOVF                         ///< Trimming overflow or underflow
#define CRS_IT_SYNCERR                  CRS_ISR_SYNCERR                         ///< SYNC error
#define CRS_IT_SYNCMISS                 CRS_ISR_SYNCMISS                        ///< SYNC missed

#define CRS_FLAG_SYNCOK                 CRS_ISR_SYNCOKF                         ///< SYNC event OK
#define CRS_FLAG_SYNCWARN               CRS_ISR_SYNCWARNF                       ///< SYNC warning
#define CRS_FLAG_ERR                    CRS_ISR_ERRF                            ///< error
#define CRS_FLAG_ESYNC                  CRS_ISR_ESYNCF                          ///< Expected SYNC
#define CRS_FLAG_TRIMOVF                CRS_ISR_TRIMOVF                         ///< Trimming overflow or underflow
#define CRS_FLAG_SYNCERR                CRS_ISR_SYNCERR                         ///< SYNC error
#define CRS_FLAG_SYNCMISS               CRS_ISR_SYNCMISS                        ///< SYNC missed
////////////////////////////////////////////////////////////////////////////////
/// @brief Version compatibility definition
////////////////////////////////////////////////////////////////////////////////
#define DMA_CCR1_EN                     DMA_CCR_EN
#define DMA_CCR1_DIR                    DMA_CCR_DIR
#define DMA_CCR1_CIRC                   DMA_CCR_CIRC
#define DMA_CCR1_PINC                   DMA_CCR_PINC
#define DMA_CCR1_MINC                   DMA_CCR_MINC
#define DMA_CCR1_PSIZE_0                DMA_CCR_PSIZE
#define DMA_CCR1_MSIZE_0                DMA_CCR_MSIZE
#define DMA_CCR1_PL0                    DMA_CCR_PL
#define DMA_CCR1_MEM2MEM                DMA_CCR_M2M


#define DMA_IT_TC                       DMA_CCR_TCIE                            //(0x00000002U)
#define DMA_IT_HT                       DMA_CCR_HTIE                            //(0x00000004U)
#define DMA_IT_TE                       DMA_CCR_TEIE                            //(0x00000008U)

#define DMA1_IT_GL1                     DMA_IFCR_CGIF1                          //(0x00000001U)
#define DMA1_IT_TC1                     DMA_IFCR_CTCIF1                         //(0x00000002U)
#define DMA1_IT_HT1                     DMA_IFCR_CHTIF1                         //(0x00000004U)
#define DMA1_IT_TE1                     DMA_IFCR_CTEIF1                         //(0x00000008U)
#define DMA1_IT_GL2                     DMA_IFCR_CGIF2                          //(0x00000010U)
#define DMA1_IT_TC2                     DMA_IFCR_CTCIF2                         //(0x00000020U)
#define DMA1_IT_HT2                     DMA_IFCR_CHTIF2                         //(0x00000040U)
#define DMA1_IT_TE2                     DMA_IFCR_CTEIF2                         //(0x00000080U)
#define DMA1_IT_GL3                     DMA_IFCR_CGIF3                          //(0x00000100U)
#define DMA1_IT_TC3                     DMA_IFCR_CTCIF3                         //(0x00000200U)
#define DMA1_IT_HT3                     DMA_IFCR_CHTIF3                         //(0x00000400U)
#define DMA1_IT_TE3                     DMA_IFCR_CTEIF3                         //(0x00000800U)
#define DMA1_IT_GL4                     DMA_IFCR_CGIF4                          //(0x00001000U)
#define DMA1_IT_TC4                     DMA_IFCR_CTCIF4                         //(0x00002000U)
#define DMA1_IT_HT4                     DMA_IFCR_CHTIF4                         //(0x00004000U)
#define DMA1_IT_TE4                     DMA_IFCR_CTEIF4                         //(0x00008000U)
#define DMA1_IT_GL5                     DMA_IFCR_CGIF5                          //(0x00010000U)
#define DMA1_IT_TC5                     DMA_IFCR_CTCIF5                         //(0x00020000U)
#define DMA1_IT_HT5                     DMA_IFCR_CHTIF5                         //(0x00040000U)
#define DMA1_IT_TE5                     DMA_IFCR_CTEIF5                         //(0x00080000U)

#if defined(__MM3N1)
#define DMA1_IT_GL6                     DMA_IFCR_CGIF6                          //(0x00100000U)
#define DMA1_IT_TC6                     DMA_IFCR_CTCIF6                         //(0x00200000U)
#define DMA1_IT_HT6                     DMA_IFCR_CHTIF6                         //(0x00400000U)
#define DMA1_IT_TE6                     DMA_IFCR_CTEIF6                         //(0x00800000U)
#define DMA1_IT_GL7                     DMA_IFCR_CGIF7                          //(0x01000000U)
#define DMA1_IT_TC7                     DMA_IFCR_CTCIF7                         //(0x02000000U)
#define DMA1_IT_HT7                     DMA_IFCR_CHTIF7                         //(0x04000000U)
#define DMA1_IT_TE7                     DMA_IFCR_CTEIF7                         //(0x08000000U)
#endif

#define DMA1_FLAG_GL1                   DMA_ISR_GIF1                            //(0x00000001U)
#define DMA1_FLAG_TC1                   DMA_ISR_TCIF1                           //(0x00000002U)
#define DMA1_FLAG_HT1                   DMA_ISR_HTIF1                           //(0x00000004U)
#define DMA1_FLAG_TE1                   DMA_ISR_TEIF1                           //(0x00000008U)
#define DMA1_FLAG_GL2                   DMA_ISR_GIF2                            //(0x00000010U)
#define DMA1_FLAG_TC2                   DMA_ISR_TCIF2                           //(0x00000020U)
#define DMA1_FLAG_HT2                   DMA_ISR_HTIF2                           //(0x00000040U)
#define DMA1_FLAG_TE2                   DMA_ISR_TEIF2                           //(0x00000080U)
#define DMA1_FLAG_GL3                   DMA_ISR_GIF3                            //(0x00000100U)
#define DMA1_FLAG_TC3                   DMA_ISR_TCIF3                           //(0x00000200U)
#define DMA1_FLAG_HT3                   DMA_ISR_HTIF3                           //(0x00000400U)
#define DMA1_FLAG_TE3                   DMA_ISR_TEIF3                           //(0x00000800U)
#define DMA1_FLAG_GL4                   DMA_ISR_GIF4                            //(0x00001000U)
#define DMA1_FLAG_TC4                   DMA_ISR_TCIF4                           //(0x00002000U)
#define DMA1_FLAG_HT4                   DMA_ISR_HTIF4                           //(0x00004000U)
#define DMA1_FLAG_TE4                   DMA_ISR_TEIF4                           //(0x00008000U)
#define DMA1_FLAG_GL5                   DMA_ISR_GIF5                            //(0x00010000U)
#define DMA1_FLAG_TC5                   DMA_ISR_TCIF5                           //(0x00020000U)
#define DMA1_FLAG_HT5                   DMA_ISR_HTIF5                           //(0x00040000U)
#define DMA1_FLAG_TE5                   DMA_ISR_TEIF5                           //(0x00080000U)

#if defined(__MM3N1)
#define DMA1_FLAG_GL6                   DMA_ISR_GIF6                            //(0x00100000U)
#define DMA1_FLAG_TC6                   DMA_ISR_TCIF6                           //(0x00200000U)
#define DMA1_FLAG_HT6                   DMA_ISR_HTIF6                           //(0x00400000U)
#define DMA1_FLAG_TE6                   DMA_ISR_TEIF6                           //(0x00800000U)
#define DMA1_FLAG_GL7                   DMA_ISR_GIF7                            //(0x01000000U)
#define DMA1_FLAG_TC7                   DMA_ISR_TCIF7                           //(0x02000000U)
#define DMA1_FLAG_HT7                   DMA_ISR_HTIF7                           //(0x04000000U)
#define DMA1_FLAG_TE7                   DMA_ISR_TEIF7                           //(0x08000000U)
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief Version compatibility definition
////////////////////////////////////////////////////////////////////////////////
#define AFIO_MAPR_SPI1_REMAP            EXTI_MAPR_SPI1
#define AFIO_MAPR_I2C1_REMAP            EXTI_MAPR_I2C1
#define AFIO_MAPR_UART1_REMAP           EXTI_MAPR_UART1
#define AFIO_MAPR_UART3_REMAP_MASK      EXTI_MAPR_UART3_MASK
#define AFIO_MAPR_UART3_REMAP           EXTI_MAPR_UART3
#define AFIO_MAPR_TIM1_REMAP_MASK       EXTI_MAPR_TIM1_MASK
#define AFIO_MAPR_TIM1_REMAP            EXTI_MAPR_TIM1
#define AFIO_MAPR_TIM1_REMAP_PARTIAL    EXTI_MAPR_TIM1_PARTIAL
#define AFIO_MAPR_TIM2_REMAP_MASK       EXTI_MAPR_TIM2_MASK
#define AFIO_MAPR_TIM2_REMAP            EXTI_MAPR_TIM2
#define AFIO_MAPR_TIM2_REMAP_PARTIAL1   EXTI_MAPR_TIM2_PARTIAL1
#define AFIO_MAPR_TIM2_REMAP_PARTIAL2   EXTI_MAPR_TIM2_PARTIAL2
#define AFIO_MAPR_TIM3_REMAP_MASK       EXTI_MAPR_TIM3_MASK
#define AFIO_MAPR_TIM3_REMAP            EXTI_MAPR_TIM3
#define AFIO_MAPR_TIM3_REMAP_PARTIAL    EXTI_MAPR_TIM3_PARTIAL
#define AFIO_MAPR_CAN_REMAP_MASK        EXTI_MAPR_CAN_MASK
#define AFIO_MAPR_CAN_REMAP             EXTI_MAPR_CAN
#define AFIO_MAPR_CAN_PARTIAL2          EXTI_MAPR_CAN_PARTIAL2
#define AFIO_MAPR_CAN_PARTIAL3          EXTI_MAPR_CAN_PARTIAL3
#define AFIO_MAPR_PD01_REMAP            EXTI_MAPR_PD01
#define AFIO_MAPR_SWJ_CFG_MASK          EXTI_MAPR_SWJ_MASK
#define AFIO_MAPR_SWJ_CFG               EXTI_MAPR_SWJ
#define AFIO_MAPR_SWJ_CFG_NOJNTRST      EXTI_MAPR_SWJ_NOJNTRST
#define AFIO_MAPR_SWJ_CFG_JTAGDISABLE   EXTI_MAPR_SWJ_JTAGDISABLE
#define AFIO_MAPR_SWJ_CFG_DISABLE       EXTI_MAPR_SWJ_DISABLE

#define AFIO_MAPR                       EXTI_MAPR
#define AFIO_EXTICR(x)                  EXTI_CR(x)
#define AFIO_EXTICR1                    EXTI_CR1
#define AFIO_EXTICR2                    EXTI_CR2
#define AFIO_EXTICR3                    EXTI_CR3
#define AFIO_EXTICR4                    EXTI_CR4
#define AFIO_EXTICR1_EXTI0              EXIT_CR1_EXTI0
#define AFIO_EXTICR2_EXTI1              EXIT_CR2_EXTI1
#define AFIO_EXTICR3_EXTI2              EXIT_CR3_EXTI2
#define AFIO_EXTICR4_EXTI3              EXIT_CR4_EXTI3

#define SYSCFG_DMARemap_TIM17           EXTI_CFGR_TIM17DMA                      ///< Remap TIM17 DMA requests from channel1 to channel2
#define SYSCFG_DMARemap_TIM16           EXTI_CFGR_TIM16DMA                      ///< Remap TIM16 DMA requests from channel3 to channel4
#define SYSCFG_DMARemap_UART1Rx         EXTI_CFGR_UART1RXDMA                    ///< Remap UART1 Rx DMA requests from channel3 to channel5
#define SYSCFG_DMARemap_UART1Tx         EXTI_CFGR_UART1TXDMA                    ///< Remap UART1 Tx DMA requests from channel2 to channel4
#define SYSCFG_DMARemap_ADC1            EXTI_CFGR_ADCDMA                        ///< Remap ADC1 DMA requests from channel1 to channel2
#define SYSCFG_CFGR_ADC_DMA_RMP         EXTI_CFGR_ADCDMA
#define SYSCFG_CFGR_TIM16_DMA_RMP       EXTI_CFGR_TIM16DMA
#define SYSCFG_CFGR_TIM17_DMA_RMP       EXTI_CFGR_TIM17DMA
#define SYSCFG_CFGR_UART1TX_DMA_RMP     EXTI_CFGR_UART1TXDMA
#define SYSCFG_CFGR_UART1RX_DMA_RMP     EXTI_CFGR_UART1RXDMA

////////////////////////////////////////////////////////////////////////////////
#define EXTI_IMR_MR0                    EXTI_IMR_0
#define EXTI_IMR_MR1                    EXTI_IMR_1
#define EXTI_IMR_MR2                    EXTI_IMR_2
#define EXTI_IMR_MR3                    EXTI_IMR_3
#define EXTI_IMR_MR4                    EXTI_IMR_4
#define EXTI_IMR_MR5                    EXTI_IMR_5
#define EXTI_IMR_MR6                    EXTI_IMR_6
#define EXTI_IMR_MR7                    EXTI_IMR_7
#define EXTI_IMR_MR8                    EXTI_IMR_8
#define EXTI_IMR_MR9                    EXTI_IMR_9
#define EXTI_IMR_MR10                   EXTI_IMR_10
#define EXTI_IMR_MR11                   EXTI_IMR_11
#define EXTI_IMR_MR12                   EXTI_IMR_12
#define EXTI_IMR_MR13                   EXTI_IMR_13
#define EXTI_IMR_MR14                   EXTI_IMR_14
#define EXTI_IMR_MR15                   EXTI_IMR_15
#define EXTI_IMR_MR16                   EXTI_IMR_16

#define EXTI_EMR_MR0                    EXTI_EMR_0
#define EXTI_EMR_MR1                    EXTI_EMR_1
#define EXTI_EMR_MR2                    EXTI_EMR_2
#define EXTI_EMR_MR3                    EXTI_EMR_3
#define EXTI_EMR_MR4                    EXTI_EMR_4
#define EXTI_EMR_MR5                    EXTI_EMR_5
#define EXTI_EMR_MR6                    EXTI_EMR_6
#define EXTI_EMR_MR7                    EXTI_EMR_7
#define EXTI_EMR_MR8                    EXTI_EMR_8
#define EXTI_EMR_MR9                    EXTI_EMR_9
#define EXTI_EMR_MR10                   EXTI_EMR_10
#define EXTI_EMR_MR11                   EXTI_EMR_11
#define EXTI_EMR_MR12                   EXTI_EMR_12
#define EXTI_EMR_MR13                   EXTI_EMR_13
#define EXTI_EMR_MR14                   EXTI_EMR_14
#define EXTI_EMR_MR15                   EXTI_EMR_15
#define EXTI_EMR_MR16                   EXTI_EMR_16

#define EXTI_RTSR_TR0                   EXTI_RTSR_0
#define EXTI_RTSR_TR1                   EXTI_RTSR_1
#define EXTI_RTSR_TR2                   EXTI_RTSR_2
#define EXTI_RTSR_TR3                   EXTI_RTSR_3
#define EXTI_RTSR_TR4                   EXTI_RTSR_4
#define EXTI_RTSR_TR5                   EXTI_RTSR_5
#define EXTI_RTSR_TR6                   EXTI_RTSR_6
#define EXTI_RTSR_TR7                   EXTI_RTSR_7
#define EXTI_RTSR_TR8                   EXTI_RTSR_8
#define EXTI_RTSR_TR9                   EXTI_RTSR_9
#define EXTI_RTSR_TR10                  EXTI_RTSR_10
#define EXTI_RTSR_TR11                  EXTI_RTSR_11
#define EXTI_RTSR_TR12                  EXTI_RTSR_12
#define EXTI_RTSR_TR13                  EXTI_RTSR_13
#define EXTI_RTSR_TR14                  EXTI_RTSR_14
#define EXTI_RTSR_TR15                  EXTI_RTSR_15
#define EXTI_RTSR_TR16                  EXTI_RTSR_16

#define EXTI_FTSR_TR0                   EXTI_FTSR_0
#define EXTI_FTSR_TR1                   EXTI_FTSR_1
#define EXTI_FTSR_TR2                   EXTI_FTSR_2
#define EXTI_FTSR_TR3                   EXTI_FTSR_3
#define EXTI_FTSR_TR4                   EXTI_FTSR_4
#define EXTI_FTSR_TR5                   EXTI_FTSR_5
#define EXTI_FTSR_TR6                   EXTI_FTSR_6
#define EXTI_FTSR_TR7                   EXTI_FTSR_7
#define EXTI_FTSR_TR8                   EXTI_FTSR_8
#define EXTI_FTSR_TR9                   EXTI_FTSR_9
#define EXTI_FTSR_TR10                  EXTI_FTSR_10
#define EXTI_FTSR_TR11                  EXTI_FTSR_11
#define EXTI_FTSR_TR12                  EXTI_FTSR_12
#define EXTI_FTSR_TR13                  EXTI_FTSR_13
#define EXTI_FTSR_TR14                  EXTI_FTSR_14
#define EXTI_FTSR_TR15                  EXTI_FTSR_15
#define EXTI_FTSR_TR16                  EXTI_FTSR_16

#define EXTI_SWIER_SWIER0               EXTI_SWIER_0
#define EXTI_SWIER_SWIER1               EXTI_SWIER_1
#define EXTI_SWIER_SWIER2               EXTI_SWIER_2
#define EXTI_SWIER_SWIER3               EXTI_SWIER_3
#define EXTI_SWIER_SWIER4               EXTI_SWIER_4
#define EXTI_SWIER_SWIER5               EXTI_SWIER_5
#define EXTI_SWIER_SWIER6               EXTI_SWIER_6
#define EXTI_SWIER_SWIER7               EXTI_SWIER_7
#define EXTI_SWIER_SWIER8               EXTI_SWIER_8
#define EXTI_SWIER_SWIER9               EXTI_SWIER_9
#define EXTI_SWIER_SWIER10              EXTI_SWIER_10
#define EXTI_SWIER_SWIER11              EXTI_SWIER_11
#define EXTI_SWIER_SWIER12              EXTI_SWIER_12
#define EXTI_SWIER_SWIER13              EXTI_SWIER_13
#define EXTI_SWIER_SWIER14              EXTI_SWIER_14
#define EXTI_SWIER_SWIER15              EXTI_SWIER_15
#define EXTI_SWIER_SWIER16              EXTI_SWIER_16

#define EXTI_PR_PR0                     EXTI_PR_0
#define EXTI_PR_PR1                     EXTI_PR_1
#define EXTI_PR_PR2                     EXTI_PR_2
#define EXTI_PR_PR3                     EXTI_PR_3
#define EXTI_PR_PR4                     EXTI_PR_4
#define EXTI_PR_PR5                     EXTI_PR_5
#define EXTI_PR_PR6                     EXTI_PR_6
#define EXTI_PR_PR7                     EXTI_PR_7
#define EXTI_PR_PR8                     EXTI_PR_8
#define EXTI_PR_PR9                     EXTI_PR_9
#define EXTI_PR_PR10                    EXTI_PR_10
#define EXTI_PR_PR11                    EXTI_PR_11
#define EXTI_PR_PR12                    EXTI_PR_12
#define EXTI_PR_PR13                    EXTI_PR_13
#define EXTI_PR_PR14                    EXTI_PR_14
#define EXTI_PR_PR15                    EXTI_PR_15
#define EXTI_PR_PR16                    EXTI_PR_16

#if defined(__MM3N1) || defined(__MM3O1)
    #define EXTI_IMR_MR17               EXTI_IMR_17
#endif

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1)
    #define EXTI_IMR_MR18               EXTI_IMR_18
#endif

#define EXTI_IMR_MR19                   EXTI_IMR_19

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
    #define EXTI_IMR_MR20               EXTI_IMR_20
#endif

#if defined(__MM3O1) || defined(__MM0P1)
    #define EXTI_IMR_MR21               EXTI_IMR_21
#endif

#if defined(__MM0P1)
    #define EXTI_IMR_MR22               EXTI_IMR_22
    #define EXTI_IMR_MR23               EXTI_IMR_23
#endif

#if defined(__MM0P1) || defined(__MM0Q1)
    #define EXTI_IMR_MR24               EXTI_IMR_24
#endif

#if defined(__MM3N1) || defined(__MM3O1)
    #define EXTI_EMR_MR17               EXTI_EMR_17
#endif

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1)
    #define EXTI_EMR_MR18               EXTI_EMR_18
#endif

#define EXTI_EMR_MR19                   EXTI_EMR_19

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
    #define EXTI_EMR_MR20               EXTI_EMR_20
#endif

#if defined(__MM3O1) || defined(__MM0P1)
    #define EXTI_EMR_MR21               EXTI_EMR_21
#endif

#if defined(__MM0P1)
    #define EXTI_EMR_MR22               EXTI_EMR_22
    #define EXTI_EMR_MR23               EXTI_EMR_23
#endif

#if defined(__MM0P1) || defined(__MM0Q1)
    #define EXTI_EMR_MR24               EXTI_EMR_24
#endif

#if defined(__MM3N1) || defined(__MM3O1)
    #define EXTI_RTSR_MR17              EXTI_RTSR_17
#endif

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1)
    #define EXTI_RTSR_MR18              EXTI_RTSR_18
#endif

#define EXTI_RTSR_MR19                  EXTI_RTSR_19

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
    #define EXTI_RTSR_MR20              EXTI_RTSR_20
#endif

#if defined(__MM3O1) || defined(__MM0P1)
    #define EXTI_RTSR_MR21              EXTI_RTSR_21
#endif

#if defined(__MM0P1)
    #define EXTI_RTSR_MR22              EXTI_RTSR_22
    #define EXTI_RTSR_MR23              EXTI_RTSR_23
#endif

#if defined(__MM0P1) || defined(__MM0Q1)
    #define EXTI_RTSR_MR24              EXTI_RTSR_24
#endif

#if defined(__MM3N1) || defined(__MM3O1)
    #define EXTI_FTSR_MR17              EXTI_FTSR_18
#endif

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1)
    #define EXTI_FTSR_MR18              EXTI_FTSR_18
#endif

#define EXTI_FTSR_MR19                  EXTI_FTSR_19

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
    #define EXTI_FTSR_MR20              EXTI_FTSR_20
#endif

#if defined(__MM3O1) || defined(__MM0P1)
    #define EXTI_FTSR_MR21              EXTI_FTSR_21
#endif

#if defined(__MM0P1)
    #define EXTI_FTSR_MR22              EXTI_FTSR_22
    #define EXTI_FTSR_MR23              EXTI_FTSR_23
#endif

#if defined(__MM0P1) || defined(__MM0Q1)
    #define EXTI_FTSR_MR24              EXTI_FTSR_24
#endif

#if defined(__MM3N1) || defined(__MM3O1)
    #define EXTI_SWIER_MR17             EXTI_SWIER_17
#endif

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1)
    #define EXTI_SWIER_MR18             EXTI_SWIER_18
#endif

#define EXTI_SWIER_MR19                 EXTI_SWIER_19

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
    #define EXTI_SWIER_MR20             EXTI_SWIER_20
#endif

#if defined(__MM3O1) || defined(__MM0P1)
    #define EXTI_SWIER_MR21             EXTI_SWIER_21
#endif

#if defined(__MM0P1)
    #define EXTI_SWIER_MR22             EXTI_SWIER_22
    #define EXTI_SWIER_MR23             EXTI_SWIER_23
#endif

#if defined(__MM0P1) || defined(__MM0Q1)
    #define EXTI_SWIER_MR24             EXTI_SWIER_24
#endif

#if defined(__MM3N1) || defined(__MM3O1)
    #define EXTI_PR_MR17                EXTI_PR_17
#endif

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1)
    #define EXTI_PR_MR18                EXTI_PR_18
#endif

#define EXTI_PR_MR19                    EXTI_PR_19

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
    #define EXTI_PR_MR20                EXTI_PR_20
#endif

#if defined(__MM3O1) || defined(__MM0P1)
    #define EXTI_PR_MR21                EXTI_PR_21
#endif

#if defined(__MM0P1)
    #define EXTI_PR_MR22                EXTI_PR_22
    #define EXTI_PR_MR23                EXTI_PR_23
#endif

#if defined(__MM0P1) || defined(__MM0Q1)
    #define EXTI_PR_MR24                EXTI_PR_24
#endif


////////////////////////////////////////////////////////////////////////////////
/// @brief Version compatibility definition
////////////////////////////////////////////////////////////////////////////////
#define GPIO_Remap_SPI1                 AFIO_MAPR_SPI1_REMAP                    ///< SPI1 Alternate Function mapping
#define GPIO_Remap_I2C1                 AFIO_MAPR_I2C1_REMAP                    ///< I2C1 Alternate Function mapping
#define GPIO_Remap_UART1                AFIO_MAPR_UART1_REMAP                   ///< UART1 Alternate Function mapping
#define GPIO_FullRemap_UART3            AFIO_MAPR_UART3_REMAP                   ///< UART3 Full Alternate Function mapping

#define GPIO_PartialRemap_TIM1          AFIO_MAPR_TIM1_REMAP_PARTIAL            ///< TIM1 Partial Alternate Function mapping

#define GPIO_FullRemap_TIM2             AFIO_MAPR_TIM2_REMAP                    ///< TIM2 Full Alternate Function mapping
#define GPIO_PartialRemap1_TIM2         AFIO_MAPR_TIM2_REMAP_PARTIAL1           ///< TIM2 Partial1 Alternate Function mapping
#define GPIO_PartialRemap2_TIM2         AFIO_MAPR_TIM2_REMAP_PARTIAL2           ///< TIM2 Partial2 Alternate Function mapping

#define GPIO_FullRemap_TIM3             AFIO_MAPR_TIM3_REMAP                    ///< TIM3 Full Alternate Function mapping
#define GPIO_PartialRemap_TIM3          AFIO_MAPR_TIM3_REMAP_PARTIAL            ///< TIM3 Partial Alternate Function mapping

#define GPIO_Remap1_CAN1                AFIO_MAPR_CAN_REMAP_PARTIAL2            ///< CAN Alternate Function mapping
#define GPIO_Remap2_CAN1                AFIO_MAPR_CAN_REMAP_PARTIAL3            ///< CAN Alternate Function mapping

#define GPIO_Remap_PD01                 AFIO_MAPR_PD01_REMAP                    ///< PD01 Alternate Function mapping

#define GPIO_Remap_SWJ_NoJTRST          AFIO_MAPR_SWJ_CFG_NOJNTRST              ///< Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST
#define GPIO_Remap_SWJ_JTAGDisable      AFIO_MAPR_SWJ_CFG_JTAGDISABLE           ///< JTAG-DP Disabled and SW-DP Enabled
#define GPIO_Remap_SWJ_Disable          AFIO_MAPR_SWJ_CFG_DISABLE               ///< Full SWJ Disabled (JTAG-DP + SW-DP)

////////////////////////////////////////////////////////////////////////////////
/// @brief Version compatibility definition
////////////////////////////////////////////////////////////////////////////////
//Bit
#define I2C_CON_MASTER_MODE             I2C_CR_MASTER
#define I2C_CON_SPEED                   I2C_CR_SPEED
#define I2C_CON_SPEED_STANDARD          I2C_CR_STD
#define I2C_CON_SPEED_FAST              I2C_CR_FAST
#define I2C_CON_10BITADDR_SLAVE         I2C_CR_SLAVE10
#define I2C_CON_10BITADDR_MASTER        I2C_CR_MASTER10
#define I2C_CON_RESTART_EN              I2C_CR_REPEN
#define I2C_CON_SLAVE_DISABLE           I2C_CR_SLAVEDIS
#define I2C_CON_STOP_DET_IFADDRESSED    I2C_CR_STOPINT
#define I2C_CON_EMPTY_CTRL              I2C_CR_EMPINT
#define I2C_CON_STOP                    I2C_CR_STOP
#define I2C_CON_RESTART                 I2C_CR_RESTART

#define I2C_TAR_TAR                     I2C_TAR_ADDR
#define I2C_TAR_GC_OR_START             I2C_TAR_GC
#define I2C_SAR_SAR                     I2C_SAR_ADDR
#define I2C_DATA_CMD_DAT                I2C_DR_DAT
#define I2C_DATA_CMD_CMD                I2C_DR_CMD
#define I2C_DATA_CMD_STOP               I2C_DR_STOP
#define I2C_DATA_CMD_RESTART            I2C_DR_RESTART

#define I2C_SS_SCL_HCNT                 I2C_SSHR_CNT
#define I2C_SS_SCL_LCNT                 I2C_SSLR_CNT
#define I2C_FS_SCL_HCNT                 I2C_FSHR_CNT
#define I2C_FS_SCL_LCNT                 I2C_FSLR_CNT

#define I2C_INTR_STAT_RX_UNDER          I2C_ISR_RX_UNDER
#define I2C_INTR_STAT_RX_OVER           I2C_ISR_RX_OVER
#define I2C_INTR_STAT_RX_FULL           I2C_ISR_RX_FULL
#define I2C_INTR_STAT_TX_OVER           I2C_ISR_TX_OVER
#define I2C_INTR_STAT_TX_EMPTY          I2C_ISR_TX_EMPTY
#define I2C_INTR_STAT_RX_REQ            I2C_ISR_RX_REQ
#define I2C_INTR_STAT_TX_ABRT           I2C_ISR_TX_ABRT
#define I2C_INTR_STAT_RX_DONE           I2C_ISR_RX_DONE
#define I2C_INTR_STAT_ACTIVITY          I2C_ISR_ACTIV
#define I2C_INTR_STAT_STOP_DET          I2C_ISR_STOP
#define I2C_INTR_STAT_START_DET         I2C_ISR_START
#define I2C_INTR_STAT_GEN_CALL          I2C_ISR_GC
#define I2C_INTR_STAT_RESTART_DET       I2C_ISR_RESTART
#define I2C_INTR_STAT_MST_ON_HOLD       I2C_ISR_HOLD

#define I2C_INTR_MASK_RX_UNDER          I2C_IMR_RX_UNDER
#define I2C_INTR_MASK_RX_OVER           I2C_IMR_RX_OVER
#define I2C_INTR_MASK_RX_FULL           I2C_IMR_RX_FULL
#define I2C_INTR_MASK_TX_OVER           I2C_IMR_TX_OVER
#define I2C_INTR_MASK_TX_EMPTY          I2C_IMR_TX_EMPTY
#define I2C_INTR_MASK_RX_REQ            I2C_IMR_RX_REQ
#define I2C_INTR_MASK_TX_ABRT           I2C_IMR_TX_ABRT
#define I2C_INTR_MASK_RX_DONE           I2C_IMR_RX_DONE
#define I2C_INTR_MASK_ACTIVITY          I2C_IMR_ACTIV
#define I2C_INTR_MASK_STOP_DET          I2C_IMR_STOP
#define I2C_INTR_MASK_START_DET         I2C_IMR_START
#define I2C_INTR_MASK_GEN_CALL          I2C_IMR_GC
#define I2C_INTR_MASK_RESTART_DET       I2C_IMR_RESTART
#define I2C_INTR_MASK_MST_ON_HOLD       I2C_IMR_HOLD

#define I2C_RAW_INTR_MASK_RX_UNDER      I2C_RAWISR_RX_UNDER
#define I2C_RAW_INTR_MASK_RX_OVER       I2C_RAWISR_RX_OVER
#define I2C_RAW_INTR_MASK_RX_FULL       I2C_RAWISR_RX_FULL
#define I2C_RAW_INTR_MASK_TX_OVER       I2C_RAWISR_TX_OVER
#define I2C_RAW_INTR_MASK_TX_EMPTY      I2C_RAWISR_TX_EMPTY
#define I2C_RAW_INTR_MASK_RX_REQ        I2C_RAWISR_RX_REQ
#define I2C_RAW_INTR_MASK_TX_ABRT       I2C_RAWISR_TX_ABRT
#define I2C_RAW_INTR_MASK_RX_DONE       I2C_RAWISR_RX_DONE
#define I2C_RAW_INTR_MASK_ACTIVITY      I2C_RAWISR_ACTIV
#define I2C_RAW_INTR_MASK_STOP_DET      I2C_RAWISR_STOP
#define I2C_RAW_INTR_MASK_START_DET     I2C_RAWISR_START
#define I2C_RAW_INTR_MASK_GEN_CALL      I2C_RAWISR_GC
#define I2C_RAW_INTR_MASK_RESTART_DET   I2C_RAWISR_RESTART
#define I2C_RAW_INTR_MASK_MST_ON_HOLD   I2C_RAWISR_HOLD

#define I2C_RX_TL                       I2C_RXTLR_TL
#define I2C_TX_TL                       I2C_TXTLR_TL

#define I2C_CLR_INTR                    I2C_ICR
#define I2C_CLR_RX_UNDER                I2C_RX_UNDER
#define I2C_CLR_RX_OVER                 I2C_RX_OVER
#define I2C_CLR_TX_OVER                 I2C_TX_OVER
#define I2C_CLR_RX_REQ                  I2C_RX_REQ
#define I2C_CLR_TX_ABRT                 I2C_TX_ABRT
#define I2C_CLR_RX_DONE                 I2C_RX_DONE
#define I2C_CLR_ACTIVITY                I2C_ACTIV
#define I2C_CLR_STOP_DET                I2C_STOP
#define I2C_CLR_START_DET               I2C_START
#define I2C_CLR_GEN_CALL                I2C_GC

#define I2C_ENABLE_ENABLE               I2C_ENR_ENABLE
#define I2C_ENABLE_ABORT                I2C_ENR_ABORT

#define I2C_STATUS_ACTIVITY             I2C_SR_ACTIV
#define I2C_STATUS_TFNF                 I2C_SR_TFNF
#define I2C_STATUS_TFE                  I2C_SR_TFE
#define I2C_STATUS_RFNE                 I2C_SR_RFNE
#define I2C_STATUS_RFF                  I2C_SR_RFF
#define I2C_STATUS_MST_ACTIVITY         I2C_SR_MST_ACTIV
#define I2C_STATUS_SLV_ACTIVITY         I2C_SR_SLV_ACTIV

#define I2C_TXFLR                       I2C_TXFLR_CNT
#define I2C_RXFLR                       I2C_RXFLR_CNT

#define I2C_SDA_TX_HOLD                 I2C_HOLD_TXCNT
#define I2C_SDA_RX_HOLD                 I2C_HOLD_RXCNT

#define I2C_DMA_CR_RDMAE                I2C_DMA_RXEN
#define I2C_DMA_CR_TDMAE                I2C_DMA_TXEN

#define I2C_SDA_SET_UP                  I2C_SETUP_CNT
#define I2C_ACK_GENERAL_CALL            I2C_GCR_GC


#define IWDG_PR_PR                      IWDG_PR_PRE
#define IWDG_PR_PR_DIV4                 IWDG_PR_PRE_DIV4
#define IWDG_PR_PR_DIV8                 IWDG_PR_PRE_DIV8
#define IWDG_PR_PR_DIV16                IWDG_PR_PRE_DIV16
#define IWDG_PR_PR_DIV32                IWDG_PR_PRE_DIV32
#define IWDG_PR_PR_DIV64                IWDG_PR_PRE_DIV64
#define IWDG_PR_PR_DIV128               IWDG_PR_PRE_DIV128
#define IWDG_PR_PR_DIV256               IWDG_PR_PRE_DIV256
////////////////////////////////////////////////////////////////////////////////
/// @brief Version compatibility definition
////////////////////////////////////////////////////////////////////////////////
#define RCC_AHBENR_DMAEN                RCC_AHBENR_DMA1

#define RCC_APB2RSTR_IOPA               RCC_APB2RSTR_GPIOA
#define RCC_APB2RSTR_IOPB               RCC_APB2RSTR_GPIOB
#define RCC_APB2RSTR_IOPC               RCC_APB2RSTR_GPIOC
#define RCC_APB2RSTR_IOPD               RCC_APB2RSTR_GPIOD
#define RCC_APB2RSTR_IOPE               RCC_APB2RSTR_GPIOE

#define RCC_APB2ENR_IOPA                RCC_APB2ENR_GPIOA
#define RCC_APB2ENR_IOPB                RCC_APB2ENR_GPIOB
#define RCC_APB2ENR_IOPC                RCC_APB2ENR_GPIOC
#define RCC_APB2ENR_IOPD                RCC_APB2ENR_GPIOD
#define RCC_APB2ENR_IOPE                RCC_APB2ENR_GPIOE

////////////////////////////////////////////////////////////////////////////////
/// @defgroup AHB_peripheral
/// @{
#define RCC_AHBPeriph_DMA1              RCC_AHBENR_DMA1
#define RCC_AHBPeriph_SRAM              RCC_AHBENR_SRAM
#define RCC_AHBPeriph_FLITF             RCC_AHBENR_FLITF
#define RCC_AHBPeriph_CRC               RCC_AHBENR_CRC
#if defined(__MM3N1) || defined(__MM0N1)
#define RCC_AHBPeriph_AES               RCC_AHBENR_AES
#endif
#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
#define RCC_AHBPeriph_GPIOA             RCC_AHBENR_GPIOA
#define RCC_AHBPeriph_GPIOB             RCC_AHBENR_GPIOB
#define RCC_AHBPeriph_GPIOC             RCC_AHBENR_GPIOC
#define RCC_AHBPeriph_GPIOD             RCC_AHBENR_GPIOD
#endif
#if defined(__MM0N1)
#define RCC_AHBPeriph_GPIOE             RCC_AHBENR_GPIOE
#endif
#if defined(__MM0P1) || defined(__MM0Q1)
#define RCC_AHBPeriph_HIV               RCC_AHBENR_HWDIV
#endif

#if defined(__MM0P1)
#define RCC_AHBPeriph_HWROOT            RCC_AHBENR_HWSQRT
#define RCC_AHBPeriph_HWDIV             RCC_AHBENR_HWDIV
#endif



/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup APB2_peripheral
/// @{
#if defined(__MM3N1)
#define RCC_APB2Periph_AFIO             RCC_APB2ENR_EXTI
#define RCC_APB2Periph_GPIOA            RCC_APB2ENR_GPIOA
#define RCC_APB2Periph_GPIOB            RCC_APB2ENR_GPIOB
#define RCC_APB2Periph_GPIOC            RCC_APB2ENR_GPIOC
#define RCC_APB2Periph_GPIOD            RCC_APB2ENR_GPIOD
#define RCC_APB2Periph_GPIOE            RCC_APB2ENR_GPIOE
#define RCC_APB2Periph_ADC2             RCC_APB2ENR_ADC2
#define RCC_APB2Periph_ALL              0x5FFD
#endif
#define RCC_APB2Periph_ADC1             RCC_APB2ENR_ADC1
#define RCC_APB2Periph_TIM1             RCC_APB2ENR_TIM1
#define RCC_APB2Periph_SPI1             RCC_APB2ENR_SPI1
#define RCC_APB2Periph_UART1            RCC_APB2ENR_UART1
#define RCC_APB2Periph_COMP             RCC_APB2ENR_COMP
#if defined(__MM0N1) || defined(__MM0P1) || defined(__MM0Q1)
#define RCC_APB2Periph_SYSCFG           RCC_APB2ENR_EXTI
#define RCC_APB2Periph_TIM14            RCC_APB2ENR_TIM14
#define RCC_APB2Periph_TIM16            RCC_APB2ENR_TIM16
#define RCC_APB2Periph_TIM17            RCC_APB2ENR_TIM17
#define RCC_APB2Periph_DBGMCU           RCC_APB2ENR_DBGMCU
#endif

#if defined(__MM0P1)
#define RCC_APB2Periph_ADC2             RCC_APB2ENR_ADC2
#endif

#if defined(__MM0Q1) || defined(__MM0Q1)
#define RCC_APB2Periph_PWM              RCC_APB2ENR_PWM
#endif

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup APB1_peripheral
/// @{
#define RCC_APB1Periph_TIM2             RCC_APB1ENR_TIM2
#define RCC_APB1Periph_TIM3             RCC_APB1ENR_TIM3
#if defined(__MM3N1)
#define RCC_APB1Periph_TIM4             RCC_APB1ENR_TIM4
#define RCC_APB1Periph_UART3            RCC_APB1ENR_UART3
#define RCC_APB1Periph_BKP              RCC_APB1ENR_BKP
#define RCC_APB1Periph_DAC              RCC_APB1ENR_DAC
#define RCC_APB1Periph_I2C2             RCC_APB1ENR_I2C2
#define RCC_APB1Periph_ALL              0x3AE64807
#endif
#if defined(__MM3N1) || defined(__MM0N1)
#define RCC_APB1Periph_USB              RCC_APB1ENR_USB
#define RCC_APB1Periph_CAN              RCC_APB1ENR_CAN
#endif
#if defined(__MM0N1)
#define RCC_APB1Periph_CRS              RCC_APB1ENR_CRS
#endif
#define RCC_APB1Periph_UART2            RCC_APB1ENR_UART2
#define RCC_APB1Periph_I2C1             RCC_APB1ENR_I2C1
#define RCC_APB1Periph_SPI2             RCC_APB1ENR_SPI2
#define RCC_APB1Periph_WWDG             RCC_APB1ENR_WWDG
#define RCC_APB1Periph_PWR              RCC_APB1ENR_PWR

////////////////////////////////////////////////////////////////////////////////
/// @brief Version compatibility definition
////////////////////////////////////////////////////////////////////////////////
#define SPI_TXREG_TXREG                 SPI_TDR_TDR
#define SPI_RXREG_RXREG                 SPI_TDR_TDR

#define SPI_CSTAT_TXEPT                 SPI_SR_TXEPT
#define SPI_CSTAT_RXAVL                 SPI_SR_RXAVL
#define SPI_CSTAT_TXFULL                SPI_SR_TXFULL
#define SPI_CSTAT_RXAVL_4BYTE           SPI_SR_RXAVL_4BYTE
#define SPI_CSTAT_TXFADDR               SPI_SR_TXFADDR
#define SPI_CSTAT_RXFADDR               SPI_SR_RXFADDR

#define SPI_INTSTAT_TX_INTF             SPI_ISR_TX_INTF
#define SPI_INTSTAT_RX_INTF             SPI_ISR_RX_INTF
#define SPI_INTSTAT_UNDERRUN_INTF       SPI_ISR_UNDERRUN_INTF
#define SPI_INTSTAT_RXOERR_INTF         SPI_ISR_RXOERR_INTF
#define SPI_INTSTAT_RXMATCH_INTF        SPI_ISR_RXMATCH_INTF
#define SPI_INTSTAT_RXFULL_INTF         SPI_ISR_RXFULL_INTF
#define SPI_INTSTAT_TXEPT_INTF          SPI_ISR_TXEPT_INTF

#define SPI_INTEN_TX_IEN                SPI_IER_TX_IEN
#define SPI_INTEN_RX_IEN                SPI_IER_RX_IEN
#define SPI_INTEN_UNDERRUN_IEN          SPI_IER_UNDERRUN_IEN
#define SPI_INTEN_RXOERR_IEN            SPI_IER_RXOERR_IEN
#define SPI_INTEN_RXMATCH_IEN           SPI_IER_RXMATCH_IEN
#define SPI_INTEN_RXFULL_IEN            SPI_IER_RXFULL_IEN
#define SPI_INTEN_TXEPT_IEN             SPI_IER_TXEPT_IEN

#define SPI_INTCLR_TX_ICLR              SPI_ICR_TX_ICLR
#define SPI_INTCLR_RX_ICLR              SPI_ICR_RX_ICLR
#define SPI_INTCLR_UNDERRUN_ICLR        SPI_ICR_UNDERRUN_ICLR
#define SPI_INTCLR_RXOERR_ICLR          SPI_ICR_RXOERR_ICLR
#define SPI_INTCLR_RXMATCH_ICLR         SPI_ICR_RXMATCH_ICLR
#define SPI_INTCLR_RXFULL_ICLR          SPI_ICR_RXFULL_ICLR
#define SPI_INTCLR_TXEPT_ICLR           SPI_ICR_TXEPT_ICLR

#define SPI_GCTL_SPIEN                  SPI_GCR_SPIEN
#define SPI_GCTL_INT_EN                 SPI_GCR_IEN
#define SPI_GCTL_MM                     SPI_GCR_MODE
#define SPI_GCTL_TXEN                   SPI_GCR_TXEN
#define SPI_GCTL_RXEN                   SPI_GCR_RXEN

#define SPI_GCTL_RXTLF                  SPI_GCR_RXTLF
#define SPI_GCTL_RXTLF_One              SPI_GCR_RXTLF_One
#define SPI_GCTL_RXTLF_Half             SPI_GCR_RXTLF_Half

#define SPI_GCTL_TXTLF_Pos              SPI_GCR_TXTLF_Pos
#define SPI_GCTL_TXTLF                  SPI_GCR_TXTLF
#define SPI_GCTL_TXTLF_One              SPI_GCR_TXTLF_One
#define SPI_GCTL_TXTLF_Half             SPI_GCR_TXTLF_Half

#define SPI_GCTL_DMAEN                  SPI_GCR_DMAEN
#define SPI_GCTL_NSS_SEL                SPI_GCR_NSS
#define SPI_GCTL_DATA_SEL               SPI_GCR_DWSEL

#if defined(__MM0P1) || defined(__MM0Q1)
    #define SPI_GCTL_NSSTOG             SPI_GCR_NSSTOG
#endif

#define SPI_CCTL_CPHA                   SPI_CCR_CPHA
#define SPI_CCTL_CPOL                   SPI_CCR_CPOL
#define SPI_CCTL_LSBFE                  SPI_CCR_LSBFE
#define SPI_CCTL_SPILEN                 SPI_CCR_SPILEN
#define SPI_CCTL_RXEDGE                 SPI_CCR_RXEDGE
#define SPI_CCTL_TXEDGE                 SPI_CCR_TXEDGE

#if defined(__MM0P1) || defined(__MM0Q1)
    #define SPI_CCTL_CPHASEL            SPI_CCR_CPHASEL
#endif

#if defined(__MM0Q1)
    #define SPI_CCTL_HISPD              SPI_CCR_HISPD
#endif

#define SPI_SPBRG_SPBRG                 SPI_BRR_DIVF

#define SPI_RXDNR_RXDNR                 SPI_RDNR_RDN

#define SPI_EXTCTL_EXTLEN               SPI_ECR_EXTLEN
#define TIM_CR1_ARPE                    TIM_CR1_ARPEN

#define TIM_SMCR_ECE                    TIM_SMCR_ECEN

#define TIM_DIER_UIE                    TIM_DIER_UIEN
#define TIM_DIER_CC1IE                  TIM_DIER_CC1IEN
#define TIM_DIER_CC2IE                  TIM_DIER_CC2IEN
#define TIM_DIER_CC3IE                  TIM_DIER_CC3IEN
#define TIM_DIER_CC4IE                  TIM_DIER_CC4IEN
#define TIM_DIER_COMIE                  TIM_DIER_COMIEN
#define TIM_DIER_TIE                    TIM_DIER_TIEN
#define TIM_DIER_BIE                    TIM_DIER_BIEN
#define TIM_DIER_UDE                    TIM_DIER_UDEN
#define TIM_DIER_CC1DE                  TIM_DIER_CC1DEN
#define TIM_DIER_CC2DE                  TIM_DIER_CC2DEN
#define TIM_DIER_CC3DE                  TIM_DIER_CC3DEN
#define TIM_DIER_CC4DE                  TIM_DIER_CC4DEN
#define TIM_DIER_COMDE                  TIM_DIER_COMDEN
#define TIM_DIER_TDE                    TIM_DIER_TDEN

#if defined(__MM0P1) || defined(__MM0Q1)
#define TIM_DIER_CC5IE                  TIM_DIER_CC5IEN
#define TIM_DIER_CC5DE                  TIM_DIER_CC5DEN
#endif

#define TIM_SR_UIF                      TIM_SR_UI
#define TIM_SR_CC1IF                    TIM_SR_CC1I
#define TIM_SR_CC2IF                    TIM_SR_CC2I
#define TIM_SR_CC3IF                    TIM_SR_CC3I
#define TIM_SR_CC4IF                    TIM_SR_CC4I
#define TIM_SR_COMIF                    TIM_SR_COMI
#define TIM_SR_TIF                      TIM_SR_TI
#define TIM_SR_BIF                      TIM_SR_BI
#define TIM_SR_CC1OF                    TIM_SR_CC1O
#define TIM_SR_CC2OF                    TIM_SR_CC2O
#define TIM_SR_CC3OF                    TIM_SR_CC3O
#define TIM_SR_CC4OF                    TIM_SR_CC4O

#if defined(__MM0P1) || defined(__MM0Q1)
#define TIM_SR_CC5IF                    TIM_SR_CC5I
#endif

#define TIM_CCMR1_OC1FE                 TIM_CCMR1_OC1FEN
#define TIM_CCMR1_OC1PE                 TIM_CCMR1_OC1PEN
#define TIM_CCMR1_OC1CE                 TIM_CCMR1_OC1CEN
#define TIM_CCMR1_OC2FE                 TIM_CCMR1_OC2FEN
#define TIM_CCMR1_OC2PE                 TIM_CCMR1_OC2PEN
#define TIM_CCMR1_OC2CE                 TIM_CCMR1_OC2CEN

#define TIM_CCMR2_OC3FE                 TIM_CCMR2_OC3FEN
#define TIM_CCMR2_OC3PE                 TIM_CCMR2_OC3PEN
#define TIM_CCMR2_OC3CE                 TIM_CCMR2_OC3CEN
#define TIM_CCMR2_OC4FE                 TIM_CCMR2_OC4FEN
#define TIM_CCMR2_OC4PE                 TIM_CCMR2_OC4PEN
#define TIM_CCMR2_OC4CE                 TIM_CCMR2_OC4CEN

#define TIM_CCER_CC1E                   TIM_CCER_CC1EN
#define TIM_CCER_CC1NE                  TIM_CCER_CC1NEN
#define TIM_CCER_CC2E                   TIM_CCER_CC2EN
#define TIM_CCER_CC2NE                  TIM_CCER_CC2NEN
#define TIM_CCER_CC3E                   TIM_CCER_CC3EN
#define TIM_CCER_CC3NE                  TIM_CCER_CC3NEN
#define TIM_CCER_CC4E                   TIM_CCER_CC4EN

#if defined(__MM0P1) || defined(__MM0Q1)
#define TIM_CCER_CC5E                   TIM_CCER_CC5EN
#endif

#define TIM_BDTR_BKE                    TIM_BDTR_BKEN
#define TIM_BDTR_AOE                    TIM_BDTR_AOEN
#define TIM_BDTR_MOE                    TIM_BDTR_MOEN

#if defined(__MM3O1) || defined(__MM0P1) || defined(__MM0Q1)
#define TIM_BDTR_DOE                    TIM_BDTR_DOEN
#endif

#if defined(__MM0P1) || defined(__MM0Q1)
#define TIM_CCMR3_OC5FE                 TIM_CCMR3_OC5FEN
#define TIM_CCMR3_OC5PE                 TIM_CCMR3_OC5PEN
#define TIM_CCMR3_OC5CE                 TIM_CCMR3_OC5CEN
#endif
#define UART_TDR_TXREG                  UART_TDR_DATA
#define UART_RDR_RXREG                  UART_RDR_DATA

#define UART_ISR_TX_INTF                UART_ISR_TX
#define UART_ISR_RX_INTF                UART_ISR_RX
#define UART_ISR_RXOERR_INTF            UART_ISR_RXOERR
#define UART_ISR_RXPERR_INTF            UART_ISR_RXPERR
#define UART_ISR_RXFERR_INTF            UART_ISR_RXFERR
#define UART_ISR_RXBRK_INTF             UART_ISR_RXBRK

#define UART_IER_TXIEN                  UART_IER_TX
#define UART_IER_RXIEN                  UART_IER_RXI
#define UART_IER_RXOERREN               UART_IER_RXOERR
#define UART_IER_RXPERREN               UART_IER_RXPERR
#define UART_IER_RXFERREN               UART_IER_RXFERR
#define UART_IER_RXBRKEN                UART_IER_RXBRK

#define UART_ICR_TXICLR                 UART_ICR_TX
#define UART_ICR_RXICLR                 UART_ICR_RX
#define UART_ICR_RXOERRCLR              UART_ICR_RXOERR
#define UART_ICR_RXPERRCLR              UART_ICR_RXPERR
#define UART_ICR_RXFERRCLR              UART_ICR_RXFERR
#define UART_ICR_RXBRKCLR               UART_ICR_RXBRK

#define UART_Mode_Rx                    UART_GCR_RX
#define UART_Mode_Tx                    UART_GCR_TX
#define UART_EN                         UART_GCR_UART
#define UART_IT_RXBRK                   UART_IER_RXBRK
#define UART_IT_ERR                     UART_IER_RXFERR
#define UART_IT_PE                      UART_IER_RXPERR
#define UART_OVER_ERR                   UART_IER_RXOERR
#define UART_IT_RXIEN                   UART_IER_RX
#define UART_IT_TXIEN                   UART_IER_TX

#define UART_BRR_DIV_MANTISSA           UART_BRR_MANTISSA
#define UART_BRR_DIV_FRACTION           UART_BRR_FRACTION

#if defined(__MM0P1) || defined(__MM0Q1)
    #define UART_ISR_TXC_INTF           UART_ISR_TXC
    #define UART_ISR_TXBRK_INTF         UART_ISR_TXBRK
    #define UART_ISR_RXB8_INTF          UART_ISR_RXB8

    #define UART_IT_RXB8                UART_IER_RXB8
    #define UART_IT_TXBRK               UART_IER_TXBRK
    #define UART_IT_TXCIEN              UART_IER_TXC
    #define UART_ICR_TXCCLR             UART_ICR_TXC
    #define UART_ICR_TXBRKCLR           UART_ICR_TXBRK
    #define UART_ICR_RXB8CLR            UART_ICR_RXB8
    #define UART_SCR_SCAEN              UART_SCR_SCARB
#endif

#define UART_DMAReq_EN                  UART_GCR_DMA

#define UART_FLAG_TXEMPTY               UART_CSR_TXEPT
#define UART_FLAG_TXFULL                UART_CSR_TXFULL
#define UART_FLAG_RXAVL                 UART_CSR_RXAVL
#define UART_FLAG_TXEPT                 UART_CSR_TXC
#define WWDG_CR_T                       WWDG_CR_CNT
