#ifndef BSTONE_OGL_TRANSFORMATIONS_H
#define BSTONE_OGL_TRANSFORMATIONS_H


namespace bstone {


class OglTransformations {
public:
    // Sets an orthographic projection.
    static void ortho(
        int width,
        int height,
        float matrix[16]);

private:
    OglTransformations();

    OglTransformations(
        const OglTransformations& that);

    ~OglTransformations();

    OglTransformations& operator=(
        const OglTransformations& that);
}; // class OglTransformations


} // namespace bstone


#endif // BSTONE_OGL_TRANSFORMATIONS_H
