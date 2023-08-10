uniform sampler2D color_map;
uniform bool useDepthMap;
uniform sampler2D depth_map;
uniform float height;
uniform float z_position;
void main()
{
   float color_alpha = texture2D(color_map, gl_TexCoord[0].xy).a;
   float height_pixel = 0.0; 
   if(useDepthMap == true) {
       vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);
       height_pixel = (depth_pixel.r + depth_pixel.g + depth_pixel.b) *.33 * height;
   }
   float z_pixel = height_pixel + z_position;
   gl_FragDepth = 1.0 - color_alpha * (0.5 + z_pixel * 0.001);
   gl_FragColor.r = gl_FragDepth;
   gl_FragColor.g = (gl_FragDepth - floor(gl_FragDepth * 256.0) / 256.0) * 256.0;
   gl_FragColor.b = (gl_FragDepth - floor(gl_FragDepth * (65536.0)) / 65536.0) * 65536.0;
   gl_FragColor.a = color_alpha;
}