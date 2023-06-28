# 169_final_particle_system

A simple particle system done for a final project, where user can control numerous properties of particle, wind force, drag cooefficient, as well as groundd collision in real time using the menu displayed on the Window that is from the IMGUI library.

to see the particle system code, go to [src/Particle.h](src/Particle.h), [src/Particle.cpp](src/Particle.cpp), [src/ParticleSystem.h](src/ParticleSystem.h), [src/ParticleSystem.cpp](src/ParticleSystem.cpp)

## Video Demo (link to youtube)
**Positions, velocities, lifespan, variances
<https://youtu.be/vY6Cuga3zCw>
Collision, aerodynamics
<https://youtu.be/4Lez-7T4HC4>



##Note
- Particles are rendered/drawn as a cube even though internally the code treat it as a sphere. The project owner decide that it's better to get all the internals right than fixing importing a sphere object from StackOverflow.
- Particle mass cannot be altered due to complete oversight. This actually makes the demo more annoying to record as large wind force "blew" all the particles out of the winddow in an instant.
- The shading language is OpenGL because that's what we use in class for the entire quarter, and this code is a modification of the starting code where only a single stationary cube is displayed.


