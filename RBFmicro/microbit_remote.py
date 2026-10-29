# ==============================================================
#  RBFmicro - Simple BBC micro:bit BLE Remote Controller
# ==============================================================
#  How to use with the Web Remote (index.html):
#  1. Go to https://makecode.microbit.org
#  2. Switch to Python mode
#  3. Click Extensions -> add 'Bluetooth'
#  4. Paste this code and flash it to your micro:bit!
# ==============================================================

# Setup Bluetooth
bluetooth.start_uart_service()

def on_connected():
    basic.show_icon(IconNames.YES)
    bluetooth.uart_write_string("D1:micro:bit OK")
    bluetooth.uart_write_string("D2:Ready to drive")

def on_disconnected():
    basic.show_icon(IconNames.NO)
    stop_motors()

bluetooth.on_bluetooth_connected(on_connected)
bluetooth.on_bluetooth_disconnected(on_disconnected)

# --------------------------------------------------------------
#  MOTOR PIN CONTROLS (Edit pins to match your robot kit)
# --------------------------------------------------------------
# Common micro:bit robot pins:
# Left Motor: P1, P2   |   Right Motor: P8, P12 (or P13, P14)
def move_forward():
    pins.digital_write_pin(DigitalPin.P1, 1)
    pins.digital_write_pin(DigitalPin.P2, 0)
    pins.digital_write_pin(DigitalPin.P8, 1)
    pins.digital_write_pin(DigitalPin.P12, 0)

def move_backward():
    pins.digital_write_pin(DigitalPin.P1, 0)
    pins.digital_write_pin(DigitalPin.P2, 1)
    pins.digital_write_pin(DigitalPin.P8, 0)
    pins.digital_write_pin(DigitalPin.P12, 1)

def turn_left():
    pins.digital_write_pin(DigitalPin.P1, 0)
    pins.digital_write_pin(DigitalPin.P2, 1)
    pins.digital_write_pin(DigitalPin.P8, 1)
    pins.digital_write_pin(DigitalPin.P12, 0)

def turn_right():
    pins.digital_write_pin(DigitalPin.P1, 1)
    pins.digital_write_pin(DigitalPin.P2, 0)
    pins.digital_write_pin(DigitalPin.P8, 0)
    pins.digital_write_pin(DigitalPin.P12, 1)

def stop_motors():
    pins.digital_write_pin(DigitalPin.P1, 0)
    pins.digital_write_pin(DigitalPin.P2, 0)
    pins.digital_write_pin(DigitalPin.P8, 0)
    pins.digital_write_pin(DigitalPin.P12, 0)

# --------------------------------------------------------------
#  HANDLE BUTTON CLICKS FROM THE WEB REMOTE
# --------------------------------------------------------------
def handle_command(cmd: str):
    if len(cmd) < 2:
        return

    action = cmd[0]       # '+' for press, '-' for release
    key = cmd[1]          # 'W', 'A', 'S', 'D', '1', '2', etc.
    pressed = (action == '+')

    # 1) Driving Controls (WASD)
    if key == 'W':
        if pressed:
            basic.show_arrow(ArrowNames.NORTH)
            bluetooth.uart_write_string("D2:FORWARD")
            move_forward()
        else:
            stop_motors()
            basic.clear_screen()
            bluetooth.uart_write_string("D2:STOPPED")

    elif key == 'S':
        if pressed:
            basic.show_arrow(ArrowNames.SOUTH)
            bluetooth.uart_write_string("D2:BACKWARD")
            move_backward()
        else:
            stop_motors()
            basic.clear_screen()
            bluetooth.uart_write_string("D2:STOPPED")

    elif key == 'A':
        if pressed:
            basic.show_arrow(ArrowNames.WEST)
            bluetooth.uart_write_string("D2:LEFT")
            turn_left()
        else:
            stop_motors()
            basic.clear_screen()
            bluetooth.uart_write_string("D2:STOPPED")

    elif key == 'D':
        if pressed:
            basic.show_arrow(ArrowNames.EAST)
            bluetooth.uart_write_string("D2:RIGHT")
            turn_right()
        else:
            stop_motors()
            basic.clear_screen()
            bluetooth.uart_write_string("D2:STOPPED")

    # 2) Emergency stop button (X)
    elif key == 'X':
        stop_motors()
        basic.show_icon(IconNames.NO)
        bluetooth.uart_write_string("D2:BRAKE!")

    # 3) Number buttons (1, 2, 3, 4)
    elif pressed and key in ['1', '2', '3', '4']:
        basic.show_string(key)
        bluetooth.uart_write_string("D1:Program " + key)
        # Students can add quick actions here:
        if key == '1':
            move_forward()
            basic.pause(500)
            turn_left()
            basic.pause(500)
            stop_motors()
            basic.clear_screen()

# Listen for incoming data from the web remote
def on_uart_received():
    cmd = bluetooth.uart_read_until(serial.delimiters(Delimiters.NEW_LINE))
    handle_command(cmd)

bluetooth.on_uart_data_received(serial.delimiters(Delimiters.NEW_LINE), on_uart_received)

# Ready indicator on micro:bit screen
basic.show_string("R")
