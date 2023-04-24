uniform vec2 origin;
uniform vec2 size;
uniform vec2 tex_size;

in vec2 pos_tex;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(origin + pos_tex * size, 1.0, 1.0);
    vec2 tex = vec2(pos_tex.x, 1.0 - pos_tex.y);
    TexCoord = tex * tex_size;
}
