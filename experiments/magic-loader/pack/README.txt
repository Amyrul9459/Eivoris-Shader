EIVORIS — MAGIC LOADER WATER PROTOTYPE
======================================

This is an early Java-style shaderpack prototype for testing through Magic Loader.
It is NOT a Bedrock .mcpack and is NOT the existing RenderDragon shader.

INSTALL / TEST
1. Import/select this ZIP using Magic Loader's shaderpack workflow.
2. Start a clean Overworld world.
3. Look at water from close range, then from 15–30 blocks away.
4. Check for a black screen, missing water, opaque water, flickering, or crashes.
5. If it loads, test while moving the camera and after 10 minutes.

WHAT THIS BUILD CONTAINS
- Conservative GLSL 1.20 water vertex and fragment passes.
- Three low-amplitude world-position wave layers.
- Vertex displacement and matching analytic surface normals.
- Fresnel-based grazing-angle tint.
- Source alpha is preserved for translucent compositing.

WHAT IT DOES NOT CONTAIN YET
- Screen-space refraction or reflections.
- Underwater fog/absorption.
- Sky, shadow, cloud, or deferred-lighting passes.
- Verified performance or compatibility on the target phone.

IMPORTANT
The prototype assumes the loader exposes the common Iris/OptiFine shader inputs
used by the Bliss reference: mc_Entity, gbufferModelView, gbufferModelViewInverse,
cameraPosition, and frameTimeCounter. The Magic Loader compatibility list is
not a formal API specification. If this build fails, capture the full compiler
error/log; do not infer that the water math itself is at fault.
