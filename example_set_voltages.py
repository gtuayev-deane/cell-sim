#!/usr/bin/env python3
from cell_sim.cellsim import CellSim
import time

def main():
    # Replace with your serial port (e.g., '/dev/ttyUSB0' on Linux/Mac or 'COM3' on Windows)
    port = '/dev/cu.usbmodem1101'
    cellsim = CellSim(port)
    
    # Test setting all voltages at once
    print("Setting all voltages to 3.5V...")
    cellsim.setAllVoltages(3.5)

    voltages = cellsim.getAllVoltages()
    print(f"Voltages: {voltages}")

    # Create a rainbow pattern from 1V to 4V across the 16 channels
    print("\nSetting rainbow voltage pattern...")
    for i in range(16):
        voltage = 1.0 + (3.0 * i / 15)  # Spread 1V to 4V across 16 channels
        cellsim.setVoltage(i + 1, voltage)

    voltages = cellsim.getAllVoltages()
    cellsim.close()

if __name__ == '__main__':
    main() 