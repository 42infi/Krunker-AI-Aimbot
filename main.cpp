#include <opencv2/opencv.hpp>
#include <fstream>
#include "SerialClass.h"


using namespace cv;
using namespace dnn;

const int aimFovX { 224 };
const int aimFovY { 192 };
const int screen_width { 1920 };
const int screen_height { 1080 };

int moveX{};
int moveY{};

float sensitivity = 0.75;
float minLockPercent = 0.8;
float enabled = true;
float showImg = true;
bool hidden = false;

class DetectionObj {
    
public:
    int x{};
    int y{};
    int width{};
    int height{};
    Point headPos{};

    DetectionObj(int x, int y, int width, int height) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->headPos = Point(x + width / 2, y + height / 5.5);
    }

};

DetectionObj* closest{};
float closestScore { 0.0f };

BITMAPINFOHEADER createBitmapHeader(int width, int height);
Mat captureScreenMat(HWND hwnd);
void printCli();


int main() {
   
    Mat frameRGB;
    Mat frameRGBA;

    
    std::vector<std::string> classes;
    //add class to detect
    classes.push_back("target");

    //load and create new yolov4 darknet from cfg and weight file
    Net net = readNetFromDarknet(".\\data\\yolov4-tiny_krunker.cfg", ".\\data\\yolov4-tiny_krunker_final.weights");
    
    //enable use of Nvidia CUDA
    net.setPreferableBackend(DNN_BACKEND_CUDA);
    net.setPreferableTarget(DNN_TARGET_CUDA);
    
    //create object detection model
    DetectionModel model = DetectionModel(net);
    model.setInputParams(1 / 255.0, Size(aimFovX, aimFovY), Scalar(), true);
    
    //read com port for arduino from txt file
    std::string port;
    std::fstream file;
    file.open("port.txt", std::ios::in);
    if (file.is_open()) {
        while (getline(file, port)) {}
        file.close();
    }

    std::cout << port << std::endl;

    //connect with arduino
    Serial* serial = new Serial(port.data());

    if (serial->IsConnected()) {
        printf("connected");
    }
    else {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        return 3;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    printCli();

    
    while (waitKey(1) < 1) {

        //toggle if text visible
        if (GetAsyncKeyState(VK_F12) & 1) {
            hidden = !hidden;
            if (hidden) {
                system("cls");
            }
            else {
                printCli();
            }
        }
        //toggle aimbot
        if (GetAsyncKeyState(VK_F10) & 1) {
            enabled = !enabled;
            printCli();
        }
        //increase aimbot sensitivity 
        if ((GetAsyncKeyState(VK_UP) & 1) && sensitivity < 1.5) {
            sensitivity += 0.025;
            printCli();
        }
        //decrease aimbot sensitivity
        if ((GetAsyncKeyState(VK_DOWN) & 1) && sensitivity > 0.3) {
            sensitivity -= 0.025;
            printCli();
        }
        //increase aimbot minimum target score for locking
        if ((GetAsyncKeyState(VK_RIGHT) & 1) && minLockPercent < 1) {
            minLockPercent += 0.05;
            printCli();
        }
        //decrease aimbot minimum target score for locking
        if ((GetAsyncKeyState(VK_LEFT) & 1) && minLockPercent > 0.5) {
            minLockPercent -= 0.05;
            printCli();
        }
        //toggle image displaying
        if (GetAsyncKeyState(VK_F2) & 1) {
            showImg = !showImg;
        }

        if (enabled) {

            //start for ai fps
            auto start = std::chrono::high_resolution_clock::now();

            std::vector<DetectionObj*> detections{};

            //capture screenshot as OpenCV Mat
            frameRGBA = captureScreenMat(GetDesktopWindow());

            //convert screenshot from RGBA to RGB for processing
            cvtColor(frameRGBA, frameRGB, COLOR_RGBA2RGB);
                        
            std::vector<int> classIds;
            std::vector<float> scores {};
            std::vector<Rect> boxes;

            //detect objects on current frame and output results to the vectors above
            model.detect(frameRGB, classIds, scores, boxes, 0.5, 0.4);

            for (int i = 0; i < classIds.size(); i++) {

                //create new detection objects from detected boxes and push them in detections
                detections.push_back(new DetectionObj(boxes[i].x, boxes[i].y, boxes[i].width, boxes[i].height));

                //draw boxes onto the output image
                if(showImg) rectangle(frameRGB, boxes[i], Scalar(0, 255, 0), 1.4);

                //draw detected class text onto the output image
                char text[100];
                snprintf(text, sizeof(text), "%s: %.2f", classes[classIds[i]].c_str(), scores[i]);
                if (showImg)  putText(frameRGB, text, Point(boxes[i].x, boxes[i].y - 5), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 1);
            }

            //find  closest target to crosshair
            closest = new DetectionObj(aimFovX, aimFovY, 10, 10);
            for (int i = 0; i < detections.size(); i++) {

                if (abs(sqrt(pow(detections[i]->headPos.x - aimFovX / 2, 2) + pow(detections[i]->headPos.y - aimFovY / 2, 2))) < abs(sqrt(pow(closest->headPos.x - aimFovX / 2, 2) + pow(closest->headPos.y - aimFovY / 2, 2)))) {
                    closest = detections[i];
                    closestScore = scores[i];
                }
                             
            }

            //calculate mouse movement
            moveX = closest->headPos.x - aimFovX / 2;
            moveY = closest->headPos.y - aimFovY / 2;

            //detect if target is on own team
            //set team name tag color to rgb 255, 255, 25 
            Vec3b curPixel;
            bool friendly = false;
            if (scores.size() > 0) {
                for (int i = closest->x - 30; i < closest->x + 10 && i < aimFovX; i++) {
                    for (int j = closest->y + 5; j > closest->y - 25 && j > 0; j--) {
                        curPixel = frameRGB.at<Vec3b>(j, i);
                        if (showImg) frameRGB.at<Vec3b>(j, i) = Vec3b(0, 0, 255);
                        if (curPixel[0] >= 20 && curPixel[0] <= 48 && curPixel[1] >= 250 && curPixel[1] <= 257 && curPixel[2] >= 249 && curPixel[2] <= 257) {
                            friendly = true;
                            break;
                        }
                    }
                    if (friendly) break;
                }
            
                //send inputs for mouse move on rightclick to the arduino
                if (GetAsyncKeyState(VK_RBUTTON) && closestScore > minLockPercent && !friendly) {
                   std::string moveCom = std::to_string((int)(moveX * sensitivity)) + ":" + std::to_string((int)(moveY * sensitivity));
                   serial->WriteData(moveCom.data(), sizeof(moveCom.data()));
                }

            }

            //calculate and draw FPS
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            putText(frameRGB, std::to_string(1000000 / duration.count()), Point(10, 15), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 0, 255));
            if (scores.size() > 0 && showImg)  line(frameRGB, Point(aimFovX / 2, aimFovY / 2), closest->headPos, Scalar(0, 0, 255), 1.75, 8, 0);
            //show output image
            if (showImg) imshow("Image", frameRGB);
            
        }
    }
    destroyAllWindows();

    return 0;
}

//function to create bitmapheader for screenshot
BITMAPINFOHEADER createBitmapHeader(int width, int height)
{
    BITMAPINFOHEADER  bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    return bi;
}

//function to create an OpenCV Mat screenshot
Mat captureScreenMat(HWND hwnd)
{
    Mat src;

    HDC hwindowDC = GetDC(hwnd);
    HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    int screenx = (screen_width / 2) - (aimFovX / 2);
    int screeny = (screen_height / 2) - (aimFovY / 2);
    int width = aimFovX;
    int height = aimFovY;

    src.create(height, width, CV_8UC4);

    HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    BITMAPINFOHEADER bi = createBitmapHeader(width, height);

    SelectObject(hwindowCompatibleDC, hbwindow);

    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, screenx, screeny, width, height, SRCCOPY);
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd, hwindowDC);

    return src;
}

//print console UI
void printCli() {
    system("cls");
    std::cout << "enabled: " << enabled << std::endl;
    std::cout << "sens: " << sensitivity << std::endl;
    std::cout << "lock min. %: " << minLockPercent << std::endl;
}
