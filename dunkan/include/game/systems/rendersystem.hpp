#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/Shader.hpp"
#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/System/Vector2.hpp"
#include "game/systems/lightsystem.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "game/types.hpp"

using RenderSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using RenderSystem_t = ADE::META_TYPES::Typelist<>;

const std::string vertexShader = \
    "varying vec3 vertex; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    vertex = (gl_ModelViewMatrix*gl_Vertex).xyz; "\
    "    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; "\
    "    gl_FrontColor = gl_Color; "\
    "}";

const std::string depth_fragShader = \
"uniform sampler2D color;" \
"uniform sampler2D depth;" \
"uniform sampler2D normal;" \
"uniform float height;" \
"uniform float z_pos;" \
"uniform bool debug_heightmap;" \
"uniform vec4 ambient_light;" \
    "uniform int NBR_LIGHTS;" \
    "varying vec3 vertex; "\
"void main()" \
"{" \
"   vec3 vertex_copy = vertex;" \
"   vec4 depth_pixel = texture2D(depth, gl_TexCoord[0].xy);" \
"   vec4 color_pixel = texture2D(color, gl_TexCoord[0].xy);" \
"	vec3 direction = -1.0 + 2.0 * texture2D(normal, gl_TexCoord[0].xy).rgb;"
"   if(depth_pixel.a == 0.0) discard;" \
"   float z_pixel = height + z_pos;"
"   vertex_copy.y -= z_pixel;"
"   vertex_copy.z = z_pixel + z_pos;"
"   gl_FragDepth = (((depth_pixel.r + depth_pixel.g + depth_pixel.b) / 3.0) * z_pixel );" \
"   if(debug_heightmap) {" \
"       gl_FragColor = vec4(gl_FragDepth, gl_FragDepth, gl_FragDepth, color_pixel.a);" \
"   } else { " \
"   gl_FragColor = ambient_light * color_pixel;" \
"   int i;" \
"   for(i = 0 ; i < NBR_LIGHTS ; i = i+1)" \
"   {" \
"	    float lighting = 0.0;" \
"	    if(gl_LightSource[i].position.w == 0.0)" \
"	    {		" \
"		    vec3 light_direction = -gl_LightSource[i].position.xyz;" \
"		    lighting = max(0.0, dot(direction,normalize(light_direction)));" \
"	    }" \
"	    else" \
"	    {" \
"		    vec3 light_direction = gl_LightSource[i].position.xyz - vertex_copy.xyz;" \
"		    float dist = length(light_direction);" \
"		    float attenuation = 1.0/( gl_LightSource[i].constantAttenuation +" \
"								  dist*gl_LightSource[i].linearAttenuation +" \
"								  dist*dist*gl_LightSource[i].quadraticAttenuation);" \
"		    lighting = max(0.0, dot(direction,normalize(light_direction))) * attenuation;" \
"	    }" \
"	    lighting *= gl_LightSource[i].diffuse.a;" \
"	    gl_FragColor.rgb +=  gl_Color.rgb * color_pixel.rgb * gl_LightSource[i].diffuse.rgb  * lighting;" \
"   }}" \
"}";

struct RenderSystem {

    bool debug_heightmap{false};

    void update(EntityManager& entity_manager, sf::RenderWindow& window) {
        window.clear();

        glClear(GL_DEPTH_BUFFER_BIT);
        glClearDepth(0.0);

        entity_manager.foreach<RenderSystem_c, RenderSystem_t>
        ([&](Entity& entity, RenderComponent& render, PhysicsComponent& physics)
        {
            sf::Vector2i position = window.mapCoordsToPixel(sf::Vector2f(physics.x, physics.y - physics.z));
            render.setPosition(position.x, position.y);
            render.setScale(sf::Vector2f(render.scale, render.scale));

            int nearby_lights = light_system.update(entity_manager);
            window.pushGLStates();

            glDepthFunc(GL_GREATER);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            sf::Shader depth;
            depth.loadFromMemory(vertexShader, depth_fragShader);
            depth.setUniform("color", render.get_texture());
            depth.setUniform("depth", render.depth_texture());
            depth.setUniform("normal", render.normal_texture());
            depth.setUniform("height", render.height * render.getScale().y * 0.001f);
            depth.setUniform("z_pos", physics.z * 0.01f);
            depth.setUniform("debug_heightmap", debug_heightmap);
            depth.setUniform("NBR_LIGHTS", nearby_lights);
            depth.setUniform("ambient_light",sf::Glsl::Vec4(sf::Color{ 180, 180, 180, 255 }));

            if(render.is_selected) {
                _debug_render_lines(render, window);
            }

            sf::Transform totalTransform;
            totalTransform = sf::Transform::Identity;

            sf::RenderStates state;
            state.transform = totalTransform;
            //state.transform.translate(position.x, position.y);

            state.shader = &depth;
            window.draw(render, state);

            window.popGLStates();
        });

#ifdef DEBUG_IMGUI
        ImGui::SFML::Render(window);
#endif

        window.display();
    }

private:
    void _debug_render_lines(RenderComponent& render, sf::RenderWindow& window) {
        float initial_x = render.getGlobalBounds().left;
        float initial_y = render.getGlobalBounds().top;
        float height = render.getGlobalBounds().height;
        float width = render.getGlobalBounds().width;

        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(initial_x, initial_y)),
            sf::Vertex(sf::Vector2f(initial_x + width, initial_y)),
            sf::Vertex(sf::Vector2f(initial_x + width, initial_y + height)),
            sf::Vertex(sf::Vector2f(initial_x, initial_y + height)),
            sf::Vertex(sf::Vector2f(initial_x, initial_y)),
        };

        window.draw(line, 5, sf::LineStrip);
    }

    LightSystem light_system {};

};


