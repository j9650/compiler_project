#include <iostream>
#include <vector>
#include <cstdint>

#include "fluid.hh"

class Image{
public:
    std::vector<int32_t> pixels;

    int32_t& access(int32_t idx){
        if (idx >= pixels.size()) {
          std::cout <<"outofscope" <<std::endl;exit(0);
        }
        else return (pixels)[idx];
    }

    int32_t read(int32_t idx) const{
        if (idx >= pixels.size()) {
          std::cout <<"outofscope" <<std::endl;exit(0);
        }
        else return (pixels)[idx];
    }

    bool write(int32_t idx, int32_t pixel){
        if (idx >= pixels.size()) {
          std::cout <<"outofscope" <<std::endl;exit(0);
        }
        else {
            (pixels)[idx] = pixel;
            return true;
        }
    }    

    int32_t sum() const{
        int32_t spix = 0;
        for (auto& pix : pixels)
            spix += pix;
        return spix;
    }

    int32_t size() const{

        return pixels.size();
    }

    Image(){
    
    }
    Image(int numpix){

        pixels.resize(numpix);
    }    
    /*--------------------*/
    bool counterPred(int num){
        if (CallNUM(access, int32_t) > num)
            return true;
        else false;

    }
    #pragma Guard(sum, counterPred, 100)

};


void UpdateImage(Image& img){
    for (int i = 0; i < 100; i++)
        img[i] = i;
    return;
}

void Combine2Image(Image& fimg1, Image& fimg2){
    ;
}

void ImagePipeline1(Image& in, Image& out){};
void ImagePipeline2(Image& in, Image& out){};
void ImagePipeline3(Image& in, Image& out){};

void example0(){
    Image img1(100);
    Image img2(100);
    Image img3(100);
    Image img4(100);
    
    __fluid__ Image fimg1(&img);
    __fluid__ Image fimg2(&img);
    __fluid__ Image fimg3(&img);

    Pred predimg1(std::bind(fimg1.counterPred, 80));
    Pred predimg2(std::bind(fimg2.counterPred, 90));
    Pred predimg3(std::bind(fimg3.counterPred, 100));

    AsyncTask(ImagePipeline1, fimg1, fimg2, {predimg1}, {});
    AsyncTask(ImagePipeline2, fimg2, fimg3, {predimg2}, {});
    AsyncTask(ImagePipeline3, fimg3,  img4, {predimg3}, {});

}


void example1(){
    Image img1(100);
    Image img2(100);

    __fluid__ Image fimg1(&img1); 
    __fluid__ Image fimg2(&img2); 


    t1 = AsyncTask(UpdateImage, fimg1, {}, {}); 
    t2 = AsyncTask(UpdateImage, fimg2, {}, {}); 
    Pred predimg1(t1.finished);
    Pred predimg2(std::bind(fimg1.counterPred, 100));
    t3 = AsyncTask(Combine2Image, fimg1, fimg2, {fimg1.read:predimg1, fimg2.read:predimg2}, {});
    std::cout << fimg1.read(3)<<std::endl;
}

double ImagePSNR(Image& img){
    ;
}



void example2(){
    Image img0(100);
    Image img1(100);
    Image img2(100);
    __fluid__ Image fimg0(&img0);
    __fluid__ Image fimg1(&img1);
    __fluid__ Image fimg2(&img2);        

    Pred predimg1(std::bind(fimg.counterPred, 80));
    Pred predimg2(std::bind(fimg.counterPred, 100));
    AsyncTask(UpdateImage, img0, {}, {}); 
    AsyncTask(ImagePipeline1, img0, img1, {ImageFluidProxy::read: predimg1}, {});
    AsyncTask(ImagePipeline2, img0, img2, {ImageFluidProxy::read: predimg2}, {});
  
}

int main(){
    example0();
}
