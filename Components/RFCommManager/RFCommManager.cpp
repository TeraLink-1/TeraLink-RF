// ======================================================================
// \title  RFCommManager.cpp
// \author mustafa
// \brief  cpp file for RFCommManager component implementation class
// ======================================================================

#include "Components/RFCommManager/RFCommManager.hpp"

namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  RFCommManager ::
    RFCommManager(const char* const compName) :
      RFCommManagerComponentBase(compName)
  {

  }

  RFCommManager ::
    ~RFCommManager()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  void RFCommManager ::
    TODO_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

}
