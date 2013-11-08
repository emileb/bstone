#include "bstone_ogl_transformations.h"

#include <cassert>
#include <cstddef>


namespace bstone {


// (static)
void OglTransformations::ortho(
    int width,
    int height,
    float matrix[16])
{
    assert(width != 0);
    assert(height != 0);
    assert(matrix != NULL);

    matrix[0] = 2.0F / width;
    matrix[1] = 0.0F;
    matrix[2] = 0.0F;
    matrix[3] = 0.0F;

    matrix[4] = 0.0F;
    matrix[5] = 2.0F / height;
    matrix[6] = 0.0F;
    matrix[7] = 0.0F;

    matrix[8] = 0.0F;
    matrix[9] = 0.0F;
    matrix[10] = -2.0F;
    matrix[11] = 0.0F;

    matrix[12] = -1.0F;
    matrix[13] = -1.0F;
    matrix[14] = -1.0F;
    matrix[15] = 1.0F;
}


} // namespace bstone
