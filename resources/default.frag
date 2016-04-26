
uniform bool    enable_tex;         // Textures enables
uniform sampler2D s_tex0;

// varying variables output by the vertex shader
varying vec2    v_textcoord;
varying vec4    v_front_color;
varying vec4    v_back_color;
varying float   v_fog_factor;
varying float   v_ucp_factor;

void main()
{
    if (!enable_tex)
    {
        if (gl_FrontFacing) gl_FragColor= v_front_color;
        else gl_FragColor= v_back_color;
    }
    else
    {
        if (gl_FrontFacing) gl_FragColor= texture2D(s_tex0, v_textcoord) * v_front_color;
        else gl_FragColor= gl_FragColor= texture2D(s_tex0, v_textcoord) * v_back_color;
    }
}
