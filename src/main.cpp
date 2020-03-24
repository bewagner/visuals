#include <iostream>
#include <array>
#include <algorithm>
#include "dlib/map.h"
#include <dlib/image_processing/frontal_face_detector.h>
int main()
{
    auto detector = dlib::get_frontal_face_detector();
    std::cout << "Hello World" << std::endl;
    return 0;
}
