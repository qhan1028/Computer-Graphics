# Computer Graphics Project 1
## An Image Editing Program
### Specifications
* [Reference Link](http://dgmm.csie.ntust.edu.tw/?ac1=courprojdetail_CG2012F_3&id=5880706e6e9fe&sid=5880711e91d54)
### Environment Setup
* Download Visual Studio 2015 [link](https://msdn.microsoft.com/zh-tw/library/dd831853.aspx)
* Download Qt 5.8.0 [link](https://download.qt.io/archive/qt/5.8/5.8.0/qt-opensource-windows-x86-msvc2015_64-5.8.0.exe.mirrorlist)
* Download VS2015 Addin 2.1.1 [link](https://download.qt.io/archive/vsaddin/qt-vs-tools-msvc2015-2.1.1.vsix.mirrorlist)
* Setup Include/Reference/Library Path
  ![](https://i.imgur.com/djTHnbj.png)
* Setup Qt Version <br>
  ![](https://i.imgur.com/veloYoq.png)

### Code Work
* File: `/project1/Qt_Opengl_Framework/Qt_Opengl_Framework/Application.cpp`
* Functions:
    * Color
        * Gray `void Application::Gray()`
        * Uniform Quantization `void Application::Quant_Uniform()`
        * Populosity `void Application::Quant_Populosity()`
    * Dither
        * Naive Threshold `void Application::Dither_Threshold()`
        * Brightness Preserving Threshold `void Application::Dither_Bright()`
        * Random `void Application::Dither_Random()`
        * Clustered `void Application::Dither_Cluster()`
        * Floyd-Steinberg `void Application::Dither_FS()`
        * Color Floyd-Steinberg `void Application::Dither_Color()`
    * Filter
        * Box `void Application::Filter_Box()`
        * Barlett `void Application::Filter_Bartlett()`
        * Guassian `void Application::Filter_Gaussian()`
    * Size & Rotate
        * Arbitrary Uniform Scale `void Application::Resize( float scale )`
        * Rotate `void Application::Rotate( float angleDegrees )`
