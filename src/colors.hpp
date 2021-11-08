// colors.hpp

#ifndef COLORS_H
#define COLORS_H

glm::vec3 hsv2rgb(glm::vec3 in)
{
    double      hh, p, q, t, ff;
    long        i;
    glm::vec3   out;

    if(in.y <= 0.0) {       // < is bogus, just shuts up warnings
        out.x = in.z;
        out.y = in.z;
        out.z = in.z;
        return out;
    }
    hh = in.x;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.z * (1.0 - in.y);
    q = in.z * (1.0 - (in.y * ff));
    t = in.z * (1.0 - (in.y * (1.0 - ff)));

    switch(i) {
    case 0:
        out.x = in.z;
        out.y = t;
        out.z = p;
        break;
    case 1:
        out.x = q;
        out.y = in.z;
        out.z = p;
        break;
    case 2:
        out.x = p;
        out.y = in.z;
        out.z = t;
        break;

    case 3:
        out.x = p;
        out.y = q;
        out.z = in.z;
        break;
    case 4:
        out.x = t;
        out.y = p;
        out.z = in.z;
        break;
    case 5:
    default:
        out.x = in.z;
        out.y = p;
        out.z = q;
        break;
    }
    return out;     
}

glm::vec3 hsv_to_rgb(glm::vec3 hsv)
{
    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;

    float c = s * v;
    float x = c * (1.0 - fabs((int)(h / 60.0) % 2 - 1.0));
    float m = v - c;

    glm::vec3 rgb;

    if      (h < 60)  rgb = {c, x, 0};
    else if (h < 120) rgb = {x, c, 0};
    else if (h < 180) rgb = {0, c, x};
    else if (h < 240) rgb = {0, x, c};
    else if (h < 300) rgb = {x, 0, c};
    else            rgb = {c, 0, x};

    rgb = rgb + m;
    return rgb;
}

// Implements the Blue-White-Red colormap
glm::vec3 bwr_colormap(float min, float max, float val)
{
    float mid = (max + min) / 2.0f;
    float r, g, b;
    float interp_factor;

    if (val < mid)
    {
        interp_factor = (val - min) / (mid - min); // How close to mid

        r = interp_factor;
        g = interp_factor;
        b = 1.0f;
    }
    else
    {
        interp_factor = (val - mid) / (max - mid); // How close to max

        r = 1.0f;
        g = 1.0f - interp_factor;
        b = 1.0f - interp_factor;
    }

    return glm::vec3(r, g, b);
}

// Implements the Cool-Warm colormap
glm::vec3 coolwarm_colormap(float min, float max, float val)
{
    glm::vec3 col_max(188, 32, 44);
    glm::vec3 col_mid(216, 219, 224);
    glm::vec3 col_min(67, 90, 204);

    col_max /= (float) 255.0f;
    col_mid /= (float) 255.0f;
    col_min /= (float) 255.0f;

    glm::vec3 ret_color;

    float mid = (max + min) / 2.0f;
    float interp_factor;

    if (val < mid)
    {
        interp_factor = (val - min) / (mid - min); // How close to mid
        ret_color = interp_factor * col_mid + (1.0f - interp_factor) * col_min;
    }
    else
    {
        interp_factor = (val - mid) / (max - mid); // How close to max
        ret_color = interp_factor * col_max + (1.0f - interp_factor) * col_mid;
    }

    return ret_color;
}

// Implements the HSV colormap
glm::vec3 hsv_colormap(float min, float max, float val)
{
    return hsv2rgb(glm::vec3(
        340 * ((val - min) / (max - min)),
        1.0f,
        1.0f));
}

#endif
