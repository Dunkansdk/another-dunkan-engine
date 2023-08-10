uniform sampler2D color_map;
uniform bool useDepthMap;
uniform sampler2D depth_map;
uniform bool useNormalMap;
uniform sampler2D normal_map;
uniform float height;
uniform float z_position;
void main()
{
    float color_alpha = texture2D(color_map, gl_TexCoord[0].xy).a;
    vec3 direction = vec3(0.0, 0.0, 1.0);
    if(useNormalMap){
        direction = -1.0 + 2.0 * texture2D(normal_map, gl_TexCoord[0].xy).rgb;
    }
    float height_pixel = 0.0; 
    if(useDepthMap){
        vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);
        height_pixel = (depth_pixel.r + depth_pixel.g + depth_pixel.b) *.33 * height;
    }
    float z_pixel = height_pixel + z_position;
    gl_FragDepth = 1.0 - color_alpha * (0.5 + z_pixel * 0.001);
    gl_FragColor.rgb = 0.5 + direction * 0.5;
    gl_FragColor.a = color_alpha;
}