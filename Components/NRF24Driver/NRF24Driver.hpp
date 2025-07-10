// ======================================================================
// \title  NRF24Driver.hpp
// \author mustafa
// \brief  hpp file for NRF24Driver component implementation class
// ======================================================================

#ifndef Components_NRF24Driver_HPP
#define Components_NRF24Driver_HPP

#include "Components/NRF24Driver/NRF24DriverComponentAc.hpp"

namespace Components {

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
     void setCE(bool state);
     void setCSN(bool state);

     // ----------------------------------------------------------------------
     // Member variables
     // ----------------------------------------------------------------------

     U8 m_currentChannel;
     U8 m_currentPower;
     bool m_isInitialized;

 };

}

#endif
