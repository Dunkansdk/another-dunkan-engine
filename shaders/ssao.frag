uniform sampler2D normal_map;
uniform sampler2D depth_map;
uniform sampler2D noise_map;
uniform float z_position;
uniform vec2 screen_ratio;
uniform float zoom;
uniform vec3 samples_hemisphere[16];
varying vec3 vertex;

void main()
{
    vec4 normal_pixel = texture2D(normal_map, gl_TexCoord[0].xy);
    vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);
    vec3 direction = -1.0 + 2.0 * texture2D(normal_map, gl_TexCoord[0].xy).rgb;
    float height_pixel = (0.5 - (depth_pixel.r + depth_pixel.g / 256.0+depth_pixel.b / 65536.0)) * 1000.0;
    vec3 frag_pos = vertex;
    frag_pos.y -= height_pixel * 0.0;
    frag_pos.z = height_pixel;
    float occlusion = 0.0;
    vec3 rVec = -1.0 + 2.0 * texture2D(noise_map, gl_TexCoord[0].xy).rgb;
    vec3 t = normalize(rVec - direction * dot(rVec, direction));
    mat3 rot = mat3(t, cross(direction,t), direction);
    for(int i =0 ; i < 16 ; ++i){
        vec3 decal = rot * samples_hemisphere[i] * 10.0;
        vec3 screen_decal = decal;
        screen_decal.y *= -1.0;
        vec3 screen_pos = gl_FragCoord.xyz  + screen_decal;
        vec3 occl_depth_pixel = texture2D(depth_map, (screen_pos.xy) * screen_ratio).rgb;
        float occl_height = (0.5 - (occl_depth_pixel.r + occl_depth_pixel.g / 256.0 + occl_depth_pixel.b / 65536.0)) * 1000.0;
        if(occl_height > (frag_pos.z+decal.z) + 1.0
        && occl_height - (frag_pos.z+decal.z) < 10.0)
            occlusion += 1.0;
    } 
    float color_rgb = 1.0 - occlusion / 12.0;
    gl_FragColor.rgb = vec3(color_rgb, color_rgb, color_rgb);
    gl_FragColor.a = 0.1;
};