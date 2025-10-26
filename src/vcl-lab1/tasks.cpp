#include <random>

#include <spdlog/spdlog.h>

#include "Labs/1-Drawing2D/tasks.h"

using VCX::Labs::Common::ImageRGB;

namespace VCX::Labs::Drawing2D {
    /******************* 1.Image Dithering *****************/
    void DitheringThreshold(
        ImageRGB &       output,
        ImageRGB const & input) {
        for (std::size_t x = 0; x < input.GetSizeX(); ++x)
            for (std::size_t y = 0; y < input.GetSizeY(); ++y) {
                glm::vec3 color = input.At(x, y);
                output.At(x, y) = {
                    color.r > 0.5 ? 1 : 0,
                    color.g > 0.5 ? 1 : 0,
                    color.b > 0.5 ? 1 : 0,
                };
            }
    }

    void DitheringRandomUniform(
        ImageRGB &       output,
        ImageRGB const & input) {
        std::default_random_engine e;
        std::uniform_real_distribution<float> u(-0.5f, 0.5f);
         for (std::size_t x = 0; x < input.GetSizeX(); ++x)
            for (std::size_t y = 0; y < input.GetSizeY(); ++y) {
                glm::vec3 color = input.At(x, y);
                float r=u(e);
                output.At(x, y) = {
                    color.r+r > 0.5 ? 1 : 0,
                    color.g+r > 0.5 ? 1 : 0,
                    color.b+r > 0.5 ? 1 : 0,
                };
            }
    }

    void DitheringRandomBlueNoise(
        ImageRGB &       output,
        ImageRGB const & input,
        ImageRGB const & noise) {
        for (std::size_t x = 0; x < input.GetSizeX(); ++x)
            for (std::size_t y = 0; y < input.GetSizeY(); ++y) {
                glm::vec3 color = input.At(x, y);
                glm::vec3 n = noise.At(x % noise.GetSizeX(), y % noise.GetSizeY());
                output.At(x, y) = {
                    color.r + n.r - 0.5 > 0.5 ? 1 : 0,
                    color.g + n.g - 0.5 > 0.5 ? 1 : 0,
                    color.b + n.b - 0.5 > 0.5 ? 1 : 0,
                };
            }
    }

    void DitheringOrdered(
        ImageRGB &       output,
        ImageRGB const & input) {
        float kernel[3][3]={
            {6,8,4},
            {1,0,3},
            {5,2,7}
        };
        for(int x=0;x<input.GetSizeX();++x){
            for(int y=0;y<input.GetSizeY();++y){
                glm::vec3 color=input.At(x,y);
                for(int i=0;i<3;++i){
                    for(int j=0;j<3;++j){
                        output.At(3*x+i,3*y+j) = {
                            color.r > (kernel[i][j])/9.0f ? 1 : 0,
                            color.g > (kernel[i][j])/9.0f ? 1 : 0,
                            color.b > (kernel[i][j])/9.0f ? 1 : 0,
                        };
                    }
                }
            }
        }
    }

    void DitheringErrorDiffuse(
        ImageRGB &       output,
        ImageRGB const & input) {
        output = input;
        for(int y=0;y<input.GetSizeY();++y){
            for(int x=0;x<input.GetSizeX();++x){
                glm::vec3 old_color=output.At(x,y);
                glm::vec3 new_color={
                    old_color.r>0.5?1:0,
                    old_color.g>0.5?1:0,
                    old_color.b>0.5?1:0
                };
                glm::vec3 error=old_color-new_color;
                output.At(x,y)=new_color;
                if(x+1<input.GetSizeX()){
                    glm::vec3 tem=glm::vec3(output.At(x+1,y));
                    tem+=error*7.0f/16.0f;
                    tem.r=glm::clamp(tem.r,0.0f,1.0f);
                    tem.g=glm::clamp(tem.g,0.0f,1.0f); 
                    tem.b=glm::clamp(tem.b,0.0f,1.0f);
                    output.At(x+1,y)=tem;}
                if(x-1>=0&&y+1<input.GetSizeY()){
                    glm::vec3 tem=glm::vec3(output.At(x-1,y+1));
                    tem+=error*3.0f/16.0f;
                    tem.r=glm::clamp(tem.r,0.0f,1.0f);
                    tem.g=glm::clamp(tem.g,0.0f,1.0f); 
                    tem.b=glm::clamp(tem.b,0.0f,1.0f);
                    output.At(x-1,y+1)=tem;
                }
                if(y+1<input.GetSizeY()){
                    glm::vec3 tem=glm::vec3(output.At(x,y+1));
                    tem+=error*5.0f/16.0f;
                    tem.r=glm::clamp(tem.r,0.0f,1.0f);
                    tem.g=glm::clamp(tem.g,0.0f,1.0f); 
                    tem.b=glm::clamp(tem.b,0.0f,1.0f);
                    output.At(x,y+1)=tem;
                }
                if(x+1<input.GetSizeX()&&y+1<input.GetSizeY()){
                    glm::vec3 tem=glm::vec3(output.At(x+1,y+1));
                    tem+=error*1.0f/16.0f;
                    tem.r=glm::clamp(tem.r,0.0f,1.0f);
                    tem.g=glm::clamp(tem.g,0.0f,1.0f); 
                    tem.b=glm::clamp(tem.b,0.0f,1.0f);
                    output.At(x+1,y+1)=tem;
                }
            }}
    }

    /******************* 2.Image Filtering *****************/
    void Blur(
        ImageRGB &       output,
        ImageRGB const & input) {
        float kernel[3][3] = {
            {1.0f / 9, 1.0f / 9, 1.0f / 9},
            {1.0f / 9, 1.0f / 9, 1.0f / 9},
            {1.0f / 9, 1.0f / 9, 1.0f / 9}
        };
        for(int x=0;x<input.GetSizeX();++x){
            for(int y=0;y<input.GetSizeY();++y){
                glm::vec3 color(0.0f,0.0f,0.0f);
                for(int i=-1;i<=1;++i)
                    for(int j=-1;j<=1;++j){
                        int nx=std::clamp(x+i,0,(int)input.GetSizeX()-1);
                        int ny=std::clamp(y+j,0,(int)input.GetSizeY()-1);
                        color+=kernel[i+1][j+1]*input.At(nx,ny);
                    }
                output.At(x,y)=color;
            }}
    }

    void Edge(
        ImageRGB &       output,
        ImageRGB const & input) {
        float kernel_x[3][3] = {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1}
        };
        float kernel_y[3][3] = {
            { 1, 2, 1},
            { 0, 0, 0},
            {-1,-2,-1}
        };
        for(int x=0;x<input.GetSizeX();++x){
            for(int y=0;y<input.GetSizeY();++y){
                glm::vec3 color(0.0f,0.0f,0.0f);
                glm::vec3 color_x(0.0f,0.0f,0.0f);
                glm::vec3 color_y(0.0f,0.0f,0.0f);
                for(int i=-1;i<=1;++i)
                    for(int j=-1;j<=1;++j){
                        int nx=std::clamp(x+i,0,(int)input.GetSizeX()-1);
                        int ny=std::clamp(y+j,0,(int)input.GetSizeY()-1);
                        color_x+=kernel_x[i+1][j+1]*input.At(nx,ny);
                        color_y+=kernel_y[i+1][j+1]*input.At(nx,ny);
                    }
                color=glm::sqrt(color_x*color_x+color_y*color_y);
                output.At(x,y)=glm::clamp(color,glm::vec3(0.0f),glm::vec3(1.0f));
            }}
    }

    /******************* 3. Image Inpainting *****************/
    void Inpainting(
        ImageRGB &         output,
        ImageRGB const &   inputBack,
        ImageRGB const &   inputFront,
        const glm::ivec2 & offset) {
        output             = inputBack;
        std::size_t width  = inputFront.GetSizeX();
        std::size_t height = inputFront.GetSizeY();
        glm::vec3 * g      = new glm::vec3[width * height];
        memset(g, 0, sizeof(glm::vec3) * width * height);
        // set boundary condition
        for (std::size_t y = 0; y < height; ++y) {
            // set boundary for (0, y), your code: g[y * width] = ?
             g[y * width] = inputBack.At(offset.x + 0, offset.y + y) - inputFront.At(0, y);
            // set boundary for (width - 1, y), your code: g[y * width + width - 1] = ?
            g[y * width + width - 1] = inputBack.At(offset.x + width - 1, offset.y + y) - inputFront.At(width - 1, y);
        }
        for (std::size_t x = 0; x < width; ++x) {
            // set boundary for (x, 0), your code: g[x] = ?
            g[x] = inputBack.At(offset.x + x, offset.y + 0) - inputFront.At(x, 0);
            // set boundary for (x, height - 1), your code: g[(height - 1) * width + x] = ?
            g[(height - 1) * width + x] = inputBack.At(offset.x + x, offset.y + height - 1) - inputFront.At(x, height - 1);
        }

        // Jacobi iteration, solve Ag = b
        for (int iter = 0; iter < 8000; ++iter) {
            for (std::size_t y = 1; y < height - 1; ++y)
                for (std::size_t x = 1; x < width - 1; ++x) {
                    g[y * width + x] = (g[(y - 1) * width + x] + g[(y + 1) * width + x] + g[y * width + x - 1] + g[y * width + x + 1]);
                    g[y * width + x] = g[y * width + x] * glm::vec3(0.25);
                }
        }

        for (std::size_t y = 0; y < inputFront.GetSizeY(); ++y)
            for (std::size_t x = 0; x < inputFront.GetSizeX(); ++x) {
                glm::vec3 color = g[y * width + x] + inputFront.At(x, y);
                output.At(x + offset.x, y + offset.y) = color;
            }
        delete[] g;
    }

    /******************* 4. Line Drawing *****************/
    void DrawLine(
        ImageRGB &       canvas,
        glm::vec3 const  color,
        glm::ivec2 const p0,
        glm::ivec2 const p1) {
        if(p1.x==p0.x){
            for(int y=std::min(p1.y,p0.y);y<=std::max(p0.y,p1.y);++y)
                canvas.At(p0.x,y)=color;
        }
        else if(p1.y==p0.y){
            for(int x=std::min(p0.x,p1.x);x<=std::max(p0.x,p1.x);++x)
                canvas.At(x,p0.y)=color;
        }
        else{
            float k=(float)(p1.y-p0.y)/(float)(p1.x-p0.x);
            if(std::abs(k)<=1){
                int x0,y0,x1,y1;
                if(p0.x>p1.x){
                     x0=p1.x;
                     y0=p1.y;
                     x1=p0.x;
                     y1=p0.y;
                }
                else{
                     x0=p0.x;
                     y0=p0.y;
                     x1=p1.x;
                     y1=p1.y;
                }
                int y = y0;
                int dx= 2 * (x1 - x0), dy=2 * (y1 - y0);
                dy=std::abs(dy);
                int dydx= dy - dx,  F = dy - (dx / 2);
                for(int x=x0;x<=x1;++x){
                    canvas.At(x,y)=color;
                    if(F<0)
                        F+=dy;
                    else{
                        y+= (y1>y0?1:-1);
                        F+=dydx;
                    }
                }
            }
            else{
                int x0,y0,x1,y1;
                if(p0.y>p1.y){
                     x0=p1.x;
                     y0=p1.y;
                     x1=p0.x;
                     y1=p0.y;
                }
                else{
                     x0=p0.x;
                     y0=p0.y;
                     x1=p1.x;
                     y1=p1.y;
                }
                int x = x0;
                int dx= 2 * (x1 - x0), dy=2 * (y1 - y0);
                dx=std::abs(dx);
                int dydx= dx - dy,  F = dx - (dy / 2);
                for(int y=y0;y<=y1;++y){
                  canvas.At(x,y)=color;
                    if(F<0)
                        F+=dx;
                    else{
                        x+= (x1>x0?1:-1);
                        F+=dydx;
                    }
                }
            }
        }
    }

    /******************* 5. Triangle Drawing *****************/
bool inside(int x,int y,glm::ivec2 const p0,glm::ivec2 const p1,glm::ivec2 const p2){
                int a1=(p1.x-p0.x)*(y-p0.y)-(p1.y-p0.y)*(x-p0.x);
                int a2=(p2.x-p1.x)*(y-p1.y)-(p2.y-p1.y)*(x-p1.x);
                int a3=(p0.x-p2.x)*(y-p2.y)-(p0.y-p2.y)*(x-p2.x);
                return (a1>=0&&a2>=0&&a3>=0)||(a1<=0&&a2<=0&&a3<=0);
            };
    void DrawTriangleFilled(
        ImageRGB &       canvas,
        glm::vec3 const  color,
        glm::ivec2 const p0,
        glm::ivec2 const p1,
        glm::ivec2 const p2) {
            int minx=std::min(p0.x,std::min(p1.x,p2.x));
            int maxx=std::max(p0.x,std::max(p1.x,p2.x));
            int miny=std::min(p0.y,std::min(p1.y,p2.y));
            int maxy=std::max(p0.y,std::max(p1.y,p2.y));
            for(int x=minx;x<=maxx;++x)
                for(int y=miny;y<=maxy;++y)
                    if(inside(x,y,p0,p1,p2))
                        canvas.At(x,y)=color;
    }

    /******************* 6. Image Supersampling *****************/
    void Supersample(
        ImageRGB &       output,
        ImageRGB const & input,
        int              rate) {
        if (rate < 1) {
            rate = 1;
        }
        int input_w = input.GetSizeX();
        int input_h = input.GetSizeY();
        int output_w = output.GetSizeX();
        int output_h = output.GetSizeY();
        float scale_x = static_cast<float>(input_w) / output_w;
        float scale_y = static_cast<float>(input_h) / output_h;
         for (std::size_t x = 0; x < output.GetSizeX(); ++x)
            for (std::size_t y = 0; y < output.GetSizeY(); ++y) {
                glm::vec3 color(0.0f,0.0f,0.0f);
                for(int i=0;i<rate;++i){
                    for(int j=0;j<rate;++j){
                        float dx=(float)(i+0.5)/(float)rate;
                        float dy=(float)(j+0.5)/(float)rate;
                        float sampl_x=(x+dx)*scale_x;
                        float sampl_y=(y+dy)*scale_y;
                        sampl_x = glm::clamp(sampl_x, 0.0f, static_cast<float>(input_w - 1));
                        sampl_y = glm::clamp(sampl_y, 0.0f, static_cast<float>(input_h- 1));
                        int x1=static_cast<int>(std::floor(sampl_x));
                        int y1=static_cast<int>(std::floor(sampl_y));
                        int x2=std::min(x1+1,(int)(input_w - 1));
                        int y2=std::min(y1+1,(int)(input_h - 1));
                        float t=sampl_x-x1;
                        float u=sampl_y-y1;
                        glm::vec3 c=(1-t)*(1-u)*input.At(x1,y1)+t*(1-u)*input.At(x2,y1)+(1-t)*u*input.At(x1,y2)+t*u*input.At(x2,y2);
                        color+=c;
                    }}
                color=color/(float)(rate*rate);
                output.At(x,y)=color;
            }
    }

    /******************* 7. Bezier Curve *****************/
    // Note: Please finish the function [DrawLine] before trying this part.
    glm::vec2 CalculateBezierPoint(
        std::span<glm::vec2> points,
        float const          t) {
        std::vector<glm::vec2> points_copy = std::vector<glm::vec2>(points.begin(), points.end());
        for(int i=1;i<points_copy.size();++i)
            for(int j=0;j<points_copy.size()-i;++j)
                points_copy[j]=(1.0f-t)*points_copy[j]+t*points_copy[j+1];
        return points_copy[0];
    }
} // namespace VCX::Labs::Drawing2D