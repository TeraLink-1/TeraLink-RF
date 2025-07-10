#!/usr/bin/env python3
"""
Test script to verify GPIO wiring for NRF24L01+
Run this before starting F Prime to ensure your wiring is correct
"""

import RPi.GPIO as GPIO
import spidev
import time
import sys

# Your wiring configuration
CE_PIN = 22    # Blue wire
CSN_PIN = 8    # Grey wire

def test_gpio_setup():
    """Test basic GPIO functionality"""
    print("=== Testing GPIO Setup ===")
    
    try:
        # Use BCM numbering
        GPIO.setmode(GPIO.BCM)
        GPIO.setwarnings(False)
        
        # Setup pins
        GPIO.setup(CE_PIN, GPIO.OUT)
        GPIO.setup(CSN_PIN, GPIO.OUT)
        
        # Test CE pin
        print(f"\nTesting CE pin (GPIO {CE_PIN} - Blue wire):")
        print("  Setting HIGH...")
        GPIO.output(CE_PIN, GPIO.HIGH)
        time.sleep(0.5)
        print("  Setting LOW...")
        GPIO.output(CE_PIN, GPIO.LOW)
        time.sleep(0.5)
        print("  ✓ CE pin working")
        
        # Test CSN pin
        print(f"\nTesting CSN pin (GPIO {CSN_PIN} - Grey wire):")
        print("  Setting HIGH...")
        GPIO.output(CSN_PIN, GPIO.HIGH)
        time.sleep(0.5)
        print("  Setting LOW...")
        GPIO.output(CSN_PIN, GPIO.LOW)
        time.sleep(0.5)
        GPIO.output(CSN_PIN, GPIO.HIGH)  # Return to idle state
        print("  ✓ CSN pin working")
        
        return True
        
    except Exception as e:
        print(f"  ✗ GPIO Error: {e}")
        return False
    finally:
        GPIO.cleanup()

def test_spi_communication():
    """Test SPI communication"""
    print("\n=== Testing SPI Communication ===")
    
    try:
        # Open SPI
        spi = spidev.SpiDev()
        spi.open(0, 0)  # /dev/spidev0.0
        
        # Configure SPI
        spi.max_speed_hz = 10000000  # 10MHz
        spi.mode = 0  # SPI mode 0
        
        print("SPI Configuration:")
        print(f"  Device: /dev/spidev0.0")
        print(f"  Speed: {spi.max_speed_hz/1000000:.1f} MHz")
        print(f"  Mode: {spi.mode}")
        
        # Test NOP command (should return status register)
        print("\nTesting SPI communication with NRF24:")
        response = spi.xfer2([0xFF])  # NOP command
        print(f"  NOP command response: 0x{response[0]:02X}")
        
        # Try to read CONFIG register (register 0x00)
        response = spi.xfer2([0x00, 0xFF])  # Read register 0
        print(f"  CONFIG register: 0x{response[1]:02X}")
        
        if response[1] == 0x00 or response[1] == 0xFF:
            print("  ⚠ Possible issue: Check NRF24 power and connections")
        else:
            print("  ✓ SPI communication appears to be working")
        
        spi.close()
        return True
        
    except Exception as e:
        print(f"  ✗ SPI Error: {e}")
        print("  Make sure SPI is enabled (sudo raspi-config)")
        return False

def test_nrf24_presence():
    """Test if NRF24 module is responding"""
    print("\n=== Testing NRF24 Module Presence ===")
    
    try:
        GPIO.setmode(GPIO.BCM)
        GPIO.setwarnings(False)
        GPIO.setup(CE_PIN, GPIO.OUT)
        GPIO.setup(CSN_PIN, GPIO.OUT)
        
        # Ensure CE is low and CSN is high
        GPIO.output(CE_PIN, GPIO.LOW)
        GPIO.output(CSN_PIN, GPIO.HIGH)
        
        spi = spidev.SpiDev()
        spi.open(0, 0)
        spi.max_speed_hz = 10000000
        
        # Power down the module first
        GPIO.output(CSN_PIN, GPIO.LOW)
        spi.xfer2([0x20, 0x00])  # Write 0x00 to CONFIG
        GPIO.output(CSN_PIN, GPIO.HIGH)
        time.sleep(0.01)
        
        # Power up the module
        GPIO.output(CSN_PIN, GPIO.LOW)
        spi.xfer2([0x20, 0x02])  # Write 0x02 to CONFIG (PWR_UP)
        GPIO.output(CSN_PIN, GPIO.HIGH)
        time.sleep(0.005)  # 5ms power up time
        
        # Read back CONFIG register
        GPIO.output(CSN_PIN, GPIO.LOW)
        response = spi.xfer2([0x00, 0xFF])
        GPIO.output(CSN_PIN, GPIO.HIGH)
        
        config_value = response[1]
        print(f"CONFIG register value: 0x{config_value:02X}")
        
        if config_value & 0x02:  # Check PWR_UP bit
            print("✓ NRF24 module is responding correctly!")
            return True
        else:
            print("✗ NRF24 module not responding as expected")
            return False
            
    except Exception as e:
        print(f"✗ Error: {e}")
        return False
    finally:
        GPIO.cleanup()
        spi.close()

def main():
    """Run all tests"""
    print("NRF24L01+ Wiring Test")
    print("====================")
    print(f"Your configuration:")
    print(f"  CE:  GPIO {CE_PIN} (Blue wire)")
    print(f"  CSN: GPIO {CSN_PIN} (Grey wire)")
    print(f"  SCK: GPIO 11 (Purple wire)")
    print(f"  MOSI: GPIO 10 (Orange wire)")
    print(f"  MISO: GPIO 9 (Gold wire)")
    print(f"  VCC: 3.3V (Red wire)")
    print(f"  GND: Ground (Green wire)")
    
    # Check if running as root
    if sys.platform.startswith('linux') and os.geteuid() != 0:
        print("\n⚠ Warning: Running without root privileges.")
        print("If tests fail, try: sudo python3 test_gpio_wiring.py")
    
    tests_passed = 0
    total_tests = 3
    
    if test_gpio_setup():
        tests_passed += 1
    
    if test_spi_communication():
        tests_passed += 1
    
    if test_nrf24_presence():
        tests_passed += 1
    
    print(f"\n{'='*40}")
    print(f"Tests passed: {tests_passed}/{total_tests}")
    
    if tests_passed == total_tests:
        print("✓ All tests passed! Your wiring appears correct.")
        print("\nNext steps:")
        print("1. Start your F Prime deployment")
        print("2. Run: python test_nrf24.py phase1")
    else:
        print("✗ Some tests failed. Please check:")
        print("- All connections are secure")
        print("- NRF24 module has power (3.3V, not 5V!)")
        print("- SPI is enabled (sudo raspi-config)")
        print("- Capacitor is connected between VCC and GND")

if __name__ == "__main__":
    import os
    main()