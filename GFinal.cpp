#include <memory>
#include<algorithm>
#include "include/GFinal.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"
#include "include/GPixel.h"
#include "include/GPoint.h"
#include "include/GColor.h"
#include "Voronoi.cpp"
#include "ProxyShader.h"

/* A 4x5 matrix to transform unpremul GColors.
 *
 *  new_color = [0 4  8 12 16] [orig_color.r]
 *              [1 5  9 13 17] [orig_color.g]
 *              [2 6 10 14 18] [orig_color.b]
 *              [3 7 11 15 19] [orig_color.a]
 *
 *  e.g. new.r = M[0] * old.r + M[4] * old.g + M[8] * old.b + M[12] * old.a + M[16]
 *       new.g = M[1] * old.r + ...
 *       ...
 *
 *  The identity matrix would look like this
 *      [1 0 0 0 0]
 *      [0 1 0 0 0]
 *      [0 0 1 0 0]
 *      [0 0 0 1 0]
 */


class LinearPosGradient : public GShader {
    public:
        LinearPosGradient(GPoint p0, GPoint p1, const GColor colors[],const float pos[],int count){
            
        };
        bool isOpaque() override {
    }

    bool setContext(const GMatrix& ctm) override {
        if (auto inverted = (ctm).invert()) {
            fInverse = *inverted;
            return true;
        }
        return false;
    }

    void shadeRow(int x, int y, int count, GPixel out[]) override {
        GPoint dstPix;
        GPoint srcPix;
        int srcX;
        int srcY;
        int finX;
        int finY;

        for (int i = 0; i < count; i++) {

            dstPix.x = x + i + (0.5f);
            dstPix.y = y + (0.5f);

            srcPix = fInverse * dstPix;

            srcX = GFloorToInt(srcPix.x);
            srcY = GFloorToInt(srcPix.y);

            out[i] = GPixel_PackARGB(200,50,60, 70);
        }
    }
    GPixel color2Pix(GColor& color){
        unsigned int newA = GRoundToInt(color.a * 255);
    unsigned int newR = GRoundToInt(color.r * color.a * 255);
    unsigned int newG = GRoundToInt(color.g * color.a * 255);
    unsigned int newB = GRoundToInt(color.b * color.a * 255);

    return GPixel_PackARGB(newA, newR, newG, newB);
    }

    private:
        std::vector<GColor> flatGradient;
        GMatrix fInverse;










};
class ColorMatrixShader : public GShader {
    public:
        ColorMatrixShader(GShader* shader, const GColorMatrix& extraTransform)
        : fRealShader(shader), fMat(extraTransform) {}
        
        bool isOpaque() override { return fRealShader->isOpaque(); }

        bool setContext(const GMatrix& ctm) override {
            return fRealShader->setContext(ctm);
        }

        void shadeRow(int x, int y, int count, GPixel row[]) override {
            fRealShader->shadeRow(x, y, count, row);
            // for(int i = 0; i<count; i++){
            //     GPixel p;
            //     float sa = GPixel_GetA(row[i]);
            //     float a =  GPixel_GetA(row[i]) / 255.0f;
            //     float r = GPixel_GetR(row[i])/(255.0*sa); 
            //     float g = GPixel_GetG(row[i])/(255.0*sa);
            //     float b = GPixel_GetB(row[i])/(255.0*sa);
                
                
            //     float r_new = (fMat[0] * r + fMat[4] * g + fMat[8] * b + fMat[12] * a + fMat[16]);
            //     float g_new = (fMat[1] * r + fMat[5] * g + fMat[9] * b + fMat[13] * a + fMat[17]);
            //     float b_new = (fMat[2] * r + fMat[6] * g + fMat[10] * b + fMat[14] * a + fMat[18]);
            //     float a_new = (fMat[3] * r + fMat[7] * g + fMat[11] * b + fMat[15] * a + fMat[19]);

            //     if(a_new < 0.0){a_new = 0.0;}
            //     if(a_new > 1.0){a_new = 1.0;}

            //     if(r_new < 0.0){ r_new = 0.0;}
            //     if(r_new > 1.0){a_new = 1.0;}

            //     if(g_new < 0.0){g_new = 0.0;}
            //     if(g_new > 1.0){g_new = 1.0;}

            //     if(b_new < 0.0){b_new = 0.0;}
            //     if(b_new > 1.0){b_new = 1.0;}
                
            //     int newA = GRoundToInt(a_new * 255.0);
            //     int newR = GRoundToInt(r_new * a_new);
            //     int newG = GRoundToInt(g_new * a_new);
            //     int newB = GRoundToInt(b_new * a_new);

            //     row[i] = GPixel_PackARGB(newA, newR, newG, newB);
            // }
            
        }

    private:
        GShader* fRealShader;
        GColorMatrix fMat;
};

class SweepGradientShader : public GShader {
    public:
        SweepGradientShader(GPoint center, float startRadians, const GColor colors[], int count){
            for (int i=0; i<count; i++) {
                flatGradient.push_back(colors[i]);
            }
        }
        
        bool isOpaque() override {
    }

    bool setContext(const GMatrix& ctm) override {
        if (auto inverted = (ctm).invert()) {
            fInverse = *inverted;
            return true;
        }
        return false;
    }

    void shadeRow(int x, int y, int count, GPixel out[]) override {
        GPoint dstPix;
        GPoint srcPix;
        int srcX;
        int srcY;
        int finX;
        int finY;

        for (int i = 0; i < count; i++) {

            dstPix.x = x + i + (0.5f);
            dstPix.y = y + (0.5f);

            srcPix = fInverse * dstPix;

            srcX = GFloorToInt(srcPix.x);
            srcY = GFloorToInt(srcPix.y);

            GColor c = flatGradient[i];
            out[i] = GPixel_PackARGB(200, 50,60, 70);
        }
    }
    GPixel color2Pix(GColor& color){
        unsigned int newA = GRoundToInt(color.a * 255);
    unsigned int newR = GRoundToInt(color.r * color.a * 255);
    unsigned int newG = GRoundToInt(color.g * color.a * 255);
    unsigned int newB = GRoundToInt(color.b * color.a * 255);

    return GPixel_PackARGB(newA, newR, newG, newB);
    }

    private:
        std::vector<GColor> flatGradient;
        GMatrix gradMat;
        GMatrix fInverse;
};
class MyFinal : public GFinal{
    public:
    MyFinal(){};
    std::shared_ptr<GShader> createVoronoiShader(const GPoint points[], const GColor colors[], int count) {
        if (count < 1) {
        return nullptr;
    }
    return std::unique_ptr<GShader>(new Voronoi(points, colors, count));
    }

     std::shared_ptr<GShader> createColorMatrixShader(const GColorMatrix& mat, GShader* realShader) {
        return std::unique_ptr<GShader>(new ColorMatrixShader(realShader, mat));
    }
    std::shared_ptr<GShader> createSweepGradient(GPoint center, float startRadians,
                                                         const GColor colors[], int count) {
        return std::unique_ptr<GShader>(new SweepGradientShader(center, startRadians, colors, count));
    }
    std::shared_ptr<GShader> createLinearPosGradient(GPoint p0, GPoint p1, const GColor colors[],const float pos[],int count) {
            return std::unique_ptr<GShader>(new LinearPosGradient(p0, p1, colors, pos, count));
                                                }
};





std::unique_ptr<GFinal> GCreateFinal(){
    return std::unique_ptr<GFinal>(new MyFinal());
};