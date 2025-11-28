import serial
import threading

ser = serial.Serial("/dev/tty.usbserial-14120", 9600, timeout=0.1)

def serial_reader():
    while True:
        try:
            line = ser.readline()
            if line:
                print("\r" + line.decode(errors="ignore").strip())
                print("> ", end="", flush=True)
        except:
            pass

thread = threading.Thread(target=serial_reader, daemon=True)
thread.start()

print("CLI connected. Type commands:")
print("(type 'esc' to escape)")

while True:
    cmd = input("> ")

    if cmd.lower() == "esc":
        break

    ser.write((cmd.upper() + "\r\n").encode())

ser.close()