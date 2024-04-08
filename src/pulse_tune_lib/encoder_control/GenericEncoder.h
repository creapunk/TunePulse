#include <inttypes.h>

class GenericEncoder
{
  protected:
    uint32_t angle;
  public:
    virtual void init() = 0;
    virtual void updateCallback() = 0;
    uint32_t getAngle();
};