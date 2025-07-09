// ======================================================================
// \title  NRF24Driver.cpp
// \author mustafa
// \brief  cpp file for NRF24Driver component implementation class
// ======================================================================

#include "Components/NRF24Driver/NRF24Driver.hpp"

namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------



  NRF24Driver ::
    NRF24Driver(const char* const compName) :
      NRF24DriverComponentBase(compName),
      m_currentChannel(0),
      m_currentPower(0),
      m_isInitialized(false)
  {

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
  INIT_cmdHandler(const FwOpcodeType opCode,
                  const U32 cmdSeq)
  {
    // TODO: Initialize NRF24 module 
    // 1. Configure SPI settings
    // 2. Reset radio
    // 3. Configure basic settings
    // 4. Set initial channel and power

    m_isInitialized = true;
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK); 
  }

  void NRF24Driver ::
    SEND_PACKET_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& data //!< Data to transmit
    )
  {
    if (!m_isInitialized) {
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
      return;
    }
    
    // TODO: Implement send logic here
    
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void NRF24Driver ::
    START_RECEIVE_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq) 
    {
      if (!m_isInitialized) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
      }

      this-> cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
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

    // TODO: Configure radio parameters
    // 1. Set RF channel
    // 2. Set TX power level
    
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
    // TODO: Implement SPI register write
    // 1. Set CSN low
    // 2. Send write command + register address
    // 3. Send data
    // 4. Set CSN high
    return true;
  }

  bool NRF24Driver ::
    readRegister(U8 reg, U8& value)
  {
    // TODO: Implement SPI register read
    // 1. Set CSN low  
    // 2. Send read command + register address
    // 3. Read data
    // 4. Set CSN high
    value = 0;
    return true;
  }

  void NRF24Driver ::
    setCE(bool state)
  {
    // Set CE pin state via GPIO port
    this->cePin_out(0, state ? Fw::Logic::HIGH : Fw::Logic::LOW);
  }

  void NRF24Driver ::
    setCSN(bool state)
  {
    // Set CSN pin state via GPIO port  
    this->csnPin_out(0, state ? Fw::Logic::HIGH : Fw::Logic::LOW);
  }

}

















