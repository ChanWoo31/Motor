
import serial
import time

# 시리얼 포트 열고, baud rate 9600으로 설정
ser = serial.Serial('/dev/ttyACM0', 9600, timeout = 5)

time.sleep(2)

while True:
    # 모터로 보낼 스텝 수를 입력으로 받아오기
    command = input()
    ser.write(command.encode('utf-8'))
    print("Sent: ", command.strip())

    # 아두이노가 모터 동작 후 "done" 메시지를 보낼 때까지 대기
    response = ser.readline().decode('utf-8').strip()
    if response == "done":
        print("Motor done")
    else:
        print("Error: ", response)

    time.sleep(2)

