#ifndef CASPECTWINDOW_H
#define CASPECTWINDOW_H

#include <Aspect_Window.hxx>

class QWidget;
class CAspectWindowPrivate;

class CAspectWindow : public Aspect_Window
{
public:
    CAspectWindow(QWidget &view);
    ~CAspectWindow();

    Aspect_Drawable NativeHandle() const final;
    Aspect_Drawable NativeParentHandle() const final;

    Aspect_TypeOfResize DoResize() final;

    Standard_Boolean IsMapped() const final;
    Standard_Boolean DoMapping() const final;
    void Map() const final;
    void Unmap() const final;

    void Position(Standard_Integer &theX1, Standard_Integer &theY1,
                  Standard_Integer &theX2, Standard_Integer &theY2 ) const final;
    Standard_Real Ratio() const final;
    void Size(Standard_Integer& theWidth, Standard_Integer& theHeight) const final;

    Aspect_FBConfig NativeFBConfig() const final;

private:
    CAspectWindowPrivate * const d_ptr;
};

#endif // CASPECTWINDOW_H
