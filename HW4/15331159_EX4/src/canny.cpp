#include "canny.h"


using namespace std;

//g++ -o hello_word hello_world.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11
canny::canny(CImg<unsigned char> a, int size, int low, int high) : img(a)
{
    vector<vector<double>> filter = createFilter(size, size, 1);

    //Print filter
    // for (int i = 0; i<filter.size(); i++)
    // {
    //     for (int j = 0; j<filter[i].size(); j++)
    //     {
    //         cout << filter[i][j] << " ";
    //     }
    // }
    grayscaled = CImg<unsigned char>(toGrayScale()); //Grayscale the image
  // cout << "after grey" << endl;
    gFiltered = CImg<unsigned char>(useFilter(grayscaled, filter)); //Gaussian Filter
  // cout << "after gaussian" << endl;
    sFiltered = CImg<unsigned char>(sobel()); //Sobel Filter

    non = CImg<unsigned char>(nonMaxSupp()); //Non-Maxima Suppression
    thres = CImg<unsigned char>(threshold(non, low, high)); //Double Threshold and Finalize

//    img.display("Original");
//    grayscaled.display("GrayScaled");
//    gFiltered.display("Gaussian Blur");
//    sFiltered.display("Sobel Filtered");
//    non.display("Non-Maxima Supp.");
    // thres.display("Final");
    // string saveName = filename;
    // saveName = "Edge_" + saveName;
    // thres.save(saveName.c_str());
}

CImg<unsigned char> canny::toGrayScale()
{
    int width = img.width();
    int height = img.height();
    int depth = img.depth();

    //New grayscale images.
    grayscaled = CImg<unsigned char>(width, height, depth, 1);

    unsigned char r,g,b;
    unsigned char grey = 0;

    /* Convert RGB image to grayscale image */
    for(int i=0;i<width;i++){
        for(int j=0;j<height;j++){
            //Return a pointer to a located pixel value.
            r = img(i,j,0,0); // First channel RED
            g = img(i,j,0,1); // Second channel GREEN
            b = img(i,j,0,2); // Third channel BLUE

            //Y = 0.299*R + 0.587*G + 0.114*B
            grey = round(0.299*((double)r) + 0.587*((double)g) + 0.114*((double)b));

            grayscaled(i,j,0,0) = grey;
        }

    }
    return grayscaled;
}

vector<vector<double> > canny::createFilter(int row, int column, double sigmaIn)
{
    vector<vector<double> > filter;

    for (int i = 0; i < row; i++)
    {
        vector<double> col;
        for (int j = 0; j < column; j++)
        {
            col.push_back(-1);
        }
        filter.push_back(col);
    }

    float coordSum = 0;
    float constant = 2.0 * sigmaIn * sigmaIn;

    // Sum is for normalization
    float sum = 0.0;

    for (int x = - row/2; x <= row/2; x++)
    {
        for (int y = -column/2; y <= column/2; y++)
        {
            coordSum = (x*x + y*y);
            filter[x + row/2][y + column/2] = (exp(-(coordSum) / constant)) / (M_PI * constant);
            sum += filter[x + row/2][y + column/2];
        }
    }

    // Normalize the Filter
    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            filter[i][j] /= sum;

    return filter;

}

CImg<unsigned char> canny::useFilter(CImg<unsigned char> img_in, vector<vector<double> > filterIn)
{
    int size = (int)filterIn.size()/2;

    int width = img.width();
    int height = img.height();
    int depth = img.depth();

    //New grayscale images.
    CImg<unsigned char> filteredImg = CImg<unsigned char>(width, height, depth, 1);

    for (int i = size; i < width - size; i++)
    {
        for (int j = size; j < height - size; j++)
        {
            double sum = 0;

            for (int x = 0; x < filterIn.size(); x++)
                for (int y = 0; y < filterIn.size(); y++)
                {
                    sum += filterIn[x][y] * (double)(img_in(i + x - size, j + y - size));
                }

            filteredImg(i-size, j-size) = sum;
        }

    }
    return filteredImg;
}

CImg<unsigned char> canny::sobel()
{

    //Sobel X Filter
    double x1[] = {-1.0, 0, 1.0};
    double x2[] = {-2.0, 0, 2.0};
    double x3[] = {-1.0, 0, 1.0};

    vector<vector<double> > xFilter(3);
    xFilter[0].assign(x1, x1+3);
    xFilter[1].assign(x2, x2+3);
    xFilter[2].assign(x3, x3+3);

    //Sobel Y Filter
    double y1[] = {1.0, 2.0, 1.0};
    double y2[] = {0, 0, 0};
    double y3[] = {-1.0, -2.0, -1.0};

    vector<vector<double> > yFilter(3);
    yFilter[0].assign(y1, y1+3);
    yFilter[1].assign(y2, y2+3);
    yFilter[2].assign(y3, y3+3);

    //Limit Size
    int size = (int)xFilter.size()/2;
    int width = gFiltered.width();
    int height = gFiltered.height();
    int depth = gFiltered.depth();

    CImg<unsigned char> filteredImg = CImg<unsigned char>(width - 2*size, height - 2*size, depth, 1);

    angles = CImg<float>(width - 2*size, height - 2*size); //AngleMap

    for (int i = size; i < width - size; i++)
    {
        for (int j = size; j < height - size; j++)
        {
            double sumx = 0;
            double sumy = 0;

            for (int x = 0; x < xFilter.size(); x++)
                for (int y = 0; y < xFilter.size(); y++)
                {
                    sumx += xFilter[x][y] * (double)(gFiltered(i + x - size, j + y - size)); //Sobel_X Filter Value
                    sumy += yFilter[x][y] * (double)(gFiltered(i + x - size, j + y - size)); //Sobel_Y Filter Value
                }
            double sumxsq = sumx*sumx;
            double sumysq = sumy*sumy;

            double sq2 = sqrt(sumxsq + sumysq);

            if(sq2 > 255) //Unsigned Char Fix
                sq2 =255;
            filteredImg(i-size, j-size) = sq2;

            if(sumx==0) //Arctan Fix
                angles(i-size, j-size) = 90;
            else
                angles(i-size, j-size) = atan(sumy/sumx);
        }
    }

    return filteredImg;
}


CImg<unsigned char> canny::nonMaxSupp()
{
    int width = sFiltered.width();
    int height = sFiltered.height();
    int depth = sFiltered.depth();

    CImg<unsigned char> nonMaxSupped(width-2, height-2, depth, 1);
    for (int i=1; i< width - 1; i++) {
        for (int j=1; j<height - 1; j++) {
            float Tangent = angles(i,j);

            nonMaxSupped(i-1, j-1) = sFiltered(i,j);
            //Horizontal Edge
            if (((-22.5 < Tangent) && (Tangent <= 22.5)) || ((157.5 < Tangent) && (Tangent <= -157.5)))
            {
                if ((sFiltered(i,j) < sFiltered(i,j+1)) || (sFiltered(i,j) < sFiltered(i,j-1)))
                    nonMaxSupped(i-1, j-1) = 0;
            }
            //Vertical Edge
            if (((-112.5 < Tangent) && (Tangent <= -67.5)) || ((67.5 < Tangent) && (Tangent <= 112.5)))
            {
                if ((sFiltered(i,j) < sFiltered(i+1,j)) || (sFiltered(i,j) < sFiltered(i-1,j)))
                    nonMaxSupped(i-1, j-1) = 0;
            }

            //-45 Degree Edge
            if (((-67.5 < Tangent) && (Tangent <= -22.5)) || ((112.5 < Tangent) && (Tangent <= 157.5)))
            {
                if ((sFiltered(i,j) < sFiltered(i-1,j+1)) || (sFiltered(i,j) < sFiltered(i+1,j-1)))
                    nonMaxSupped(i-1, j-1) = 0;
            }

            //45 Degree Edge
            if (((-157.5 < Tangent) && (Tangent <= -112.5)) || ((22.5 < Tangent) && (Tangent <= 67.5)))
            {
                if ((sFiltered(i,j) < sFiltered(i+1,j+1)) || (sFiltered(i,j) < sFiltered(i-1,j-1)))
                    nonMaxSupped(i-1, j-1) = 0;
            }
        }
    }
    return nonMaxSupped;
}

CImg<unsigned char> canny::threshold(CImg<unsigned char> imgin,int low, int high)
{
    if(low > 255)
        low = 255;
    if(high > 255)
        high = 255;

    int width = imgin.width();
    int height = imgin.height();
    int depth = imgin.depth();

    CImg<unsigned char> EdgeMat = imgin;

    for (int i=0; i<width; i++)
    {
        for (int j = 0; j<height; j++)
        {
            EdgeMat(i,j) = imgin(i,j);
            if(EdgeMat(i,j) > high)
                EdgeMat(i,j) = 255;
            else if(EdgeMat(i,j) < low)
                EdgeMat(i,j) = 0;
            else
            {
                bool anyHigh = false;
                bool anyBetween = false;
                for (int x=i-1; x < i+2; x++)
                {
                    for (int y = j-1; y<j+2; y++)
                    {
                        if(x <= 0 || y <= 0 || EdgeMat.width() || y > EdgeMat.height()) //Out of bounds
                            continue;
                        else
                        {
                            if(EdgeMat(x,y) > high)
                            {
                                EdgeMat(i,j) = 255;
                                anyHigh = true;
                                break;
                            }
                            else if(EdgeMat(x,y) <= high && EdgeMat(x,y) >= low)
                                anyBetween = true;
                        }
                    }
                    if(anyHigh)
                        break;
                }
                if(!anyHigh && anyBetween)
                    for (int x=i-2; x < i+3; x++)
                    {
                        for (int y = j-1; y<j+3; y++)
                        {
                            if(x < 0 || y < 0 || x > EdgeMat.width() || y > EdgeMat.height()) //Out of bounds
                                continue;
                            else
                            {
                                if(EdgeMat(x,y) > high)
                                {
                                    EdgeMat(i,j) = 255;
                                    anyHigh = true;
                                    break;
                                }
                            }
                        }
                        if(anyHigh)
                            break;
                    }
                if(!anyHigh)
                    EdgeMat(i,j) = 0;
            }
        }
    }
    return EdgeMat;
}

CImg<unsigned char>canny::returenResult() {
  return thres;
}

CImg<unsigned char>canny::returnGuassianResult() {
  return gFiltered;
}
