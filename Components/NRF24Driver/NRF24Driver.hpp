// ======================================================================
// \title  NRF24Driver.hpp
// \author mustafa
// \brief  hpp file for NRF24Driver component implementation class
// ======================================================================

#ifndef Components_NRF24Driver_HPP
#define Components_NRF24Driver_HPP

#include "Components/NRF24Driver/NRF24DriverComponentAc.hpp"

namespace Components {

// ======================================================================
// NRF24L01+ Register Definitions
// ======================================================================

// Register Map
static const U8 NRF24_REG_CONFIG     = 0x00;
static const U8 NRF24_REG_EN_AA      = 0x01;
static const U8 NRF24_REG_EN_RXADDR  = 0x02;
static const U8 NRF24_REG_SETUP_AW   = 0x03;
static const U8 NRF24_REG_SETUP_RETR = 0x04;
static const U8 NRF24_REG_RF_CH      = 0x05;
static const U8 NRF24_REG_RF_SETUP   = 0x06;
static const U8 NRF24_REG_STATUS     = 0x07;
static const U8 NRF24_REG_RX_ADDR_P0 = 0x0A;
static const U8 NRF24_REG_TX_ADDR    = 0x10;
static const U8 NRF24_REG_RX_PW_P0   = 0x11;
static const U8 NRF24_REG_FIFO_STATUS = 0x17;

// Commands
static const U8 NRF24_CMD_R_REGISTER    = 0x00;
static const U8 NRF24_CMD_W_REGISTER    = 0x20;
static const U8 NRF24_CMD_R_RX_PAYLOAD  = 0x61;
static const U8 NRF24_CMD_W_TX_PAYLOAD  = 0xA0;
static const U8 NRF24_CMD_FLUSH_TX      = 0xE1;
static const U8 NRF24_CMD_FLUSH_RX      = 0xE2;
static const U8 NRF24_CMD_NOP           = 0xFF;

// Configuration bits
static const U8 NRF24_CONFIG_PWR_UP     = 0x02;
static const U8 NRF24_CONFIG_PRIM_RX    = 0x01;
static const U8 NRF24_CONFIG_EN_CRC     = 0x08;
static const U8 NRF24_CONFIG_CRCO       = 0x04;

// Status bits
static const U8 NRF24_STATUS_RX_DR      = 0x40;
static const U8 NRF24_STATUS_TX_DS      = 0x20;
static const U8 NRF24_STATUS_MAX_RT     = 0x10;

// RF Setup bits
static const U8 NRF24_RF_SETUP_RF_PWR_LOW   = 0x00;  // -18dBm
static const U8 NRF24_RF_SETUP_RF_PWR_HIGH  = 0x02;  // -12dBm
static const U8 NRF24_RF_SETUP_RF_PWR_MAX   = 0x04;  // -6dBm
static const U8 NRF24_RF_SETUP_RF_PWR_MIN   = 0x06;  // 0dBm
static const U8 NRF24_RF_SETUP_RF_DR_LOW    = 0x20;  // 250kbps
static const U8 NRF24_RF_SETUP_RF_DR_HIGH   = 0x08;  // 2Mbps

// Constants
static const U8 NRF24_PAYLOAD_SIZE = 32;
static const U8 NRF24_ADDR_WIDTH = 5;

 class NRF24Driver :
   public NRF24DriverComponentBase
 {

   public:

     // ----------------------------------------------------------------------
     // Component construction and destruction
     // ----------------------------------------------------------------------

     //! Construct NRF24Driver object
     NRF24Driver(
         const char* const compName //!< The component name
     );

     //! Initialize component
     void init(
         const NATIVE_INT_TYPE queueDepth, 
         const NATIVE_INT_TYPE instance = 0
     );

     //! Destroy NRF24Driver object
     ~NRF24Driver();

   private:

     // ----------------------------------------------------------------------
     // Command handlers
     // ----------------------------------------------------------------------

     void INIT_cmdHandler(
         const FwOpcodeType opCode,
         const U32 cmdSeq
     ) override;

     void SEND_PACKET_cmdHandler(
         const FwOpcodeType opCode,
         const U32 cmdSeq,
         const Fw::CmdStringArg& data
     ) override;

     void START_RECEIVE_cmdHandler(
         const FwOpcodeType opCode,
         const U32 cmdSeq
     ) override;

     void CONFIGURE_cmdHandler(
         const FwOpcodeType opCode,
         const U32 cmdSeq,
         const U8 channel,
         const U8 power
     ) override;

     // ----------------------------------------------------------------------
     // Helper functions
     // ----------------------------------------------------------------------

     bool writeRegister(U8 reg, U8 value);
     bool readRegister(U8 reg, U8& value);
     bool writeRegisterMulti(U8 reg, const U8* data, U8 length);
     bool readRegisterMulti(U8 reg, U8* data, U8 length);
     
     void setCE(bool state);
     void setCSN(bool state);
     
     bool initializeRadio();
     bool setTxMode();
     bool setRxMode();
     bool transmitPacket(const U8* data, U8 length);
     bool receivePacket(U8* data, U8& length);
     void flushTx();
     void flushRx();
     U8 getStatus();

     // ----------------------------------------------------------------------
     // Member variables
     // ----------------------------------------------------------------------

     U8 m_currentChannel;
     U8 m_currentPower;
     bool m_isInitialized;
     bool m_rxMode;
     U8 m_txAddr[NRF24_ADDR_WIDTH];
     U8 m_rxAddr[NRF24_ADDR_WIDTH];

 };

}

#endif