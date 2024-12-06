#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"
#include "include/GPixel.h"
#include "include/GPoint.h"
#include "include/GColor.h"
#include "include/GFinal.h"
#include <iostream>

class ColorMatrixShader : public GShader {
    public:
        ColorMatrixShader(GShader* shader, const GColorMatrix& extraTransform)
        : fRealShader(shader), fExtraTransform(extraTransform) {}
        
        bool isOpaque() override { return fRealShader->isOpaque(); }

        bool setContext(const GMatrix& ctm) override {
            return fRealShader->setContext(ctm * fExtraTransform);
        }

        void shadeRow(int x, int y, int count, GPixel row[]) override {
            fRealShader->shadeRow(x, y, count, row);
        }

    private:
        GShader* fRealShader;
        GColorMatrix  fExtraTransform;
};

static inline std::shared_ptr<GShader> GCreateProxyShader(GShader* shader, const GMatrix& extraTransform) {
    return std::unique_ptr<GShader>(new ProxyShader(shader, extraTransform));