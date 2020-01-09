#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

void error_tip() {
    std::cout << "Parameter example as follow, support 2 case:" << std::endl;
    std::cout << "$ ./app -h" << std::endl;
    std::cout << "$ ./app mat2nv12 img.png" << std::endl;
    std::cout << "$ ./app mat2nv12 img.png ressized_nv12_width ressized_nv12_height" << std::endl;
    std::cout << "$ ./app nv122mat nv12_fn.data nv12_width, nv12_height out_mat.png" << std::endl;
}

int mat2nv12(int argc, char** argv) {
    if(argc != 3 && argc != 5) {
        error_tip();
        return EXIT_FAILURE;
    }

    std::string fn = std::string(argv[2]);
    cv::Mat src = cv::imread(fn, 1);
    if(src.empty()) {
        std::cout << "Can't imread: " << fn << std::endl;
        return EXIT_FAILURE;
    }

    int nv12_w = src.cols;
    int nv12_h = src.rows;
    if(argc == 5) {
        nv12_w = std::atoi(argv[3]);
        nv12_h = std::atoi(argv[4]);
    }

    std::string out_fn = std::string("out_nv12_") + std::to_string(nv12_w) + "_" + std::to_string(nv12_h) + ".dat";
    std::cout << "Result nv12 name = " << out_fn << std::endl;

    cv::Mat dst;
    cv::resize(src, dst, cv::Size(nv12_w, nv12_h));

    std::vector<char> yuvbuf(nv12_w*nv12_h*3/2);
    cv::Mat yuv = cv::Mat(nv12_h * 3/2, nv12_w, CV_8UC1, &yuvbuf[0]);

    cv::cvtColor(dst, yuv, cv::COLOR_BGR2YUV_YV12);

    FILE* fout = fopen(out_fn.c_str(), "wb");
    fwrite(&yuvbuf[0], sizeof(char), yuvbuf.size(), fout);
    fclose(fout);

    std::cout << "Convert success" << std::endl;
    return EXIT_SUCCESS;
}

int nv122mat(int argc, char** argv) {
    if(argc != 6) {
        error_tip();
        return EXIT_FAILURE;
    }

    std::vector<char> nv12buf;
    int nv12_width = std::atoi(argv[3]);
    int nv12_height = std::atoi(argv[4]);
    std::string out_fn = argv[5];

    // Read yuv buffer
    std::ifstream ifs(std::string(argv[2]),  std::ifstream::in);
    if(!ifs) {
        std::cout << "Can't read: " << argv[2] << std::endl;
        return EXIT_FAILURE;
    }

    ifs.seekg(0, std::ios::end);
    size_t size = ifs.tellg();
    nv12buf.resize(size);
    ifs.seekg(0);
    ifs.read(&nv12buf[0], size);
    ifs.close();

    cv::Mat picYV12 = cv::Mat(nv12_height * 3/2, nv12_width, CV_8UC1, &nv12buf[0]);
    cv::Mat picBGR;
    cv::cvtColor(picYV12, picBGR, cv::COLOR_YUV2BGR_YV12);

    cv::imwrite(out_fn, picBGR);  //only for test

    std::cout << "Convert success" << std::endl;
    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if(argc == 2 && argv[1] == std::string("-h")) {
        error_tip();
        return EXIT_SUCCESS;
    }

    if(argc > 2) {
        if(argv[1] == std::string("mat2nv12")) {
            return mat2nv12(argc, argv);
        }
        if(argv[1] == std::string("nv122mat")) {
            return nv122mat(argc, argv);
        }
    }
    else {
        error_tip();
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}