#!/usr/bin/env python3
import threading
import time
from queue import Queue

import serial.tools.list_ports

from lib.cellsim import CellSim

import IPython


def find_cellsim_port():
    """Scan all serial ports to find a CellSim device."""
    ports = list(serial.tools.list_ports.comports())
    if not ports:
        print("No serial ports found!")
        return None

    print("Scanning for CellSim device...")

    # Queue for the first found port
    port_queue = Queue()
    stop_event = threading.Event()

    def try_port(port):
        if stop_event.is_set():
            return
        try:
            print(f"Trying port {port.device}...")
            test_cellsim = CellSim(port.device)
            response = test_cellsim.client.send_command("PING")
            test_cellsim.close()
            if response and "OK:PONG" in response[0]:
                print(f"Found CellSim device on port {port.device}")
                port_queue.put(port.device)
                stop_event.set()  # Signal other threads to stop
        except Exception as e:
            print(f"Port {port.device} failed: {str(e)}")

    # Start a thread for each port
    threads = []
    for port in ports:
        thread = threading.Thread(target=try_port, args=(port,))
        thread.daemon = True  # Make threads daemon so they exit when main thread exits
        threads.append(thread)
        thread.start()

    # Wait for either a port to be found or all threads to complete
    while True:
        if not port_queue.empty():
            stop_event.set()  # Signal remaining threads to stop
            return port_queue.get()

        if all(not t.is_alive() for t in threads):
            break

        time.sleep(0.1)

    print("No CellSim device found!")
    return None


def main():
    # Find the CellSim device
    port = find_cellsim_port()
    if port is None:
        exit(1)

    cellsim = CellSim(port)
    cellsim.setAllVoltages(3.5)
    time.sleep(1)

    cellsim.enableOutputAll()
    time.sleep(1)

    voltages = cellsim.getAllVoltages()
    print(f"Voltages: {voltages}")

    currents = cellsim.getAllCurrents()
    print(f"Currents: {currents}")

    IPython.embed()

    cellsim.disableOutputAll()
    time.sleep(1)

    voltages = cellsim.getAllVoltages()
    print(f"Voltages: {voltages}")

    currents = cellsim.getAllCurrents()
    print(f"Currents: {currents}")

    cellsim.close()


if __name__ == "__main__":
    main()
