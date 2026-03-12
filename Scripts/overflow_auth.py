import serial
import time

import serial.tools.list_ports

try:
    # Let user select port so it works on windows as well
    ports = serial.tools.list_ports.comports()

    if not ports:
        print("No serial ports found.")
        exit()

    for i, port in enumerate(ports):
        print(f"{i + 1}.", port.device, port.description)

    while True:
        try:
            choice = int(input("Select port: "))
            if 1 <= choice <= len(ports):
                break
            else:
                print(f"Invalid choice. Please select a number between 1 and {len(ports)}.")
        except ValueError:
            print("Please enter a valid number.")

    ser = serial.Serial(
        port=ports[choice - 1].device,
        baudrate=115200,
        timeout=1
    )
    
    while True:
        ser.write("1".encode("ascii"))
        time.sleep(0.01)
except KeyboardInterrupt:
    ser.close()
    print("\nUser interrupt")