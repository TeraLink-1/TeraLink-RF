// ======================================================================
// \title  NRF24Driver.cpp
// \author mustafa
// \brief  cpp file for NRF24Driver component implementation class
// ======================================================================

#include "Components/NRF24Driver/NRF24Driver.hpp"
#include <cstring>

namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  NRF24Driver ::
    NRF24Driver(const char* const compName) :
      NRF24DriverComponentBase(compName),
      m_currentChannel(76),  // Default channel
      m_currentPower(3),     // Max power
      m_isInitialized(false),
      m_rxMode(false)
  {
    // Default addresses (can be configured later)
    U8 defaultTxAddr[NRF24_ADDR_WIDTH] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    U8 defaultRxAddr[NRF24_ADDR_WIDTH] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    
    memcpy(m_txAddr, defaultTxAddr, NRF24_ADDR_WIDTH);
    memcpy(m_rxAddr, defaultRxAddr, NRF24_ADDR_WIDTH);
  }

  void NRF24Driver ::
    init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance)
  {
    NRF24DriverComponentBase::init(queueDepth, instance);
  }

  NRF24Driver ::
    ~NRF24Driver()
  {
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void NRF24Driver ::
    INIT_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq)
  {
    // Initialize nRF24L01+ module
    if (initializeRadio()) {
      m_isInitialized = true;
      this->log_ACTIVITY_HI_InitComplete();
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
      this->log_WARNING_HI_Error(-1);
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
  }

  void NRF24Driver ::
    SEND_PACKET_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& data
    )
  {
    if (!m_isInitialized) {
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
      return;
    }
    
    // Set to TX mode and transmit
    if (setTxMode()) {
      U8 dataLen = static_cast<U8>(strlen(data.toChar()));
      if (dataLen > NRF24_PAYLOAD_SIZE) {
        dataLen = NRF24_PAYLOAD_SIZE;
      }
      
      if (transmitPacket(reinterpret_cast<const U8*>(data.toChar()), dataLen)) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
      } else {
        this->log_WARNING_HI_Error(-2);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
      }
    } else {
      this->log_WARNING_HI_Error(-3);
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
  }

  void NRF24Driver ::
    START_RECEIVE_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq) 
  {
    if (!m_isInitialized) {
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
      return;
    }

    if (setRxMode()) {
      m_rxMode = true;
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
      this->log_WARNING_HI_Error(-4);
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
  }

  void NRF24Driver ::
    CONFIGURE_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const U8 channel,
        const U8 power
    )
  {
    if (channel > 125) {
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
      return;
    }

    if (power > 3) {
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
      return;
    }

    // Configure radio parameters
    if (m_isInitialized) {
      // Set RF channel
      if (!writeRegister(NRF24_REG_RF_CH, channel)) {
        this->log_WARNING_HI_Error(-5);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
      }
      
      // Set TX power level
      U8 rfSetup = 0;
      if (!readRegister(NRF24_REG_RF_SETUP, rfSetup)) {
        this->log_WARNING_HI_Error(-6);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
      }
      
      // Clear power bits and set new power level
      rfSetup &= ~0x06; // Clear bits 1-2
      switch (power) {
        case 0: rfSetup |= NRF24_RF_SETUP_RF_PWR_LOW; break;
        case 1: rfSetup |= NRF24_RF_SETUP_RF_PWR_HIGH; break;
        case 2: rfSetup |= NRF24_RF_SETUP_RF_PWR_MAX; break;
        case 3: rfSetup |= NRF24_RF_SETUP_RF_PWR_MIN; break;
      }
      
      if (!writeRegister(NRF24_REG_RF_SETUP, rfSetup)) {
        this->log_WARNING_HI_Error(-7);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
      }
    }
    
    m_currentChannel = channel;
    m_currentPower = power;
    
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  // ----------------------------------------------------------------------
  // Helper functions
  // ----------------------------------------------------------------------

  bool NRF24Driver ::
    writeRegister(U8 reg, U8 value)
  {
    setCSN(false);  // Start transaction
    
    U8 command = NRF24_CMD_W_REGISTER | reg;
    U8 writeData[2] = {command, value};
    U8 readData[2] = {0, 0};
    
    Fw::Buffer writeBuffer(writeData, sizeof(writeData));
    Fw::Buffer readBuffer(readData, sizeof(readData));
    
    // SPI port returns void, so we assume success
    this->spiOut_out(0, writeBuffer, readBuffer);
    
    setCSN(true);   // End transaction
    
    return true;  // Assume success since we can't check status
  }

  bool NRF24Driver ::
    readRegister(U8 reg, U8& value)
  {
    setCSN(false);  // Start transaction
    
    U8 command = NRF24_CMD_R_REGISTER | reg;
    U8 writeData[2] = {command, 0x00};
    U8 readData[2] = {0, 0};
    
    Fw::Buffer writeBuffer(writeData, sizeof(writeData));
    Fw::Buffer readBuffer(readData, sizeof(readData));
    
    // SPI port returns void, so we assume success
    this->spiOut_out(0, writeBuffer, readBuffer);
    
    setCSN(true);   // End transaction
    
    value = readData[1];
    return true;  // Assume success since we can't check status
  }

  bool NRF24Driver ::
    writeRegisterMulti(U8 reg, const U8* data, U8 length)
  {
    if (length > 32) return false;  // Safety check
    
    setCSN(false);
    
    U8 command = NRF24_CMD_W_REGISTER | reg;
    U8 writeData[33];  // Command + up to 32 bytes
    U8 readData[33];
    
    writeData[0] = command;
    memcpy(&writeData[1], data, length);
    
    Fw::Buffer writeBuffer(writeData, length + 1);
    Fw::Buffer readBuffer(readData, length + 1);
    
    // SPI port returns void, so we assume success
    this->spiOut_out(0, writeBuffer, readBuffer);
    
    setCSN(true);
    
    return true;  // Assume success since we can't check status
  }

  bool NRF24Driver ::
    readRegisterMulti(U8 reg, U8* data, U8 length)
  {
    if (length > 32) return false;  // Safety check
    
    setCSN(false);
    
    U8 command = NRF24_CMD_R_REGISTER | reg;
    U8 writeData[33];  // Command + up to 32 bytes
    U8 readData[33];
    
    writeData[0] = command;
    memset(&writeData[1], 0, length);
    
    Fw::Buffer writeBuffer(writeData, length + 1);
    Fw::Buffer readBuffer(readData, length + 1);
    
    // SPI port returns void, so we assume success
    this->spiOut_out(0, writeBuffer, readBuffer);
    
    setCSN(true);
    
    memcpy(data, &readData[1], length);
    return true;  // Assume success since we can't check status
  }

  void NRF24Driver ::
    setCE(bool state)
  {
    this->cePin_out(0, state ? Fw::Logic::HIGH : Fw::Logic::LOW);
  }

  void NRF24Driver ::
    setCSN(bool state)
  {
    this->csnPin_out(0, state ? Fw::Logic::HIGH : Fw::Logic::LOW);
  }

  bool NRF24Driver ::
    initializeRadio()
  {
    // Reset CE and CSN
    setCE(false);
    setCSN(true);
    
    // Small delay for startup
    Os::Task::delay(Fw::TimeInterval(0, 100000));  // 100ms delay
    
    // Power down first
    if (!writeRegister(NRF24_REG_CONFIG, 0x08)) return false;  // CRC enabled, power down
    
    // Set address width to 5 bytes
    if (!writeRegister(NRF24_REG_SETUP_AW, 0x03)) return false;
    
    // Set channel and power
    if (!writeRegister(NRF24_REG_RF_CH, m_currentChannel)) return false;
    
    // Set RF setup (1Mbps, max power)
    if (!writeRegister(NRF24_REG_RF_SETUP, NRF24_RF_SETUP_RF_PWR_MIN)) return false;
    
    // Set addresses
    if (!writeRegisterMulti(NRF24_REG_TX_ADDR, m_txAddr, NRF24_ADDR_WIDTH)) return false;
    if (!writeRegisterMulti(NRF24_REG_RX_ADDR_P0, m_rxAddr, NRF24_ADDR_WIDTH)) return false;
    
    // Enable auto-acknowledgment for pipe 0
    if (!writeRegister(NRF24_REG_EN_AA, 0x01)) return false;
    
    // Enable RX address for pipe 0
    if (!writeRegister(NRF24_REG_EN_RXADDR, 0x01)) return false;
    
    // Set payload size for pipe 0
    if (!writeRegister(NRF24_REG_RX_PW_P0, NRF24_PAYLOAD_SIZE)) return false;
    
    // Clear status flags
    if (!writeRegister(NRF24_REG_STATUS, 0x70)) return false;
    
    // Flush FIFOs
    flushTx();
    flushRx();
    
    return true;
  }

  bool NRF24Driver ::
    setTxMode()
  {
    m_rxMode = false;
    setCE(false);
    
    // Configure as TX (PWR_UP=1, PRIM_RX=0)
    U8 config = NRF24_CONFIG_PWR_UP | NRF24_CONFIG_EN_CRC;
    if (!writeRegister(NRF24_REG_CONFIG, config)) return false;
    
    Os::Task::delay(Fw::TimeInterval(0, 2000));  // 2ms delay
    return true;
  }

  bool NRF24Driver ::
    setRxMode()
  {
    m_rxMode = true;
    setCE(false);
    
    // Configure as RX (PWR_UP=1, PRIM_RX=1)
    U8 config = NRF24_CONFIG_PWR_UP | NRF24_CONFIG_PRIM_RX | NRF24_CONFIG_EN_CRC;
    if (!writeRegister(NRF24_REG_CONFIG, config)) return false;
    
    Os::Task::delay(Fw::TimeInterval(0, 2000));  // 2ms delay
    setCE(true);         // Start listening
    return true;
  }

  bool NRF24Driver ::
    transmitPacket(const U8* data, U8 length)
  {
    if (length > NRF24_PAYLOAD_SIZE) {
      length = NRF24_PAYLOAD_SIZE;
    }
    
    // Clear TX FIFO
    flushTx();
    
    // Write payload
    setCSN(false);
    U8 writeData[33];
    writeData[0] = NRF24_CMD_W_TX_PAYLOAD;
    memcpy(&writeData[1], data, length);
    
    // Pad with zeros if needed
    if (length < NRF24_PAYLOAD_SIZE) {
      memset(&writeData[1 + length], 0, NRF24_PAYLOAD_SIZE - length);
    }
    
    U8 readData[33];
    Fw::Buffer writeBuffer(writeData, NRF24_PAYLOAD_SIZE + 1);
    Fw::Buffer readBuffer(readData, NRF24_PAYLOAD_SIZE + 1);
    
    this->spiOut_out(0, writeBuffer, readBuffer);
    setCSN(true);
    
    // Pulse CE to start transmission
    setCE(true);
    Os::Task::delay(Fw::TimeInterval(0, 1000));  // 1ms delay
    setCE(false);
    
    // Wait for transmission to complete (simple polling)
    U8 statusReg;
    U32 timeout = 1000;  // 1 second timeout
    while (timeout--) {
      if (!readRegister(NRF24_REG_STATUS, statusReg)) return false;
      
      if (statusReg & (NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT)) {
        // Clear status flags
        writeRegister(NRF24_REG_STATUS, statusReg & 0x70);
        return (statusReg & NRF24_STATUS_TX_DS) != 0;  // Success if TX_DS set
      }
      
      Os::Task::delay(Fw::TimeInterval(0, 1000));  // 1ms delay
    }
    
    return false;  // Timeout
  }

  bool NRF24Driver ::
    receivePacket(U8* data, U8& length)
  {
    U8 status = getStatus();
    
    if (status & NRF24_STATUS_RX_DR) {
      // Clear RX_DR flag
      writeRegister(NRF24_REG_STATUS, NRF24_STATUS_RX_DR);
      
      // Read payload
      setCSN(false);
      U8 writeData[33];
      U8 readData[33];
      writeData[0] = NRF24_CMD_R_RX_PAYLOAD;
      memset(&writeData[1], 0, NRF24_PAYLOAD_SIZE);
      
      Fw::Buffer writeBuffer(writeData, NRF24_PAYLOAD_SIZE + 1);
      Fw::Buffer readBuffer(readData, NRF24_PAYLOAD_SIZE + 1);
      
      this->spiOut_out(0, writeBuffer, readBuffer);
      setCSN(true);
      
      memcpy(data, &readData[1], NRF24_PAYLOAD_SIZE);
      length = NRF24_PAYLOAD_SIZE;
      return true;
    }
    
    return false;
  }

  void NRF24Driver ::
    flushTx()
  {
    setCSN(false);
    U8 writeData = NRF24_CMD_FLUSH_TX;
    U8 readData = 0;
    Fw::Buffer writeBuffer(&writeData, 1);
    Fw::Buffer readBuffer(&readData, 1);
    this->spiOut_out(0, writeBuffer, readBuffer);
    setCSN(true);
  }

  void NRF24Driver ::
    flushRx()
  {
    setCSN(false);
    U8 writeData = NRF24_CMD_FLUSH_RX;
    U8 readData = 0;
    Fw::Buffer writeBuffer(&writeData, 1);
    Fw::Buffer readBuffer(&readData, 1);
    this->spiOut_out(0, writeBuffer, readBuffer);
    setCSN(true);
  }

  U8 NRF24Driver ::
    getStatus()
  {
    setCSN(false);
    U8 writeData = NRF24_CMD_NOP;
    U8 readData = 0;
    Fw::Buffer writeBuffer(&writeData, 1);
    Fw::Buffer readBuffer(&readData, 1);
    this->spiOut_out(0, writeBuffer, readBuffer);
    setCSN(true);
    return readData;
  }

}