import serial
import subprocess

with open('COM-Port.txt') as f:
    sername = f.readline().strip()

ser = serial.Serial(sername, 115200, timeout=0)
pid = subprocess.Popen(['C:\\VideoLAN\\VLC\\vlc.exe', '-f','--loop', '--no-video-title-show' , 'C:\\Users\\Sheep\\Desktop\\8.Brain\\video\\Black.mp4'])


while True:
    '''
    line = ser.readline()
    line = line.decode('utf8').strip()

    sp = line.split('::')
    print(sp)
    pid = subprocess.Popen(sp, shell=True).pid
    '''
    line = ser.readline()
    line = line.decode('utf8').strip()

    if '0' in line:
        break
    elif '1' in line:
        pid = subprocess.Popen(['C:\\VideoLAN\\VLC\\vlc.exe', '--video-on-top' , '-f', '--play-and-exit' , '--no-video-title-show' , 'C:\\Users\\Sheep\\Desktop\\8.Brain\\video\\1.mp4'])
    elif '2' in line:
        pid = subprocess.Popen(['C:\\VideoLAN\\VLC\\vlc.exe', '--video-on-top' , '-f', '--play-and-exit' , '--no-video-title-show' , 'C:\\Users\\Sheep\\Desktop\\8.Brain\\video\\2.mp4'])
    elif '3' in line:
        pid = subprocess.Popen(['C:\\VideoLAN\\VLC\\vlc.exe', '--video-on-top' , '-f', '--play-and-exit' , '--no-video-title-show' , 'C:\\Users\\Sheep\\Desktop\\8.Brain\\video\\3.4.6.mp4'])
    elif '4' in line:
        pid = subprocess.Popen(['C:\\VideoLAN\\VLC\\vlc.exe', '--video-on-top' , '-f', '--play-and-exit' , '--no-video-title-show' , 'C:\\Users\\Sheep\\Desktop\\8.Brain\\video\\5-2..mp4'])
    elif 'T' in line:
        pid = subprocess.Popen(['C:\\VideoLAN\\VLC\\vlc.exe', '-f', '--play-and-exit' , '--no-video-title-show' , 'C:\\Users\\Sheep\\Desktop\\8.Brain\\video\\Tower.mp4'])
    elif 'A' in line:
        pid = subprocess.Popen(['C:\\VideoLAN\\VLC\\vlc.exe', '-f', '--play-and-exit' , '--no-video-title-show' , 'C:\\Users\\Sheep\\Desktop\\8.Brain\\video\\Ai.mp4'])
    elif 'H' in line:
        pid = subprocess.Popen(['C:\\VideoLAN\\VLC\\vlc.exe', '-f', '--play-and-exit' , '--no-video-title-show' , 'C:\\Users\\Sheep\\Desktop\\8.Brain\\video\\Human.mp4'])
