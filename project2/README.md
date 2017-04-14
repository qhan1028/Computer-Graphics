# Computer Graphics Project 2
## Maze
### Specifications
* [Reference Link](http://dgmm.csie.ntust.edu.tw/?ac1=courprojdetail_CG2012F_3&id=588070746df1a&sid=588937bc9568a)
### Environment Setup
* Download Visual Studio 2015 [link](https://msdn.microsoft.com/zh-tw/library/dd831853.aspx)
* Download Qt 5.4.2 [link](https://download.qt.io/archive/qt/5.4/5.4.2/qt-opensource-windows-x86-msvc2013_64_opengl-5.4.2.exe.mirrorlist)
* Download VS2015 Addin 2.1.1 [link](https://download.qt.io/archive/vsaddin/qt-vs-tools-msvc2015-2.1.1.vsix.mirrorlist)
* Setup Include/Reference/Library Path
  ![](https://i.imgur.com/FHJeoQp.png)

* Setup Qt Version <br>
  ![](https://i.imgur.com/uBUsenC.png)

### Code Work
* File: `/project2/Project2_Framework/Maze/OpenGLWidget.cpp`
* Functions: **(Bold font is the hard part.)** <br>
  `void Map_3D()`
    * `int findCell(...)` Find initial cell
        * `bool incell(...)`
    * `void drawCell(...)` Recursive function
        * `void drawWall(...)` Recursion terminate
            1. `void worldToView(...)` Transform coordinate from global (world) to local (player)
            2. **`void viewToSight(...)`** Split player FOV
                a. `int inSight(...)`
                b. `int inAngle(...)`
                c. `double findAngle(...)`
                d. `void findIntersection(...)`, `void findPoint(...)`
            3. `void sightTo2D(...)` Project 3D to 2D
            4. `void draw2D(...)`
        * `int findNeighborCell(...)`
        * **`void splitSight(...)`** Split FOV to next cell
            1. `int inAngle(...)`
        * `void drawCell(...)` Next recursion
