#include <iostream>
#include <cstdlib>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int thresholdV = 33, ValK = 13, dilatacionMas = 10, ValErocion = 1, Valapertura = 1, cierre = 100, BlackHat = 1, TopHat = 100;

void eventoTrack(int v, void* pP) {

}

int main()
{
    VideoCapture videoCap("video.mp4");
    VideoCapture camaraCap(0);

    if (videoCap.isOpened()) {
        Mat videoCapFrame;
        Mat camaraCapFrame;

        Mat videoDiff;
        Mat camaraDiff;

        Mat ROIVideo;
        Mat gris;
        Mat frameAnterior;
        Mat frameActual;

        Mat frameMediana;
        Mat frameGauss;

        bool bandera = true;

        namedWindow("Video Original", WINDOW_AUTOSIZE);
        namedWindow("Camara", WINDOW_AUTOSIZE);
        namedWindow("Video Diff", WINDOW_AUTOSIZE);
        namedWindow("Camara Diff", WINDOW_AUTOSIZE);
        namedWindow("Video ROI", WINDOW_AUTOSIZE);
        namedWindow("Video Mediana", WINDOW_AUTOSIZE);
        namedWindow("Video Gaussiano", WINDOW_AUTOSIZE);

        createTrackbar("Threshold", "Camara", &thresholdV, 255, eventoTrack, NULL);
        createTrackbar("K", "Camara", &ValK, 100, eventoTrack, NULL);
        createTrackbar("Dilatacion", "Camara", &dilatacionMas, 100, eventoTrack, NULL);
        createTrackbar("Erocion", "Camara", &ValErocion, 100, eventoTrack, NULL);
        createTrackbar("Apertura", "Camara", &Valapertura, 100, eventoTrack, NULL);
        createTrackbar("Cierre", "Camara", &cierre, 100, eventoTrack, NULL);
        createTrackbar("Black Hat", "Camara", &BlackHat, 100, eventoTrack, NULL);
        createTrackbar("Top Hat", "Camara", &TopHat, 100, eventoTrack, NULL);

        while (true) {
            videoCap >> videoCapFrame;
            camaraCap >> camaraCapFrame;
            resize(videoCapFrame, videoCapFrame, Size(), 0.50, 0.50);
            resize(camaraCapFrame, camaraCapFrame, Size(), 0.5, 0.5);

            ROIVideo = videoCapFrame.clone();

            cvtColor(videoCapFrame, gris, COLOR_BGR2GRAY);
            frameActual = gris.clone();

            if (frameAnterior.cols == 0) {
                frameAnterior = gris.clone();
            }

            absdiff(frameAnterior, frameActual, videoDiff);
            frameAnterior = frameActual.clone();

            threshold(videoDiff, videoDiff, thresholdV, 255, THRESH_BINARY);

            if (ValErocion > 0) {
                Mat elementMORPH_ERODE = getStructuringElement(MORPH_CROSS, Size(ValErocion, ValErocion), Point(-1, -1));

                morphologyEx(videoDiff, videoDiff, MORPH_ERODE, elementMORPH_ERODE);
                morphologyEx(camaraCapFrame, camaraDiff, MORPH_ERODE, elementMORPH_ERODE);
            }
            if (dilatacionMas > 0) {
                Mat elementMORPH_DILATE = getStructuringElement(MORPH_CROSS, Size(dilatacionMas, dilatacionMas), Point(-1, -1));

                morphologyEx(videoDiff, videoDiff, MORPH_DILATE, elementMORPH_DILATE);
                morphologyEx(camaraCapFrame, camaraDiff, MORPH_DILATE, elementMORPH_DILATE);
            }

            if (BlackHat > 0) {
                Mat elementMORPH_BLACKHAT = getStructuringElement(MORPH_CROSS, Size(BlackHat, BlackHat), Point(-1, -1));

                morphologyEx(videoDiff, videoDiff, MORPH_BLACKHAT, elementMORPH_BLACKHAT);
                morphologyEx(camaraCapFrame, camaraDiff, MORPH_BLACKHAT, elementMORPH_BLACKHAT);
            }

            if (TopHat > 0) {
                Mat elementMORPH_TOPHAT = getStructuringElement(MORPH_CROSS, Size(TopHat, TopHat), Point(-1, -1));

                morphologyEx(videoDiff, videoDiff, MORPH_TOPHAT, elementMORPH_TOPHAT);
                morphologyEx(camaraCapFrame, camaraDiff, MORPH_TOPHAT, elementMORPH_TOPHAT);
            }

            if (Valapertura > 0) {
                Mat elementMORPH_OPEN = getStructuringElement(MORPH_ELLIPSE, Size(Valapertura, Valapertura), Point(-1, -1));

                morphologyEx(videoDiff, videoDiff, MORPH_OPEN, elementMORPH_OPEN);
            }

            if (cierre > 0) {
                Mat elementMORPH_CLOSE = getStructuringElement(MORPH_RECT, Size(cierre, cierre), Point(-1, -1));

                morphologyEx(videoDiff, videoDiff, MORPH_CLOSE, elementMORPH_CLOSE);
            }

            if (ValK % 2 != 0) {
                medianBlur(videoDiff, frameMediana, ValK);
                medianBlur(videoDiff, videoDiff, ValK);
            }

            for (int i = 1; i < ValK; i = i + 2)
            {
                GaussianBlur(videoDiff, frameGauss, Size(i, i), 0, 0);
            }

            int pixel = 0;
            for (int i = 0; i < videoCapFrame.rows; i++) {
                for (int j = 0; j < videoCapFrame.cols; j++) {
                    pixel = (int)videoDiff.at<uchar>(i, j);
                    if (pixel != 0) {
                        //cout << "Pixel " << pixel << endl;
                        break;
                    }
                    ROIVideo.at<Vec3b>(i, j) = videoDiff.at<Vec3b>();

                }
                for (int j = videoDiff.cols - 1; j >= 0; j--) {
                    int pixel = (int)videoDiff.at<uchar>(i, j);
                    if (pixel != 0) {
                        break;
                    }
                    ROIVideo.at<Vec3b>(i, j) = videoDiff.at<Vec3b>();

                }
            }

            if (videoCapFrame.rows == 0 || videoCapFrame.cols == 0)
                break;

//            imshow("Video Original", videoCapFrame);
            imshow("Video Diff", videoDiff);
            //imshow("Camara Diff", camaraDiff);
            imshow("Camara", videoCapFrame);
            imshow("Video ROI", ROIVideo);
            imshow("Video Mediana", frameMediana);
            imshow("Video Gaussiano", frameGauss);
            if (waitKey(23) == 27)
                break;
        }
        destroyAllWindows();
    }


}
