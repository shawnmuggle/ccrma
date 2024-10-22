printf("matrixmode\n");
mglMatrixMode(MGL_PROJECTION);

printf("loadidentity\n");
mglLoadIdentity();

printf("ortho\n");
mglOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);


mglMatrixMode(MGL_MODELVIEW);
mglLoadIdentity();

mglColor(255, 255, 255);
mglBegin(MGL_TRIANGLES);
mglVertex2(0.25, 0.25);
mglVertex2(0.75, 0.25);
mglVertex2(0.75, 0.75);
mglEnd();

// mglBegin(MGL_QUADS);
// mglVertex2(0.25, 0.05);
// mglVertex2(0.85, 0.25);
// mglVertex2(0.75, 0.95);
// mglVertex2(0.05, 0.85);
// mglEnd();

