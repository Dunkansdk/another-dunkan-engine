uniform sampler2D color_map;
uniform bool useDepthMap;
uniform sampler2D depth_map;
uniform float height;
uniform float z_position;
void main()
{
   vec4 color_pixel = texture2D(color_map, gl_TexCoord[0].xy);
   float height_pixel = 0.0;
   if(useDepthMap == true){
        vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);
       height_pixel = (depth_pixel.r + depth_pixel.g + depth_pixel.b) *.33 * height;
   }
   float z_pixel = height_pixel + z_position;
   gl_FragDepth = 1.0 - color_pixel.a * (0.5 + z_pixel * 0.001);
   gl_FragColor = gl_Color * color_pixel;
}