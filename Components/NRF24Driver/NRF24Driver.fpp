module Components {
    @ Low-level SPI communication driver for NRF24L01+ radio module
    active component NRF24Driver {

        # ###############################################################################
        # Hardware ports
        # ###############################################################################

        
        @ SPI port for communication with the NRF24L01+ module
        output port spiOut: Drv.SpiReadWrite

        @ GPIO port for CE pin control 
        output port cePin: Drv.GpioWrite

        @ GPIO port for CSN pin control
        output port csnPin: Drv.GpioWrite

        # ###############################################################################
        # Data ports
        # ###############################################################################

        @ Port to output received data packets
        output port dataOut: Fw.BufferSend

        # ###############################################################################
        # Commands
        # ###############################################################################

        @ initalize NRF24L01+ module
        async command INIT() opcode 0

        @ Send data Packet
        async command SEND_PACKET(
            data: string size 64 @< Data buffer to tramsit 
        ) opcode 1

        @ Set radio to reciver mode 
        async command START_RECEIVE() opcode 2

        @ Configure radio parameters 
        async command CONFIGURE(
            channel: U8 @< Radio channel (0-125)
            power: U8 @< TX power level (0-3)
        ) opcode 3

        # ###############################################################################
        # Events
        # ###############################################################################


        @ NRF24L01+ module initialized
        event InitComplete() severity activity high format "NRF24L01+ initialized"

        @ Error occurred  
        event Error(
            error: I32 @< Error code
        ) severity warning high format "NRF24 error: {}"


        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending command registrations
        command reg port cmdRegOut

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command responses
        command resp port cmdResponseOut

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

        @ Port to return the value of a parameter
        param get port prmGetOut

        @ Port to set the value of a parameter
        param set port prmSetOut

    }
}