# python
import pyautogui
from win32api import GetKeyState
from win32con import VK_CAPITAL
import time

screen_width = 1920
screen_height = 1080
fovX = 608
fovY = 480

while True:
    if GetKeyState(VK_CAPITAL) == 1:
        curTime = time.localtime()
        fullTime = f"{curTime.tm_sec}-{curTime.tm_min}-{curTime.tm_hour}-{curTime.tm_mday}-{curTime.tm_mon}-{curTime.tm_year}"
        myScreenshot = pyautogui.screenshot(region=(
            screen_width/2 - fovX/2, screen_height/2 - fovY/2, fovX, fovY))
        myScreenshot.save(
            'YOURPATH' + fullTime + '.png')
        myScreenshot.close()

        time.sleep(1)
