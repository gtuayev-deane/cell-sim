from .client import CellSimClient
import time


class CellSim:
    def __init__(self, port, baudrate=115200, timeout=5):
        """Initialize the CellSim with a serial connection using CellSimClient."""
        self.client = CellSimClient(port, baudrate, timeout)

    def setVoltage(self, channel: int, voltage: float):
        """Set the voltage target for the given cell channel (1-16)."""
        cmd = f"SETV {channel} {voltage}"
        response = self.client.send_command(cmd)
        return response

    def getVoltage(self, channel: int):
        """Get the voltage reading for the given cell channel (1-16)."""
        cmd = f"GETV {channel}"
        response = self.client.send_command(cmd)
        for line in response:
            stripped = line.strip()
            if stripped.startswith("OK:voltage:"):
                try:
                    return round(float(stripped[len("OK:voltage:"):]), 6)
                except Exception:
                    pass
            elif stripped.startswith("Error:"):
                return None
        return None

    def enableDMM(self, channel: int):
        """Enable the DMM for the given cell channel (1-16)."""
        cmd = f"ENABLE_DMM {channel}"
        return self.client.send_command(cmd)

    def disableDMM(self):
        """Disable the DMM."""
        cmd = "DISABLE_DMM"
        return self.client.send_command(cmd)
    
    def setAllVoltages(self, voltage: float):
        """Set all voltage readings for all 16 cells."""
        cmd = f"SETALLV {voltage}"
        response = self.client.send_command(cmd)
        for line in response:
            if line.startswith("Error:"):
                return False
        return True

    def getAllVoltages(self):
        """Get all voltage readings for all 16 cells in a compact format."""
        cmd = "GETALLV"
        response = self.client.send_command(cmd)
        voltages = [None] * 16
        for line in response:
            stripped = line.strip()
            if stripped.startswith("OK:voltages:"):
                try:
                    data = stripped[len("OK:voltages:"):].strip()
                    parts = data.split(",")
                    voltages = [float(p) for p in parts]
                except Exception:
                    pass
                break
            elif stripped.startswith("Error:"):
                break
        return voltages

    def getAllCurrents(self):
        """Get all current readings for all 16 cells in a compact format."""
        cmd = "GETALLI"
        response = self.client.send_command(cmd)
        currents = [None] * 16
        for line in response:
            stripped = line.strip()
            if stripped.startswith("OK:currents:"):
                try:
                    data = stripped[len("OK:currents:"):].strip()
                    parts = data.split(",")
                    currents = [float(p) for p in parts]
                except Exception:
                    pass
                break
            elif stripped.startswith("Error:"):
                break
        return currents
    
    def enableOutputAll(self):
        """Enable the output for all 16 cells."""
        cmd = "ENABLE_OUTPUT_ALL"
        return self.client.send_command(cmd)

    def disableOutputAll(self):
        """Disable the output for all 16 cells."""
        cmd = "DISABLE_OUTPUT_ALL"
        return self.client.send_command(cmd)
    
    def enableOutput(self, channel: int):
        """Enable the output for the given cell channel (1-16)."""
        cmd = f"ENABLE_OUTPUT {channel}"
        return self.client.send_command(cmd)

    def disableOutput(self, channel: int):
        """Disable the output for the given cell channel (1-16)."""
        cmd = f"DISABLE_OUTPUT {channel}"
        return self.client.send_command(cmd)
    
    def enableLoadSwitchAll(self):
        """Enable the load switch for all 16 cells."""
        cmd = "ENABLE_LOAD_SWITCH_ALL"
        return self.client.send_command(cmd)

    def disableLoadSwitchAll(self):
        """Disable the load switch for all 16 cells."""
        cmd = "DISABLE_LOAD_SWITCH_ALL"
        return self.client.send_command(cmd)

    def enableLoadSwitch(self, channel: int):
        """Enable the load switch for the given cell channel (1-16)."""
        cmd = f"ENABLE_LOAD_SWITCH {channel}"
        return self.client.send_command(cmd)
    
    def disableLoadSwitch(self, channel: int):
        """Disable the load switch for the given cell channel (1-16)."""
        cmd = f"DISABLE_LOAD_SWITCH {channel}"
        return self.client.send_command(cmd)

    def close(self):
        """Close the underlying serial connection."""
        self.client.close() 

    def calibrate(self, channel: int):
        """Calibrate the given cell channel (1-16)."""
        cmd = f"CALIBRATE {channel}"
        response = self.client.send_command(cmd)
        if response and "OK:calibrated" in response[0]:
            return response
        raise Exception("Calibration failed")

    def calibrateAll(self):
        """Calibrate all 16 cells."""
        cmd = "CALIBRATE_ALL"
        response = self.client.send_command(cmd)
        if response and "OK:all_calibrated" in response[0]:
            return response
        raise Exception("Calibration failed")
