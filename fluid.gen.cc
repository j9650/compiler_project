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
};

class ImageFluidProxy{
public:
    int64_t CallNUM_access;
    Guard*  guard_sum;
    Fluid<Image>* image;
    SymbolTable sb;
    std::vector<Pred*> GlobalGuard;
    std::vector<Pred*> TaskGuard;
    std::vector<Pred*> FuncGuard;
    Task* task;
    int32_t& access(Pred* pred, int32_t idx){
        CallNUM_access++;
        task->TaskGuard["accessPredPtrINT32INT32"]->run();
        pred->run();
        return image->write()->access(idx);
    }

    int32_t read(Pred* pred,int32_t idx) const{
        pred->run();
        task->TaskGuard["readPredPtrINT32INT32"]->run();
        return image->read()->read(idx);
    }

    bool write(Pred* pred, int32_t idx, int32_t pixel){
        pred->run();
        task->TaskGuard["writePredPtrINT32INT32INT32"]->run();
        return image->write()->write(idx, pixel);
    }    

    int32_t sum(Pred* pred) const{
        pred->run();
        task->TaskGuard["sumConstPredPtrINT32"]->run();
        guard_sum->run();//static guard
        return image->read()->sum();
    }

    int32_t size(Pred* pred) const{
        pred->run();
        task->TaskGuard["sizeConstPredPtrINT32"]->run();
        return image->read()->size();
    }
   
    ImageFluidProxy(Image* _image){
        image = new Fluid<Image>(_image);
        CallNUM_access = 0;
        guard_sum = newGuard(image->read()->counterPred, 100);
        sb.add("accessPredPtrINT32INT32");
        sb.add("readPredPtrINT32INT32");
        sb.add("writePredPtrINT32INT32INT32");
        sb.add("sumConstPredPtrINT32");
        sb.add("sizeConstPredPtrINT32");
    }
};

void UpdateImage(ImageFluidProxy* img){
    for (int i = 0; i < 100; i++)
        img->write(nullptr, i, i);
    return;
}

void Combine2Image(ImageFluidProxy* fimg1, ImageFluidProxy* fimg2){
    for(int i = 0; i < fimg1->size(); i++) {
        fimg1->write(nullptr, i, fimg1->read(nullptr, i)+fimg2->read(nullptr, i));
    }
}

void ImagePipeline1(ImageFluidProxy* in, ImageFluidProxy* out){};
void ImagePipeline2(ImageFluidProxy* in, ImageFluidProxy* out){};
void ImagePipeline3(ImageFluidProxy* in, ImageFluidProxy* out){};

void example0(){
    Image img1(100);
    Image img2(100);
    Image img3(100);
    Image img4(100);

    ImageFluidProxy fimg1(&img1);
    ImageFluidProxy fimg2(&img2);
    ImageFluidProxy fimg3(&img3);

    Pred predimg1(std::bind(fimg1.counterPred, 70));
    Pred predimg2(std::bind(fimg2.counterPred, 80));
    Pred predimg3(std::bind(fimg3.counterPred, 100));

    Task* task1 = TaskFactory.newTask<decltype(ImagePipeline1),...> (ImagePipeline1, fimg1, fimg2, {predimg1}, {});
    Task* task2 = TaskFactory.newTask<decltype(ImagePipeline2),...> (ImagePipeline2, fimg2, fimg3, {predimg2}, {});
    Task* task3 = TaskFactory.newTask<decltype(ImagePipeline3),...> (ImagePipeline3, fimg3,  img4, {predimg3}, {});

}



void example1(){
    Image img1(100);
    Image img2(100);

    ImageFluidProxy fimg1(&img1); 
    ImageFluidProxy fimg2(&img2); 

    Task* t1 = TaskFactory.newAsyncTask<decltype(UpdateImage), ...>(UpdateImage, fimg1, {}, {}); 
    Task* t2 = TaskFactory.newAsyncTask<decltype(UpdateImage), ...>(UpdateImage, fimg2, {}, {}); 
    Pred predimg1(t1.finished);
    Pred predimg2(std::bind(fimg1.counterPred, 100));
    Task* t3 = TaskFactory.newAsyncTask<decltype(Combine2Image), ...>(Combine2Image, fimg1, fimg2, {{fimg1, "readPredPtrINT32INT32", predimg1}, {fimg2,"readPredPtrINT32INT32", predimg2}}, {}); 

    std::cout << fimg1.read(idx) <<std::endl;
}

double ImagePSNR(Fluid<Image>& img){
    ;
}




void example2(){
    Image img0(100);
    Image img1(100);
    Image img2(100);

    ImageFluidProxy fimg0(&img0);
    ImageFluidProxy fimg1(&img1);
    ImageFluidProxy fimg2(&img2);        

    Pred predimg1(std::bind(fimg.counterPred, 80));
    Pred predimg2(std::bind(fimg.counterPred, 100));

    AsyncTask(UpdateImage, img0, {}, {}); 
    AsyncTask(ImagePipeline1, img0, img1, {"readPredPtrINT32INT32" : predimg1}, {});
    AsyncTask(ImagePipeline2, img0, img2, {"readPredPtrINT32INT32" : predimg2}, {});
}
