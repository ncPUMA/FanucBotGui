#ifndef CBOTCROSS_H
#define CBOTCROSS_H

#include <NCollection_Vector.hxx>

class AIS_InteractiveObject;
class CBotCrossPrivate;

class CBotCross
{
public:
    CBotCross();
    virtual ~CBotCross();

    NCollection_Vector <Handle(AIS_InteractiveObject)> objects(const char *text) const;

private:
    CBotCross(const CBotCross &) = delete;
    CBotCross& operator=(const CBotCross &) = delete;

private:
    CBotCrossPrivate * const d_ptr;
};

#endif // CBOTCROSS_H
