import serial
import time
import argparse

import serial.tools.list_ports

DEFAULT_KEY = "VP2026"

try:
    argParser = argparse.ArgumentParser(prog='unlock_auth', description='Sends unlocking sequence to serial port.')
    argParser.add_argument('-k', '--key', default=DEFAULT_KEY)
    args = argParser.parse_args()

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

    # Serielle Schnittstelle konfigurieren (Anpassen an deine Einstellungen)
    ser = serial.Serial(
        port=ports[choice - 1].device,       # Port anpassen (z. B. '/dev/ttyUSB0' auf Linux)
        baudrate=115200,   # Baudrate anpassen
        timeout=1
    )

    key = "A" + args.key + "\n"

    for l in key:
        ser.write(l.encode('ascii'))
        time.sleep(0.5)
except KeyboardInterrupt:
    ser.close()
    print("\nSkript beendet.")