import serial
import time


class CellSimClient:
    def __init__(self, port, baudrate=115200, timeout=0.2):
        """Initialize serial connection to the CellSim."""
        self.serial = serial.Serial(port, baudrate, timeout=timeout)

    def send_command(self, cmd):
        """Send a command and return the response lines."""
        # Clear any pending input and send command
        self.serial.reset_input_buffer()
        self.serial.write(f"{cmd}\n".encode())
        
        # Read response with retries
        start_time = time.time()
        # Use appropriate timeouts for different commands
        if cmd.startswith("SETV"):
            max_wait = 1.0  # 1s for SETV
        elif cmd in ["GETALLV", "GETALLI"]:
            max_wait = 1.0  # 1s for commands that read all cells
        else:
            max_wait = 1.0  # 1s for other commands
            
        lines = []
        while (time.time() - start_time) < max_wait:
            if self.serial.in_waiting:
                line = self.serial.readline().decode().strip()
                if line:
                    lines.append(line)
                    # If we got a valid response (OK: or Error:), we're done
                    if line.startswith("OK:") or line.startswith("Error:"):
                        return lines
            else:
                time.sleep(0.001)  # 1ms sleep when no data
        
        return lines

    def close(self):
        """Close the serial connection."""
        if self.serial.is_open:
            self.serial.close() 