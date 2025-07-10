// ======================================================================
// \title  RFCommManager.hpp
// \author mustafa
// \brief  hpp file for RFCommManager component implementation class
// ======================================================================

#ifndef Components_RFCommManager_HPP
#define Components_RFCommManager_HPP

#include "Components/RFCommManager/RFCommManagerComponentAc.hpp"

namespace Components {

  class RFCommManager :
    public RFCommManagerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct RFCommManager object
      RFCommManager(
          const char* const compName //!< The component name
      );

      //! Destroy RFCommManager object
      ~RFCommManager();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------

      //! Handler implementation for command TODO
      //!
      //! TODO
      void TODO_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq //!< The command sequence number
      ) override;

  };

}

#endif
