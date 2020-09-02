#include <iostream>
#include <typeinfo>
#include "Stopwatch.h"
#include "DynaRTTI.h"

using namespace std;

class Shape {
    protected:
    RTTI_BASE_CLASS_TREE(
        _FT_Shape,
            _FT_Square,
                _FT_Rectangle, _LT_Rectangle,
            _LT_Square,
            _FT_Ellipse,
                _FT_Circle, _LT_Circle,
            _LT_Ellipse,
        _LT_Shape
    );
    
    RTTI_BASE_CLASS_PROTECTED_CONSTRUCTOR(Shape)

    private:
    RTTI_BASE_CLASS_PRIVATE

    public:
    RTTI_PUBLIC_DEFAULT_CONSTRUCTOR(Shape, Shape)
    virtual ~Shape() = default;     // Needed so that dyna_cast will work for timing comparisons

    RTTI_BASE_CLASS_PUBLIC
    RTTI_PUBLIC_METHODS(Shape, Shape)
};

class Square : public Shape {
    protected:
    RTTI_PROTECTED_CONSTRUCTOR(Shape, Square)

    public:
    RTTI_PUBLIC_DEFAULT_CONSTRUCTOR(Square, Square)
    RTTI_PUBLIC_METHODS(Shape, Square);
};

class Rectangle : public Square {
    protected:
    RTTI_PROTECTED_CONSTRUCTOR(Square, Rectangle)

    public:
    RTTI_PUBLIC_DEFAULT_CONSTRUCTOR(Rectangle, Rectangle)
    RTTI_PUBLIC_METHODS(Shape, Rectangle);
};

class Ellipse : public Shape {
    protected:
    RTTI_PROTECTED_CONSTRUCTOR(Shape, Ellipse)

    public:
    RTTI_PUBLIC_DEFAULT_CONSTRUCTOR(Ellipse, Ellipse)
    RTTI_PUBLIC_METHODS(Shape, Ellipse);
};

class Circle : public Ellipse {
    protected:
    RTTI_PROTECTED_CONSTRUCTOR(Ellipse, Circle)

    public:
    RTTI_PUBLIC_DEFAULT_CONSTRUCTOR(Circle, Circle)
    RTTI_PUBLIC_METHODS(Shape, Circle);
};

void printResults(const Shape testShape) {
    cout << "Shape is of type Circle    :" << (Circle::isTypeOf(&testShape) ? "true" : "false") << endl;
    cout << "Shape is of type Ellipse   :" << (Ellipse::isTypeOf(&testShape) ? "true" : "false") << endl;
    cout << "Shape is of type Rectangle :" << (Rectangle::isTypeOf(&testShape) ? "true" : "false") << endl;
    cout << "Shape is of type Square    :" << (Square::isTypeOf(&testShape) ? "true" : "false") << endl;
    cout << "Shape is of type Shape     :" << (Shape::isTypeOf(&testShape) ? "true" : "false") << endl;
    cout << "--------------------------------------------" << endl;
}

void calcDynaRTTI(const Shape testShape) {
    Circle::isTypeOf(&testShape);
    Ellipse::isTypeOf(&testShape);
    Rectangle::isTypeOf(&testShape);
    Square::isTypeOf(&testShape);
    Shape::isTypeOf(&testShape);
}

void calcStdRTTI(Shape testShape) {
    if (dynamic_cast<Circle*>(&testShape)) {
        ;
    }
    if (dynamic_cast<Ellipse*>(&testShape)) {
        ;
    }
    if (dynamic_cast<Rectangle*>(&testShape)) {
        ;
    }
    if (dynamic_cast<Square*>(&testShape)) {
        ;
    }
    if (dynamic_cast<Shape*>(&testShape)) {
        ;
    }
}

void doPrintDynaRTTI() {
    cout << "--------------------------------------------" << endl;
    printResults(Shape());
    printResults(Square());
    printResults(Rectangle());
    printResults(Ellipse());
    printResults(Circle());
}

void doTimingCompare() {
    auto count = 1000000;
    auto sw = Stopwatch();
    auto shape = Shape();
    auto square = Square();
    auto rect   = Rectangle();
    auto ellipse = Ellipse();
    auto circle  = Circle();
    sw.start();
    cout << "--------------------------------------------" << endl;
    for (int i = 0; i < count; ++i) {
        calcDynaRTTI(shape);
        calcDynaRTTI(square);
        calcDynaRTTI(rect);
        calcDynaRTTI(ellipse);
        calcDynaRTTI(circle);
    }
    sw.printMessage("Dyna RTTI");
    cout << "--------------------------------------------" << endl;
    for (int i = 0; i < count; ++i) {
        calcStdRTTI(shape);
        calcStdRTTI(square);
        calcStdRTTI(rect);
        calcStdRTTI(ellipse);
        calcStdRTTI(circle);
    }
    sw.stop();
    sw.printMessage("Std RTTI (dyna_cast)");
}

int main() {
    doTimingCompare();
}