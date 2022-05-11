
# Krunker AI Aimbot 
![Player detection](https://cdn.discordapp.com/attachments/560173144107909120/968434358086160424/Screenshot_2.png)

An aimbot/aim-assist for Krunker.io written in C++ using the YOLOv4 neuronal network object detection to detect players which utilizes NVIDIA CUDA for accelerated GPU processing. The mouse movement is done by using an Arduino Leonardo with USB HostShield in order to avoid potential anti-cheat blocks/detection.

## About the project 

I got the idea for this project from [the following](https://www.unknowncheats.me/forum/valorant/469268-yolo-aim-augmentation-v2-0-a.html) thread in the Unknown Cheats forum. My goal was to create a legitimate-looking aimbot that works with the Face IT anti-cheat running hence I use an Arduino for mouse movement. That way only one active mouse input is present and I don't have to risk using potentially detected built-in windows functions. But you can use ``SendInput`` if you dont have to deal with an anti cheat just change the Arduino communication to your mouse input function.

## Build it yourself 

Feel free to use my code for your projects. With a few tweaks here and there and newly trained weights it's easy to port it to other projects for your desired game. Before starting out you are required to build OpenCV from source with CUDA support according to the supported CUDA version of your GPU first ([find out here](https://en.wikipedia.org/wiki/CUDA#GPUs_supported)). NVIDIA CUDA and NVIDIA cuDNN also have to be installed additionally. In case you need any help I included some useful sources below. I also have to mention that AMD GPU's aren't compatible with CUDA and therefore OpenCV. In case you have an AMD card your best option is to go with OpenCL, a similar tool. I used CUDA v11.5 and cuDNN 8.3.2.44 for CUDA v11.5. I've attached a python script to capture screenshots in-game to create your own dataset of images.

## Tips for staying undetected

+ Execute .exe invisible with vbs script (change aiaimbot.exe to your exe name and put it in the same folder or insert a path). Like this if the Face It anti cheats captures a screenshot nothing suspicous is on screen.
In case you are skilled enough you also can try hooking the screenshot function and solve the problem that way. 
``Set WshShell = CreateObject("WScript.Shell")
WshShell.Run chr(34) & "aiaimbot.exe" & Chr(34), 0
Set WshShell = Nothing``

+ Name the .exe like another program that also uses the GPU alot like a bitcoin miner for example
+ Change parts of my code to avoid detection through signature scans
+ Obfuscate the source code
+ Use low settings because I assume, even if Face It denies it, that there are manual ban's from Face It in Krunker done by spectating
+ Im not sure if Face It scans open serial ports but this could also lead to detection
+ Also I read about detection of USB device descriptors but havent looked into it myself

## Useful sources

- https://www.unknowncheats.me/forum/valorant/469268-yolo-aim-augmentation-v2-0-a.html
- [Website I used to label my images for training](https://cvat.org/)
- [YOLOv4 darknet](https://github.com/AlexeyAB/darknet) 
- [Train YOLOv4 with darknet video tutorial](https://www.youtube.com/watch?v=-NEB5P-SLi0) 
- [Build OpenCV, darknet and detect object on image](https://www.youtube.com/watch?v=FE2GBeKuqpc)
