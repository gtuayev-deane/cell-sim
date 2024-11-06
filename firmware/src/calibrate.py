import serial
import csv
import time

# Configure serial connection
ser = serial.Serial(
    port='/dev/cu.usbmodem101',  # Change this to match your port
    baudrate=115200,
    timeout=1
)

# Open CSV file
with open('buck_sweep_data.csv', 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(['Setpoint', 'Voltage'])  # Write header
    
    try:
        while True:
            line = ser.readline().decode('utf-8').strip()
            
            if line.startswith('>Buck setpoint:'):
                setpoint = float(line.split(':')[1].strip())
                
                # Read next line for voltage
                voltage_line = ser.readline().decode('utf-8').strip()
                if voltage_line.startswith('>Buck voltage:'):
                    voltage = float(voltage_line.split(':')[1].strip())
                    
                    # Write data to CSV
                    writer.writerow([setpoint, voltage])
                    csvfile.flush()  # Ensure data is written immediately
                    
                    print(f"Recorded: Setpoint={setpoint}, Voltage={voltage}")
                    
    except KeyboardInterrupt:
        print("\nStopping data collection...")
        ser.close()
