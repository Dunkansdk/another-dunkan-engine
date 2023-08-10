uniform sampler2D color_map;
uniform bool useDepthMap;
uniform sampler2D depth_map;
uniform sampler2D material_map;
uniform float height;
uniform bool enable_materialMap;
uniform float z_position;
uniform float p_roughness;
uniform float p_metalness;
uniform float p_translucency;

void main()
{
    vec4 color_pixel = texture2D(color_map, gl_TexCoord[0].xy);
    vec4 material_pixel = vec4(p_roughness,p_metalness,p_translucency, color_pixel.a);
    if(enable_materialMap){
        material_pixel = texture2D(material_map, gl_TexCoord[0].xy);
    }
    float height_pixel = 0.0; 
    if(useDepthMap == true){
        vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);
        height_pixel = (depth_pixel.r + depth_pixel.g + depth_pixel.b) *.33 * height;
    }
    float z_pixel = height_pixel + z_position;
    if(color_pixel.a < 0.9)
        color_pixel.a = 0.0;
    gl_FragDepth = 1.0 - color_pixel.a * (0.5 + z_pixel * 0.001);
    gl_FragColor = gl_Color * material_pixel; 
}